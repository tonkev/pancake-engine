#include "ecs/component_view.hpp"

#include "ecs/world.hpp"
#include "util/type_desc.hpp"

using namespace pancake;

ComponentView::ComponentView(const ComponentMask& mask, World& world)
    : _mask(mask), _world(world), _valid_cache(false) {
  for (const ComponentId id : _mask) {
    const TypeDesc& desc = Components::getDesc(id);
    (void)_cache[desc];
  }
}

void ComponentView::invalidate() {
  _valid_cache = false;
}

void ComponentView::update() {
  std::scoped_lock update_lock(_update_mutex);

  if (_valid_cache) {
    return;
  }

  for (auto& [_, comps] : _cache) {
    comps.clear();
  }

  for (const auto& [arch_mask, arch] : _world.getArchetypes()) {
    if ((arch_mask & _mask) == _mask) {
      for (ArchetypeId arch_id = 0; arch_id < arch->size(); ++arch_id) {
        for (auto& [comp_desc, comps] : _cache) {
          comps.push_back(arch->getComponent(arch_id, comp_desc));
        }
      }
    }
  }

  _valid_cache = true;
}