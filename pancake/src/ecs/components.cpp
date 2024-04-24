#include "ecs/components.hpp"

#include "util/array_type_desc.hpp"
#include "util/assert.hpp"
#include "util/dynamic_buffer_type_desc.hpp"
#include "util/type_desc_visitor.hpp"

using namespace pancake;

Components::ComponentInfo::ComponentInfo(const TypeDesc& desc) : desc(desc) {}

ComponentId Components::add(const TypeDesc& desc) {
  class FindEntityMembers : public TypeDescVisitor {
   public:
    FindEntityMembers(std::vector<size_t>& entity_member_offsets, size_t offset = 0)
        : _entity_member_offsets(entity_member_offsets), _offset(offset) {}

    virtual void operator()(const PrimitiveTypeDesc& desc) const override {}
    virtual void operator()(const ArrayTypeDesc& desc) const override {
      const TypeDesc& element_desc = desc.elementDesc();
      for (size_t i = 0; i < desc.length(); ++i) {
        element_desc.visit(
            FindEntityMembers(_entity_member_offsets, _offset + element_desc.size()));
      }
    }
    virtual void operator()(const StructTypeDesc& desc) const override {
      if (desc == TypeDescLibrary::get<Entity>()) {
        _entity_member_offsets.push_back(_offset);
      } else {
        for (const StructTypeDesc::Field& field : desc.getFields()) {
          field.desc().visit(FindEntityMembers(_entity_member_offsets, _offset + field.offset()));
        }
      }
    }
    virtual void operator()(const DynamicBufferTypeDesc& desc) const override {
      // We can't cache entity members for dynamic buffers :(
    }

   private:
    std::vector<size_t>& _entity_member_offsets;
    const size_t _offset;
  };

  if (!_component_desc_ids.contains(desc)) {
    _component_desc_ids.insert({desc, _component_infos.size()});
    ComponentInfo& info = _component_infos.emplace_back(desc);
    desc.visit(FindEntityMembers(info.entity_member_offsets));
  }
  return _component_desc_ids.at(desc);
}

void Components::add(const ComponentTypeDescs& descs) {
  for (const TypeDesc& desc : descs) {
    add(desc);
  }
}

ComponentId Components::_getId(const TypeDesc& desc) const {
  ensure(_component_desc_ids.contains(desc));
  return _component_desc_ids.at(desc);
}

const TypeDesc& Components::_getDesc(ComponentId comp_id) const {
  ensure(comp_id < _component_infos.size());
  return _component_infos[comp_id].desc;
}

bool Components::_isComponent(const TypeDesc& desc) const {
  return _component_desc_ids.contains(desc);
}

ComponentId Components::getId(const TypeDesc& desc) {
  return Components::get()._getId(desc);
}

const TypeDesc& Components::getDesc(ComponentId comp_id) {
  return get()._getDesc(comp_id);
}

bool Components::isComponent(const TypeDesc& desc) {
  return get()._isComponent(desc);
}

std::span<const size_t> Components::getEntityMemberOffsets(ComponentId comp_id) const {
  ensure(comp_id < _component_infos.size());
  return _component_infos[comp_id].entity_member_offsets;
}

Components& Components::get() {
  static Components components;
  return components;
}