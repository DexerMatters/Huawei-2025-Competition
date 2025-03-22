#include <libs.hpp>
#include <sstream>

int request_count = 0;

int T = 0;

int N = 0;

int V = 0;

int M = 0;

int G = 0;

int partition_size = 0;

int disk[MAX_DISK_NUM][MAX_DISK_SIZE];

int disk_point[MAX_DISK_NUM];

std::vector<ivector> fre_del;

std::vector<ivector> fre_write;

std::vector<ivector> fre_read;

Request request[MAX_REQUEST_NUM];
Object object[MAX_OBJECT_NUM];

void scan_numbers(ivector& vector) {
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::copy(std::istream_iterator<int>(iss),
        std::istream_iterator<int>(),
        std::back_inserter(vector));
}
