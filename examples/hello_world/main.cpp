#include <HttpServer/HttpRequest.hpp>
#include <HttpServer/HttpResponse.hpp>
#include <HttpServer/HttpServer.hpp>

#include <iostream>
#include <string>

using httpserver::HttpRequest;
using httpserver::HttpResponse;
using httpserver::HttpServer;

namespace {

HttpResponse handleRequest(const HttpRequest &request) {
  if (request.header.path != "/" || request.header.method != "GET") {
    return {"HTTP/1.1 404 Not Found", "text/plain", "Not found"};
  }
  const std::string message = "Hello, world!";
  return {"HTTP/1.1 200 OK", "text/plain", message};
}

} // namespace

int main() {
  try {
    std::cout << "HelloWorld example: http://127.0.0.1:8000" << std::endl;
    HttpServer server(8000);
    server.run(
        [](const HttpRequest &request) { return handleRequest(request); });
  } catch (const std::exception &ex) {
    std::cerr << "Server error: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
