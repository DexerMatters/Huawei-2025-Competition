#include <iostream>
#include <libs.hpp>

int main(int, char**) {
    request_initalization();
    request_freq_init();

    std::cout << "OK\n";

    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        request_timestamp();
        request_delete();
        request_write();
        request_read();
    }

    cleanAll();
    return 0;
}
