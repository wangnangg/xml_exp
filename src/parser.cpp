#include "parser.hpp"
#include <string>
#include <cassert>

std::vector<std::string> parse_formal(const std::string& formal_str)
{
    static const char sp = ';';
    size_t last_pos = 0;
    size_t pos = formal_str.find_first_of(sp, last_pos);
    while (pos != std::string::npos)
    {

    }
}
function parse_func(xml_node* node, exp_alloc& alloc)
{
    assert(node->type() == rx::node_element);
    assert(node->name() == std::string("function"));
    auto attr_formal = node->first_attribute("formal");
    auto formal_str = attr_formal->value();
}
