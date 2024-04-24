#pragma once

#include <string>
#include <vector>

namespace pancake {
struct SessionConfig {
 public:
  class CmdLineOptions {
   public:
    CmdLineOptions(int argc, const char* const* argv);

    const char* consume();

   private:
    int _argi;
    int _argc;
    const char* const* _argv;
  };

  using CmdLineOptionRulePtr = void (*)(CmdLineOptions& options,
                                        SessionConfig& config,
                                        std::string_view option);

  void parse(int argc, const char* const* argv);

 protected:
  const CmdLineOptionRulePtr getRule(std::string_view option) const;

 public:
  bool log_system_graphs = false;
  std::vector<std::string> resource_paths{};
};
}  // namespace pancake