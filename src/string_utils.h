#pragma once
#include <cstdio>
#include <cstring>
#include <cstdarg>

namespace Nuke{

int asprintf(char ** ptr, const char * fmt, ...) {
    size_t fmt_len = std::strlen(fmt);
    int size = fmt_len * 2 + 50;
    *ptr = new char[size];
    va_list ap;
    while (1) {     // Maximum two passes on a POSIX system...
        va_start(ap, fmt);
        int n = vsnprintf(*ptr, size, fmt, ap);
        va_end(ap);
        if (n > -1 && n < size) {
            // Notice that only when this returned value is non-negative and less than n,
            // the string has been completely written.
            return n;
        }else{
            delete *ptr;
            size *= 2;
            *ptr = new char[size];
        }
    }
    return -1;
}


}
