#if defined(RAZ_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace Raz {

template <typename... Args>
World& Application::addWorld(Args&&... args) {
  m_worlds.emplace_back(std::make_unique<World>(std::forward<Args>(args)...));
  m_activeWorlds.setBit(m_worlds.size() - 1);

  return *m_worlds.back();
}

template <typename FuncT>
void Application::run(FuncT&& callback) {
#if defined(RAZ_PLATFORM_EMSCRIPTEN)
  static auto emCallback = [this, callback = std::forward<FuncT>(callback)] () {
    runOnce();
    callback(m_deltaTime);
  };

  emscripten_set_main_loop_arg([] (void* lambda) {
    (*static_cast<decltype(&emCallback)>(lambda))();
  }, &emCallback, 0, 1);
#else
  while (runOnce())
    callback(m_deltaTime);
#endif
}

} // namespace Raz
