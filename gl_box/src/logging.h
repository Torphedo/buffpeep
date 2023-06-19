#pragma once
#include <stdio.h>

static const char error[] = "31";
static const char warning[] = "33";
static const char info[] = "32";

// This is a bit messy, but essentially it prints a color-coded message with
// the current function. For main(), LOG_MSG(info, "test\n"); would print:
// "\033[32mmain()\033[0m: test\n"
// In the console, this appears as "main(): test" with "main()" colored green.
#define LOG_MSG(type, ...) printf("\033[%sm%s()\033[0m: ", type, __func__); printf(__VA_ARGS__)

