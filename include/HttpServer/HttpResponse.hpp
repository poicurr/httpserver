#pragma once

#include <string>

namespace httpserver {

struct HttpResponse {
  std::string message;
  std::string mimetype;
  std::string body;
};

} // namespace httpserver
