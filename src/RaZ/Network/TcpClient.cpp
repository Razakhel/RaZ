#include "RaZ/Network/TcpClient.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "asio/connect.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/write.hpp"

namespace Raz {

struct TcpClient::Impl {
  Impl() : socket(context), resolver(context) {}

  asio::io_context context;
  asio::ip::tcp::socket socket;
  asio::ip::tcp::resolver resolver;
};

TcpClient::TcpClient() : m_impl{ std::make_unique<Impl>() } {}

void TcpClient::connect(const std::string& host, unsigned short port) {
  Logger::debug("[TcpClient] Connecting to {}:{}...", host, port);

  asio::error_code error;
  asio::connect(m_impl->socket, m_impl->resolver.resolve(host, std::to_string(port)), error);

  if (error)
    throw std::invalid_argument(std::format("[TcpClient] Failed to connect to {}:{}: {}", host, port, error.message()));

  Logger::debug("[TcpClient] Connected");
}

std::string TcpClient::send(const std::string& request) {
  Logger::debug("[TcpClient] Sending '{}'...", request);

  asio::write(m_impl->socket, asio::buffer(request));

  // Getting back the response from the server
  std::array<char, 1024> buffer {};
  const size_t length = m_impl->socket.read_some(asio::buffer(buffer));
  return std::string(buffer.data(), length);
}

void TcpClient::close() {
  Logger::debug("[TcpClient] Closing...");
  m_impl->socket.shutdown(asio::socket_base::shutdown_both);
  m_impl->socket.close();
  Logger::debug("[TcpClient] Closed");
}

TcpClient::~TcpClient() = default;

}
