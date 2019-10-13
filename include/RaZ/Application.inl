#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace Raz {

template <typename F>
void Application::run(F&& callback) {

#ifdef __EMSCRIPTEN__
  auto emCallback = [callback=std::forward<F>(callback), this] {
    runOnce();
    callback();
  };

  emscripten_set_main_loop_arg([](void* lambda){
    (*static_cast<decltype(&emCallback)>(lambda))();
  }, &emCallback, 0, 1);
#else
  while(runOnce()) {
    std::forward<F>(callback)();
  }
#endif
}

} // namespace Raz
