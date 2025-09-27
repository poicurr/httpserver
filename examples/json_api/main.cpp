#include <HttpServer/HttpRequest.hpp>
#include <HttpServer/HttpResponse.hpp>
#include <HttpServer/HttpServer.hpp>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using httpserver::HttpRequest;
using httpserver::HttpResponse;
using httpserver::HttpServer;

namespace {

std::string currentIso8601() {
  const auto now = std::chrono::system_clock::now();
  const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
  std::tm utcTime{};
#ifdef _WIN32
  gmtime_s(&utcTime, &nowTime);
#else
  gmtime_r(&nowTime, &utcTime);
#endif
  std::ostringstream oss;
  oss << std::put_time(&utcTime, "%Y-%m-%dT%H:%M:%SZ");
  return oss.str();
}

HttpResponse handleRequest(const HttpRequest &request) {
  if (request.header.method != "GET") {
    return {"HTTP/1.1 405 Method Not Allowed", "application/json",
            "{\"error\":\"method not allowed\"}"};
  }
  std::ostringstream body;
  body << "{\"path\":\"" << request.header.path << "\",";
  body << "\"server_time\":\"" << currentIso8601() << "\"}";
  return {"HTTP/1.1 200 OK", "application/json", body.str()};
}

} // namespace

int main() {
  try {
    std::cout << "JsonApi example: http://127.0.0.1:8000/info" << std::endl;
    HttpServer server(8000);
    server.run(
        [](const HttpRequest &request) { return handleRequest(request); });
  } catch (const std::exception &ex) {
    std::cerr << "Server error: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
