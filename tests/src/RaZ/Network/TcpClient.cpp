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
}

TEST_CASE("TcpClient send", "[network]") {
  Raz::TcpServer server;
  std::thread serverThread([&server] () { server.start(1234); });

  Raz::TcpClient client;
  client.connect("localhost", 1234);
  REQUIRE(client.isConnected());

  client.send("test");
  CHECK(client.receive() == "test"); // Blocks until some data has been received

  client.send("other test");
  std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Waiting for the server to send back data
  CHECK(client.recoverAvailableByteCount() == 10); // This can return a different number if the server didn't have time to reply anything, hence the wait
  CHECK(client.receive() == "other test");

  client.disconnect();
  CHECK_FALSE(client.isConnected());

  server.stop();
  serverThread.join();
}
