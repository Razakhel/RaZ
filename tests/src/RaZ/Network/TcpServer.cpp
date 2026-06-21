#include "RaZ/Network/TcpClient.hpp"
#include "RaZ/Network/TcpServer.hpp"

#include <catch2/catch_test_macros.hpp>

#include <future>
#include <latch>

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
  Raz::TcpServer server(1234);

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

TEST_CASE("TcpServer reception callback") {
  Raz::TcpServer server(1234);

  uint8_t receivedCount = 0;
  server.setReceivedCallback([&receivedCount] (std::span<const std::byte> data) {
    std::vector<std::byte> response(data.begin(), data.end());
    response.emplace_back(static_cast<std::byte>(' '));
    response.emplace_back(static_cast<std::byte>(receivedCount + 48)); // Actual character representing the count
    ++receivedCount;
    return response;
  });

  Raz::TcpClient client("localhost", 1234);

  client.send("test");
  CHECK(client.receive() == "test 0");

  client.send("other test");
  CHECK(client.receive() == "other test 1");
}

TEST_CASE("TcpServer broadcast") {
  Raz::TcpServer server(1234);

  // Due to a potential data race (which occurs under Linux and/or with GCC), connections must be fully done before broadcasting;
  //  since this test doesn't represent what an actual usage would be, the synchronization is done here
  // In a real-life situation broadcasting should always happen in response to an event, guaranteeing a prior connection
  std::latch remainingConnectionCount(2);
  server.setConnectedCallback([&remainingConnectionCount] () noexcept { remainingConnectionCount.count_down(); });

  Raz::TcpClient client1("localhost", 1234);
  Raz::TcpClient client2("localhost", 1234);

  remainingConnectionCount.wait();

  constexpr std::string_view message = "test";
  server.broadcast(message);
  CHECK(client1.receive() == message);
  CHECK(client2.receive() == message);
}
