#include <mana/util/scope_guard.hpp>
#include <cassert>
#include <stdexcept>

void test_scope_guard_defer() {
    int counter = 0;
    {
        auto guard = mana::util::defer([&] { counter++; });
        assert(counter == 0);
    }
    assert(counter == 1);
}

void test_scope_guard_dismiss() {
    int counter = 0;
    {
        auto guard = mana::util::defer([&] { counter++; });
        guard.dismiss();
        assert(counter == 0);
    }
    assert(counter == 0);
}

void test_scope_guard_success() {
    int counter = 0;
    {
        auto guard = mana::util::ScopeSuccess([&] { counter++; });
        assert(counter == 0);
    }
    assert(counter == 1);  // No exception, so it fires
}

void test_scope_guard_failure() {
    int counter = 0;
    {
        auto guard = mana::util::ScopeFailure([&] { counter++; });
        assert(counter == 0);
    }
    assert(counter == 0);  // No exception, so it doesn't fire
}
