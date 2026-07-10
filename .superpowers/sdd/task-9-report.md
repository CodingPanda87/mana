# Task 9: fixed.hpp - Compile-time Friendly Containers

## Status: DONE

## Commit

- `55bff2c` feat(util): add constexpr containers: fixed_string, fixed_vector, small_vector

## Files Created/Modified

- `G:/Code/Project/mana/include/mana/util/fixed.hpp` (rewritten from stub)
- `G:/Code/Project/mana/tests/test_fixed.cpp` (rewritten from stub)

## Implementation Summary

### fixed_string<N>
- Compile-time fixed-size string, usable as C++20 NTTP
- CTAD deduction guide: `fixed_string("test")` deduces `fixed_string<5>`
- Template constructor `const char (&)[M]` for cross-size literal construction (e.g., `fixed_string<16>("hello")`)
- Template `operator==` for cross-size comparison (supports C++20 `!=` synthesis)
- `operator<=>` for same-size three-way comparison
- `operator+` for concatenation producing `fixed_string<N+M-1>`
- All methods are `constexpr`

### fixed_vector<T, N>
- Fixed-capacity dynamic array, fully constexpr-friendly
- `push_back` with overflow detection: uses `__assume(false)` on MSVC (constexpr-compatible) when `__cpp_constexpr_exceptions` is not available; `throw` when constexpr exceptions are supported
- Supports `initializer_list` construction, `pop_back`, `clear`, iterators

### small_vector<T, N>
- Small buffer optimization: inline stack storage for up to N elements
- Transparently grows to heap when capacity exceeded (2x growth)
- Proper copy/move constructors, destructor with stack/heap distinction
- Placement new for element construction, explicit destructor calls

## Test Results

All 5 test functions pass:
- `test_fixed_string` - runtime construction, size, view, c_str
- `test_fixed_string_nttp` - constexpr construction, static_assert equality/inequality
- `test_fixed_vector` - runtime push_back, pop_back, clear, empty/full
- `test_fixed_vector_constexpr` - constexpr lambda construction, static_assert size/values
- `test_small_vector` - stack storage then heap transition on overflow

## Deviations from Task Brief

1. Added template constructor `fixed_string(const char (&str)[M])` to handle cross-size string literal construction (the brief's `fixed_string<16>("hello"` fails because `"hello"` is `char[6]`, not `char[16]`)
2. Changed `operator==` to a template for cross-size comparison (the brief's same-only `operator==` cannot handle `static_assert(fs != fixed_string("other"))` where sizes differ)
3. Used `__assume(false)` with `#ifdef __cpp_constexpr_exceptions` guard instead of bare `throw` in `fixed_vector::push_back` -- MSVC C++23 does not support `throw` in constexpr context

## Portability Concern

`__assume(false)` is MSVC-specific. If cross-compiler support is needed, add `__builtin_unreachable()` for GCC/Clang. Not an issue for this project which targets MSVC.

---

## Fix Report (2026-07-11)

### Issues Fixed

1. **Critical: `fixed_vector::push_back` debug fall-through (both overloads).** Added `return;` after `__assume(false)` and wrapped the `if` body in braces so that in debug builds (no optimization), execution does not fall through to the out-of-bounds write `data_[size_++] = value;`.

2. **Important: `small_vector` Rule of Five violation.** Added copy assignment operator `operator=(const small_vector&)` and move assignment operator `operator=(small_vector&&) noexcept` with self-assignment guards and proper stack/heap storage handling.

3. **Important: Wrong include for `::operator new`/`::operator delete`.** Changed `#include <memory>` to `#include <new>`. The allocation functions are declared in `<new>`, not `<memory>`.

4. **Important: Unused `#include <cstring>`.** Removed it (no `memcpy`/`memset`/etc. usage in the file).

### Commits

- Pending fix commit

### Test Results

All tests pass in Debug build:
- `All fixed container tests passed!`
- All other test suites also pass (utility, string, logging, IO, context, thread pool, event bus, framework, math, geo, result, scope guard, enum, hash, type traits, bit, numeric, optional, variant)
