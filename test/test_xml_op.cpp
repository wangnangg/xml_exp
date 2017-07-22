#include <fstream>
#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "rapidxml.hpp"

namespace rx = rapidxml;

std::vector<char> read_file(const char* filename)
{
    std::ifstream file(filename);
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    buffer.push_back('\0');
    std::cout << buffer.size() << std::endl;
    return buffer;
}

TEST(test_xml_op, read)
{
    rx::xml_document<> doc;
    auto buffer = read_file("xml/log.xml");
    doc.parse<0>(&buffer[0]);
    auto root = doc.first_node("debug");
    std::cout << root << "\n";
    for (auto node = root->first_node(); node; node = node->next_sibling())
    {
        std::cout << node->name() << "\n";
    }
}
