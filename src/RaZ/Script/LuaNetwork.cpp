#include "RaZ/Network/TcpClient.hpp"
#include "RaZ/Network/TcpServer.hpp"
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
    tcpClient["connect"]                   = &TcpClient::connect;
    tcpClient["send"]                      = &TcpClient::send;
    tcpClient["recoverAvailableByteCount"] = &TcpClient::recoverAvailableByteCount;
    tcpClient["receive"]                   = &TcpClient::receive;
    tcpClient["close"]                     = &TcpClient::close;
  }

  {
    sol::usertype<TcpServer> tcpServer = state.new_usertype<TcpServer>("TcpServer",
                                                                       sol::constructors<TcpServer()>());
    tcpServer["start"] = &TcpServer::start;
    tcpServer["stop"]  = &TcpServer::stop;
  }
}

} // namespace Raz
