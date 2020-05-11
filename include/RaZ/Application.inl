#if defined(RAZ_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#endif

namespace Raz {

template <typename F>
void Application::run(F&& callback) {
#if defined(RAZ_PLATFORM_EMSCRIPTEN)
  auto emCallback = [callback = std::forward<F>(callback), this] {
    runOnce();
    callback();
  };

  emscripten_set_main_loop_arg([] (void* lambda) {
    (*static_cast<decltype(&emCallback)>(lambda))();
  }, &emCallback, 0, 1);
#else
  while (runOnce())
    callback();
#endif
}

} // namespace Raz
