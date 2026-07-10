#include <mana/util/io.hpp>
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

void test_write_and_read() {
    std::string test_path = "test_io_util.txt";
    std::string content = "Hello, World!";

    assert(mana::util::io::write(test_path, content) == true);

    auto result = mana::util::io::read_all(test_path);
    assert(result.has_value());
    assert(result.value() == content);

    std::filesystem::remove(test_path);
}

void test_exists() {
    std::string test_path = "test_io_exists.txt";

    assert(mana::util::io::exists(test_path) == false);

    std::ofstream file(test_path);
    file << "test";
    file.close();

    assert(mana::util::io::exists(test_path) == true);

    std::filesystem::remove(test_path);
}

void test_file_size() {
    std::string test_path = "test_io_size.txt";
    std::string content = "Hello";

    mana::util::io::write(test_path, content);

    auto size = mana::util::io::file_size(test_path);
    assert(size.has_value());
    assert(size.value() == content.length());

    std::filesystem::remove(test_path);
}

void test_read_nonexistent() {
    auto result = mana::util::io::read_all("nonexistent_io.txt");
    assert(result.has_value() == false);
}

void test_append() {
    std::string test_path = "test_io_append.txt";

    mana::util::io::write(test_path, "Hello");
    mana::util::io::append(test_path, " World");

    auto result = mana::util::io::read_all(test_path);
    assert(result.has_value());
    assert(result.value() == "Hello World");

    std::filesystem::remove(test_path);
}

void test_read_lines() {
    std::string test_path = "test_io_lines.txt";

    mana::util::io::write(test_path, "line1\nline2\nline3");

    auto lines = mana::util::io::read_lines(test_path);
    assert(lines.size() == 3);
    assert(lines[0] == "line1");
    assert(lines[1] == "line2");
    assert(lines[2] == "line3");

    std::filesystem::remove(test_path);
}
