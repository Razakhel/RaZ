#include "RaZ/Network/TcpServer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "asio/ip/tcp.hpp"
#include "asio/write.hpp"

#include <thread>

namespace Raz {

namespace {

void runSession(asio::ip::tcp::socket socket) {
  try {
    while (true) {
      std::array<char, 1024> data {};
      std::error_code error;

      const size_t length = socket.read_some(asio::buffer(data), error);

      if (error == asio::error::eof || error == asio::error::connection_reset) {
        Logger::debug("[TcpServer] Connection with {} closed", socket.remote_endpoint().address().to_string());
        break;
      }

      if (error)
        throw std::system_error(error);

      Logger::debug("[TcpServer] Received: {}", data.data());

      // Answering with the same received data
      asio::write(socket, asio::buffer(data, length));
    }
  } catch (const std::exception& exception) {
    Logger::error("[TcpServer] Exception occurred: {}", exception.what());
  }
}

}

struct TcpServer::Impl {
  Impl() : acceptor(context) {}

  asio::io_context context;
  asio::ip::tcp::acceptor acceptor;
};

TcpServer::TcpServer() : m_impl{ std::make_unique<Impl>() } {}

void TcpServer::start(unsigned short port) {
  Logger::debug("[TcpServer] Starting on port {}...", port);

  setup(port);

  while (true) {
    Logger::debug("[TcpServer] Awaiting connection on port {}...", port);

    std::error_code error;
    asio::ip::tcp::socket socket = m_impl->acceptor.accept(error);

    if (error == asio::error::interrupted)
      break; // Server closed

    Logger::debug("[TcpServer] Connected to {}", socket.remote_endpoint().address().to_string());
    std::thread(runSession, std::move(socket)).detach();
  }
}

void TcpServer::stop() {
  Logger::debug("[TcpServer] Stopping...");
  m_impl->acceptor.close();
  m_impl->context.stop();
  Logger::debug("[TcpServer] Stopped");
}

TcpServer::~TcpServer() = default;

void TcpServer::setup(unsigned short port) {
  const asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
  m_impl->acceptor.open(endpoint.protocol());
  m_impl->acceptor.set_option(asio::socket_base::reuse_address(true));
  m_impl->acceptor.bind(endpoint);
  m_impl->acceptor.listen();
}

} // namespace Raz
