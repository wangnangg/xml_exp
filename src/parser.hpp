#pragma once

#include "absyn_tree.hpp"
#include "rapidxml.hpp"
namespace rx = rapidxml;
using xml_node = rx::xml_node<>;

struct function
{
    std::string name;
    std::vector<std::string> formal;
    exp* exp;
};

function parse_func(const xml_node& node, exp_alloc& alloc);
func_env_crt parse_func_env();
