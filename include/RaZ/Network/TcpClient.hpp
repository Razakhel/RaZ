#pragma once

#ifndef RAZ_TCPCLIENT_HPP
#define RAZ_TCPCLIENT_HPP

#include <memory>
#include <string>

namespace Raz {

class TcpClient {
public:
  TcpClient();
  TcpClient(const std::string& host, unsigned short port) : TcpClient() { connect(host, port); }
  TcpClient(const TcpClient&) = delete;
  TcpClient(TcpClient&&) noexcept = default;

  bool isConnected() const;

  /// Connects the client to a server.
  /// \param host Host to establish a connection with.
  /// \param port Port to connect to.
  void connect(const std::string& host, unsigned short port);
  /// Sends data to the server the client is connected to.
  /// \note This operation is blocking and returns only when all data has been sent or if an error occurred.
  /// \param data Data to be sent.
  void send(const std::string& data);
  /// Recovers the number of currently available bytes to be received.
  /// \return Number of bytes that can be received.
  std::size_t recoverAvailableByteCount();
  /// Receives the data that have been sent to the client.
  std::string receive();
  /// Disconnects the client from the server.
  void disconnect();

  TcpClient& operator=(const TcpClient&) = delete;
  TcpClient& operator=(TcpClient&&) noexcept = default;

  ~TcpClient();

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace Raz

#endif //RAZ_TCPCLIENT_HPP
