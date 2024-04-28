#include "core/session.hpp"
#include "resources/resource.hpp"

using namespace pancake;

int main(int argc, const char* argv[]) {
  SessionConfig config;
  config.parse(argc, argv);

  if (const auto* rule = config.getRule<ResourcePathsRule>(); nullptr != rule) {
    Resource::setLoadPaths(rule->resourcePaths());
  }

  Session* session = Session::create(std::move(config));
  session->run();
  delete session;

  return 0;
}