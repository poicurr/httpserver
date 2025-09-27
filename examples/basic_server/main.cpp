#include <HttpServer/HttpRequest.hpp>
#include <HttpServer/HttpResponse.hpp>
#include <HttpServer/HttpServer.hpp>
#include <StringUtils/StringUtils.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using httpserver::HttpRequest;
using httpserver::HttpResponse;
using httpserver::HttpServer;

namespace {

std::filesystem::path resourceRoot() {
#ifdef HTTP_SERVER_EXAMPLE_RESOURCE_DIR
  return std::filesystem::path(HTTP_SERVER_EXAMPLE_RESOURCE_DIR);
#else
  return std::filesystem::path("resources");
#endif
}

HttpResponse makeNotFound() {
  return HttpResponse{"HTTP/1.1 404 Not Found", "text/html", ""};
}

std::string resolveMimeType(const std::filesystem::path &path) {
  const auto ext = strutil::toLower(path.extension().string());
  if (ext == ".ico")
    return "image/x-icon";
  if (ext == ".svg")
    return "image/svg+xml";
  if (ext == ".png")
    return "image/png";
  if (ext == ".js")
    return "application/javascript";
  if (ext == ".css")
    return "text/css";
  if (ext == ".html" || ext == ".htm")
    return "text/html";
  return "application/octet-stream";
}

std::filesystem::path resolveTargetPath(const std::string &uri) {
  if (uri.empty())
    return {};
  const auto base = resourceRoot();
  if (uri == "/")
    return base / "index.html";
  std::filesystem::path requested =
      std::filesystem::path(uri).lexically_normal().relative_path();
  for (const auto &part : requested) {
    if (part == "..")
      return {};
  }
  return base / requested;
}

std::string loadFile(const std::filesystem::path &path) {
  std::ifstream ifs(path, std::ios::binary);
  if (!ifs)
    return {};
  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}

std::string decodeFormBody(const std::string &body) {
  auto normalized = body;
  std::replace(normalized.begin(), normalized.end(), '+', ' ');
  return strutil::decodeURIComponent(normalized);
}

HttpResponse handleGet(const HttpRequest &request) {
  const auto target = resolveTargetPath(request.header.path);
  if (target.empty())
    return makeNotFound();
  std::error_code ec;
  const auto status = std::filesystem::status(target, ec);
  if (ec || !std::filesystem::is_regular_file(status))
    return makeNotFound();
  const auto body = loadFile(target);
  if (body.empty()) {
    const auto size = std::filesystem::file_size(target, ec);
    if (!ec && size > 0)
      return makeNotFound();
  }
  return HttpResponse{"HTTP/1.1 200 OK", resolveMimeType(target), body};
}

HttpResponse handlePost(const HttpRequest &request) {
  if (request.header.path != "/send")
    return makeNotFound();
  const auto decoded = decodeFormBody(request.body);
  std::cout << "request: " << decoded << std::endl;
  return HttpResponse{"HTTP/1.1 200 OK", "text/html",
                      strutil::encodeURIComponent(decoded)};
}

} // namespace

int main() {
  try {
    std::cout << "Server is running at http://127.0.0.1:8000" << std::endl;
    HttpServer server(8000);
    server.run([](const HttpRequest &request) {
      const auto method = strutil::toUpper(request.header.method);
      if (method == "GET")
        return handleGet(request);
      if (method == "POST")
        return handlePost(request);
      return makeNotFound();
    });
  } catch (const std::exception &ex) {
    std::cerr << "Failed to start server: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
