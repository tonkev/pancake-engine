#include "resources/resources.hpp"

#include "resources/text_resource.hpp"
#include "util/componentify_json.hpp"
#include "util/jsonify_component.hpp"

#include <algorithm>
#include <sstream>

using namespace pancake;

Resources::BaseCreator::BaseCreator(Resource::Type type) {
  getCreators().emplace(type, *this);
}

bool Resources::SlashInvariantLess::operator()(const std::string& lhs,
                                               const std::string& rhs) const {
  const size_t lhs_size = lhs.size();
  const size_t rhs_size = rhs.size();
  const size_t min_size = (lhs_size < rhs_size) ? lhs_size : rhs_size;

  for (size_t i = 0; i < min_size; ++i) {
    const char lc = lhs[i];
    const char rc = rhs[i];
    if ((lc != rc) && (!(((lc == '/') || (lc == '\\')) && ((rc == '/') || (rc == '\\'))))) {
      return lc < rc;
    }
  }

  return lhs_size < rhs_size;
}

Resources::Resources::ImportInfo::ImportInfo(const Resource& resource)
    : guid(resource.guid()), type(resource.type()) {}

Resources::Resources() {
  const TypeDesc& desc = TypeDescLibrary::get<ImportInfo>();
  ImportInfo import_info;

  TextResource import_res("import.json", GUID::null);
  import_res.load();
  std::stringstream import_stream(std::string(import_res.constText()));
  JSONObject import_obj(import_stream);
  for (const auto& [path, value] : import_obj.pairs()) {
    desc.visit(ComponentifyJSON(&import_info, *value));

    bool invalid = Resource::Type::QuakeMap < import_info.type;
    if (!invalid) {
      _import_map.emplace(path, import_info);
    }
  }
}

Resources::~Resources() {
  const TypeDesc& desc = TypeDescLibrary::get<ImportInfo>();

  JSONObject import_obj;
  for (auto& [path, info] : _import_map) {
    desc.visit(JSONifyComponent(import_obj, path, &info));
  }

  TextResource import_res("import.json", GUID::null);
  std::stringstream import_stream;

  import_obj.serialise(import_stream);
  import_res.text() = import_stream.str();

  import_res.save();
}

const Resources::ResourceMap& Resources::getResources() const {
  return _res_map;
}

Resource* Resources::getOrCreate(const std::string& path, bool load) {
  Resource* res = nullptr;
  const Creators& creators = getCreators();

  auto import_it = _import_map.find(path);
  if (const auto import_it = _import_map.find(path); (import_it != _import_map.end())) {
    const ImportInfo& import_info = import_it->second;
    if (const auto res_it = _res_map.find(import_info.guid); (res_it != _res_map.end())) {
      res = res_it->second.get();
    } else if (const auto creator_it = creators.find(import_info.type);
               (creator_it != creators.end())) {
      res = _res_map
                .emplace(import_info.guid, creator_it->second.get().create(path, import_info.guid))
                .first->second.get();
      if (load) {
        res->load();
      }
    }
  }

  return res;
}

Resource* Resources::getOrCreate(const GUID& guid) {
  Resource* res = nullptr;
  const Creators& creators = getCreators();

  if (const auto res_it = _res_map.find(guid); (res_it != _res_map.end())) {
    res = res_it->second.get();
  } else if (const auto import_it = std::ranges::find_if(
                 _import_map, [&guid](const auto& pair) { return guid == pair.second.guid; });
             (import_it != _import_map.end())) {
    const ImportInfo& import_info = import_it->second;
    if (const auto creator_it = creators.find(import_info.type); (creator_it != creators.end())) {
      res = _res_map
                .emplace(import_info.guid,
                         creator_it->second.get().create(import_it->first, import_info.guid))
                .first->second.get();
      res->load();
    }
  }

  return res;
}

Resources::Creators& Resources::getCreators() {
  static Creators creators;
  return creators;
}