#include "core/session_config.hpp"

#include "util/fewi.hpp"

#include <map>

using namespace pancake;

void SessionConfig::parse(int argc, const char* const* argv) {
  CmdLineOptions options(argc, argv);

  std::string_view option = options.consume();
  while (!option.empty()) {
    const CmdLineOptionRulePtr rule = getRule(option);

    if (nullptr == rule) {
      FEWI::warn() << "Unrecognised option : " << option;
      break;
    }

    (*rule)(options, *this, option);

    option = options.consume();
  }
}

SessionConfig::CmdLineOptions::CmdLineOptions(int argc, const char* const* argv)
    : _argi(1), _argc(argc), _argv(argv) {}

const char* SessionConfig::CmdLineOptions::consume() {
  if (_argi < _argc) {
    return _argv[_argi++];
  }
  return "";
}

void logSystemGraphsRule(SessionConfig::CmdLineOptions& options,
                         SessionConfig& config,
                         std::string_view option) {
  config.log_system_graphs = true;
}

void resourcePathsRule(SessionConfig::CmdLineOptions& options,
                       SessionConfig& config,
                       std::string_view option) {
  std::string paths = options.consume();
  if (paths.empty() || paths.starts_with("-")) {
    FEWI::warn() << "No value found for " << option;
  } else {
    size_t prev_pos = 0;
    size_t pos;
    do {
      pos = paths.find(';', prev_pos);
      config.resource_paths.emplace_back(paths.substr(prev_pos, pos - prev_pos));
      prev_pos = pos + 1;
    } while (std::string::npos != pos);
  }
}

const SessionConfig::CmdLineOptionRulePtr SessionConfig::getRule(std::string_view option) const {
  static std::map<std::string, SessionConfig::CmdLineOptionRulePtr, std::less<>> rules{
      {"--log-system-graphs", logSystemGraphsRule}, {"--resource-paths", resourcePathsRule}};
  if (const auto it = rules.find(option); it != rules.end()) {
    return it->second;
  }
  return nullptr;
}