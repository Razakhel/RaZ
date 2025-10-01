#include "RaZ/RaZ.hpp"

#include <iostream>

int main() {
  try {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    Raz::TcpServer server(1234);
    // The server runs synchronously; starting it in a separate thread to avoid blocking the main one
    std::thread([&server] () {
      Raz::Threading::setCurrentThreadName("Server thread");
      server.start();
    }).detach();

    Raz::TcpClient client("localhost", 1234);

    while (true) {
      std::string request;
      std::cin >> request;

      if (request == "q")
        break;

      const std::string response = client.send(request);
      std::cout << "Received back: " << response << '\n';
    }

    client.close();
    server.stop();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: {}", exception.what());
  }

  return EXIT_SUCCESS;
}
