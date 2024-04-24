#include "util/guid.hpp"

#include "util/type_desc_library.hpp"

#include <iomanip>
#include <random>
#include <sstream>

using namespace pancake;

const TypeDesc& GUID::DESC =
    TypeDescLibrary::get<GUID>().setName("GUID").addField("_id",
                                                          0,
                                                          TypeDescLibrary::get<uint64_t>());

const GUID GUID::null{};

GUID::GUID() : _id(0) {}

GUID GUID::gen() {
  static std::mt19937_64 engine = []() {
    std::random_device rd;
    return std::mt19937_64(rd());
  }();
  static std::uniform_int_distribution<unsigned long long> distrib(
      1, std::numeric_limits<uint64_t>::max());

  GUID guid;
  guid._id = distrib(engine);

  return guid;
}

GUID GUID::fromHex(const std::string& hex_str) {
  GUID guid;
  guid._id = std::stoul(hex_str, 0, 16);
  return guid;
}

std::string GUID::hex() const {
  std::stringstream stream;
  stream << std::hex << _id;
  return stream.str();
}
