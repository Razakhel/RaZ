#include "RaZ/Network/TcpServer.hpp"
#include "RaZ/Utils/Logger.hpp"
#include "RaZ/Utils/Threading.hpp"

#include "asio/ip/tcp.hpp"
#include "asio/write.hpp"

#include <thread>

namespace Raz {

class TcpSession;

struct TcpServer::Impl {
  Impl() : acceptor(context) {}

  asio::io_context context;
  asio::ip::tcp::acceptor acceptor;
  std::thread contextThread;

  std::function<void()> connectedCallback;
  std::function<void()> disconnectedCallback;
};

class TcpSession : public std::enable_shared_from_this<TcpSession> {
public:
  TcpSession(asio::ip::tcp::socket socket, TcpServer::Impl& server) noexcept : m_socket{ std::move(socket) }, m_server{ server } {}

  void run() {
    Logger::debug("[TcpSession] Connected to {}", m_socket.remote_endpoint().address().to_string());
    if (m_server.connectedCallback)
      m_server.connectedCallback();
    receive();
  }

private:
  void receive() {
    m_socket.async_read_some(asio::buffer(m_data), [self = shared_from_this()] (const asio::error_code& error, std::size_t length) {
      if (error == asio::error::eof || error == asio::error::connection_reset) {
        Logger::debug("[TcpSession] Connection with {} closed", self->m_socket.remote_endpoint().address().to_string());
        if (self->m_server.disconnectedCallback)
          self->m_server.disconnectedCallback();
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
  TcpServer::Impl& m_server;
  std::array<char, 1024> m_data {};
};

TcpServer::TcpServer() : m_impl{ std::make_unique<Impl>() } {}

bool TcpServer::isRunning() const {
  return m_impl->acceptor.is_open();
}

void TcpServer::setConnectedCallback(std::function<void()> connectedCallback) {
  m_impl->connectedCallback = std::move(connectedCallback);
}

void TcpServer::setDisconnectedCallback(std::function<void()> disconnectedCallback) {
  m_impl->disconnectedCallback = std::move(disconnectedCallback);

}

void TcpServer::start(unsigned short port) {
  Logger::debug("[TcpServer] Starting on port {}...", port);

  if (isRunning())
    stop();
  setup(port);
  accept();

  m_impl->contextThread = std::thread([this] () {
    Threading::setCurrentThreadName("TCP server");
    m_impl->context.run();
  });
}

void TcpServer::stop() {
  Logger::debug("[TcpServer] Stopping...");

  m_impl->acceptor.close();
  m_impl->context.stop();
  if (m_impl->contextThread.joinable())
    m_impl->contextThread.join();

  Logger::debug("[TcpServer] Stopped");
}

TcpServer::~TcpServer() {
  if (isRunning())
    stop();
}

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
      std::make_shared<TcpSession>(std::move(socket), *m_impl)->run();

    accept();
  });
}

} // namespace Raz
