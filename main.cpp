#include <iostream>
#include <libs.hpp>
#include <chrono>

#define TIME_SPEND(code, name) \
    {code;}
// { \
    //     auto start = std::chrono::high_resolution_clock::now(); \
    //     code; \
    //     auto end = std::chrono::high_resolution_clock::now(); \
    //     std::chrono::duration<double> elapsed = end - start; \
    //     std::cerr << '[' << name << ']' << "\t Elapsed time:\t" << elapsed.count() << " seconds\n"; \
    // }

int main(int, char**) {
    request_initalization();
    request_freq_init();
    //skip_freq_init();

    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        TIME_SPEND(request_timestamp(), "request_timest");
        TIME_SPEND(request_delete(), "request_delete");
        TIME_SPEND(request_write(), "request_writee");
        TIME_SPEND(request_read(), "request_readdd");
    }

    return 0;
}
