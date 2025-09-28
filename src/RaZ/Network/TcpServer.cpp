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

      if (error == asio::error::eof) {
        Logger::debug("[TcpServer] Connection closed");
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
  explicit Impl(unsigned short port) : acceptor(context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {}

  asio::io_context context;
  asio::ip::tcp::acceptor acceptor;
};

TcpServer::TcpServer(unsigned short port) : m_impl{ std::make_unique<Impl>(port) } {}

void TcpServer::start() {
  Logger::debug("[TcpServer] Starting...");

  while (true) {
    Logger::debug("[TcpServer] Awaiting connection on port {}...", m_impl->acceptor.local_endpoint().port());

    std::error_code error;
    asio::ip::tcp::socket socket = m_impl->acceptor.accept(error);

    if (error == asio::error::interrupted)
      break; // Server closed

    Logger::debug("[TcpServer] Connected");
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

} // namespace Raz
