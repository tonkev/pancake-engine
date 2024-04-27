#include "core/session_config.hpp"

#include "util/fewi.hpp"

#include <map>

using namespace pancake;

CmdLineOptions::CmdLineOptions(int argc, const char* const* argv)
    : _argi(1), _argc(argc), _argv(argv) {}

const char* CmdLineOptions::consume() {
  if (_argi < _argc) {
    return _argv[_argi++];
  }
  return "";
}

SessionConfigRule::Creators& SessionConfigRule::getCreators() {
  static Creators creators;
  return creators;
}

SessionConfig::SessionConfig() {
  for (const auto& [id, creator] : SessionConfigRule::getCreators()) {
    const auto it = _rules.emplace(id, creator());
    if (it.second) {
      for (const std::string& option : it.first->second->getOptions()) {
        _option_rules.emplace(option, it.first->second);
      }
    }
  }
}

SessionConfig::~SessionConfig() {
  for (const auto& [_, rule] : _rules) {
    delete rule;
  }
}

void SessionConfig::parse(int argc, const char* const* argv) {
  CmdLineOptions options(argc, argv);

  std::string_view option = options.consume();
  while (!option.empty()) {
    if (auto rule_it = _option_rules.find(option); rule_it != _option_rules.end()) {
      (*(rule_it->second))(options, option);
    } else {
      FEWI::warn() << "Unrecognised option : " << option;
    }

    option = options.consume();
  }
}

void LogSystemGraphsRule::operator()(CmdLineOptions& options, std::string_view option) {
  _value = true;
}

const std::set<std::string>& LogSystemGraphsRule::getOptions() const {
  static const std::set<std::string> options{"--log-system-graphs"};
  return options;
}

bool LogSystemGraphsRule::value() const {
  return _value;
}

void ResourcePathsRule::operator()(CmdLineOptions& options, std::string_view option) {
  std::string paths = options.consume();
  if (paths.empty() || paths.starts_with("-")) {
    FEWI::warn() << "No value found for " << option;
  } else {
    size_t prev_pos = 0;
    size_t pos;
    do {
      pos = paths.find(';', prev_pos);
      _resource_paths.emplace_back(paths.substr(prev_pos, pos - prev_pos));
      prev_pos = pos + 1;
    } while (std::string::npos != pos);
  }
}

const std::set<std::string>& ResourcePathsRule::getOptions() const {
  static const std::set<std::string> options{"--resource-paths"};
  return options;
}

const std::vector<std::string>& ResourcePathsRule::resourcePaths() const {
  return _resource_paths;
}

SessionConfigRule::StaticAdder<LogSystemGraphsRule> _log_system_graphs_rule_adder;
SessionConfigRule::StaticAdder<ResourcePathsRule> _resource_paths_rule_adder;