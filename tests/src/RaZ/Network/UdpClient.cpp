#include "RaZ/Network/UdpClient.hpp"
#include "RaZ/Network/UdpServer.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>

TEST_CASE("UdpClient basic", "[network]") {
  Raz::UdpClient client;

  CHECK_NOTHROW(client.close());

  // Data transfer functions throw when the client isn't connected
  CHECK_THROWS(client.recoverAvailableByteCount());
  CHECK_THROWS(client.send("test"));
  CHECK_THROWS(client.receive());
}

TEST_CASE("UdpClient send", "[network]") {
  Raz::UdpServer server;
  std::thread serverThread([&server] () { server.start(1234); });

  Raz::UdpClient client;
  REQUIRE_NOTHROW(client.setDestination("localhost", 1234));

  client.send("test");
  CHECK(client.receive() == "test"); // Blocks until some data has been received

  client.send("other test");
  std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Waiting for the server to send back data
  CHECK(client.recoverAvailableByteCount() == 10); // This can return a different number if the server didn't have time to reply anything, hence the wait
  CHECK(client.receive() == "other test");

  client.close();

  server.stop();
  serverThread.join();
}
