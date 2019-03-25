namespace Raz {

namespace Threading {

template <typename Func, typename... Args, typename ResultType>
std::future<ResultType> launchAsync(Func action, Args&&... args) {
  return std::async(std::move(action), std::forward<Args>(args)...);
}

} // namespace Threading

} // namespace Raz
