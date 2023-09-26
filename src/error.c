#include <stdio.h>
#include <stdarg.h>
#include "error.h"

#define RED "\033[31m"
#define DEFAULT_COLOR "\033[0m"

void print_error(int err_code, const char * format, ...) {
    va_list argument_list;
    va_start(argument_list,format);

    
    fprintf(stderr, "%s Error [%d]%s: %s",RED, err_code, DEFAULT_COLOR,format);
    vfprintf(stderr,format, argument_list);
    va_end(argument_list);
}
