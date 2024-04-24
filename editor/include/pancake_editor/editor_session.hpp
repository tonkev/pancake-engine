#pragma once

#include "pancake/core/session.hpp"
#include "pancake/ecs/components.hpp"

#include <concepts>

namespace pancake_editor {
class BaseEditorSession : public pancake::Session {
 protected:
  using pancake::Session::Session;
  virtual ~BaseEditorSession() = default;

  virtual void registerComponents() const override;
  virtual void configure() override final;
};

template <typename T>
concept Sessionable = std::is_base_of_v<pancake::Session, T>;

template <Sessionable T>
class EditorSession : public BaseEditorSession {
 protected:
  using BaseEditorSession::BaseEditorSession;
  virtual ~EditorSession() = default;

  virtual void registerComponents() const override {
    pancake::Components::get().add(T::getComponentTypeDescs());
    BaseEditorSession::registerComponents();
  }
};
}  // namespace pancake_editor