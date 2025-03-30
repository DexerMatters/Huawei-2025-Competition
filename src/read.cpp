#include <libs.hpp>
#include <sstream>
#include <algorithm>

void make_read_request(int req_id, int object_id) {
    request_count++;
    bvector process(object[object_id].size + 1, false);
    Request req{ object_id, process, false };

    // Add request to the request list
    request[req_id] = req;
    record_request(req_id);
}



void read() {
    // Preparation
    ivector completed_reqs;

    reset_tokens();

    // Evaluate the distance towards the first requested data unit
    for (int i = 1; i <= N; i++) {

        // To protect the iterator
        // We need to copy the requests
        auto& reqs_ = ordered_requests[i];
        auto reqs = ivector(reqs_.size()); // Copied requests
        std::copy(reqs_.begin(), reqs_.end(), reqs.begin());

        if (reqs.empty()) {
            // Do nothing
            std::cout << "#\n";
            continue;
        }

        // We need to find the closest request to the disk point
        // Position of the disk point is `disk_point[i]`
        // The closest request should be just farther than the disk point
        // so that the disk point can move to the closest request
        std::sort(reqs.begin(), reqs.end(),
            [&, i](int a, int b) {
                int dist_a = calculate_distance(i, disk_point[i], request[a].object_id);
                int dist_b = calculate_distance(i, disk_point[i], request[b].object_id);
                return dist_a < dist_b;
            }
        );

        auto& closest_req = request[*reqs.begin()];
        auto closest_obj = object[closest_req.object_id];
        auto skip_needed = calculate_distance(i, disk_point[i], closest_req.object_id);

        // Which replica
        int i_ = which_replica(i, request[*reqs.begin()].object_id);

        // Skipping
        if (skip_needed >= G) {
            disk_point[i] = closest_obj.unit[i_][1];
            std::cout << "j " << disk_point[i] << "\n";
            continue;
        }

        std::cout << rep_char('p', skip_needed);
        disk_point[i] = closest_obj.unit[i_][1];
        tokens[i] -= skip_needed;

        // Reading
        int nth = 1;
        int* data = disk[i];
        int read_id = data[disk_point[i]];
        auto read_req = reqs.begin();
        while (tokens[i] - READING_COSTS[nth] >= 0) {
            // Read Once
            std::cout << "r";
            tokens[i] -= READING_COSTS[nth++];

            // Check if the request is completed
            if (process_request(i, *read_req) && read_id != 0) {
                completed_reqs.push_back(*read_req);
            }

            // Next
            disk_point[i] = (disk_point[i] % V) + 1;
            int next_id = data[disk_point[i]];
            auto next_req = read_req;
            if (next_req != reqs.end()) {
                next_req++;
            }
            if (read_id != next_id && next_id != 0) {
                // Change request
                read_req++;
                if (read_req == reqs.end()) {
                    break;
                }
            }

            if (reqs_.empty()) {
                break;
            }

            // Update read_id
            read_id = next_id;
        }

        // Complete
        std::cout << "#\n";
    }

    // Print completed requests
    std::cout << completed_reqs.size() << '\n';
    for (auto req : completed_reqs) {
        std::cout << req << '\n';
    }
    std::cout.flush();
}

// Start reading

// Move the disk point to the destination



