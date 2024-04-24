#pragma once

#include "core/renderer.hpp"

#include "gl3/gl3_texture.hpp"
#include "graphics/atlassed_texture.hpp"
#include "graphics/image_atlas.hpp"
#include "pancake.hpp"
#include "resources/texture_props_resource.hpp"
#include "util/guid.hpp"

#include <unordered_map>
#include <vector>

namespace pancake {
class AtlassedTexture;
class ImageAtlas;
class GL3Mesh;
class GL3Shader;
class GL3Texture;
class GL3Renderer : public Renderer {
 public:
  GL3Renderer(Resources& resources);
  virtual ~GL3Renderer();

  virtual int bindTexture(const Texture& texture) override;

  virtual const Texture& getBlankTexture() const override;
  virtual Ptr<Mesh> getUnitSquare() const override;
  virtual Ptr<Shader> getDefaultShader() const override;

  virtual void preRender(Session& session, Resources& resources) override;
  virtual void render() override;

 protected:
  virtual void drawMeshInstances(const Mesh& mesh,
                                 std::span<const CommonPerInstanceData> cpids) override;

  virtual Texture* createTexture(const TexturePropsResource& texture_props) override;
  virtual std::unique_ptr<Framebuffer> createFramebuffer(const GUID& guid,
                                                         const FramebufferInfo& info) override;

  virtual std::unique_ptr<Mesh> createMesh(const GUID& guid) override;
  virtual std::unique_ptr<Mesh> createMesh(const GUID& guid,
                                           std::span<const Vertex> vertices,
                                           std::span<const unsigned int> indices) override;
  virtual std::unique_ptr<Shader> createShader(const ShaderResourceInterface& res) override;

  virtual void useDrawOptions(const DrawOptions& options) override;

  virtual void copyToScreen(Framebuffer& framebuffer) override;
  virtual void blit(Framebuffer& dst, const Framebuffer& src) override;

 private:
  unsigned int _instance_vbo;
  Ptr<ImageAtlas> _image_atlas;
  TexturePropsResource _atlas_texture_props;
  Ptr<GL3Texture> _atlas_texture;
};
}  // namespace pancake