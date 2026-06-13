#pragma once

#ifndef RAZ_TCPSERVER_HPP
#define RAZ_TCPSERVER_HPP

#include <functional>
#include <memory>

namespace Raz {

class TcpServer {
  friend class TcpSession;

public:
  TcpServer();
  explicit TcpServer(unsigned short port) : TcpServer() { start(port); }
  TcpServer(const TcpServer&) = delete;
  TcpServer(TcpServer&&) noexcept = default;

  bool isRunning() const;
  void setConnectedCallback(std::function<void()> connectedCallback);
  void setDisconnectedCallback(std::function<void()> disconnectedCallback);

  /// Starts the server and listens for connections.
  /// \param port Port to listen for connections on.
  void start(unsigned short port);
  /// Stops the server.
  void stop();

  TcpServer& operator=(const TcpServer&) = delete;
  TcpServer& operator=(TcpServer&&) noexcept = default;

  ~TcpServer();

private:
  void setup(unsigned short port);
  void accept();

  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace Raz

#endif //RAZ_TCPSERVER_HPP
