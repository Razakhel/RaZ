#include "RaZ/Network/TcpClient.hpp"
#include "RaZ/Network/TcpServer.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>

TEST_CASE("TcpClient basic", "[network]") {
  Raz::TcpClient client;

  CHECK_FALSE(client.isConnected());
  CHECK_NOTHROW(client.disconnect());

  // Data transfer functions throw when the client isn't connected
  CHECK_THROWS(client.recoverAvailableByteCount());
  CHECK_THROWS(client.send("test"));
  CHECK_THROWS(client.receive());
  CHECK_THROWS(client.receiveAtLeast(1));
  CHECK_THROWS(client.receiveExactly(1));
  CHECK_THROWS(client.receiveUntil("\0"));
}

TEST_CASE("TcpClient connection", "[network]") {
  Raz::TcpServer server;
  Raz::TcpClient client;

  CHECK_THROWS(client.connect("localhost", 1234)); // No server to connect to

  server.start(1234);

  CHECK_NOTHROW(client.connect("localhost", 1234));
  CHECK(client.isConnected());

  CHECK_NOTHROW(client.disconnect());
  CHECK_FALSE(client.isConnected());

  CHECK_NOTHROW(server.stop());
}

TEST_CASE("TcpClient send and receive", "[network]") {
  Raz::TcpServer server(1234);
  server.setReceivedCallback([] (std::span<const std::byte> data) {
    return std::vector(data.begin(), data.end()); // Echoing back the received data
  });

  Raz::TcpClient client("localhost", 1234);
  REQUIRE(client.isConnected());

  client.send("test");
  CHECK(client.receive() == "test"); // Blocks until some data has been received

  client.send("other test");
  std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Waiting for the server to send back data
  CHECK(client.recoverAvailableByteCount() == 10); // This can return a different number if the server didn't have time to send anything back, hence the wait
  CHECK(client.receive() == "other test");

  client.disconnect();
}

TEST_CASE("TcpClient receive at least", "[network]") {
  Raz::TcpServer server(1234);
  server.setReceivedCallback([] (std::span<const std::byte> data) {
    return std::vector(data.begin(), data.end()); // Echoing back the received data
  });

  Raz::TcpClient client("localhost", 1234);
  REQUIRE(client.isConnected());

  client.send("data");
  CHECK(client.receiveAtLeast(4) == "data");

  client.send("other data");
  CHECK(client.receiveUntil("o") == "o"); // Discarding the first character; the rest should have been read and be still available internally
  CHECK(client.receiveAtLeast(1) == "ther data");

  client.disconnect();
}

TEST_CASE("TcpClient receive exactly", "[network]") {
  Raz::TcpServer server(1234);
  server.setReceivedCallback([] (std::span<const std::byte> data) {
    return std::vector(data.begin(), data.end()); // Echoing back the received data
  });

  Raz::TcpClient client("localhost", 1234);
  REQUIRE(client.isConnected());

  client.send("test");
  CHECK(client.receiveExactly(1) == "t");
  CHECK(client.receiveExactly(3) == "est");

  client.disconnect();
}

TEST_CASE("TcpClient receive until delimiter", "[network]") {
  Raz::TcpServer server(1234);
  server.setReceivedCallback([] (std::span<const std::byte> data) {
    return std::vector(data.begin(), data.end()); // Echoing back the received data
  });

  Raz::TcpClient client("localhost", 1234);
  REQUIRE(client.isConnected());

  client.send("some test\r\n");
  CHECK(client.receiveUntil(" ") == "some "); // The received data includes the delimiter
  CHECK(client.receiveUntil("\r\n") == "test\r\n");

  client.disconnect();
}
