#include "components/ui.hpp"

using namespace pancake;

UIContainer::AnchoringType UIContainer::getAnchoringType(int axis) const {
  return ((0 == axis) ? anchoring_x : anchoring_y);
}

UIContainer::PositioningType UIContainer::getPositioningType(int axis) const {
  return ((0 == axis) ? positioning_x : positioning_y);
}

UIContainer::SizingType UIContainer::getSizingType(int axis) const {
  return ((0 == axis) ? sizing_x : sizing_y);
}