#include "RaZ/RaZ.hpp"

int main() {
  try {
    Raz::Application app;
    Raz::World& world = app.addWorld();

    Raz::Logger::setLoggingLevel(Raz::LoggingLevel::ALL);

    auto& renderSystem  = world.addSystem<Raz::RenderSystem>(1280u, 720u, "RaZ", Raz::WindowSetting::DEFAULT, 2);
    Raz::Window& window = renderSystem.getWindow();

    window.addKeyCallback(Raz::Keyboard::ESCAPE, [&app] (float) noexcept { app.quit(); });
    window.setCloseCallback([&app] () noexcept { app.quit(); });

    Raz::Entity& camera = world.addEntityWithComponent<Raz::Transform>(Raz::Vec3f(0.f, 0.f, 5.f));
    camera.addComponent<Raz::Camera>(renderSystem.getSceneWidth(), renderSystem.getSceneHeight());

    //////////
    // Chat //
    //////////

    Raz::TcpServer server;
    try {
      server.start(1234);

      server.setConnectedCallback([&server] () { server.broadcast("Client connected\n"); });
      server.setDisconnectedCallback([&server] () { server.broadcast("Client disconnected\n"); });
      server.setReceivedCallback([&server] (std::span<const std::byte> data) {
        const std::string_view dataStr(reinterpret_cast<const char*>(data.data()), data.size());
        server.broadcast(dataStr);
        return std::vector<std::byte>();
      });
    } catch (...) {
      // Only the first server needs to be valid. The others will fail to start and throw an exception, which is expected
      // TODO: if the first run stops so will the server, making all other runs unable to send messages
    }

    Raz::TcpClient client("localhost", 1234);

    constexpr Raz::Vec2f origChatSize(500.f, 200.f);
    Raz::OverlayWindow& chatWindow = window.getOverlay().addWindow("Chat", origChatSize, Raz::Vec2f(0.f, static_cast<float>(window.getHeight()) - origChatSize.y()));
    // TODO: the chat history's text area is currently editable as there's no exposed option to make it otherwise
    Raz::OverlayTextArea& chatHistory = chatWindow.addTextArea(" ", [] (const std::string&) noexcept {}, {}, origChatSize.y() - 82.f);
    Raz::OverlayTextbox& chatMessage  = chatWindow.addTextbox("Message", [] (const std::string&) noexcept {});
    chatWindow.addButton("Send", [&client, &chatMessage] () {
      client.send("X: " + chatMessage.getText() + '\n');
      chatMessage.clear();
    });

    app.run([&client, &chatHistory] (const Raz::FrameTimeInfo&) {
      // TODO: add "on received" callback(s) to the client
      if (client.recoverAvailableByteCount() > 0)
        chatHistory += client.receiveUntil("\n");
    });

    client.disconnect();
  } catch (const std::exception& exception) {
    Raz::Logger::error("Exception occurred: {}", exception.what());
  }

  return EXIT_SUCCESS;
}
