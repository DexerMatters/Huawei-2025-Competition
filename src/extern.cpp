#include <libs.hpp>
#include <sstream>
#include <algorithm>

#if defined(__linux__) || defined(__APPLE__)
#include <iterator>
#endif

int request_count = 0;

int T = 0;

int N = 0;

int V = 0;

int M = 0;

int G = 0;

int partition_size = 0;

int disk[MAX_DISK_NUM][MAX_DISK_SIZE];

int disk_point[MAX_DISK_NUM];

ivector tokens;

std::vector<std::list<int>> ordered_requests;

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

void reset_tokens() {
    std::fill(tokens.begin(), tokens.end(), G);
}


int calculate_distance(int point, int dest) {
    // Calculate the distance between the current point and the destination
    // Note that the disk is a circular array with the capacity. 
    // And disk starts from 1 to capacity.
    if (point == dest) {
        return 0;
    }
    if (point < dest) {
        return dest - point;
    }
    return V - point + dest;
}

bool is_farther_than(int dest1, int dest2) {
    // Note that the disk is a circular array with the capacity. 
    // And disk starts from 1 to capacity.
    if (dest1 == dest2) {
        return true;
    }
    if (dest1 < dest2) {
        return dest2 - dest1 > V / 2;
    }
    return dest1 - dest2 < V / 2;
}


void record_request(int req_id) {
    auto& obj = object[request[req_id].object_id];
    for (int i = 1; i <= REP_NUM; i++) {
        auto disk_id = obj.replica[i];
        auto& list = ordered_requests[disk_id];
        auto head = obj.unit[disk_id][1];
        if (list.empty()) {
            list.push_back(req_id);
            continue;
        }
        auto it = std::find_if(list.begin(), list.end(), [&](auto req) {
            return object[request[req].object_id].unit[disk_id][1] >= head;
            });
        if (it == list.end()) {
            list.push_back(req_id);
        }
        else {
            list.insert(it, req_id);
        }
    }
}

void delete_recorded_request(int req_id) {
    auto& obj = object[request[req_id].object_id];
    for (int i = 1; i <= REP_NUM; i++) {
        auto disk_id = obj.replica[i];
        auto& list = ordered_requests[disk_id];
        auto it = std::find(list.begin(), list.end(), req_id);
        if (it != list.end()) {
            list.erase(it);
        }
    }
}

bool process_request(int disk_id, int req_id) {
    auto& req = request[req_id];
    auto unit = object[req.object_id].unit[disk_id];
    auto size = object[req.object_id].size;
    bool completed = true;
    for (int i = 1; i <= size; i++) {
        if (unit[i] == disk_point[disk_id]) {
            req.process[i] = true;
        }
        completed = completed && req.process[i];
    }
    if (completed) {
        req.is_done = true;
        delete_recorded_request(req_id);
    }
    return completed;
}