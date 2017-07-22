#pragma once
#include <cassert>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class undef_ref : public std::exception
{
public:
    undef_ref(std::string symbol) {}
};

class scope
{
private:
    double* _base;
    double* _end;
    double* _cap;

    scope(double* base, double* end, double* cap)
        : _base(base), _end(end), _cap(cap)
    {
        assert(_end <= _cap);
    }
    friend class stack;

public:
    double& operator[](size_t offset) { return *(_base + offset); }
    scope enter_scope(size_t size) { return scope(_end, _end + size, _cap); }
};

class stack
{
    std::vector<double> _val;

public:
    stack(size_t size) : _val(size, 0.0) {}
    scope init_scope()
    {
        return scope(&_val[0], &_val[0], (&_val[_val.size()]));
    }
};

class exp;

using visitor_t = std::function<void(exp*)>;
class exp
{
public:
    exp(const exp&) = delete;
    exp() = default;
    virtual double eval(scope sc) const = 0;
    virtual void visit(const visitor_t& visit) { visit(this); }
};
using func_map_t = std::unordered_map<std::string, exp*>;
class func_env
{
    func_map_t _func_map;
    func_env(func_map_t func_map);
    friend class func_env_crtor;

public:
    exp* operator[](const std::string& name) const
    {
        return _func_map.at(name);
    }
};

void link_func(func_env&);

class func_env_crtor
{
    func_map_t _func_map;

public:
    void add_func(std::string name, const std::vector<std::string>& formal,
                  exp* body);
    operator func_env() const;
};

inline double eval(exp* e, scope sc) { return e->eval(sc); }

class exp_crtor
{
    std::vector<std::unique_ptr<exp>> _alloc_exp;

public:
    template <typename exp_type, typename... param_types>
    exp_type* create(param_types&&... params)
    {
        auto exp_ptr =
            std::make_unique<exp_type>(std::forward<param_types>(params)...);
        auto exp = exp_ptr.get();
        _alloc_exp.push_back(std::move(exp_ptr));
        return exp;
    }
};

void link(exp* target, const func_env& env);
void alloc_var(const std::vector<std::string>& formal, exp* body);

class value_exp : public exp
{
    double _val;

public:
    value_exp(double v) : _val(v) {}
    virtual double eval(scope sc) const override { return _val; }
};

class var_exp : public exp
{
    std::string _name;
    size_t _offset;

public:
    var_exp(std::string vname) : _name(std::move(vname)) {}
    virtual double eval(scope sc) const override { return sc[_offset]; }
    void alloc(size_t offset) { _offset = offset; }
    const std::string& name() const { return _name; }
};

class if_exp : public exp
{
    exp* _cond;
    exp* _true_br;
    exp* _false_br;

public:
    if_exp(exp* cond, exp* tbr, exp* fbr)
        : _cond(cond), _true_br(tbr), _false_br(fbr)
    {
    }
    virtual double eval(scope sc) const override
    {
        if (_cond->eval(sc) > 0)
        {
            return _true_br->eval(sc);
        }
        else
        {
            return _false_br->eval(sc);
        }
    }
    virtual void visit(const visitor_t& visit) override
    {
        visit(this);
        _cond->visit(visit);
        _true_br->visit(visit);
        _false_br->visit(visit);
    }
};

class bin_exp : public exp
{
protected:
    exp* _op1;
    exp* _op2;

public:
    bin_exp(exp* x, exp* y) : _op1(x), _op2(y) {}
    virtual double eval(scope sc) const override = 0;
    virtual void visit(const visitor_t& visit) override
    {
        visit(this);
        _op1->visit(visit);
        _op2->visit(visit);
    }
};

class add_exp : public bin_exp
{
public:
    add_exp(exp* x, exp* y) : bin_exp(x, y) {}
    virtual double eval(scope sc) const override
    {
        return _op1->eval(sc) + _op2->eval(sc);
    }
};

class mul_exp : public bin_exp
{
public:
    mul_exp(exp* x, exp* y) : bin_exp(x, y) {}
    virtual double eval(scope sc) const override
    {
        return _op1->eval(sc) * _op2->eval(sc);
    }
};

class div_exp : public bin_exp
{
public:
    div_exp(exp* x, exp* y) : bin_exp(x, y) {}
    virtual double eval(scope sc) const override
    {
        return _op1->eval(sc) / _op2->eval(sc);
    }
};

class call_exp : public exp
{
    std::string _name;
    std::vector<exp*> _argv;
    exp* _body;

public:
    call_exp(std::string pname, std::vector<exp*> parg)
        : _name(pname), _argv(std::move(parg))
    {
    }
    virtual double eval(scope sc) const override
    {
        auto arg_count = _argv.size();
        auto new_scope = sc.enter_scope(arg_count);
        for (size_t i = 0; i < arg_count; i++)
        {
            new_scope[i] = _argv[i]->eval(sc);
        }
        return _body->eval(new_scope);
    }
    virtual void visit(const visitor_t& visit) override
    {
        for (auto e : _argv)
        {
            e->visit(visit);
        }
        visit(this);
    }
    void link(exp* body) { _body = body; }
    const std::string& name() const { return _name; }
};
