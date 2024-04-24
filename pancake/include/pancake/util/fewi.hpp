#pragma once

#include <iostream>
#include <list>
#include <source_location>
#include <sstream>
#include <string>

namespace pancake {
class FEWI {
 public:
  enum class Severity { Fatal, Error, Warning, Info };

  struct Message {
   public:
    Message(Severity severity, std::string_view message, std::source_location location);
    ~Message() = default;

    Severity severity;
    std::string message;
    std::source_location location;
  };

  class MessageStream : public std::ostringstream {
   public:
    MessageStream(Severity severity, std::source_location location);
    ~MessageStream();

   private:
    Severity _severity;
    std::source_location _location;
  };

  ~FEWI();

  void log(Severity severity, std::string_view message, std::source_location location);

  Severity max_severity() const;

  static FEWI& get();

  static void fatal(std::string_view message,
                    std::source_location location = std::source_location::current());
  static void error(std::string_view message,
                    std::source_location location = std::source_location::current());
  static void warn(std::string_view message,
                   std::source_location location = std::source_location::current());
  static void info(std::string_view message,
                   std::source_location location = std::source_location::current());

  static MessageStream fatal(std::source_location location = std::source_location::current());
  static MessageStream error(std::source_location location = std::source_location::current());
  static MessageStream warn(std::source_location location = std::source_location::current());
  static MessageStream info(std::source_location location = std::source_location::current());

 private:
  FEWI();

  std::list<Message> _messages;
  Severity _max_severity = Severity::Info;
};
}  // namespace pancake

std::ostream& operator<<(std::ostream& os, const pancake::FEWI::Message& message);