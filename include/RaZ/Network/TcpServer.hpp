#pragma once

#ifndef RAZ_TCPSERVER_HPP
#define RAZ_TCPSERVER_HPP

#include <memory>

namespace Raz {

class TcpServer {
public:
  explicit TcpServer(unsigned short port);

  /// Starts the server and listens for connections.
  /// \note This operation is blocking; call the function from another thread if needed.
  void start();
  /// Stops the server.
  void stop();

  ~TcpServer();

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace Raz

#endif //RAZ_TCPSERVER_HPP
