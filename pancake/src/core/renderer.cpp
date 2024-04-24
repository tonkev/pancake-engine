#include "core/renderer.hpp"

#include "core/session.hpp"
#include "gl3/gl3_renderer.hpp"
#include "graphics/atlassed_texture.hpp"
#include "graphics/framebuffer.hpp"
#include "graphics/mesh.hpp"
#include "graphics/shader.hpp"
#include "resources/resources.hpp"
#include "resources/texture_props_resource.hpp"
#include "resources/tileset_resource.hpp"

using namespace pancake;

Mat4f Renderer::CameraInfo::projection(const Framebuffer& framebuffer) const {
  if (perspective) {
    const Vec2i& size = framebuffer.getSize();
    return Mat4f::perspective(fov, size.y() / static_cast<float>(size.x()), near, far);
  } else {
    return Mat4f::ortographic(ortho_size.x(), ortho_size.y(), far);
  }
}

Renderer::Renderer()
    : _textures(),
      _render_size(512, 512),
      _screen_size(512, 512),
      _render_match_screen(false),
      _mesh_update_queue(),
      _texture_update_queue(),
      _tileset_update_queue(),
      _shader_update_queue(),
      _tilesets(),
      _cameras(),
      _cam_draw_calls(),
      _draw_calls() {}

void Renderer::init() {
  FramebufferInfo info;
  info.render_targets[0].clear_colour = Vec4f::ones();
  info.size = _render_size;
  info.num_targets = 1;
  _framebuffers.emplace(GUID::null, createFramebuffer(GUID::null, info));

  MaterialResource defaultMatRes("", GUID::null);
  defaultMatRes.setShader(getDefaultShader()->guid());
  defaultMatRes.addInput(ShaderInput("colour", Vec4f::ones()));
  defaultMatRes.addInput(ShaderInput("tex", TextureRef(getBlankTexture().guid(), -1)));
  _materials.emplace(GUID::null, GUID::null)
      .first->second.resourceUpdated<MaterialOrigin>(defaultMatRes);
}

void Renderer::queueMeshUpdate(const GUID& guid) {
  _mesh_update_queue.emplace(guid);
}

void Renderer::queueTextureUpdate(const GUID& guid) {
  _texture_update_queue.emplace_back(guid);
}

void Renderer::queueTilesetUpdate(const GUID& guid) {
  _tileset_update_queue.emplace_back(guid);
}

void Renderer::queueShaderUpdate(const GUID& guid) {
  _shader_update_queue.emplace(guid);
}

OptFramebufferConstRef Renderer::getFramebuffer(const GUID& guid) const {
  OptFramebufferConstRef framebuffer;
  if (const auto it = _framebuffers.find(guid); it != _framebuffers.end()) {
    framebuffer = *it->second;
  }
  return framebuffer;
}

OptTextureConstRef Renderer::getTexture(const GUID& guid) const {
  OptTextureConstRef texture;
  if (_textures.contains(guid)) {
    texture = *_textures.at(guid);
  }
  return texture;
}

OptTilesetConstRef Renderer::getTileset(const GUID& guid) const {
  OptTilesetConstRef tileset;
  if (_tilesets.contains(guid)) {
    tileset = _tilesets.at(guid);
  }
  return tileset;
}

OptMaterialConstRef Renderer::getMaterial(const GUID& guid) const {
  OptMaterialConstRef material;
  if (const auto it = _materials.find(guid); it != _materials.end()) {
    material = it->second;
  }
  return material;
}

void Renderer::setScreenSize(const Vec2i& size) {
  _screen_size = size;
  if (_render_match_screen) {
    _render_size = _screen_size;
  }
}

void Renderer::matchRenderSizeToScreenSize(bool value) {
  _render_match_screen = value;
  if (_render_match_screen) {
    _render_size = _screen_size;
  }
}

const Vec2i& Renderer::renderSize() const {
  return _render_size;
}

const Vec2i& Renderer::screenSize() const {
  return _screen_size;
}

Vec2f Renderer::screenToRenderPosition(const Vec2f& screen_pos) const {
  Vec2f pos = screen_pos;

  if (_render_size != _screen_size) {
    const Vec2f frame_size = Vec2f(_render_size);
    const Vec2f screen_size = Vec2f(_screen_size);

    const Vec2f screen_to_frame = frame_size.mask(screen_size.reciprocal());
    const float max_ratio = (std::max)(screen_to_frame.x(), screen_to_frame.y());

    const Vec2f size = screen_size * max_ratio;
    const Vec2f offset = (frame_size - size) * 0.5f;

    pos = (pos * max_ratio) + offset;
  }

  return pos;
}

void Renderer::submitFramebuffer(const GUID& guid, const FramebufferInfo& framebuffer_info) {
  if (const auto it = _framebuffers.find(guid); it != _framebuffers.end()) {
    it->second->update(framebuffer_info);
    it->second->setActiveFlag();
  } else {
    std::unique_ptr<Framebuffer>& framebuffer =
        _framebuffers.emplace(guid, createFramebuffer(guid, framebuffer_info)).first->second;
    framebuffer->setActiveFlag();
  }
  if (0 <= framebuffer_info.blit_priority) {
    _blitting_framebuffers.emplace(framebuffer_info.blit_priority, guid);
  }
}

void Renderer::submitCamera(const Transform2D& transform, const Camera2D& camera) {
  _cameras.emplace_back(transform.inverseMatrix3D(), Vec3f(transform.translation(), 0.f),
                        Vec2f::ones(), camera.mask, camera.framebuffer, 0.01f, 1000.f, 0.f, false);
}

void Renderer::submitCamera(const Transform3D& transform, const Camera3D& camera) {
  _cameras.emplace_back(transform.inverseMatrix(), transform.translation(), Vec2f::ones(),
                        camera.mask, camera.framebuffer, camera.near, camera.far, camera.fov,
                        camera.perspective);
}

void Renderer::submitMaterial(const GUID& guid, Resources& resources) {
  Material* material;
  if (const auto it = _materials.find(guid); it != _materials.end()) {
    material = &it->second;
  } else {
    material = &_materials.emplace(guid, guid).first->second;
  }
  material->checkAndApplyResourceUpdates(resources);
}

void Renderer::submitLight(const LightInfo& light) {
  _lights.emplace_back(light);
}

void Renderer::submit(const CameraMask& mask,
                      const GUID& material,
                      const std::set<ShaderInput>& input_overrides,
                      const GUID& mesh,
                      const Mat4f& model,
                      const Entity& entity) {
  _cam_draw_calls[mask][material][input_overrides][mesh].emplace_back(model, entity);
}

void Renderer::submit(int stage,
                      const GUID& framebuffer,
                      const DrawOptions& options,
                      const GUID& shader,
                      const std::set<ShaderInput>& inputs,
                      const GUID& mesh,
                      const CommonPerInstanceData& cpid) {
  _draw_calls[stage][framebuffer][options][shader][inputs][mesh].push_back(cpid);
  _mesh_update_queue.emplace(mesh);
  _shader_update_queue.emplace(shader);

  for (const ShaderInput& input : inputs) {
    input.submitted(*this);
  }
}

void Renderer::preRender(Session& session, Resources& resources) {
  float time = session.time();
  Mat4f projection;
  DrawOptions draw_options;
  std::set<ShaderInput> inputs;
  CommonPerInstanceData cpid;

  const auto process = [&](const CameraInfo& cam_info, const Material& material,
                           const auto& mesh_models) {
    const GUID& shader = material.getShader();
    int stage = material.getStage();
    for (const auto& [mesh, models] : mesh_models) {
      for (const auto& [model, entity] : models) {
        cpid.mvp_transform = projection * cam_info.view * model;
        cpid.model_transform = model;
        cpid.entity = entity;
        submit(stage, cam_info.fb, draw_options, shader, inputs, mesh, cpid);
      }
    }
  };

  for (const CameraInfo& cam_info : _cameras) {
    if (const auto it = _framebuffers.find(cam_info.fb); it != _framebuffers.end()) {
      projection = cam_info.projection(*(it->second));
      for (const auto& [draw_mask, draw_calls] : _cam_draw_calls) {
        if ((cam_info.mask & draw_mask) != CameraMask::empty()) {
          for (const auto& [mat_id, inputs_mesh_models] : draw_calls) {
            if (const auto& mat_opt = getMaterial(mat_id); mat_opt.has_value()) {
              const Material& material = mat_opt.value();
              draw_options.depth_test = material.getDepthTest();

              for (const auto& [input_overrides, mesh_models] : inputs_mesh_models) {
                inputs = input_overrides;

                const auto& mat_inputs = material.getInputs();
                inputs.insert(mat_inputs.begin(), mat_inputs.end());

                inputs.insert(ShaderInput("projection_transform", projection));
                inputs.insert(ShaderInput("time", time));

                if (std::string_view view_input_name = material.getViewInputName();
                    !view_input_name.empty()) {
                  inputs.insert(
                      ShaderInput(std::string(view_input_name) + "_position", cam_info.position));
                  inputs.insert(
                      ShaderInput(std::string(view_input_name) + "_transform", cam_info.view));
                }

                if (std::string_view light_pass_input_name = material.getLightPassInputName();
                    !light_pass_input_name.empty()) {
                  auto light_input_it = inputs.end();
                  for (const LightInfo& light : _lights) {
                    if (light_input_it != inputs.end()) {
                      inputs.erase(light_input_it);
                    }
                    light_input_it =
                        inputs.emplace(ShaderInput(light_pass_input_name, light)).first;

                    process(cam_info, material, mesh_models);
                  }
                } else {
                  process(cam_info, material, mesh_models);
                }
              }
            }
          }
        }
      }
    }
  }

  for (const GUID& guid : _tileset_update_queue) {
    auto it = _tilesets.find(guid);
    if (it == _tilesets.end()) {
      if (const auto& res = resources.getOrCreate<TilesetResource>(guid); res.has_value()) {
        it = _tilesets.emplace(guid, guid).first;
      }
    }
    if (it != _tilesets.end()) {
      it->second.update(resources);
      queueTextureUpdate(it->second.getTextureProps());
    }
  }
  _tileset_update_queue.clear();

  for (const GUID& guid : _texture_update_queue) {
    if (!_textures.contains(guid)) {
      if (const auto& res = resources.getOrCreate<TexturePropsResource>(guid); res.has_value()) {
        if (Texture* tex = createTexture(res.value()); nullptr != tex) {
          _textures.emplace(guid, tex);
        }
      }
    }
  }
  _texture_update_queue.clear();

  for (const GUID& guid : _mesh_update_queue) {
    auto it = _meshes.find(guid);
    if (it == _meshes.end()) {
      if (const auto& res = resources.getOrCreate<MeshResourceInterface>(guid); res.has_value()) {
        if (std::unique_ptr<Mesh> mesh = createMesh(guid); nullptr != mesh) {
          it = _meshes.emplace(guid, mesh.release()).first;
        }
      }
    }
    if (it != _meshes.end()) {
      it->second->checkAndApplyResourceUpdates(resources);
    }
  }
  _mesh_update_queue.clear();

  for (const GUID& guid : _shader_update_queue) {
    auto it = _shaders.find(guid);
    if (it == _shaders.end()) {
      if (const auto& res = resources.getOrCreate<ShaderResourceInterface>(guid); res.has_value()) {
        if (std::unique_ptr<Shader> shader = createShader(res.value()); nullptr != shader) {
          it = _shaders.emplace(guid, shader.release()).first;
        }
      }
    }
    if (it != _shaders.end()) {
      it->second->checkAndApplyResourceUpdates(resources);
    }
  }
  _shader_update_queue.clear();

  FramebufferInfo render_info;
  render_info.render_targets[0].clear_colour = Vec4f::ones();
  render_info.size = _render_size;
  render_info.num_targets = 1;
  submitFramebuffer(GUID::null, render_info);

  std::erase_if(_framebuffers, [](const auto& pair) {
    return (GUID::null != pair.first) && (!pair.second->getActiveFlag());
  });
  for (const auto& [guid, framebuffer] : _framebuffers) {
    framebuffer->preRender();
  }
}

void Renderer::render() {
  for (const auto& [stage, fb_options_shader_inputs_mesh_cpids] : _draw_calls) {
    for (const auto& [fb_guid, options_shader_inputs_mesh_cpids] :
         fb_options_shader_inputs_mesh_cpids) {
      if (const auto it = _framebuffers.find(fb_guid); it != _framebuffers.end()) {
        it->second->bind();
      } else {
        continue;
      }

      for (const auto& [options, shader_inputs_mesh_cpids] : options_shader_inputs_mesh_cpids) {
        useDrawOptions(options);
        for (const auto& [shader_id, inputs_mesh_cpids] : shader_inputs_mesh_cpids) {
          if (const auto shader_it = _shaders.find(shader_id); shader_it != _shaders.end()) {
            shader_it->second->use();
            for (const auto& [inputs, mesh_cpids] : inputs_mesh_cpids) {
              for (const ShaderInput& input : inputs) {
                input.bind(*(shader_it->second), *this);
              }
              for (const auto& [mesh_id, cpids] : mesh_cpids) {
                if (const auto mesh_it = _meshes.find(mesh_id); mesh_it != _meshes.end()) {
                  drawMeshInstances(*(mesh_it->second), cpids);
                }
              }
            }
          }
        }
      }
    }
  }

  Framebuffer& main_framebuffer = *_framebuffers.at(GUID::null);
  if (!_blitting_framebuffers.empty()) {
    main_framebuffer.bind();

    Ptr<Shader> default_shader = getDefaultShader();
    default_shader->use();

    Ptr<Mesh> unit_square = getUnitSquare();
    CommonPerInstanceData cpid[1];
    cpid[0].mvp_transform = Mat4f::scale(2.f, -2.f, 2.f);
    cpid[0].model_transform = Mat4f::ones();

    for (const auto& [priority, guid] : _blitting_framebuffers) {
      if (const auto it = _framebuffers.find(guid); it != _framebuffers.end()) {
        if (const auto tex_opt = it->second->texture(0); tex_opt.has_value()) {
          ShaderInput("colour", Vec4f::ones()).bind(*default_shader, *this);
          ShaderInput("tex", TextureRef(tex_opt.value().get().guid(), -1))
              .bind(*default_shader, *this);
          drawMeshInstances(*unit_square, cpid);
        }
      }
    }
  }

  copyToScreen(main_framebuffer);

  _lights.clear();
  _cameras.clear();
  _cam_draw_calls.clear();
  _draw_calls.clear();
  _blitting_framebuffers.clear();
}

void Renderer::drawDebugLine(const Vec2f& a, const Vec2f& b, const CameraMask& mask) {
  Transform2D transform;
  {
    Transform2D::Accessor accessor = transform.modify();
    accessor.translation() = (a + b) * 0.5f;
    accessor.rotation() = (b - a).atan2();
    accessor.scale().x() = (b - a).norm();
    accessor.scale().y() = 0.1f;
  }
  // submit(mask, 1000, getDefaultShader()->guid(),
  //       {ShaderInput("colour", Vec4f(1.f, 0.f, 0.f, 1.f)), ShaderInput("tex", TextureRef())},
  //       getUnitSquare()->guid(), transform.matrix3D() * Mat4f::translation(0.f, 0.f, -0.1f));
}

void Renderer::drawDebugRect(const Vec2f& a, const Vec2f& b, const CameraMask& mask) {
  Transform2D transform;
  {
    Transform2D::Accessor accessor = transform.modify();
    accessor.translation() = (a + b) * 0.5f;
    accessor.scale() = (b - a);
  }
  // submit(mask, 1000, getDefaultShader()->guid(),
  //       {ShaderInput("colour", Vec4f(1.f, 0.f, 0.f, 1.f)), ShaderInput("tex", TextureRef())},
  //       getUnitSquare()->guid(), transform.matrix3D() * Mat4f::translation(0.f, 0.f, -0.1f));
}

Renderer* Renderer::create(Resources& resources) {
  return new GL3Renderer(resources);
}