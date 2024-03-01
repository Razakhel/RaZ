#pragma once

#ifndef RAZ_THREADING_HPP
#define RAZ_THREADING_HPP

// <thread> must be included first, since it creates the definition checked below
#include <thread>

// std::thread is not available on MinGW with Win32 threads
#if defined(__MINGW32__) && !defined(_GLIBCXX_HAS_GTHREADS)
#pragma message("Warning: Threads are not available with your compiler; check that you're using POSIX threads and not Win32 ones.")
#else
#define RAZ_THREADS_AVAILABLE
#endif

#if defined(RAZ_THREADS_AVAILABLE)

#include <functional>
#include <future>

namespace Raz {

class ThreadPool;

namespace Threading {

struct IndexRange {
  std::size_t beginIndex;
  std::size_t endIndex;
};

template <typename IterT, typename = void>
class IterRange {};

template <typename IterT>
class IterRange<IterT, std::void_t<typename std::iterator_traits<IterT>::iterator_category>> {
public:
  IterRange(IterT begin, IterT end) noexcept : m_begin{ begin }, m_end{ end } {}

  IterT begin() const noexcept { return m_begin; }
  IterT end() const noexcept { return m_end; }

private:
  IterT m_begin;
  IterT m_end;
};

/// Gets the number of concurrent threads available to the system.
/// This number doesn't necessarily represent the CPU's actual number of threads.
/// \return Number of threads available.
inline unsigned int getSystemThreadCount() noexcept { return std::max(std::thread::hardware_concurrency(), 1u); }

/// Gets the default thread pool, initialized with the default number of threads (defined by getSystemThreadCount()).
/// \return Reference to the default thread pool.
ThreadPool& getDefaultThreadPool();

/// Pauses the current thread for the specified amount of time.
/// \param milliseconds Pause duration in milliseconds.
inline void sleep(uint64_t milliseconds) { std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds)); }

/// Calls a function asynchronously, to be executed without blocking the calling thread.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam FuncT Type of the action to be executed.
/// \tparam Args Types of the arguments to be forwarded to the given function.
/// \tparam ResultT Return type of the given function.
/// \param action Action to be performed asynchronously.
/// \param args Arguments to be forwarded to the given function.
/// \return A std::future holding the future result of the process. This should not be discarded, as doing so will fail to run asynchronously.
template <typename FuncT, typename... Args, typename ResultT = std::invoke_result_t<FuncT&&, Args&&...>>
[[nodiscard]] std::future<ResultT> launchAsync(FuncT&& action, Args&&... args);

/// Calls a function in parallel.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \param action Action to be performed by each thread.
/// \param threadCount Amount of threads to start an instance on.
void parallelize(const std::function<void()>& action, unsigned int threadCount = getSystemThreadCount());

/// Calls the given functions in parallel, each on its own thread.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \param actions Actions to be performed in parallel.
void parallelize(std::initializer_list<std::function<void()>> actions);

/// Calls a function in parallel over an index range.
/// The indices are automatically split, giving a separate start/past-the-end index range to each thread.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam BegIndexT Type of the begin index.
/// \tparam EndIndexT Type of the end index.
/// \tparam FuncT Type of the action to be executed.
/// \param beginIndex Starting index of the whole range. Must be lower than the end index.
/// \param endIndex Past-the-last index of the whole range. Must be greater than the begin index.
/// \param action Action to be performed by each thread, taking an index range as boundaries.
/// \param threadCount Amount of threads to start an instance on.
template <typename BegIndexT, typename EndIndexT, typename FuncT, typename = std::enable_if_t<std::is_integral_v<BegIndexT> && std::is_integral_v<EndIndexT>>>
void parallelize(BegIndexT beginIndex, EndIndexT endIndex, const FuncT& action, unsigned int threadCount = getSystemThreadCount());

/// Calls a function in parallel over an iterator range.
/// The iterators are automatically split, giving a separate start/past-the-end iterator range to each thread.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam IterT Type of the iterators.
/// \tparam FuncT Type of the action to be executed.
/// \param begin Begin iterator of the whole range. Must be lower than the end iterator.
/// \param end End iterator of the whole range. Must be greater than the begin iterator.
/// \param action Action to be performed by each thread, taking an iterator range as boundaries.
/// \param threadCount Amount of threads to start an instance on.
template <typename IterT, typename FuncT, typename = typename std::iterator_traits<IterT>::iterator_category>
void parallelize(IterT begin, IterT end, const FuncT& action, unsigned int threadCount = getSystemThreadCount());

/// Calls a function in parallel over a collection.
/// The collection is automatically split, giving a separate start/past-the-end iterator range to each thread.
/// \note The container must either be a constant-size C array, or have public begin() & end() functions.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam ContainerT Type of the collection to iterate over.
/// \tparam FuncT Type of the action to be executed.
/// \param collection Collection to iterate over on multiple threads.
/// \param action Action to be performed by each thread, taking an iterator range as boundaries.
/// \param threadCount Amount of threads to start an instance on.
template <typename ContainerT, typename FuncT, typename = decltype(std::begin(std::declval<ContainerT>()))>
void parallelize(ContainerT&& collection, FuncT&& action, unsigned int threadCount = getSystemThreadCount()) {
  parallelize(std::begin(collection), std::end(collection), std::forward<FuncT>(action), threadCount);
}

} // namespace Threading

} // namespace Raz

#include "Threading.inl"

#endif // RAZ_THREADS_AVAILABLE

#endif // RAZ_THREADING_HPP
