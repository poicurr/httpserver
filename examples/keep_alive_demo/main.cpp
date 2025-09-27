#include <HttpServer/HttpRequest.hpp>
#include <HttpServer/HttpResponse.hpp>
#include <HttpServer/HttpServer.hpp>

#include <atomic>
#include <iostream>
#include <sstream>
#include <string>

using httpserver::HttpRequest;
using httpserver::HttpResponse;
using httpserver::HttpServer;

namespace {

std::atomic<int> g_requestCount{0};

HttpResponse handleRequest(const HttpRequest &request) {
  if (request.header.method != "GET" || request.header.path != "/status") {
    return {"HTTP/1.1 404 Not Found", "text/plain", "Not found"};
  }
  const bool keepAlive =
      request.header.headers.count("Connection") > 0 &&
      request.header.headers.at("Connection") == "keep-alive";
  std::ostringstream body;
  body << "total requests: " << ++g_requestCount << "\n";
  body << "keep-alive requested: " << (keepAlive ? "yes" : "no") << "\n";
  body << "hint: curl -H \"Connection: keep-alive\" "
          "http://127.0.0.1:8000/status";
  return {"HTTP/1.1 200 OK", "text/plain", body.str()};
}

} // namespace

int main() {
  try {
    std::cout << "KeepAlive demo: http://127.0.0.1:8000/status" << std::endl;
    HttpServer server(8000);
    server.run(
        [](const HttpRequest &request) { return handleRequest(request); });
  } catch (const std::exception &ex) {
    std::cerr << "Server error: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
