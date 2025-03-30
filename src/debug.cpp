#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

int debug_scanf(const char* format, ...) {
    // Static file pointer to keep the file open across calls
    static FILE* fp = fopen("/home/dexer/Repos/cpp/competition/docs/data/sample_practice.in", "r");

    // Open the file on the first call
    if (fp == NULL) {
        fp = fopen("input.txt", "r");
        if (fp == NULL) {
            return -1; // Return -1 if file cannot be opened
        }
    }

    // Buffer to store the line; 1024 bytes should be sufficient for typical inputs
    char line[4096];

    // Read the next line from the file
    if (fgets(line, sizeof(line), fp) == NULL) {
        return EOF; // Return EOF if end of file is reached or an error occurs
    }

    // Initialize variable argument list
    va_list args;
    va_start(args, format);

    // Parse the line using vsscanf and store values in the provided arguments
    int ret = vsscanf(line, format, args);

    // Clean up the variable argument list
    va_end(args);

    // Return the number of items successfully scanned
    return ret;
}