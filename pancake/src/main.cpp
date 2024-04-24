#include "core/session.hpp"
#include "resources/resource.hpp"

using namespace pancake;

int main(int argc, const char* argv[]) {
  SessionConfig config;
  config.parse(argc, argv);

  Resource::setLoadPaths(config.resource_paths);

  Session* session = Session::create(config);
  session->run();
  delete session;

  return 0;
}