#pragma once

#ifndef RAZ_THREADING_HPP
#define RAZ_THREADING_HPP

// std::thread is not available on MinGW with Win32 threads
#if !defined(__MINGW32__) || defined(_GLIBCXX_HAS_GTHREADS)
#define RAZ_THREADS_AVAILABLE
#endif

#ifdef RAZ_THREADS_AVAILABLE

#include <functional>
#include <future>
#include <thread>

namespace Raz::Threading {

struct IndexRange {
  std::size_t beginIndex;
  std::size_t endIndex;
};

template <typename ContainerType>
class IterRange {
  using ContainerIter      = typename ContainerType::iterator;
  using ContainerConstIter = typename ContainerType::const_iterator;

public:
  IterRange(ContainerIter begin, ContainerIter end) : m_begin{ begin }, m_end{ end } {}

  ContainerConstIter cbegin() const noexcept { return m_begin; }
  ContainerIter begin() { return m_begin; }
  ContainerConstIter* cend() const noexcept { return m_end; }
  ContainerIter end() { return m_end; }

private:
  ContainerIter m_begin;
  ContainerIter m_end;
};

/// Gets the number of concurrent threads available to the system.
/// This number doesn't necessarily represent the CPU's actual number of threads.
/// \return Number of threads available.
unsigned int getSystemThreadCount();

/// Pauses the current thread for the specified amount of time.
/// \param milliseconds Pause duration in milliseconds.
inline void sleep(uint64_t milliseconds) { std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds)); }

/// Calls a function asynchronously, to be executed without blocking the calling thread.
/// \tparam T Return type of the given function.
/// \tparam Args Types of the arguments to be forwarded to the given function.
/// \param action Action to be performed asynchronously.
/// \param args Arguments to be forwarded to the given function.
/// \return A std::future holding the future result of the process.
template <typename Func, typename... Args, typename ResultType = std::result_of_t<Func&&(Args&&...)>>
std::future<ResultType> launchAsync(Func action, Args&&... args);

/// Calls a function in parallel on a given number of separate threads of execution.
/// \param action Action to be performed by each thread.
/// \param threadCount Amount of threads to start an instance on.
void parallelize(const std::function<void()>& action, std::size_t threadCount = getSystemThreadCount());

/// Calls a function in parallel on a given number of separate threads of execution.
/// The collection is automatically splitted by indices, giving a separate start/end range to each thread.
/// \note The container must either be a constant-size C array or have a size() function.
/// \tparam ContainerType Type of the collection to iterate over.
/// \param collection Collection to iterate over on multiple threads.
/// \param action Action to be performed by each thread, giving an index range as boundaries.
/// \param threadCount Amount of threads to start an instance on.
template <typename ContainerType>
void parallelize(const ContainerType& collection, const std::function<void(IndexRange)>& action, std::size_t threadCount = getSystemThreadCount());

/// Calls a function in parallel on a given number of separate threads of execution.
/// The collection is automatically splitted by iterator ranges, giving a separate start/end range to each thread.
/// \note The container must either be a constant-size C array, or have a public ContainerType::iterator type and begin() & size() functions.
/// \tparam ContainerType Type of the collection to iterate over.
/// \param collection Collection to iterate over on multiple threads.
/// \param action Action to be performed by each thread, giving an iterator range as boundaries.
/// \param threadCount Amount of threads to start an instance on.
template <typename ContainerType>
void parallelize(ContainerType& collection,
                 const std::function<void(IterRange<std::common_type_t<ContainerType>>)>& action,
                 std::size_t threadCount = getSystemThreadCount());

} // namespace Raz::Threading

#include "Threading.inl"

#else
#pragma message("Warning: Threads are not available with your compiler; check that you're using POSIX threads and not Win32 ones.")
#endif // RAZ_THREADS_AVAILABLE

#endif // RAZ_THREADING_HPP
