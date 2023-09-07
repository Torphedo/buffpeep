#include <stdio.h>
#include <stdarg.h>

int logging_print(char* type, char* function, char* format_str, ...) {
    // Print "__func__(): " with function name in color and the rest in white
    printf("\033[%sm%s\033[0m(): ", type, function);

    va_list arg_list = {0};
    va_start(arg_list, format_str);
    int return_code = vprintf(format_str, arg_list);
    va_end(arg_list);

    return return_code;
}

