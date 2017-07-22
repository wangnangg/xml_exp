#include "absyn_tree.hpp"
#include "gtest/gtest.h"
#include "rapidxml.hpp"

TEST(test_absyn_tree, eval_add_exp)
{
    exp_crtor crtor;
    stack st(2000);
    auto sc = st.init_scope();
    auto val1 = crtor.create<value_exp>(1.0);
    auto val2 = crtor.create<value_exp>(2.0);
    ASSERT_EQ(eval(val1, sc), 1.0);
    ASSERT_EQ(eval(val2, sc), 2.0);
    auto add_exp1 = crtor.create<add_exp>(val1, val2);
    auto add_exp2 = crtor.create<add_exp>(val1, add_exp1);
    ASSERT_EQ(eval(add_exp1, sc), 3.0);
    ASSERT_EQ(eval(add_exp2, sc), 4.0);
}
TEST(test_absyn_tree, func_call)
{
    exp_crtor crtor;
    stack st(2000);
    auto sc = st.init_scope();
    func_env_crtor env_crt;
    auto var1 = crtor.create<var_exp>("a");
    auto var2 = crtor.create<var_exp>("b");
    auto body = crtor.create<add_exp>(var1, var2);
    env_crt.add_func("add", {"a", "b"}, body);
    func_env env = env_crt;
    auto val1 = crtor.create<value_exp>(1.0);
    auto val2 = crtor.create<value_exp>(2.0);
    auto val3 = crtor.create<value_exp>(10.0);
    auto call1 = crtor.create<call_exp>("add", std::vector<exp*>{val1, val2});
    auto call2 = crtor.create<call_exp>("add", std::vector<exp*>{val3, val2});
    auto add_exp1 = crtor.create<add_exp>(call1, call2);
    link(call1, env);
    link(add_exp1, env);
    ASSERT_EQ(eval(call1, sc), 3.0);
    ASSERT_EQ(eval(add_exp1, sc), 15.0);
}

TEST(test_absyn_tree, factor_func)
{
    exp_crtor crtor;
    stack st(2000);
    auto sc = st.init_scope();
    func_env_crtor env_crt;
    auto var_n = crtor.create<var_exp>("n");
    auto minus1 = crtor.create<value_exp>(-1.0);
    auto e1 = crtor.create<add_exp>(var_n, minus1);
    auto e2 = crtor.create<call_exp>("factor", std::vector<exp*>{e1});
    auto tbr = crtor.create<mul_exp>(var_n, e2);
    auto fbr = crtor.create<value_exp>(1.0);
    auto body = crtor.create<if_exp>(var_n, tbr, fbr);
    env_crt.add_func("factor", {"n"}, body);
    func_env env = env_crt;
    auto val5 = crtor.create<value_exp>(5.0);
    auto call1 = crtor.create<call_exp>("factor", std::vector<exp*>{val5});
    link(call1, env);
    ASSERT_EQ(eval(call1, sc), 120.0);
    auto val10 = crtor.create<value_exp>(10.0);
    auto call2 = crtor.create<call_exp>("factor", std::vector<exp*>{val10});
    link(call2, env);
    ASSERT_EQ(eval(call2, sc), 3628800.0);
}
