#pragma once
#include <cstdio>
#include <functional>
#include "string.h"

void operator"" _puts(const char * str, std::size_t)
{
    printf("%s", str);
}

namespace Nuke{

struct PrintContext{
    typedef const char * F;
    F fmt;
    PrintContext(F f) : fmt(f){
    }
    template <typename ... T>
    void operator()(T&& ... args){
        printf(fmt, std::forward<T>(args)...);
    }
};

struct FmtContext{
    typedef const char * F;
    F fmt;
    char * fp = nullptr;
    FmtContext(F f) : fmt(f){
    }
    ~FmtContext(){
        delete fp;
    }
    template <typename ... T>
    const char * operator()(T&& ... args){
        Nuke::asprintf(&fp, fmt, std::forward<T>(args)...);
        return (const char *)fp;
    }
};

}

Nuke::PrintContext operator"" _print (const char * str, std::size_t)
{
    return Nuke::PrintContext{str};
}

Nuke::FmtContext operator"" _fmt (const char * str, std::size_t)
{
    return Nuke::FmtContext{str};
}

namespace Nuke{

}
