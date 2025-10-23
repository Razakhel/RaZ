#include "RaZ/Network/TcpClient.hpp"
#include "RaZ/Network/TcpServer.hpp"
#include "RaZ/Network/UdpClient.hpp"
#include "RaZ/Network/UdpServer.hpp"
#include "RaZ/Script/LuaWrapper.hpp"

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Raz {

void LuaWrapper::registerNetworkTypes() {
  sol::state& state = getState();

  {
    sol::usertype<TcpClient> tcpClient = state.new_usertype<TcpClient>("TcpClient",
                                                                       sol::constructors<TcpClient(),
                                                                                         TcpClient(const std::string&, unsigned short)>());
    tcpClient["isConnected"]               = &TcpClient::isConnected;
    tcpClient["connect"]                   = &TcpClient::connect;
    tcpClient["send"]                      = &TcpClient::send;
    tcpClient["recoverAvailableByteCount"] = &TcpClient::recoverAvailableByteCount;
    tcpClient["receive"]                   = &TcpClient::receive;
    tcpClient["disconnect"]                = &TcpClient::disconnect;
  }

  {
    sol::usertype<TcpServer> tcpServer = state.new_usertype<TcpServer>("TcpServer",
                                                                       sol::constructors<TcpServer()>());
    tcpServer["start"] = &TcpServer::start;
    tcpServer["stop"]  = &TcpServer::stop;
  }

  {
    sol::usertype<UdpClient> udpClient = state.new_usertype<UdpClient>("UdpClient",
                                                                       sol::constructors<UdpClient()>());
    udpClient["setDestination"]            = &UdpClient::setDestination;
    udpClient["send"]                      = &UdpClient::send;
    udpClient["recoverAvailableByteCount"] = &UdpClient::recoverAvailableByteCount;
    udpClient["receive"]                   = &UdpClient::receive;
    udpClient["close"]                     = &UdpClient::close;
  }

  {
    sol::usertype<UdpServer> udpServer = state.new_usertype<UdpServer>("UdpServer",
                                                                       sol::constructors<UdpServer()>());
    udpServer["start"] = &UdpServer::start;
    udpServer["stop"]  = &UdpServer::stop;
  }
}

} // namespace Raz
