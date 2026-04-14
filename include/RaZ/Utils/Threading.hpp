#pragma once

#ifndef RAZ_THREADING_HPP
#define RAZ_THREADING_HPP

#include "RaZ/Utils/ThreadPool.hpp"

#include <functional>
#include <future>

namespace Raz::Threading {

struct IndexRange {
  std::size_t beginIndex;
  std::size_t endIndex;
};

template <std::input_iterator IterT>
class IterRange {
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

/// Sets the name of the current thread.
/// \param name Name to assign to the current thread.
void setCurrentThreadName(const std::string& name);

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
/// \param action Action to be performed in parallel.
/// \param taskCount Number of tasks to start.
void parallelize(const std::function<void()>& action, unsigned int taskCount = getSystemThreadCount());

/// Calls the given functions in parallel.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \param actions Actions to be performed in parallel.
void parallelize(std::initializer_list<std::function<void()>> actions);

/// Calls a function in parallel over an index range.
/// The given range is automatically split, providing a separate start/past-the-end subrange to each task.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam BegIndexT Type of the begin index.
/// \tparam EndIndexT Type of the end index.
/// \tparam FuncT Type of the action to be executed.
/// \param beginIndex Starting index of the whole range. Must be lower than the end index.
/// \param endIndex Past-the-last index of the whole range. Must be greater than the begin index.
/// \param action Action to be performed in parallel, taking an index range as boundaries.
/// \param threadPool Thread pool to enqueue tasks into.
/// \param taskCount Number of tasks to start.
template <std::integral BegIndexT, std::integral EndIndexT, typename FuncT>
void parallelize(BegIndexT beginIndex, EndIndexT endIndex, const FuncT& action, ThreadPool& threadPool, unsigned int taskCount);

/// Calls a function in parallel over an index range.
/// The given range is automatically split, providing a separate start/past-the-end subrange to each task.
/// There will be as many tasks spawned as there are threads in the given thread pool.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam BegIndexT Type of the begin index.
/// \tparam EndIndexT Type of the end index.
/// \tparam FuncT Type of the action to be executed.
/// \param beginIndex Starting index of the whole range. Must be lower than the end index.
/// \param endIndex Past-the-last index of the whole range. Must be greater than the begin index.
/// \param action Action to be performed in parallel, taking an index range as boundaries.
/// \param threadPool Thread pool to enqueue tasks into.
template <std::integral BegIndexT, std::integral EndIndexT, typename FuncT>
void parallelize(BegIndexT beginIndex, EndIndexT endIndex, const FuncT& action, ThreadPool& threadPool = getDefaultThreadPool()) {
  parallelize(beginIndex, endIndex, action, threadPool, threadPool.getThreadCount());
}

/// Calls a function in parallel over an iterator range.
/// The given range is automatically split, providing a separate start/past-the-end subrange to each task.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam IterT Type of the iterators.
/// \tparam FuncT Type of the action to be executed.
/// \param begin Begin iterator of the whole range. Must be lower than the end iterator.
/// \param end End iterator of the whole range. Must be greater than the begin iterator.
/// \param action Action to be performed in parallel, taking an iterator range as boundaries.
/// \param threadPool Thread pool to enqueue tasks into.
/// \param taskCount Number of tasks to start.
template <std::input_iterator IterT, typename FuncT>
void parallelize(IterT begin, IterT end, const FuncT& action, ThreadPool& threadPool, unsigned int taskCount);

/// Calls a function in parallel over an iterator range.
/// The given range is automatically split, providing a separate start/past-the-end subrange to each task.
/// There will be as many tasks spawned as there are threads in the given thread pool.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam IterT Type of the iterators.
/// \tparam FuncT Type of the action to be executed.
/// \param begin Begin iterator of the whole range. Must be lower than the end iterator.
/// \param end End iterator of the whole range. Must be greater than the begin iterator.
/// \param action Action to be performed in parallel, taking an iterator range as boundaries.
/// \param threadPool Thread pool to enqueue tasks into.
template <std::input_iterator IterT, typename FuncT>
void parallelize(IterT begin, IterT end, const FuncT& action, ThreadPool& threadPool = getDefaultThreadPool()) {
  parallelize(begin, end, action, threadPool, threadPool.getThreadCount());
}

/// Calls a function in parallel over a collection.
/// The given collection is automatically split, providing a separate start/past-the-end subrange to each task.
/// \note The container must either be a constant-size C array or have public begin() & end() functions.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam ContainerT Type of the collection to iterate over.
/// \tparam FuncT Type of the action to be executed.
/// \param collection Collection to iterate over in parallel.
/// \param action Action to be performed in parallel, taking an iterator range as boundaries.
/// \param threadPool Thread pool to enqueue tasks into.
/// \param taskCount Number of tasks to start.
template <std::ranges::input_range ContainerT, typename FuncT>
void parallelize(ContainerT&& collection, const FuncT& action, ThreadPool& threadPool, unsigned int taskCount) {
  parallelize(std::begin(collection), std::end(collection), action, threadPool, taskCount);
}

/// Calls a function in parallel over a collection.
/// The given collection is automatically split, providing a separate start/past-the-end subrange to each task.
/// There will be as many tasks spawned as there are threads in the given thread pool.
/// \note The container must either be a constant-size C array or have public begin() & end() functions.
/// \note If using Emscripten this call will be synchronous, threads being unsupported with it for now.
/// \tparam ContainerT Type of the collection to iterate over.
/// \tparam FuncT Type of the action to be executed.
/// \param collection Collection to iterate over in parallel.
/// \param action Action to be performed in parallel, taking an iterator range as boundaries.
/// \param threadPool Thread pool to enqueue tasks into.
template <std::ranges::input_range ContainerT, typename FuncT>
void parallelize(ContainerT&& collection, const FuncT& action, ThreadPool& threadPool = getDefaultThreadPool()) {
  parallelize(std::begin(collection), std::end(collection), action, threadPool);
}

/// Calls a function in parallel over an index range, then merges (reduces) their results sequentially into a single one.
/// The given range is automatically split, providing a separate start/past-the-end subrange to each task.
/// \tparam BegIndexT Type of the begin index.
/// \tparam EndIndexT Type of the end index.
/// \tparam ParallelFuncT Type of the parallelization action to be executed. Must return the type to be reduced.
/// \tparam ReduceFuncT Type of the reduce action to be executed.
/// \param beginIndex Starting index of the whole range. Must be lower than the end index.
/// \param endIndex Past-the-last index of the whole range. Must be greater than the begin index.
/// \param action Action to be performed in parallel, taking an index range as boundaries and returning a value.
/// \param reduce Action to be performed sequentially, taking two parallel results and returning their reduction.
/// \param threadPool Thread pool to enqueue tasks into.
/// \param taskCount Number of tasks to start.
/// \return Final result of the reduction steps.
template <std::integral BegIndexT, std::integral EndIndexT, typename ParallelFuncT, typename ReduceFuncT>
auto parallelizeReduce(BegIndexT beginIndex, EndIndexT endIndex,
                       const ParallelFuncT& action, const ReduceFuncT& reduce,
                       ThreadPool& threadPool, unsigned int taskCount);

/// Calls a function in parallel over an index range, then merges (reduces) their results sequentially into a single one.
/// The given range is automatically split, providing a separate start/past-the-end subrange to each task.
/// There will be as many tasks spawned as there are threads in the given thread pool.
/// \tparam BegIndexT Type of the begin index.
/// \tparam EndIndexT Type of the end index.
/// \tparam ParallelFuncT Type of the parallelization action to be executed. Must return the type to be reduced.
/// \tparam ReduceFuncT Type of the reduce action to be executed.
/// \param beginIndex Starting index of the whole range. Must be lower than the end index.
/// \param endIndex Past-the-last index of the whole range. Must be greater than the begin index.
/// \param action Action to be performed in parallel, taking an index range as boundaries and returning a value.
/// \param reduce Action to be performed sequentially, taking two parallel results and returning their reduction.
/// \param threadPool Thread pool to enqueue tasks into.
/// \return Final result of the reduction steps.
template <std::integral BegIndexT, std::integral EndIndexT, typename ParallelFuncT, typename ReduceFuncT>
auto parallelizeReduce(BegIndexT beginIndex, EndIndexT endIndex,
                       const ParallelFuncT& action, const ReduceFuncT& reduce,
                       ThreadPool& threadPool = getDefaultThreadPool()) {
  return parallelizeReduce(beginIndex, endIndex, action, reduce, threadPool, threadPool.getThreadCount());
}

/// Calls a function in parallel over an iterator range, then merges (reduces) their results sequentially into a single one.
/// The given range is automatically split, providing a separate start/past-the-end subrange to each task.
/// \tparam IterT Type of the iterators.
/// \tparam ParallelFuncT Type of the parallelization action to be executed. Must return the type to be reduced.
/// \tparam ReduceFuncT Type of the reduce action to be executed.
/// \param begin Begin iterator of the whole range. Must be lower than the end iterator.
/// \param end End iterator of the whole range. Must be greater than the begin iterator.
/// \param action Action to be performed in parallel, taking an iterator range as boundaries and returning a value.
/// \param reduce Action to be performed sequentially, taking two parallel results and returning their reduction.
/// \param threadPool Thread pool to enqueue tasks into.
/// \param taskCount Number of tasks to start.
/// \return Final result of the reduction steps.
template <std::input_iterator IterT, typename ParallelFuncT, typename ReduceFuncT>
auto parallelizeReduce(IterT begin, IterT end, const ParallelFuncT& action, const ReduceFuncT& reduce, ThreadPool& threadPool, unsigned int taskCount);

/// Calls a function in parallel over an iterator range, then merges (reduces) their results sequentially into a single one.
/// The given range is automatically split, providing a separate start/past-the-end subrange to each task.
/// There will be as many tasks spawned as there are threads in the given thread pool.
/// \tparam IterT Type of the iterators.
/// \tparam ParallelFuncT Type of the parallelization action to be executed. Must return the type to be reduced.
/// \tparam ReduceFuncT Type of the reduce action to be executed.
/// \param begin Begin iterator of the whole range. Must be lower than the end iterator.
/// \param end End iterator of the whole range. Must be greater than the begin iterator.
/// \param action Action to be performed in parallel, taking an iterator range as boundaries and returning a value.
/// \param reduce Action to be performed sequentially, taking two parallel results and returning their reduction.
/// \param threadPool Thread pool to enqueue tasks into.
/// \return Final result of the reduction steps.
template <std::input_iterator IterT, typename ParallelFuncT, typename ReduceFuncT>
auto parallelizeReduce(IterT begin, IterT end, const ParallelFuncT& action, const ReduceFuncT& reduce, ThreadPool& threadPool = getDefaultThreadPool()) {
  return parallelizeReduce(begin, end, action, reduce, threadPool, threadPool.getThreadCount());
}

/// Calls a function in parallel over a collection, then merges (reduces) their results sequentially into a single one.
/// The given collection is automatically split, providing a separate start/past-the-end subrange to each task.
/// \tparam ContainerT Type of the collection to iterate over.
/// \tparam ParallelFuncT Type of the parallelization action to be executed. Must return the type to be reduced.
/// \tparam ReduceFuncT Type of the reduce action to be executed.
/// \param collection Collection to iterate over in parallel.
/// \param action Action to be performed in parallel, taking an iterator range as boundaries and returning a value.
/// \param reduce Action to be performed sequentially, taking two parallel results and returning their reduction.
/// \param threadPool Thread pool to enqueue tasks into.
/// \param taskCount Number of tasks to start.
/// \return Final result of the reduction steps.
template <std::ranges::input_range ContainerT, typename ParallelFuncT, typename ReduceFuncT>
auto parallelizeReduce(ContainerT&& collection, const ParallelFuncT& action, const ReduceFuncT& reduce, ThreadPool& threadPool, unsigned int taskCount) {
  return parallelizeReduce(std::begin(collection), std::end(collection), action, reduce, threadPool, taskCount);
}

/// Calls a function in parallel over a collection, then merges (reduces) their results sequentially into a single one.
/// The given collection is automatically split, providing a separate start/past-the-end subrange to each task.
/// There will be as many tasks spawned as there are threads in the given thread pool.
/// \tparam ContainerT Type of the collection to iterate over.
/// \tparam ParallelFuncT Type of the parallelization action to be executed. Must return the type to be reduced.
/// \tparam ReduceFuncT Type of the reduce action to be executed.
/// \param collection Collection to iterate over in parallel.
/// \param action Action to be performed in parallel, taking an iterator range as boundaries and returning a value.
/// \param reduce Action to be performed sequentially, taking two parallel results and returning their reduction.
/// \param threadPool Thread pool to enqueue tasks into.
/// \return Final result of the reduction steps.
template <std::ranges::input_range ContainerT, typename ParallelFuncT, typename ReduceFuncT>
auto parallelizeReduce(ContainerT&& collection, const ParallelFuncT& action, const ReduceFuncT& reduce, ThreadPool& threadPool = getDefaultThreadPool()) {
  return parallelizeReduce(std::begin(collection), std::end(collection), action, reduce, threadPool);
}

} // namespace Raz::Threading

#include "Threading.inl"

#endif // RAZ_THREADING_HPP
