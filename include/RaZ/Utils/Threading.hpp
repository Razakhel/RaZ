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

template <typename ContainerT, typename = void>
class IterRange {};

template <typename ContainerT>
class IterRange<ContainerT, std::void_t<typename ContainerT::iterator, typename ContainerT::const_iterator>> {
  using ContainerIter      = typename ContainerT::iterator;
  using ContainerConstIter = typename ContainerT::const_iterator;

public:
  IterRange(ContainerIter begin, ContainerIter end) : m_begin{ begin }, m_end{ end } {}

  ContainerConstIter cbegin() const noexcept { return m_begin; }
  ContainerIter begin() const noexcept { return m_begin; }
  ContainerConstIter cend() const noexcept { return m_end; }
  ContainerIter end() const noexcept { return m_end; }

private:
  ContainerIter m_begin;
  ContainerIter m_end;
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

/// Calls a function in parallel on a given number of separate threads of execution.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \param action Action to be performed by each thread.
/// \param threadCount Amount of threads to start an instance on.
void parallelize(const std::function<void()>& action, unsigned int threadCount = getSystemThreadCount());

/// Calls the given functions in parallel, each on its own thread.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \param actions Actions to be performed in parallel.
void parallelize(std::initializer_list<std::function<void()>> actions);

/// Calls a function in parallel on a given number of separate threads of execution.
/// The collection is automatically split by indices, giving a separate start/past-the-end range to each thread.
/// \note The container must either be a constant-size C array or have a size() function.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam ContainerT Type of the collection to iterate over.
/// \tparam FuncT Type of the action to be executed.
/// \param collection Collection to iterate over on multiple threads.
/// \param action Action to be performed by each thread, giving an index range as boundaries.
/// \param threadCount Amount of threads to start an instance on.
template <typename ContainerT, typename FuncT, typename = std::enable_if_t<std::is_invocable_v<FuncT, IndexRange>>>
void parallelize(const ContainerT& collection, FuncT&& action, unsigned int threadCount = getSystemThreadCount());

/// Calls a function in parallel on a given number of separate threads of execution.
/// The collection is automatically split by iterator ranges, giving a separate start/past-the-end range to each thread.
/// \note The container must either be a constant-size C array, or have a public ContainerT::iterator type and begin() & size() functions.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam ContainerT Type of the collection to iterate over.
/// \tparam FuncT Type of the action to be executed.
/// \param collection Collection to iterate over on multiple threads.
/// \param action Action to be performed by each thread, giving an iterator range as boundaries.
/// \param threadCount Amount of threads to start an instance on.
template <typename ContainerT, typename FuncT, typename = std::enable_if_t<std::is_invocable_v<FuncT, IterRange<ContainerT>>>>
void parallelize(ContainerT& collection, FuncT&& action, unsigned int threadCount = getSystemThreadCount());

} // namespace Threading

} // namespace Raz

#include "Threading.inl"

#endif // RAZ_THREADS_AVAILABLE

#endif // RAZ_THREADING_HPP
