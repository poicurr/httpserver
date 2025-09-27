#pragma once

#include <map>
#include <string>

namespace httpserver {

struct HttpRequestHeader {
  std::string method;
  std::string path;
  std::string version;
  std::map<std::string, std::string> headers;
};

struct HttpRequest {
  HttpRequestHeader header;
  std::string body;
};

} // namespace httpserver
