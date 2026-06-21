#include "RaZ/Network/HttpClient.hpp"
#include "RaZ/Network/TcpClient.hpp"
#include "RaZ/Network/TcpServer.hpp"
#include "RaZ/Network/UdpClient.hpp"
#include "RaZ/Network/UdpServer.hpp"
#include "RaZ/Script/LuaWrapper.hpp"
#include "RaZ/Utils/TypeUtils.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

using namespace TypeUtils;

void LuaWrapper::registerNetworkTypes() {
  sol::state& state = getState();

  {
    sol::usertype<HttpClient> httpClient = state.new_usertype<HttpClient>("HttpClient",
                                                                          sol::constructors<HttpClient(),
                                                                                            HttpClient(std::string)>());
    httpClient["connect"]    = &HttpClient::connect;
    httpClient["get"]        = &HttpClient::get;
    httpClient["disconnect"] = &HttpClient::disconnect;
  }

  {
    sol::usertype<TcpClient> tcpClient = state.new_usertype<TcpClient>("TcpClient",
                                                                       sol::constructors<TcpClient(),
                                                                                         TcpClient(const std::string&, unsigned short)>());
    tcpClient["isConnected"]               = &TcpClient::isConnected;
    tcpClient["connect"]                   = &TcpClient::connect;
    tcpClient["send"]                      = &TcpClient::send;
    tcpClient["recoverAvailableByteCount"] = &TcpClient::recoverAvailableByteCount;
    tcpClient["receive"]                   = sol::overload([] (TcpClient& c) { return c.receive(); },
                                                           PickOverload<bool>(&TcpClient::receive));
    tcpClient["receiveAtLeast"]            = sol::overload([] (TcpClient& c, std::size_t b) { return c.receiveAtLeast(b); },
                                                           PickOverload<std::size_t, bool>(&TcpClient::receiveAtLeast));
    tcpClient["receiveExactly"]            = sol::overload([] (TcpClient& c, std::size_t b) { return c.receiveExactly(b); },
                                                           PickOverload<std::size_t, bool>(&TcpClient::receiveExactly));
    tcpClient["receiveUntil"]              = sol::overload([] (TcpClient& c, std::string_view d) { return c.receiveUntil(d); },
                                                           PickOverload<std::string_view, bool>(&TcpClient::receiveUntil));
    tcpClient["disconnect"]                = &TcpClient::disconnect;
  }

  {
    sol::usertype<TcpServer> tcpServer = state.new_usertype<TcpServer>("TcpServer",
                                                                       sol::constructors<TcpServer(),
                                                                                         TcpServer(unsigned short)>());
    tcpServer["isRunning"]               = &TcpServer::isRunning;
    tcpServer["setConnectedCallback"]    = &TcpServer::setConnectedCallback;
    tcpServer["setDisconnectedCallback"] = &TcpServer::setDisconnectedCallback;
    tcpServer["setReceivedCallback"]     = &TcpServer::setReceivedCallback;
    tcpServer["start"]                   = &TcpServer::start;
    tcpServer["broadcast"]               = PickOverload<std::string_view>(&TcpServer::broadcast);
    tcpServer["stop"]                    = &TcpServer::stop;
  }

  {
    sol::usertype<UdpClient> udpClient = state.new_usertype<UdpClient>("UdpClient",
                                                                       sol::constructors<UdpClient(),
                                                                                         UdpClient(const std::string&, unsigned short)>());
    udpClient["setDestination"]            = &UdpClient::setDestination;
    udpClient["send"]                      = &UdpClient::send;
    udpClient["recoverAvailableByteCount"] = &UdpClient::recoverAvailableByteCount;
    udpClient["receive"]                   = &UdpClient::receive;
    udpClient["close"]                     = &UdpClient::close;
  }

  {
    sol::usertype<UdpServer> udpServer = state.new_usertype<UdpServer>("UdpServer",
                                                                       sol::constructors<UdpServer(),
                                                                                         UdpServer(unsigned short)>());
    udpServer["isRunning"] = &UdpServer::isRunning;
    udpServer["start"]     = &UdpServer::start;
    udpServer["stop"]      = &UdpServer::stop;
  }
}

} // namespace Raz
