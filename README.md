# HttpServer

Lightweight and reusable header-only HTTP server library for C++.
Add `#include <HttpServer/HttpServer.hpp>` and start handling requests in minutes.

## Features

- Header-only: no separate build step required
- Works on both Linux and Windows; socket setup handled internally
- Simple event loop with keep-alive support
- Depends on [StringUtils](https://github.com/poicurr/StringUtils) for robust string handling

## CMake Integration

### Using FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
  HttpServer
  GIT_REPOSITORY https://github.com/poicurr/HttpServer.git
  GIT_TAG main
)
FetchContent_MakeAvailable(HttpServer)

target_link_libraries(MyApp PRIVATE HttpServer)
```

### Using a Local Checkout

```cmake
add_subdirectory(external/HttpServer)

target_link_libraries(MyApp PRIVATE HttpServer)
```

Linking against `HttpServer` automatically fetches `StringUtils`. On Windows the required Winsock libraries are linked internally, so no extra linker flags are necessary.

### Building the Examples

```sh
cmake -S . -B build -DHTTP_SERVER_BUILD_EXAMPLES=ON
cmake --build build
```

Enable the `HTTP_SERVER_BUILD_EXAMPLES` option to build every sample under `examples/`.

## Quick Start

```cpp
#include <HttpServer/HttpRequest.hpp>
#include <HttpServer/HttpResponse.hpp>
#include <HttpServer/HttpServer.hpp>
#include <StringUtils/StringUtils.hpp>

int main() {
  try {
    httpserver::HttpServer server(8000);
    server.run([](const httpserver::HttpRequest &request) {
      const auto method = strutil::toUpper(request.header.method);
      if (method == "GET") {
        return httpserver::HttpResponse{"HTTP/1.1 200 OK", "text/plain",
                                         "Hello, HttpServer!"};
      }
      return httpserver::HttpResponse{"HTTP/1.1 405 Method Not Allowed",
                                      "text/plain", ""};
    });
  } catch (const std::exception &ex) {
    std::cerr << "Failed to start server: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
```

Read headers and body from `httpserver::HttpRequest`, then return an `httpserver::HttpResponse` with the desired status line, content type, and payload.

## Examples

- `examples/basic_server`: static file hosting and simple form handling
- `examples/hello_world`: minimal fixed response
- `examples/json_api`: mock JSON endpoint
- `examples/echo_server`: echoes the request body for debugging
- `examples/keep_alive_demo`: showcases keep-alive behavior

## License

MIT License

