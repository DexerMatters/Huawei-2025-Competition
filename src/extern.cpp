#include <libs.hpp>
#include <sstream>
#include <algorithm>
#include <vector>
#include <numeric>

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

std::vector<int> tag_sort_by_busy_time;

std::vector<int> sizes_sorted_by_tag;



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

/**
 * @brief Computes the indices of the maximum elements in each row of a 2D matrix
 *        and returns a sorted list of row indices based on these maximum values.
 *
 * This function processes a 2D matrix (vector of vectors) and determines the index
 * of the maximum element in each row. If a row is empty, it assigns -1 as the index
 * for that row. After determining the maximum indices for all rows, it sorts the
 * row indices based on the corresponding maximum values in ascending order.
 *
 * @param matrix A 2D vector of integers representing the input matrix.
 * @return A vector of integers representing the sorted row indices based on the
 *         maximum values in each row. Rows with empty content are assigned -1.
 *
 * @note If multiple rows have the same maximum value, their relative order in the
 *       output is determined by their original row indices.
 */
std::vector<int> getMaxIndices(const std::vector<std::vector<int>>& matrix) {
    std::vector<int> maxIndices;
    for (const auto& row : matrix) {
        if (!row.empty()) {
            auto maxElementIt = std::max_element(row.begin(), row.end());
            maxIndices.push_back(std::distance(row.begin(), maxElementIt));
        } else {
            maxIndices.push_back(-1); // Handle empty rows by returning -1
        }
    }

    // Create a vector of indices from 0 to maxIndices.size() - 1
    std::vector<int> sortedIndices(maxIndices.size());
    std::iota(sortedIndices.begin(), sortedIndices.end(), 0);

    // Sort the indices based on the values in maxIndices
    std::sort(sortedIndices.begin(), sortedIndices.end(), [&maxIndices](int a, int b) {
        return maxIndices[a] < maxIndices[b];
    });

    return sortedIndices;
}

/**
 * @brief Computes the sum of each row in a 2D matrix and returns a vector of the sums.
 *
 * This function processes a 2D matrix (vector of vectors) and calculates the sum of
 * elements in each row. The resulting sums are stored in a vector, where each element
 * corresponds to the sum of the respective row in the input matrix.
 *
 * @param matrix A 2D vector of integers representing the input matrix.
 * @return A vector of integers representing the sum of elements in each row of the matrix.
 */
std::vector<int> getRowSums(const std::vector<std::vector<int>>& matrix) {
    std::vector<int> rowSums;
    for (const auto& row : matrix) {
        int sum = std::accumulate(row.begin(), row.end(), 0);
        rowSums.push_back(sum);
    }
    return rowSums;
}



