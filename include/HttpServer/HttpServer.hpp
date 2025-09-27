#pragma once

#include <HttpServer/HttpRequest.hpp>
#include <HttpServer/HttpResponse.hpp>
#include <StringUtils/StringUtils.hpp>

#include <array>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#ifdef __linux__

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#elif defined(_WIN32)

#include <winsock2.h>
#include <ws2tcpip.h>

#endif

namespace httpserver {

inline constexpr std::size_t kBufferSize = 8192;

inline std::string valueOf(const HttpRequestHeader &header,
                           const std::string &key) {
  const auto it = header.headers.find(key);
  if (it == header.headers.end())
    return {};
  return it->second;
}

namespace detail {

#ifdef _WIN32
using SocketHandle = SOCKET;
constexpr SocketHandle kInvalidSocket = INVALID_SOCKET;
#else
using SocketHandle = int;
constexpr SocketHandle kInvalidSocket = -1;
#endif

inline int parseContentLength(const std::string &headers) {
  const std::string key = "Content-Length:";
  const auto pos = headers.find(key);
  if (pos == std::string::npos)
    return 0;
  const auto valueBegin = pos + key.size();
  const auto lineEnd = headers.find("\r\n", valueBegin);
  const auto lenStr =
      strutil::trim(headers.substr(valueBegin, lineEnd - valueBegin));
  if (lenStr.empty())
    return 0;
  try {
    return std::stoi(lenStr);
  } catch (...) {
    return 0;
  }
}

inline void closeSocket(SocketHandle socket) {
#ifdef _WIN32
  if (socket != INVALID_SOCKET)
    closesocket(socket);
#else
  if (socket >= 0)
    close(socket);
#endif
}

} // namespace detail

class HttpServer {
public:
  explicit HttpServer(unsigned short port);
  ~HttpServer();

  HttpServer(const HttpServer &) = delete;
  HttpServer &operator=(const HttpServer &) = delete;
  HttpServer(HttpServer &&) = delete;
  HttpServer &operator=(HttpServer &&) = delete;

  template <class RequestHandler> void run(RequestHandler &&handler) {
    auto requestHandler = std::forward<RequestHandler>(handler);
    while (true) {
      sockaddr_in dstAddr;
      std::memset(&dstAddr, 0, sizeof(dstAddr));
      socklen_t dstAddrSize = sizeof(dstAddr);
      detail::SocketHandle client = accept(
          m_socket, reinterpret_cast<sockaddr *>(&dstAddr), &dstAddrSize);
#ifdef _WIN32
      if (client == detail::kInvalidSocket)
        continue;
#else
      if (client < 0)
        continue;
#endif
      handleClient(client, requestHandler);
    }
  }

private:
  template <class RequestHandler>
  void handleClient(detail::SocketHandle client, RequestHandler &handler) {
    bool keepAlive = false;
    do {
      const auto rawRequest = receiveRequest(client);
      if (rawRequest.empty())
        break;
      const auto request = parseRequest(rawRequest);
      if (request.header.method.empty())
        break;
      keepAlive = strutil::toLower(valueOf(request.header, "Connection")) ==
                  "keep-alive";
      const auto response = handler(request);
      sendResponse(client, response, keepAlive);
    } while (keepAlive);
    detail::closeSocket(client);
  }

  std::string receiveRequest(detail::SocketHandle client) const;
  HttpRequest parseRequest(const std::string &raw) const;
  void sendResponse(detail::SocketHandle client, const HttpResponse &response,
                    bool keepAlive) const;

#ifdef _WIN32
  bool m_isWsaInitialized;
#endif
  detail::SocketHandle m_socket;
};

inline HttpServer::HttpServer(unsigned short port)
#ifdef _WIN32
    : m_isWsaInitialized(false), m_socket(detail::kInvalidSocket)
#else
    : m_socket(detail::kInvalidSocket)
#endif
{
#ifdef _WIN32
  WSADATA data;
  if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
    throw std::runtime_error("WSAStartup failed");
  }
  m_isWsaInitialized = true;
#endif

  m_socket = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
  if (m_socket == INVALID_SOCKET) {
    throw std::runtime_error("socket creation failed");
  }
#else
  if (m_socket < 0) {
    throw std::runtime_error("socket creation failed");
  }
#endif

  int reuse = 1;
  if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<const char *>(&reuse), sizeof(reuse)) < 0) {
    detail::closeSocket(m_socket);
    throw std::runtime_error("setsockopt failed");
  }

  sockaddr_in srcAddr;
  std::memset(&srcAddr, 0, sizeof(srcAddr));
  srcAddr.sin_port = htons(port);
  srcAddr.sin_family = AF_INET;
  srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(m_socket, reinterpret_cast<sockaddr *>(&srcAddr), sizeof(srcAddr)) <
      0) {
    detail::closeSocket(m_socket);
    throw std::runtime_error("bind failed");
  }

  if (listen(m_socket, SOMAXCONN) < 0) {
    detail::closeSocket(m_socket);
    throw std::runtime_error("listen failed");
  }
}

inline HttpServer::~HttpServer() {
  detail::closeSocket(m_socket);
#ifdef _WIN32
  if (m_isWsaInitialized)
    WSACleanup();
#endif
}

inline std::string
HttpServer::receiveRequest(detail::SocketHandle client) const {
  std::string data;
  data.reserve(kBufferSize);
  std::array<char, kBufferSize> buffer{};
  int expectedBodySize = -1;
  while (true) {
    const int received =
        recv(client, buffer.data(), static_cast<int>(buffer.size()), 0);
    if (received <= 0)
      break;
    data.append(buffer.data(), received);
    const auto headerEnd = data.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
      continue;
    if (expectedBodySize < 0) {
      expectedBodySize = detail::parseContentLength(data.substr(0, headerEnd));
    }
    const auto bodySize = static_cast<int>(data.size() - (headerEnd + 4));
    if (bodySize >= expectedBodySize)
      break;
  }
  return data;
}

inline HttpRequest HttpServer::parseRequest(const std::string &raw) const {
  HttpRequest request;
  const auto headerEnd = raw.find("\r\n\r\n");
  if (headerEnd == std::string::npos)
    return HttpRequest{};

  const auto headerText = raw.substr(0, headerEnd);
  const auto bodyText = raw.substr(headerEnd + 4);

  std::istringstream headerStream(headerText);
  std::string requestLine;
  if (!std::getline(headerStream, requestLine))
    return HttpRequest{};
  if (!requestLine.empty() && requestLine.back() == '\r')
    requestLine.pop_back();

  std::istringstream requestLineStream(requestLine);
  requestLineStream >> request.header.method >> request.header.path >>
      request.header.version;
  if (request.header.method.empty() || request.header.path.empty() ||
      request.header.version.empty()) {
    return HttpRequest{};
  }

  std::string line;
  while (std::getline(headerStream, line)) {
    if (!line.empty() && line.back() == '\r')
      line.pop_back();
    const auto delimiter = line.find(':');
    if (delimiter == std::string::npos)
      continue;
    const auto key = strutil::trimRight(line.substr(0, delimiter));
    const auto value = strutil::trimLeft(line.substr(delimiter + 1));
    if (!key.empty())
      request.header.headers[key] = value;
  }

  request.body = bodyText;
  return request;
}

inline void HttpServer::sendResponse(detail::SocketHandle client,
                                     const HttpResponse &response,
                                     bool keepAlive) const {
  std::ostringstream ss;
  ss << response.message << "\r\n";
  ss << "Content-Length: " << response.body.size() << "\r\n";
  ss << "Content-Type: " << response.mimetype << "\r\n";
  ss << "Connection: " << (keepAlive ? "keep-alive" : "close") << "\r\n";
  ss << "\r\n";
  ss << response.body;
  const auto serialized = ss.str();
  send(client, serialized.data(), static_cast<int>(serialized.size()), 0);
}

} // namespace httpserver

