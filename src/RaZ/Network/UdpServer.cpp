#include "RaZ/Network/UdpServer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "asio/ip/udp.hpp"

namespace Raz {

struct UdpServer::Impl {
  Impl() : socket(context) {}

  asio::io_context context;
  asio::ip::udp::socket socket;
  asio::ip::udp::endpoint senderEndpoint;
  std::array<char, 1024> data {};
};

UdpServer::UdpServer() : m_impl{ std::make_unique<Impl>() } {}

void UdpServer::start(unsigned short port) {
  Logger::debug("[UdpServer] Starting on port {}...", port);

  setup(port);
  receive();

  m_impl->context.run();
}

void UdpServer::stop() {
  Logger::debug("[UdpServer] Stopping...");
  // shutdown() shouldn't be called on UDP sockets
  m_impl->socket.close();
  m_impl->context.stop();
  Logger::debug("[UdpServer] Stopped");
}

UdpServer::~UdpServer() = default;

void UdpServer::setup(unsigned short port) {
  if (m_impl->context.stopped())
    m_impl->context.restart();

  const asio::ip::udp::endpoint endpoint(asio::ip::udp::v4(), port);
  m_impl->socket.open(endpoint.protocol());
  m_impl->socket.set_option(asio::socket_base::reuse_address(true));
  m_impl->socket.bind(endpoint);
}

void UdpServer::receive() {
  m_impl->socket.async_receive_from(asio::buffer(m_impl->data), m_impl->senderEndpoint, [this] (const asio::error_code& error, std::size_t bytesReceived) {
    if (error == asio::error::interrupted || error == asio::error::operation_aborted)
      return; // Server closed

    if (error) {
      Logger::error("[UdpServer] Error while receiving data: {}", error.message());
    } else {
      Logger::debug("[UdpServer] Received data from {}: {}", m_impl->senderEndpoint.address().to_string(), std::string_view(m_impl->data.data(), bytesReceived));
      echo(bytesReceived); // Replying with the same received data
    }

    receive();
  });
}

void UdpServer::echo(std::size_t length) {
  m_impl->socket.async_send_to(asio::buffer(m_impl->data, length), m_impl->senderEndpoint, [] (const asio::error_code& error, std::size_t) {
    if (error)
      Logger::error("[UdpServer] Error while echoing: {}", error.message());
  });
}

} // namespace Raz
