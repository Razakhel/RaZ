#if !defined(_MSC_VER) || !defined(__SANITIZE_ADDRESS__) // MSVC's ASan redefines the new & delete operators, which end up being in conflict with those

#include <tracy/Tracy.hpp>

// See https://en.cppreference.com/w/cpp/memory/new/operator_new

void* operator new(std::size_t size) {
  size = std::max(static_cast<std::size_t>(1), size);

  if (void* ptr = std::malloc(size)) {
    TracyAlloc(ptr, size);
    return ptr;
  }

  throw std::bad_alloc();
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
  try {
      return operator new(size);
  } catch (...) {
      return nullptr;
  }
}

void* operator new[](std::size_t size) {
  size = std::max(static_cast<std::size_t>(1), size);

  if (void* ptr = std::malloc(size)) {
    TracyAlloc(ptr, size);
    return ptr;
  }

  throw std::bad_alloc();
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept {
  try {
      return operator new[](size);
  } catch (...) {
      return nullptr;
  }
}

// See https://en.cppreference.com/w/cpp/memory/new/operator_delete

void operator delete(void* ptr) noexcept {
  TracyFree(ptr);
  std::free(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept {
  operator delete(ptr);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept {
  operator delete(ptr);
}

void operator delete[](void* ptr) noexcept {
  TracyFree(ptr);
  std::free(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept {
  operator delete[](ptr);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept {
  operator delete[](ptr);
}

#endif
