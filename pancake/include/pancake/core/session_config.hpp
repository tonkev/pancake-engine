#pragma once

#include "util/containers.hpp"
#include "util/type_id.hpp"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace pancake {
class CmdLineOptions {
 public:
  CmdLineOptions(int argc, const char* const* argv);

  const char* consume();

 private:
  int _argi;
  int _argc;
  const char* const* _argv;
};

class SessionConfig;
class SessionConfigRule {
 public:
  using Creators = std::unordered_map<std::reference_wrapper<const TypeId>,
                                      SessionConfigRule* (*)(),
                                      HashRefWrapperByAddr<const TypeId>,
                                      EqualToRefWrapperByAddr<const TypeId>>;

  template <typename T>
  class StaticAdder {
   private:
    static SessionConfigRule* creator() { return new T(); }

   public:
    StaticAdder() { getCreators().emplace(TypeId::get<T>(), creator); }
  };

  virtual ~SessionConfigRule() = default;

  virtual void operator()(CmdLineOptions& options, std::string_view option) = 0;

  virtual const std::set<std::string>& getOptions() const = 0;

 private:
  static Creators& getCreators();

  friend SessionConfig;
};

template <class T>
concept SessionConfigRuleable = std::is_base_of_v<SessionConfigRule, T>;

class SessionConfig {
 public:
  SessionConfig();
  ~SessionConfig();

  void parse(int argc, const char* const* argv);

  template <SessionConfigRuleable Rule>
  const Rule* getRule() const {
    if (const auto it = _rules.find(TypeId::get<Rule>()); it != _rules.end()) {
      return static_cast<const Rule*>(&(*(it->second)));
    }
    return nullptr;
  }

 private:
  std::unordered_map<std::reference_wrapper<const TypeId>,
                     SessionConfigRule*,
                     HashRefWrapperByAddr<const TypeId>,
                     EqualToRefWrapperByAddr<const TypeId>>
      _rules;
  std::map<std::string, SessionConfigRule*, std::less<>> _option_rules;
};

class LogSystemGraphsRule : public SessionConfigRule {
 public:
  virtual ~LogSystemGraphsRule() = default;
  virtual void operator()(CmdLineOptions& options, std::string_view option) override;
  virtual const std::set<std::string>& getOptions() const override;
  bool value() const;

 private:
  bool _value = false;
};

class ResourcePathsRule : public SessionConfigRule {
 public:
  virtual ~ResourcePathsRule() = default;
  virtual void operator()(CmdLineOptions& options, std::string_view option) override;
  virtual const std::set<std::string>& getOptions() const override;
  const std::vector<std::string>& resourcePaths() const;

 private:
  std::vector<std::string> _resource_paths;
};
}  // namespace pancake