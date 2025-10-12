#include "RaZ/Network/TcpServer.hpp"
#include "RaZ/Utils/Logger.hpp"

#include "asio/ip/tcp.hpp"
#include "asio/write.hpp"

#include <thread>

namespace Raz {

namespace {

class TcpSession : public std::enable_shared_from_this<TcpSession> {
public:
  explicit TcpSession(asio::ip::tcp::socket socket) noexcept : m_socket{ std::move(socket) } {}

  void run() {
    Logger::debug("[TcpSession] Connected to {}", m_socket.remote_endpoint().address().to_string());
    receive();
  }

private:
  void receive() {
    m_socket.async_read_some(asio::buffer(m_data), [self = shared_from_this()] (const asio::error_code& error, std::size_t length) {
      if (error == asio::error::eof || error == asio::error::connection_reset) {
        Logger::debug("[TcpSession] Connection with {} closed", self->m_socket.remote_endpoint().address().to_string());
        return;
      }

      if (error) {
        Logger::error("[TcpSession] Error while receiving data: {}", error.message());
      } else {
        Logger::debug("[TcpSession] Received: {}", std::string_view(self->m_data.data(), length));
        self->echo(length); // Replying with the same received data
      }

      self->receive();
    });
  }

  void echo(std::size_t length) {
    asio::async_write(m_socket, asio::buffer(m_data, length), [] (const asio::error_code& error, std::size_t) {
      if (error)
        Logger::error("[TcpSession] Error while echoing: {}", error.message());
    });
  }

  asio::ip::tcp::socket m_socket;
  std::array<char, 1024> m_data {};
};

} // namespace

struct TcpServer::Impl {
  Impl() : acceptor(context) {}

  asio::io_context context;
  asio::ip::tcp::acceptor acceptor;
};

TcpServer::TcpServer() : m_impl{ std::make_unique<Impl>() } {}

void TcpServer::start(unsigned short port) {
  Logger::debug("[TcpServer] Starting on port {}...", port);

  setup(port);
  accept();

  m_impl->context.run();
}

void TcpServer::stop() {
  Logger::debug("[TcpServer] Stopping...");
  m_impl->acceptor.close();
  m_impl->context.stop();
  Logger::debug("[TcpServer] Stopped");
}

TcpServer::~TcpServer() = default;

void TcpServer::setup(unsigned short port) {
  if (m_impl->context.stopped())
    m_impl->context.restart();

  const asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
  m_impl->acceptor.open(endpoint.protocol());
  m_impl->acceptor.set_option(asio::socket_base::reuse_address(true));
  m_impl->acceptor.bind(endpoint);
  m_impl->acceptor.listen();
}

void TcpServer::accept() {
  Logger::debug("[TcpServer] Awaiting connection...");

  m_impl->acceptor.async_accept([this] (const asio::error_code& error, asio::ip::tcp::socket socket) {
    if (error == asio::error::interrupted || error == asio::error::operation_aborted)
      return; // Server closed

    if (error)
      Logger::error("[TcpServer] Error while accepting connection: {}", error.message());
    else
      std::make_shared<TcpSession>(std::move(socket))->run();

    accept();
  });
}

} // namespace Raz
