#include "TestUtils.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("LuaNetwork HttpClient", "[script][lua][network]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local httpClient = HttpClient.new()
    --httpClient       = HttpClient.new("localhost") -- Throws as there's no available server

    --httpClient:connect("localhost") -- Throws as there's no available server
    -- Request functions throw when not connected
    --httpClient:get("/")
    httpClient:disconnect()
  )"));
}

TEST_CASE("LuaNetwork TcpClient", "[script][lua][network]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local tcpClient = TcpClient.new()
    --tcpClient       = TcpClient.new("localhost", 1234) -- Throws as there's no available server

    --tcpClient:connect("localhost", 1234) -- Throws as there's no available server
    assert(not tcpClient:isConnected())
    -- Data transfer functions throw when not connected
    --tcpClient:send("data")
    --tcpClient:recoverAvailableByteCount()
    --tcpClient:receive()
    --tcpClient:receiveAtLeast(1)
    --tcpClient:receiveExactly(1)
    --tcpClient:receiveUntil("\0")
    tcpClient:disconnect()
  )"));
}

TEST_CASE("LuaNetwork TcpServer", "[script][lua][network]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local tcpServer = TcpServer.new()
    tcpServer       = TcpServer.new(1234)

    assert(tcpServer:isRunning())
    tcpServer:setConnectedCallback(function () end)
    tcpServer:setDisconnectedCallback(function () end)
    tcpServer:setReceivedCallback(function () end)
    tcpServer:start(1234)
    tcpServer:broadcast("test")
    tcpServer:stop()
  )"));
}

TEST_CASE("LuaNetwork UdpClient", "[script][lua][network]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local udpClient = UdpClient.new()
    udpClient       = UdpClient.new("localhost", 1234)

    udpClient:setDestination("localhost", 1234)
    udpClient:send("data")
    udpClient:recoverAvailableByteCount()
    --udpClient:receive() -- Can't receive data without an active connection
    udpClient:close()
  )"));
}

TEST_CASE("LuaNetwork UdpServer", "[script][lua][network]") {
  CHECK(TestUtils::executeLuaScript(R"(
    local udpServer = UdpServer.new()
    udpServer       = UdpServer.new(1234)

    assert(udpServer:isRunning())
    udpServer:start(1234)
    udpServer:stop()
  )"));
}
