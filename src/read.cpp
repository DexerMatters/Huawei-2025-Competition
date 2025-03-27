#include <libs.hpp>
#include <sstream>
#include <algorithm>

void make_read_request(int req_id, int object_id) {
    request_count++;
    bvector process(object[object_id].size, false);
    Request req{ object_id, process, false };

    // Add request to the request list
    request[req_id] = req;
    record_request(req_id);
}



std::pair<std::vector<std::string>, ivector> read(int object_id, int req_id) {
    // Preparation
    std::vector<std::string> actions;
    ivector completed_reqs;

    reset_tokens();
    make_read_request(req_id, object_id);

    // Evaluate the distance towards the first requested data unit
    for (int i = 1; i <= N; i++) {
        auto& reqs = ordered_requests[i];
        std::stringstream action;
        if (reqs.empty()) {
            // Do nothing
            action << "#\n";
            actions.push_back(action.str());
            continue;
        }
        // We need to find the closest request to the disk point
        // Position of the disk point is `disk_point[i]`
        // The closest request should be just farther than the disk point
        // so that the disk point can move to the closest request
        auto closest = std::find_if(reqs.begin(), reqs.end(), [&](int req) {
            return is_farther_than(disk_point[i], object[req].unit[i][1]);
            });
        auto closest_obj = object[request[*closest].object_id];
        auto skip_needed = calculate_distance(disk_point[i], closest_obj.unit[i][1]);

        // Skipping
        if (skip_needed >= G) {
            action << "j " << skip_needed << '\n';
            disk_point[i] = closest_obj.unit[i][1];
            tokens[i] = 0;
            continue;
        }

        action << rep_char('p', skip_needed);
        disk_point[i] = closest_obj.unit[i][1];
        tokens[i] -= skip_needed;

        // Reading
        int nth = 1;
        int* data = disk[i];
        int read_id = data[disk_point[i]];
        auto read_req = closest;
        while (tokens[i] - READING_COSTS[nth] >= 0) {
            // Read Once
            read_id = data[disk_point[i]];
            action << "r";
            tokens[i] -= READING_COSTS[nth++];

            // Check if the request is completed
            if (process_request(i, *read_req)) {
                completed_reqs.push_back(*read_req);
            }

            // Next
            disk_point[i] = (disk_point[i] % V) + 1;
            int next_id = data[disk_point[i]];
            if (read_id != next_id) {
                // Change request
                read_req++;
                if (read_req == reqs.end()) {
                    break;
                }
            }

            // Update read_id
            read_id = next_id;
        }

        // Complete
        action << "#\n";
        actions.push_back(action.str());
    }

    return { actions, completed_reqs };
}

// Start reading

// Move the disk point to the destination



