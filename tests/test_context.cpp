#include <mana/context.h>
#include <cassert>
#include <iostream>

void test_context_put_get() {
    mana::context::Context ctx;
    ctx.put("name", std::string("panda"));
    ctx.put("age", 42);

    auto name = ctx.get<std::string>("name");
    assert(name.has_value());
    assert(name.value() == "panda");

    auto age = ctx.get<int>("age");
    assert(age.has_value());
    assert(age.value() == 42);
}

void test_context_has() {
    mana::context::Context ctx;
    assert(ctx.has("key") == false);

    ctx.put("key", std::string("value"));
    assert(ctx.has("key") == true);
}

void test_context_remove() {
    mana::context::Context ctx;
    ctx.put("key", std::string("value"));
    assert(ctx.has("key") == true);

    ctx.remove("key");
    assert(ctx.has("key") == false);
}

void test_context_clear() {
    mana::context::Context ctx;
    ctx.put("key1", std::string("value1"));
    ctx.put("key2", std::string("value2"));

    ctx.clear();
    assert(ctx.has("key1") == false);
    assert(ctx.has("key2") == false);
}

void test_context_keys() {
    mana::context::Context ctx;
    ctx.put("key1", std::string("value1"));
    ctx.put("key2", std::string("value2"));

    auto keys = ctx.keys();
    assert(keys.size() == 2);
}

void test_context_stack() {
    mana::context::Context ctx1;
    ctx1.put("level", std::string("outer"));

    mana::context::attach(std::move(ctx1));
    assert(mana::context::current().get<std::string>("level").value() == "outer");

    {
        mana::context::Context ctx2;
        ctx2.put("level", std::string("inner"));
        mana::context::attach(std::move(ctx2));

        assert(mana::context::current().get<std::string>("level").value() == "inner");
        mana::context::detach();
    }

    assert(mana::context::current().get<std::string>("level").value() == "outer");
    mana::context::detach();
}
