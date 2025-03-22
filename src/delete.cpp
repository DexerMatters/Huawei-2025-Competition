#include <libs.hpp>

ivector del(int object_id) {
    ivector aborted_requests;
    release_unit(object_id);
    for (int i = request_count; i >= 1; i--) {
        if (request[i].object_id == object_id && !request[i].is_done) {
            request[i].is_done = true;
            request[i].prev_req_id = 0;

            aborted_requests.push_back(i);

            // Find the next undone request
            for (int j = i + 1; j <= request_count; j++) {
                if (!request[j].is_done) {
                    request[j].prev_req_id = request[i].prev_req_id;
                    break;
                }
            }
        }
}


void release_unit(int object_id) {
    auto& obj = object[object_id];
    for (int i = 1; i <= REP_NUM; i++) {
        if (obj.unit[i] == nullptr)
            continue;
        for (int j = 1; j <= obj.size; j++) {
            disk[obj.replica[i]][obj.unit[i][j]] = 0;
    }
}