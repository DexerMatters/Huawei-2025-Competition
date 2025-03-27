#include <libs.hpp>
#include <sstream>

void make_read_request(int req_id, int object_id) {
    request_count++;
    Request req{ object_id, 0, false };
    // Find previous undone request
    for (int i = req_id - 1; i >= 1; i--) {
        if (!request[i].is_done) {
            req.prev_req_id = i;
            break;
        }
    }
    // Add request to the request list
    request[req_id] = req;
}

std::string read(int object_id, int req_id) {

    std::stringstream ss;

    make_read_request(req_id, object_id);

    // Travsersing the request list by following the prev_req_id
    for (int i = req_id; i != 0; i = request[i].prev_req_id) {
        if (request[i].object_id == object_id) {
            // TODO: Implement the read operation
        }
    }
}

void move_point(int disk_id) {
    // Disk is a circular array, starting from 1 to V
    disk_point[disk_id] = disk_point[disk_id] % V + 1;
}