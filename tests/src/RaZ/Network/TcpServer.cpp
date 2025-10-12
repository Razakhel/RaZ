#include "RaZ/Network/TcpServer.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>

TEST_CASE("TcpServer basic", "[network]") {
  Raz::TcpServer server;

  CHECK_NOTHROW(server.stop()); // Stopping a non-running server isn't an error

  std::thread serverThread([&server] () { server.start(1234); });
  std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Waiting for the server to start

  server.stop();
  serverThread.join();
}
