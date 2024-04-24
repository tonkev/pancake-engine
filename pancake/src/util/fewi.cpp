#include "util/fewi.hpp"

#include <time.h>
#include <iomanip>
#include <iostream>

using namespace pancake;

FEWI::Message::Message(Severity severity, std::string_view message, std::source_location location)
    : severity(severity), message(message), location(location) {}

FEWI::MessageStream::MessageStream(Severity severity, std::source_location location)
    : _severity(severity), _location(location) {}

FEWI::MessageStream::~MessageStream() {
  FEWI::get().log(_severity, view(), _location);
}

std::ostream& operator<<(std::ostream& os, const FEWI::Message& message) {
  switch (message.severity) {
    case FEWI::Severity::Fatal:
      os << "[FATAL]";
      break;
    case FEWI::Severity::Error:
      os << "[ERROR]";
      break;
    case FEWI::Severity::Warning:
      os << "[WARN]";
      break;
    case FEWI::Severity::Info:
      os << "[INFO]";
      break;
  }

  time_t t = time(nullptr);
  tm local_tm;
#ifdef _WIN32
  localtime_s(&local_tm, &t);
#endif
#ifdef __linux__
  localtime_r(&t, &local_tm);
#endif

  os << " " << std::put_time(&local_tm, "%F %T : ");

#ifndef NDEBUG
  os << message.location.file_name() << " : " << message.location.function_name() << " : "
     << message.location.line() << " : ";
#endif

  os << message.message << std::endl;

  return os;
}

FEWI::FEWI() {}
FEWI::~FEWI() {}

void FEWI::log(Severity severity, std::string_view message, std::source_location location) {
  if (severity < _max_severity) {
    _max_severity = severity;
  }

  Message& fewi_message = _messages.emplace_back(severity, message, location);
  std::cerr << fewi_message;
}

FEWI::Severity FEWI::max_severity() const {
  return _max_severity;
}

FEWI& FEWI::get() {
  static FEWI fewi;
  return fewi;
}

void FEWI::fatal(std::string_view message, std::source_location location) {
  get().log(Severity::Fatal, message, location);
}
void FEWI::error(std::string_view message, std::source_location location) {
  get().log(Severity::Error, message, location);
}
void FEWI::warn(std::string_view message, std::source_location location) {
  get().log(Severity::Warning, message, location);
}
void FEWI::info(std::string_view message, std::source_location location) {
  get().log(Severity::Info, message, location);
}

FEWI::MessageStream FEWI::fatal(std::source_location location) {
  return MessageStream(Severity::Fatal, location);
}
FEWI::MessageStream FEWI::error(std::source_location location) {
  return MessageStream(Severity::Error, location);
}
FEWI::MessageStream FEWI::warn(std::source_location location) {
  return MessageStream(Severity::Warning, location);
}
FEWI::MessageStream FEWI::info(std::source_location location) {
  return MessageStream(Severity::Info, location);
}