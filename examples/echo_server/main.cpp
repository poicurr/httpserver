#include <HttpServer/HttpRequest.hpp>
#include <HttpServer/HttpResponse.hpp>
#include <HttpServer/HttpServer.hpp>
#include <StringUtils/StringUtils.hpp>

#include <iostream>
#include <string>

using httpserver::HttpRequest;
using httpserver::HttpResponse;
using httpserver::HttpServer;

namespace {

HttpResponse makeNotFound() {
  return {"HTTP/1.1 404 Not Found", "text/plain", "Not found"};
}

HttpResponse handlePost(const HttpRequest &request) {
  const auto decoded = strutil::decodeURIComponent(request.body);
  std::cout << "Echo: " << decoded << std::endl;
  return {"HTTP/1.1 200 OK", "text/plain", decoded};
}

HttpResponse handleRequest(const HttpRequest &request) {
  if (request.header.method == "POST" && request.header.path == "/echo") {
    return handlePost(request);
  }
  if (request.header.method == "GET" && request.header.path == "/") {
    const std::string help = "POST /echo with body=message to echo it back";
    return {"HTTP/1.1 200 OK", "text/plain", help};
  }
  return makeNotFound();
}

} // namespace

int main() {
  try {
    std::cout << "EchoServer example: http://127.0.0.1:8000" << std::endl;
    HttpServer server(8000);
    server.run(
        [](const HttpRequest &request) { return handleRequest(request); });
  } catch (const std::exception &ex) {
    std::cerr << "Server error: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
