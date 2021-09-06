#pragma once

#ifndef RAZ_COMPILERUTILS_HPP
#define RAZ_COMPILERUTILS_HPP

#if defined(_MSC_VER)
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

/// Disables the given MSVC warning; must be a 4 digits number.
/// \note Make sure the warnings' state has been pushed before.
/// \param Warning ID of the warning to disable.
#define DISABLE_WARNING_MSVC(Warning) DISABLE_WARNING(Warning)

#define DISABLE_WARNING_GCC(Warning)
#define DISABLE_WARNING_CLANG(Warning)
#elif defined(__GNUC__) || defined(__clang__)
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
/// \param Warning Enquoted name of the warning to disable.
#define DISABLE_WARNING(Warning) DO_PRAGMA(GCC diagnostic ignored #Warning)

#if defined(__clang__)
/// Disables the given Clang warning; must be an unquoted string starting with -W.
/// \note Make sure the warnings' state has been pushed before.
/// \param Warning Enquoted name of the warning to disable.
#define DISABLE_WARNING_CLANG(Warning) DISABLE_WARNING(Warning)

#define DISABLE_WARNING_GCC(Warning)
#else
/// Disables the given GCC warning; must be an unquoted string starting with -W.
/// \note Make sure the warnings' state has been pushed before.
/// \param Warning Enquoted name of the warning to disable.
#define DISABLE_WARNING_GCC(Warning) DISABLE_WARNING(Warning)

#define DISABLE_WARNING_CLANG(Warning)
#endif

#define DISABLE_WARNING_MSVC(Warning)
#else
#define DO_PRAGMA(Arg)
#define PUSH_WARNINGS_STATE
#define POP_WARNINGS_STATE
#define DISABLE_WARNING(Warning)
#define DISABLE_WARNING_GCC(Warning)
#define DISABLE_WARNING_CLANG(Warning)
#define DISABLE_WARNING_MSVC(Warning)
#endif

#endif // RAZ_COMPILERUTILS_HPP
