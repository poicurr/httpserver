#include <HttpRequest.hpp>
#include <HttpResponse.hpp>
#include <HttpServer.hpp>
#include <fstream>
#include <iostream>

const int port = 8000;

std::string loadfile(std::ifstream& ifs) {
  static char buffer[1024];
  auto ret = std::string{};
  while (!ifs.eof()) {
    ifs.read(buffer, 1024);
    ret.append(buffer, ifs.gcount());
  }
  return ret;
}

std::string mimetype(const std::string& target) {
  auto mimetype = std::string{"text/html"};
  if (endsWith(target, ".ico")) {
    mimetype = "image/x-icon";
  } else if (endsWith(target, ".js")) {
    mimetype = "application/javascript";
  } else if (endsWith(target, ".css")) {
    mimetype = "text/css";
  }
  return mimetype;
}

HttpResponse get(const HttpRequest& request) {
  const auto path = request.header.path;
  if (path.empty())
    return HttpResponse{"HTTP/1.1 404 Not Found", "text/html", ""};

  auto target = std::string{};
  if (path == "/") {
    target = "./resources/index.html";
  } else {
    target = "./resources" + path;
  }

  std::ifstream ifs(target, std::ios::binary);
  if (!ifs.is_open())
    return HttpResponse{"HTTP/1.1 404 Not Found", "text/html", ""};
  return HttpResponse{"HTTP/1.1 200 OK", mimetype(target), loadfile(ifs)};
}

HttpResponse post(const HttpRequest& request) {
  const auto path = request.header.path;
  return HttpResponse{"HTTP/1.1 404 Not Found", "text/html", ""};
}

int main(int argc, char const* argv[]) {
  std::cout << "Server running on http://localhost:" << port << std::endl;
  HttpServer server(port);
  server.run([](const HttpRequest& request) {
    const auto method = toUpper(request.header.method);
    if (method == "GET") {
      return get(request);
    } else if (method == "POST") {
      return post(request);
    }
    return HttpResponse{"HTTP/1.1 404 Not Found", "text/html", ""};
  });
}
