#include "messages/ui.hpp"

#include "ecs/messages.hpp"

using namespace pancake;

const Messages::StaticAdder<UIOnHover> ui_on_hover_adder{};

const Messages::StaticAdder<UIOnClick> ui_on_click_adder{};