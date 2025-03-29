#include <libs.hpp>
#include <sstream>

#if defined(__linux__) || defined(__APPLE__)
#include <math.h>
#endif

// ⚠️ Declare disk_group_maneger as a global variable
DiskGroup disk_group_maneger;

void request_initalization()
{
    scanf_s("%d %d %d %d %d", &T, &M, &N, &V, &G);
    partition_size = V / M;

    ordered_requests = std::vector<std::list<int>>(N + 1, std::list<int>());
    tokens = ivector(N + 1, G);

    // Fill the disk with zeros
    for (int i = 1; i <= N; i++)
        std::fill(disk[i], disk[i] + V + 1, 0);

    // Initialize disk points
    std::fill(disk_point, disk_point + N + 1, 1);

    // fre_xx is a 2D array
    // fre_xx[1][1] ... fre_xx[M][ceil(T/1800)]
    fre_del = std::vector<ivector>(M + 1, { 0 });
    fre_write = std::vector<ivector>(M + 1, { 0 });
    fre_read = std::vector<ivector>(M + 1, { 0 });
}
/****
Input format:

fre_del[1][1] fre_del[1][2] ... fre_del[1][ceil(T/1800)]
fre_del[2][1] fre_del[2][2] ... fre_del[2][ceil(T/1800)]
...
fre_del[M][1] fre_del[M][2] ... fre_del[M][ceil(T/1800)]
fre_write[1][1] fre_write[1][2] ... fre_write[1][ceil(T/1800)]
fre_write[2][1] fre_write[2][2] ... fre_write[2][ceil(T/1800)]
...
fre_write[M][1] fre_write[M][2] ... fre_write[M][ceil(T/1800)]
fre_read[1][1] fre_read[1][2] ... fre_read[1][ceil(T/1800)]
fre_read[2][1] fre_read[2][2] ... fre_read[2][ceil(T/1800)]
...
fre_read[M][1] fre_read[M][2] ... fre_read[M][ceil(T/1800)]
*/
void  request_freq_init()
{
    // Read fre_del
    for (int i = 1; i <= M; i++)
        scan_numbers(fre_del[i]);

    // Read fre_write
    for (int i = 1; i <= M; i++)
        scan_numbers(fre_write[i]);

    sizes_sorted_by_tag = getRowSums(fre_write);

    // Read fre_read
    for (int i = 1; i <= M; i++)
        scan_numbers(fre_read[i]);
    tag_sort_by_busy_time = getMaxIndices(fre_read);

    // ⚠️ Declare disk_group_maneger as a global variable
    disk_group_maneger.set(
    tag_sort_by_busy_time,
    sizes_sorted_by_tag,
    G / 1, N, V, M);

    std::cout.flush();
}

void request_timestamp()
{
    int timestamp = 0;
    scanf_s("%*s%d", &timestamp);
    std::cout << "TIMESTAMP " << timestamp << std::endl;
    std::cout.flush();
}

void request_write()
{

    int n_write;
    int object_id, object_tag, object_size;
    int start_id = 1;

    scanf_s("%d", &n_write);

    for (int i = 1; i <= n_write; i++)
    {
        // Write object and store it in the object array
        scanf_s("%d %d %d", &object_id, &object_tag, &object_size);
        auto obj = disk_group_maneger.write_to_group(object_id, object_tag, object_size);
        object[object_id] = obj;

        if (i == 1)
        {
            start_id = object_id;
        }
    }

    for (int id = start_id; id <= start_id + n_write - 1; id++)
    {
        auto& obj = object[id];
        std::cout << id << '\n';
        for (int i = 1; i <= REP_NUM; i++)
        {
            std::cout << obj.replica[i] << ' ';
            for (int j = 1; j <= obj.size - 1; j++)
                std::cout << obj.unit[i][j] << ' ';
            std::cout << obj.unit[i][obj.size] << '\n';
        }
    }
}

void request_delete()
{
    int n_delete;
    int object_id;
    ivector aborted_requests;
    scanf_s("%d", &n_delete);

    for (int i = 1; i <= n_delete; i++)
    {
        scanf_s("%d", &object_id);
        auto reqs = del(object_id);
        aborted_requests.insert(aborted_requests.end(), reqs.begin(), reqs.end());
    }

    std::cout << aborted_requests.size() << '\n';
    for (auto req : aborted_requests)
    {
        std::cout << req << '\n';
    }
}

void request_read() {
    int n_read;
    ivector req_ids;
    ivector object_ids;
    scanf_s("%d", &n_read);

    for (int i = 1; i <= n_read; i++) {
        int object_id;
        int req_id;
        scanf_s("%d %d", &req_id, &object_id);

        req_ids.push_back(req_id);
        object_ids.push_back(object_id);
    }

    // Make requests
    for (int i = 0; i < n_read; i++) {
        make_read_request(req_ids[i], object_ids[i]);
    }

    // Process requests
    auto [actions, completed_reqs] = read();
    for (auto& action : actions) {
        std::cout << action;
    }
    std::cout << completed_reqs.size() << '\n';
    for (int req : completed_reqs) {
        std::cout << req << '\n';
    }

}