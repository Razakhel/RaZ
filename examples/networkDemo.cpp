#include "RaZ/RaZ.hpp"

#include <iostream>

int main() {
  try {
    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    Raz::TcpServer server(1234);
    Raz::TcpClient client("localhost", 1234);

    server.setConnectedCallback([] () { Raz::Logger::info("Client connected"); });
    server.setDisconnectedCallback([] () { Raz::Logger::info("Client disconnected"); });

    while (true) {
      std::cout << "Write text to be sent ('q' to quit):\n";
      std::string request;
      std::cin >> request;

      if (request == "q")
        break;

      client.send(request);
      std::cout << std::format("Received back: {}\n", client.receive());
    }

    client.disconnect();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: {}", exception.what());
  }

  return EXIT_SUCCESS;
}
