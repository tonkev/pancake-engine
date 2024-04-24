#include "gl3/gl3_sdl3_window.hpp"

#include "util/fewi.hpp"

#include "GL/gl3w.h"
#include "SDL3/SDL.h"

#if defined(PANCAKE_ENABLE_IMGUI)
#include "imgui/imgui.h"

#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#endif

using namespace pancake;

GL3SDL3Window::GL3SDL3Window() : SDL3Window(SDL_WINDOW_OPENGL), _gl_context(nullptr) {
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  _gl_context = SDL_GL_CreateContext(_window);
  if (!_gl_context) {
    FEWI::fatal(std::string("Failed to initialise SDL GL Context! : ") + SDL_GetError());
  } else if (gl3wInit()) {
    FEWI::fatal(std::string("Failed to initialise gl3w! : ") + SDL_GetError());
  }

#if defined(PANCAKE_ENABLE_IMGUI)
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui_ImplSDL3_InitForOpenGL(_window, _gl_context);
  ImGui_ImplOpenGL3_Init();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
#endif
}

GL3SDL3Window::~GL3SDL3Window() {
#if defined(PANCAKE_ENABLE_IMGUI)
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
#endif

  SDL_GL_DeleteContext(_gl_context);
}

void GL3SDL3Window::newImGuiFrame() {
#if defined(PANCAKE_ENABLE_IMGUI)
  ImGui::EndFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
#endif
}

void GL3SDL3Window::flip() {
#if defined(PANCAKE_ENABLE_IMGUI)
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

  SDL_GL_SwapWindow(_window);
}