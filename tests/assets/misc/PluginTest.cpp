#if defined(_WIN32)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

extern "C" {

EXPORT void dummy() {}
EXPORT int multByTwo(int arg) { return arg * 2; }

} // extern "C"
