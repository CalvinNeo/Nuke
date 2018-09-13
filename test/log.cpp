#include "log.h"
#include <cstring>
#include <cstdio>

class IntLogger{
public:
    static auto get_new_fmt(){
        static char new_fmt[1024];
        return new_fmt;
    }
    int value;
    IntLogger(int i) : value(i){
    }
    void dolog(IntLogger & context, char const * file_name, char const * func_name, int line, int level, char const * fmt, va_list va) {
        std::snprintf(IntLogger::get_new_fmt(), 1024, "LOG[%d] %s:%d(In function %s): Context value: %d, Message %s\n", level, file_name, line, func_name, value, fmt);
        std::vfprintf(stdout, IntLogger::get_new_fmt(), va);
        fflush(stdout);
    }
};

int main(){
    log(1, IntLogger(1), "%s", "aaa");
    log(1, Nuke::BaseLogger(), "%s", "aaa");
}
