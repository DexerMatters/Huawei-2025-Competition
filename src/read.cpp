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



std::pair<std::vector<std::string>, ivector> read() {
    // Preparation
    std::vector<std::string> actions;
    ivector completed_reqs;

    reset_tokens();

    // Evaluate the distance towards the first requested data unit
    for (int i = 1; i <= N; i++) {

        // To protect the iterator
        // We need to copy the requests
        auto& reqs_ = ordered_requests[i];
        auto reqs = ivector(reqs_.size()); // Copied requests
        std::copy(reqs_.begin(), reqs_.end(), reqs.begin());

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
        auto closest = reqs.begin();
        int skip_needed = 0;
        for (auto it = reqs.begin(); it != reqs.end(); ++it) {
            // Skip the last
            if (it == reqs.end()) {
                break;
            }
            int req = *it;
            auto it_ = it; // Copy iterator
            auto skip_needed_now = calculate_distance(
                disk_point[i], object[request[*it].object_id].unit[i][1]
            );
            auto skip_needed_next = calculate_distance(
                disk_point[i], object[request[*(++it_)].object_id].unit[i][1]
            );
            if (skip_needed_now < skip_needed_next) {
                closest = it;
                skip_needed = skip_needed_now;
                break;
            }
        }

        auto closest_obj = object[request[*closest].object_id];

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
            action << "r";
            tokens[i] -= READING_COSTS[nth++];

            // Check if the request is completed
            if (process_request(i, *read_req) && read_id != 0) {
                completed_reqs.push_back(*read_req);
            }

            // Next
            disk_point[i] = (disk_point[i] % V) + 1;
            int next_id = data[disk_point[i]];
            if (read_id != next_id && next_id != 0) {
                // Change request
                read_req++;
                if (read_req == reqs.end()) {
                    if (reqs.empty()) {
                        break;
                    }
                    read_req = reqs.begin();
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



