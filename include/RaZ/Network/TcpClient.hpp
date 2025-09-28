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

  /// Connects the client to a server.
  /// \param host Host to establish a connection with.
  /// \param port Port to connect to.
  void connect(const std::string& host, unsigned short port);
  /// Sends data to the server the client is connected to.
  /// \note This operation is blocking and returns only when all data has been sent or if an error occurred.
  /// \param request Data to be sent.
  /// \return Response sent by the server.
  std::string send(const std::string& request);
  /// Closes the client.
  void close();

  ~TcpClient();

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace Raz

#endif //RAZ_TCPCLIENT_HPP
