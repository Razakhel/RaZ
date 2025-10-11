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

bool TcpClient::isConnected() const {
  return m_impl->socket.is_open();
}

void TcpClient::connect(const std::string& host, unsigned short port) {
  Logger::debug("[TcpClient] Connecting to {}:{}...", host, port);

  asio::error_code error;
  asio::connect(m_impl->socket, m_impl->resolver.resolve(host, std::to_string(port)), error);

  if (error)
    throw std::invalid_argument(std::format("[TcpClient] Failed to connect to {}:{}: {}", host, port, error.message()));

  Logger::debug("[TcpClient] Connected");
}

void TcpClient::send(const std::string& data) {
  Logger::debug("[TcpClient] Sending '{}'...", data);

  asio::error_code error;
  asio::write(m_impl->socket, asio::buffer(data), error);

  if (error)
    throw std::runtime_error(std::format("[TcpClient] Failed to send data: {}", error.message()));
}

std::size_t TcpClient::recoverAvailableByteCount() {
  asio::detail::io_control::bytes_readable command(true);
  m_impl->socket.io_control(command);
  return command.get();
}

std::string TcpClient::receive() {
  std::array<char, 1024> buffer {};
  const size_t length = m_impl->socket.read_some(asio::buffer(buffer));
  return std::string(buffer.data(), length);
}

void TcpClient::disconnect() {
  if (!isConnected())
    return;

  Logger::debug("[TcpClient] Closing...");
  m_impl->socket.shutdown(asio::socket_base::shutdown_both);
  m_impl->socket.close();
  Logger::debug("[TcpClient] Closed");
}

TcpClient::~TcpClient() = default;

}
