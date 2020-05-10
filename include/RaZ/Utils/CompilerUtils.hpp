#pragma once

#ifndef RAZ_COMPILERUTILS_HPP
#define RAZ_COMPILERUTILS_HPP

#if defined(RAZ_COMPILER_MSVC)
/// Declares a pragma directive, passing it the given argument.
/// \param Arg Argument to be passed to the pragma directive.
#define DO_PRAGMA(Arg) __pragma(Arg)

/// Saves the warnings' state to be reapplied later.
/// \see POP_WARNINGS_STATE.
#define PUSH_WARNINGS_STATE DO_PRAGMA(warning(push))

/// Reapplies the previously pushed warnings' state.
/// \see PUSH_WARNINGS_STATE.
#define POP_WARNINGS_STATE DO_PRAGMA(warning(pop))

/// Disables the given MSVC warning; must be a 4 digits number.
/// \note Make sure the warnings' state has been pushed before.
/// \param Warning ID of the warning to disable.
#define DISABLE_WARNING(Warning) DO_PRAGMA(warning(disable : Warning))
#elif defined(RAZ_COMPILER_GCC) || defined(RAZ_COMPILER_CLANG)
/// Declares a pragma directive, passing it the given argument.
/// \param Arg Argument to be passed to the pragma directive.
#define DO_PRAGMA(Arg) _Pragma(#Arg)

/// Saves the warnings' state to be reapplied later.
/// \see POP_WARNINGS_STATE.
#define PUSH_WARNINGS_STATE DO_PRAGMA(GCC diagnostic push)

/// Reapplies the previously pushed warnings' state.
/// \see PUSH_WARNINGS_STATE.
#define POP_WARNINGS_STATE DO_PRAGMA(GCC diagnostic pop)

/// Disables the given GCC/Clang warning; must be an unquoted string starting with -W.
/// \note Make sure the warnings' state has been pushed before.
/// \param Warning String literal of the warning to disable.
#define DISABLE_WARNING(Warning) DO_PRAGMA(GCC diagnostic ignored #Warning)
#else
/// Declares a pragma directive, passing it the given argument.
/// \param Arg Argument to be passed to the pragma directive.
#define DO_PRAGMA(Arg)
/// Saves the warnings' state to be reapplied later.
/// \see POP_WARNINGS_STATE.
#define PUSH_WARNINGS_STATE
/// Reapplies the previously pushed warnings' state.
/// \see PUSH_WARNINGS_STATE.
#define POP_WARNINGS_STATE
/// Disables the given compiler-specific warning.
/// \note Make sure the warnings' state has been pushed before.
/// \see PUSH_WARNINGS_STATE.
/// \param Warning Warning to disable.
#define DISABLE_WARNING(Warning)
#endif

#endif // RAZ_COMPILERUTILS_HPP
