#pragma once

#ifndef RAZ_UDPCLIENT_HPP
#define RAZ_UDPCLIENT_HPP

#include <memory>
#include <string>

namespace Raz {

class UdpClient {
public:
  UdpClient();
  UdpClient(const std::string& host, unsigned short port) : UdpClient() { setDestination(host, port); }
  UdpClient(const UdpClient&) = delete;
  UdpClient(UdpClient&&) noexcept = default;

  /// Sets the server's information to send data to.
  /// \param host Host to send data to.
  /// \param port Port to send data to.
  void setDestination(const std::string& host, unsigned short port);
  /// Sends data to the destination server.
  /// \note This operation is blocking and returns only when all data has been sent or if an error occurred.
  /// \param data Data to be sent.
  void send(const std::string& data);
  /// Recovers the number of currently available bytes to be received.
  /// \return Number of bytes that can be received.
  std::size_t recoverAvailableByteCount();
  /// Receives the data that has been sent to the client.
  /// \note This operation is blocking and returns only when at least one byte has been received or if an error occurred.
  std::string receive();
  /// Closes the client.
  void close();

  UdpClient& operator=(const UdpClient&) = delete;
  UdpClient& operator=(UdpClient&&) noexcept = default;

  ~UdpClient();

private:
  struct Impl;
  std::unique_ptr<Impl> m_impl;
};

} // namespace Raz

#endif //RAZ_UDPCLIENT_HPP
