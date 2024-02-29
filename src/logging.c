#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
#include <Windows.h>

// Enables ANSI escape codes on Windows
unsigned short enable_win_ansi() {
    DWORD prev_console_mode;
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (console_handle != INVALID_HANDLE_VALUE) {
        GetConsoleMode(console_handle, (LPDWORD)&prev_console_mode);
        if (prev_console_mode != 0) {
            SetConsoleMode(console_handle, prev_console_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT);
            return 1;
        }
    }
    return 0;
}
#endif

int logging_print(char* type, char* function, char* format_str, ...) {
    // Print "__func__(): " with function name in color and the rest in white
    printf("\033[%sm%s\033[0m(): ", type, function);

    va_list arg_list = {0};
    va_start(arg_list, format_str);
    int return_code = vprintf(format_str, arg_list);
    va_end(arg_list);

    return return_code;
}

