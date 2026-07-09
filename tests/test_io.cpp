#include <mana/io.h>
#include <cassert>
#include <iostream>
#include <fstream>

void test_write_and_read() {
    mana::io::FileManager fm;
    std::string test_path = "test_file.txt";
    std::string content = "Hello, World!";

    // Write file
    bool written = fm.write(test_path, content);
    assert(written == true);

    // Read file
    auto result = fm.read_all(test_path);
    assert(result.has_value());
    assert(result.value() == content);

    // Cleanup
    std::remove(test_path.c_str());
}

void test_exists() {
    mana::io::FileManager fm;
    std::string test_path = "test_exists.txt";

    // File doesn't exist yet
    assert(fm.exists(test_path) == false);

    // Create file
    std::ofstream file(test_path);
    file << "test";
    file.close();

    // Now exists
    assert(fm.exists(test_path) == true);

    // Cleanup
    std::remove(test_path.c_str());
}

void test_file_size() {
    mana::io::FileManager fm;
    std::string test_path = "test_size.txt";
    std::string content = "Hello";

    // Write file
    fm.write(test_path, content);

    // Check size
    auto size = fm.file_size(test_path);
    assert(size.has_value());
    assert(size.value() == content.length());

    // Cleanup
    std::remove(test_path.c_str());
}

void test_read_nonexistent() {
    mana::io::FileManager fm;
    auto result = fm.read_all("nonexistent.txt");
    assert(result.has_value() == false);
}
