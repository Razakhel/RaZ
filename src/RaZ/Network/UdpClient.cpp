#include "RaZ/Network/UdpClient.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "asio/ip/udp.hpp"

namespace Raz {

struct UdpClient::Impl {
  Impl() : socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)) {}

  asio::io_context context;
  asio::ip::udp::socket socket;
  asio::ip::udp::endpoint serverEndpoint;
  std::array<char, 1024> data {};
};

UdpClient::UdpClient() : m_impl{ std::make_unique<Impl>() } {}

void UdpClient::setDestination(const std::string& host, unsigned short port) {
  asio::error_code error;
  asio::ip::udp::resolver resolver(m_impl->context);
  const asio::ip::udp::resolver::results_type endpoints = resolver.resolve(asio::ip::udp::v4(), host, std::to_string(port), error);

  if (error)
    throw std::invalid_argument(std::format("[UdpClient] Failed to set destination to {}:{}: {}", host, port, error.message()));

  m_impl->serverEndpoint = std::move(*endpoints.begin());
}

void UdpClient::send(const std::string& data) {
  Logger::debug("[UdpClient] Sending '{}'...", data);
  m_impl->socket.send_to(asio::buffer(data), m_impl->serverEndpoint);
}

std::size_t UdpClient::recoverAvailableByteCount() {
  asio::detail::io_control::bytes_readable command(true);
  m_impl->socket.io_control(command);
  return command.get();
}

std::string UdpClient::receive() {
  std::array<char, 1024> buffer {};
  asio::ip::udp::endpoint senderEndpoint;
  const size_t length = m_impl->socket.receive_from(asio::buffer(buffer), senderEndpoint);
  Logger::debug("[UdpClient] Received data from {}: {}", senderEndpoint.address().to_string(), std::string_view(buffer.data(), length));
  return std::string(buffer.data(), length);
}

void UdpClient::close() {
  Logger::debug("[UdpClient] Closing...");
  // shutdown() shouldn't be called on UDP sockets
  m_impl->socket.close();
  Logger::debug("[UdpClient] Closed");
}

UdpClient::~UdpClient() = default;

}
