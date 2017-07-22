#include "absyn_tree.hpp"
#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
void link(exp* target, const func_env& env)
{
    target->visit([&](exp* curr_exp) {
        call_exp* calle = dynamic_cast<call_exp*>(curr_exp);
        if (calle)
        {
            calle->link(env[calle->name()]);
        }
    });
}

size_t arg_index(const std::vector<std::string>& args, const std::string& name)
{
    for (size_t i = 0; i < args.size(); i++)
    {
        if (args[i] == name)
        {
            return i;
        }
    }
    throw undef_ref(name);
}

void alloc_var(const std::vector<std::string>& formal, exp* body)
{
    body->visit([&](exp* curr_exp) {
        var_exp* vare = dynamic_cast<var_exp*>(curr_exp);
        if (vare)
        {
            vare->alloc(arg_index(formal, vare->name()));
        }
    });
}

func_env::func_env(func_map_t func_map) : _func_map(std::move(func_map))
{
    for (auto& pair : _func_map)
    {
        link(pair.second, *this);
    }
}
void func_env_crtor::add_func(std::string name,
                              const std::vector<std::string>& formal, exp* body)
{
    body->visit([&](exp* curr_exp) {
        var_exp* vare = dynamic_cast<var_exp*>(curr_exp);
        if (vare)
        {
            vare->alloc(arg_index(formal, vare->name()));
        }
    });
    _func_map[std::move(name)] = body;
}

func_env_crtor::operator func_env() const { return func_env(_func_map); }
