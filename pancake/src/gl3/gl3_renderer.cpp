#include "gl3/gl3_renderer.hpp"

#include "gl3/gl3_framebuffer.hpp"
#include "gl3/gl3_mesh.hpp"
#include "gl3/gl3_shader.hpp"
#include "graphics/atlassed_texture.hpp"
#include "resources/gl3_shader_resource.hpp"
#include "resources/image_resource.hpp"
#include "resources/resources.hpp"
#include "resources/text_resource.hpp"
#include "util/matrix.hpp"

#include "GL/gl3w.h"

#include <algorithm>

using namespace pancake;

GL3Renderer::GL3Renderer(Resources& resources)
    : _next_texture_slot(0), _texture_slots(16, GUID::null) {
  glGenBuffers(1, &_instance_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _instance_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Mat4f) * 100, nullptr, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  const Vertex vertices[] = {
      {Vec4f(-0.5f, 0.5f, 0.0f, 0.0f), Vec4f(0.0f, 0.0f, 1.0f, 0.0f), Vec4f(1.0f, 0.0f, 0.0f, 0.0f),
       Vec4f::ones(), Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f)},
      {Vec4f(0.5f, 0.5f, 0.0f, 0.0f), Vec4f(0.0f, 0.0f, 1.0f, 0.0f), Vec4f(1.0f, 0.0f, 0.0f, 0.0f),
       Vec4f::ones(), Vec2f(1.0f, 0.0f), Vec2f(1.0f, 0.0f)},
      {Vec4f(-0.5f, -0.5f, 0.0f, 0.0f), Vec4f(0.0f, 0.0f, 1.0f, 0.0f),
       Vec4f(1.0f, 0.0f, 0.0f, 0.0f), Vec4f::ones(), Vec2f(0.0f, 1.0f), Vec2f(0.0f, 1.0f)},
      {Vec4f(0.5f, -0.5f, 0.0f, 0.0f), Vec4f(0.0f, 0.0f, 1.0f, 0.0f), Vec4f(1.0f, 0.0f, 0.0f, 0.0f),
       Vec4f::ones(), Vec2f(1.0f, 1.0f), Vec2f(1.0f, 1.0f)},
  };
  const unsigned int indices[] = {0, 1, 2, 1, 2, 3};
  _meshes.emplace(GUID::null, createMesh(GUID::null, vertices, indices).release());

  TexturePropsResource blank_props("", GUID::null);
  if (std::optional<std::reference_wrapper<ImageResource>> blank_img_opt =
          resources.getOrCreate<ImageResource>("blank.png", false);
      blank_img_opt.has_value()) {
    ImageResource tmp("", 8, 8, 4, GUID::null);
    tmp.fill(static_cast<char>(255), static_cast<char>(255), static_cast<char>(255),
             static_cast<char>(255));

    ImageResource& blank_img = blank_img_opt->get();
    blank_img.swapContent(tmp);

    blank_props.setSourceImage(blank_img.guid());
  }
  _textures.emplace(GUID::null, createTexture(blank_props));

  {
    std::optional<std::reference_wrapper<TextResource>> vert_src =
        resources.getOrCreate<TextResource>("shaders/default_vert.glsl");
    std::optional<std::reference_wrapper<TextResource>> frag_src =
        resources.getOrCreate<TextResource>("shaders/default_frag.glsl");
    std::optional<std::reference_wrapper<GL3ShaderResource>> shader_src_opt =
        resources.getOrCreate<GL3ShaderResource>("shaders/default.glsl_shader", false);
    if (vert_src.has_value() && frag_src.has_value() && shader_src_opt.has_value()) {
      GL3ShaderResource& shader_src = shader_src_opt.value();
      shader_src.setVertexSourceGuid(vert_src.value().get().guid());
      shader_src.setFragmentSourceGuid(frag_src.value().get().guid());
      shader_src.ensureUpdated(resources);
      _shaders.emplace(GUID::null, createShader(shader_src));
    }
  }

  {
    std::optional<std::reference_wrapper<TextResource>> vert_src =
        resources.getOrCreate<TextResource>("shaders/gbuffer_vert.glsl");
    std::optional<std::reference_wrapper<TextResource>> frag_src =
        resources.getOrCreate<TextResource>("shaders/gbuffer_frag.glsl");
    std::optional<std::reference_wrapper<GL3ShaderResource>> shader_src_opt =
        resources.getOrCreate<GL3ShaderResource>("shaders/gbuffer.glsl_shader", false);
    if (vert_src.has_value() && frag_src.has_value() && shader_src_opt.has_value()) {
      GL3ShaderResource& shader_src = shader_src_opt.value();
      shader_src.setVertexSourceGuid(vert_src.value().get().guid());
      shader_src.setFragmentSourceGuid(frag_src.value().get().guid());
      shader_src.ensureUpdated(resources);
      _shaders.emplace(shader_src.guid(), createShader(shader_src));
    }
  }
}

GL3Renderer::~GL3Renderer() {
  //_image_atlas->image().save();
  glDeleteBuffers(1, &_instance_vbo);
}

GL3Renderer::AtlasInfo GL3Renderer::createAtlas() {
  AtlasInfo atlas;

  Vec2i atlas_size(4096 * 2);
  int out_width = 0;
  do {
    atlas_size = atlas_size / 2;
    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGBA32F, atlas_size.x(), atlas_size.y(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &out_width);
  } while (out_width == 0);

  TexturePropsResource tex_props("", GUID::gen());
  tex_props.setSize(atlas_size);

  atlas.image = std::make_shared<ImageAtlas>("atlas.png", atlas_size.x(), atlas_size.y(), 4);
  atlas.texture = std::make_shared<GL3Texture>(tex_props);

  return atlas;
}

std::unique_ptr<Mesh> GL3Renderer::createMesh(const GUID& guid) {
  return std::unique_ptr<Mesh>(new GL3Mesh(guid, _instance_vbo));
}

std::unique_ptr<Mesh> GL3Renderer::createMesh(const GUID& guid,
                                              std::span<const Vertex> vertices,
                                              std::span<const unsigned int> indices) {
  return std::unique_ptr<Mesh>(new GL3Mesh(guid, vertices, indices, _instance_vbo));
}

std::unique_ptr<Shader> GL3Renderer::createShader(const ShaderResourceInterface& res) {
  return std::unique_ptr<Shader>(new GL3Shader(res));
}

void GL3Renderer::useDrawOptions(const DrawOptions& options) {
  if (options.depth_test) {
    glEnable(GL_DEPTH_TEST);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
}

int GL3Renderer::bindTexture(const Texture& texture) {
  const GUID binding_guid = texture.bindingGuid();
  for (int i = 0; i < _texture_slots.size(); ++i) {
    if (_texture_slots[i] == binding_guid) {
      texture.bind(i);
      return i;
    }
  }

  int slot = _next_texture_slot;
  _texture_slots[slot] = binding_guid;
  texture.bind(slot);

  _next_texture_slot = (_next_texture_slot + 1) % _texture_slots.size();

  return slot;
}

Ptr<Mesh> GL3Renderer::getUnitSquare() const {
  return _meshes.at(GUID::null);
}

const Texture& GL3Renderer::getBlankTexture() const {
  return *_textures.at(GUID::null);
}

Ptr<Shader> GL3Renderer::getDefaultShader() const {
  return _shaders.at(GUID::null);
}

Texture* GL3Renderer::createTexture(const TexturePropsResource& texture_props) {
  if (_atlas_infos.empty()) {
    _atlas_infos.push_back(createAtlas());
  }

  AtlasInfo& last_atlas = _atlas_infos.back();
  return new AtlassedTexture(last_atlas.texture, last_atlas.image, texture_props);
}

std::unique_ptr<Framebuffer> GL3Renderer::createFramebuffer(const GUID& guid,
                                                            const FramebufferInfo& info) {
  return std::make_unique<GL3Framebuffer>(guid, info);
}

void GL3Renderer::drawMeshInstances(const Mesh& mesh,
                                    std::span<const CommonPerInstanceData> cpids) {
  glBindBuffer(GL_ARRAY_BUFFER, _instance_vbo);
  size_t iterations = static_cast<size_t>(std::ceil(cpids.size() / 100.f));
  for (size_t i = 0; i < (iterations * 100); i += 100) {
    size_t elements = ((cpids.size() - i) < 100) ? (cpids.size() - i) : 100;
    glBufferData(GL_ARRAY_BUFFER, sizeof(CommonPerInstanceData) * elements, &cpids[i],
                 GL_DYNAMIC_DRAW);
    mesh.draw(static_cast<unsigned int>(elements));
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GL3Renderer::copyToScreen(Framebuffer& framebuffer) {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, static_cast<GL3Framebuffer&>(framebuffer).id());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

  const Vec2f framebuffer_size = framebuffer.getSize();
  const Vec2i& screen_size = screenSize();

  const Vec2f frame_to_screen = Vec2f(screen_size).mask(framebuffer_size.reciprocal());
  const float min_ratio = (std::min)(frame_to_screen.x(), frame_to_screen.y());

  const Vec2i dest_size = framebuffer_size * min_ratio;
  const Vec2i dest_offset = (screen_size - dest_size) / 2;

  glClearColor(0.f, 0.f, 0.f, 0.f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBlitFramebuffer(0, 0, static_cast<int>(framebuffer_size.x()),
                    static_cast<int>(framebuffer_size.y()), dest_offset.x(), dest_offset.y(),
                    dest_offset.x() + dest_size.x(), dest_offset.y() + dest_size.y(),
                    GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void GL3Renderer::blit(Framebuffer& dst, const Framebuffer& src) {
  static_cast<GL3Framebuffer&>(dst).blit(static_cast<const GL3Framebuffer&>(src));
}

void GL3Renderer::preRender(Session& session, Resources& resources) {
  Renderer::preRender(session, resources);

  for (AtlasInfo& atlas : _atlas_infos) {
    const uint64_t prev_gen = atlas.image->gen();
    atlas.image->update(resources);
    if (prev_gen != atlas.image->gen()) {
      atlas.texture->update(atlas.image->image());

      const auto& rejected_images = atlas.image->getRejectedImages();
      if (!rejected_images.empty()) {
        AtlasInfo atlas = createAtlas();
        _atlas_infos.push_back(atlas);

        for (auto& [_, tex] : _textures) {
          for (const GUID& guid : rejected_images) {
            if (tex->imageGuid() == guid) {
              static_cast<AtlassedTexture&>(*tex).setAtlas(atlas.texture, atlas.image);
            }
          }
        }
      }
    }
  }

  for (auto& [_, tex] : _textures) {
    tex->update(resources);
  }

  for (const auto& [guid, shader_ptr] : _shaders) {
    GL3Shader& shader = static_cast<GL3Shader&>(*shader_ptr.get());
    shader.checkAndApplyResourceUpdates(resources);
  }
}

void GL3Renderer::render() {
  Renderer::render();
}