#include "editor_session.hpp"

#include "pancake/core/renderer.hpp"

using namespace pancake;
using namespace pancake_editor;

void BaseEditorSession::registerComponents() const {
  Session::registerComponents();
}

void BaseEditorSession::configure() {
  renderer().matchRenderSizeToScreenSize(true);
}