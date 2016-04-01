#pragma once

template <typename F>
struct _ScopeExit {
    _ScopeExit(F f) : f(f) {}
    ~_ScopeExit() { f(); }
    F f;
};

template <typename F>
static _ScopeExit<F> _MakeScopeExit(F f) {
    return _ScopeExit<F>(f);
}

#define STRING_JOIN2(arg1, arg2) DO_STRING_JOIN2(arg1, arg2)
#define DO_STRING_JOIN2(arg1, arg2) arg1 ## arg2
#define DEFER(code) auto STRING_JOIN2(_scope_exit_,__LINE__) = _MakeScopeExit([=](){code});