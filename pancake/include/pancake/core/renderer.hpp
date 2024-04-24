#pragma once

#include "components/2d.hpp"
#include "components/3d.hpp"
#include "components/core.hpp"
#include "graphics/common.hpp"
#include "graphics/draw_options.hpp"
#include "graphics/framebuffer.hpp"
#include "graphics/light_info.hpp"
#include "graphics/material.hpp"
#include "graphics/shader_input.hpp"
#include "graphics/texture.hpp"
#include "graphics/tileset.hpp"
#include "pancake.hpp"
#include "util/matrix.hpp"

#include <map>
#include <optional>
#include <set>
#include <span>
#include <unordered_map>
#include <vector>

namespace pancake {
class AtlassedTexture;
class GUID;
class Mesh;
class Session;
class Shader;
struct Vertex;
class Resources;
class TextResource;
class TexturePropsResource;

struct CommonPerInstanceData {
  Mat4f mvp_transform = Mat4f::ones();
  Mat4f model_transform = Mat4f::ones();
  Entity entity = Entity::null;
};

class Renderer {
 public:
  Renderer();
  virtual ~Renderer() = default;

  void init();

  void queueMeshUpdate(const GUID& guid);
  void queueTextureUpdate(const GUID& guid);
  void queueTilesetUpdate(const GUID& guid);
  void queueShaderUpdate(const GUID& guid);

  OptFramebufferConstRef getFramebuffer(const GUID& guid) const;
  OptMaterialConstRef getMaterial(const GUID& guid) const;
  OptTextureConstRef getTexture(const GUID& guid) const;
  OptTilesetConstRef getTileset(const GUID& guid) const;

  std::optional<Ptr<Texture>> getTexturePtr(const GUID& guid);

  void setScreenSize(const Vec2i& size);
  void matchRenderSizeToScreenSize(bool value);

  const Vec2i& renderSize() const;
  const Vec2i& screenSize() const;

  Vec2f screenToRenderPosition(const Vec2f& screen_pos) const;

  void submitFramebuffer(const GUID& guid, const FramebufferInfo& framebuffer_info);
  void submitCamera(const Transform2D& transform, const Camera2D& camera);
  void submitCamera(const Transform3D& transform, const Camera3D& camera);
  void submitMaterial(const GUID& guid, Resources& resources);
  void submitLight(const LightInfo& light_info);

  void submit(const CameraMask& mask,
              const GUID& material,
              const std::set<ShaderInput>& input_overrides,
              const GUID& mesh,
              const Mat4f& model,
              const Entity& entity = Entity::null);

  void submit(int stage,
              const GUID& framebuffer,
              const DrawOptions& options,
              const GUID& shader,
              const std::set<ShaderInput>& inputs,
              const GUID& mesh,
              const CommonPerInstanceData& cpid);

  virtual int bindTexture(const Texture& texture) = 0;

  virtual const Texture& getBlankTexture() const = 0;
  virtual Ptr<Mesh> getUnitSquare() const = 0;
  virtual Ptr<Shader> getDefaultShader() const = 0;

  void drawDebugLine(const Vec2f& a, const Vec2f& b, const CameraMask& mask);
  void drawDebugRect(const Vec2f& a, const Vec2f& b, const CameraMask& mask);

  virtual void preRender(Session& session, Resources& resources);
  virtual void render();

  static Renderer* create(Resources& resources);

 protected:
  virtual Texture* createTexture(const TexturePropsResource& texture_props) = 0;
  virtual std::unique_ptr<Framebuffer> createFramebuffer(const GUID& guid,
                                                         const FramebufferInfo& info) = 0;

  virtual std::unique_ptr<Mesh> createMesh(const GUID& guid) = 0;
  virtual std::unique_ptr<Mesh> createMesh(const GUID& guid,
                                           std::span<const Vertex> vertices,
                                           std::span<const unsigned int> indices) = 0;
  virtual std::unique_ptr<Shader> createShader(const ShaderResourceInterface& res) = 0;

  virtual void useDrawOptions(const DrawOptions& options) = 0;

  virtual void drawMeshInstances(const Mesh& mesh,
                                 std::span<const CommonPerInstanceData> cpids) = 0;

  virtual void copyToScreen(Framebuffer& framebuffer) = 0;
  virtual void blit(Framebuffer& dst, const Framebuffer& src) = 0;

  std::unordered_map<GUID, Ptr<Texture>> _textures;
  std::unordered_map<GUID, Ptr<Mesh>> _meshes;
  std::unordered_map<GUID, Ptr<Shader>> _shaders;
  std::unordered_map<GUID, Material> _materials;

 private:
  Vec2i _render_size;
  Vec2i _screen_size;
  bool _render_match_screen;

  std::set<GUID> _mesh_update_queue;
  std::vector<GUID> _texture_update_queue;
  std::vector<GUID> _tileset_update_queue;
  std::set<GUID> _shader_update_queue;

  std::unordered_map<GUID, Tileset> _tilesets;

  std::unordered_map<GUID, std::unique_ptr<Framebuffer>> _framebuffers;
  std::set<std::pair<int, GUID>> _blitting_framebuffers;

  std::vector<LightInfo> _lights;

  struct CameraInfo {
    Mat4f view;
    Vec3f position;
    Vec2f ortho_size;
    CameraMask mask;
    GUID fb;
    float near;
    float far;
    float fov;
    bool perspective;

    Mat4f projection(const Framebuffer& framebuffer) const;
  };

  std::vector<CameraInfo> _cameras;
  std::map<
      CameraMask,
      std::unordered_map<
          GUID /* MATERIAL */,
          std::map<std::set<ShaderInput>,
                   std::unordered_map<GUID /* MESH */, std::vector<std::pair<Mat4f, Entity>>>>>>
      _cam_draw_calls;
  std::map<
      int /* STAGE */,
      std::map<GUID /* FRAMEBUFFER */,
               std::map<DrawOptions,
                        std::unordered_map<
                            GUID /* SHADER */,
                            std::map<std::set<ShaderInput>,
                                     std::unordered_map<GUID /* MESH */,
                                                        std::vector<CommonPerInstanceData>>>>>>>
      _draw_calls;
};
}  // namespace pancake