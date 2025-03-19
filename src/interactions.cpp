#include <libs.hpp>

void start_interaction() {
    scanf_s("%d %d %d %d %d", N, V, M, G, T);

    // TODO

    std::cout << "OK\n";
    std::cout.flush();

    // Initialize disk pointers
    for (int i = 1; i <= N; i++) {
        disk_point[i] = 1;
    }

    for (int t = 1; t <= T + EXTRA_TIME; t++) {
        request_timestamp();

        //TODO
    }

    cleanAll();
}

void request_timestamp() {
    int timestamp;
    scanf_s("TIMESTAMP %d", &timestamp);
    std::cout << "TIMESTAMP " << timestamp << std::endl;
    std::cout.flush();
}