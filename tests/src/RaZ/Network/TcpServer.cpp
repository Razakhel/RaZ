#include "RaZ/Network/TcpClient.hpp"
#include "RaZ/Network/TcpServer.hpp"

#include <catch2/catch_test_macros.hpp>

#include <future>

TEST_CASE("TcpServer basic", "[network]") {
  Raz::TcpServer server;
  CHECK_FALSE(server.isRunning());

  CHECK_NOTHROW(server.stop()); // Stopping a non-running server isn't an error
  CHECK_FALSE(server.isRunning());

  CHECK_NOTHROW(server.start(1234));
  CHECK(server.isRunning());
  CHECK_NOTHROW(server.start(1234)); // Starting an already running server restarts it properly
  CHECK(server.isRunning());

  CHECK_NOTHROW(server.stop());
  CHECK_FALSE(server.isRunning());
  CHECK_NOTHROW(server.stop()); // Stopping an already stopped server does nothing
  CHECK_FALSE(server.isRunning());
}

TEST_CASE("TcpServer connection callbacks") {
  Raz::TcpServer server;
  CHECK_NOTHROW(server.start(1234));

  std::promise<void> connectionPromise;
  std::promise<void> disconnectionPromise;
  server.setConnectedCallback([&connectionPromise] () { connectionPromise.set_value(); });
  server.setDisconnectedCallback([&disconnectionPromise] () { disconnectionPromise.set_value(); });

  Raz::TcpClient client;

  client.connect("localhost", 1234);
  CHECK_NOTHROW(connectionPromise.get_future().get());

  client.disconnect();
  CHECK_NOTHROW(disconnectionPromise.get_future().get());
}
