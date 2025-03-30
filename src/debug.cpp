#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <algorithm>

int debug_scanf(const char* format, ...) {
    // Static file pointer to keep the file open across calls
    //static FILE* fp = fopen("/home/dexer/Repos/cpp/competition/docs/data/sample_practice.in", "r");

    static std::fstream file;
    if (!file.is_open()) {
        file.open("/home/dexer/Repos/cpp/competition/docs/data/sample_practice.in", std::ios::in);
        if (!file) {
            perror("Error opening file");
            return -1;
        }
    }
    // Read a line from the file
    std::string line;
    if (!std::getline(file, line)) {
        return EOF; // End of file or error
    }
    // Remove trailing newline character
    if (!line.empty() && line.back() == '\n') {
        line.pop_back();
    }
    // Remove leading whitespace
    line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
    // Remove trailing whitespace
    line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), line.end());
    // Check if the line is empty after trimming
    if (line.empty()) {
        return EOF; // End of file or error
    }
    // Initialize variable argument list
    va_list args;
    va_start(args, format);

    // Parse the line using vsscanf and store values in the provided arguments
    int ret = vsscanf(line.c_str(), format, args);

    // Clean up the variable argument list
    va_end(args);

    // Return the number of items successfully scanned
    return ret;
}