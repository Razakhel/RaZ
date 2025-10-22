#pragma once

#ifndef RAZ_UDPSERVER_HPP
#define RAZ_UDPSERVER_HPP

#include <memory>

namespace Raz {

class UdpServer {
public:
  UdpServer();
  UdpServer(const UdpServer&) = delete;
  UdpServer(UdpServer&&) noexcept = default;

  /// Starts the server.
  /// \note This operation is blocking; call the function from another thread if needed.
  /// \param port Port to communicate on.
  void start(unsigned short port);
  /// Stops the server.
  void stop();

  UdpServer& operator=(const UdpServer&) = delete;
  UdpServer& operator=(UdpServer&&) noexcept = default;

  ~UdpServer();

private:
  void setup(unsigned short port);
  void receive();
  void echo(std::size_t length);

  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace Raz

#endif //RAZ_UDPSERVER_HPP
