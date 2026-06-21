#pragma once

#ifndef RAZ_TCPSERVER_HPP
#define RAZ_TCPSERVER_HPP

#include <functional>
#include <memory>
#include <span>
#include <string_view>

namespace Raz {

class TcpServer {
  friend class TcpSession;

public:
  TcpServer();
  explicit TcpServer(unsigned short port) : TcpServer() { start(port); }
  TcpServer(const TcpServer&) = delete;
  TcpServer(TcpServer&&) noexcept = default;

  bool isRunning() const;
  /// Sets a function to be called when a client has connected to the server.
  /// \param connectedCallback Function to be called on connection.
  void setConnectedCallback(std::function<void()> connectedCallback);
  /// Sets a function to be called when a client has disconnected from the server.
  /// \param disconnectedCallback Function to be called on disconnection.
  void setDisconnectedCallback(std::function<void()> disconnectedCallback);
  /// Sets a function to be called when the server has received data from a client. The function's return value will be sent back to the sender.
  /// \param receivedCallback Function to be called on data reception.
  void setReceivedCallback(std::function<std::vector<std::byte>(std::span<const std::byte>)> receivedCallback);

  /// Starts the server and listens for connections.
  /// \param port Port to listen for connections on.
  void start(unsigned short port);
  /// Sends data to all connected clients.
  /// \param data Data to be sent.
  void broadcast(std::vector<std::byte> data);
  /// Sends data to all connected clients.
  /// \param data Data to be sent.
  void broadcast(std::string_view data) {
    broadcast({ reinterpret_cast<const std::byte*>(data.data()), reinterpret_cast<const std::byte*>(data.data()) + data.size() });
  }
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
