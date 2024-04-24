#include "ecs/encompassers.hpp"

#include "util/assert.hpp"

using namespace pancake;

EncompasserId Encompassers::add(const TypeId& desc) {
  if (!_encompasser_desc_ids.contains(desc)) {
    _encompasser_desc_ids.insert({desc, _encompasser_descs.size()});
    _encompasser_descs.push_back(desc);
  }
  return _encompasser_desc_ids.at(desc);
}

EncompasserId Encompassers::_getId(const TypeId& desc) const {
  ensure(_encompasser_desc_ids.contains(desc));
  return _encompasser_desc_ids.at(desc);
}

const TypeId& Encompassers::_getTypeId(EncompasserId enc_id) const {
  ensure(enc_id < _encompasser_descs.size());
  return _encompasser_descs[enc_id];
}

const Encompassers::EncompasserDescs& Encompassers::_getDescs() const {
  return _encompasser_descs;
}

bool Encompassers::_isEncompasser(const TypeId& desc) const {
  return _encompasser_desc_ids.contains(desc);
}

EncompasserId Encompassers::getId(const TypeId& desc) {
  return Encompassers::get()._getId(desc);
}

const TypeId& Encompassers::getTypeId(EncompasserId enc_id) {
  return get()._getTypeId(enc_id);
}

const Encompassers::EncompasserDescs& Encompassers::getDescs() {
  return get()._getDescs();
}

bool Encompassers::isEncompasser(const TypeId& desc) {
  return get()._isEncompasser(desc);
}

Encompassers& Encompassers::get() {
  static Encompassers encompassers;
  return encompassers;
}