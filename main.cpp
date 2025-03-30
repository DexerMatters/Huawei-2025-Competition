#include <iostream>
#include <libs.hpp>

int main(int, char**) {
    request_initalization();
    request_freq_init();

    

    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        std::cerr << "Time: " << t << '\n';
        request_timestamp();
        std::cerr << "Time: " << t << " " << "del" << '\n';
        request_delete();
        std::cerr << "Time: " << t << " " << "write" << '\n';
        request_write();
        std::cerr << "Time: " << t << " " << "read" << '\n';
        request_read();
    }

    cleanAll();
    return 0;
}
