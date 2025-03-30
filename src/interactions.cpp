#include <libs.hpp>
#include <sstream>
#include <algorithm>

#if defined(__linux__) || defined(__APPLE__)
#include <math.h>
#endif

// ⚠️ Declare disk_group_maneger as a global variable
DiskGroup disk_group_maneger;

void request_initalization()
{
    scanf_s("%d %d %d %d %d", &T, &M, &N, &V, &G);
    if (M)
    {
       // std::cout<<"DiskGroup::set" <<"  "<< M << std::endl;
    }
    
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
struct ClassStats {
    int total_write;  // 每个时间片的写入次数
    int max_read = 0;                  // 单次最大读取值
    int max_read_time = -1;            // -1表示从未发生读取
};
void  request_freq_init()
{
    /*
    // Read fre_del
    for (int i = 1; i <= M; i++)
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++)
        {
            int x;
            scanf_s("%d", &x);
            fre_del[i].push_back(x);
        }

    // Read fre_write
    for (int i = 1; i <= M; i++)
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++)
        {
            int x;
            scanf_s("%d", &x);
            fre_write[i].push_back(x);
        }

    auto total_writes_per_class = getRowSums(fre_write);

    // Read fre_read
    for (int i = 1; i <= M; i++)
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++)
        {
            int x;
            scanf_s("%d", &x);
            fre_read[i].push_back(x);
        }

    std::cout << "OK\n";
    std::cout.flush();
}

void skip_freq_init()
{
    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d");
        }
    }

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d");
        }
    }

    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= (T - 1) / FRE_PER_SLICING + 1; j++) {
            scanf("%*d");
        }
    }

    std::cout << "OK\n";
        scan_numbers(fre_read[i]);
    auto sorted_classes_by_max_read_time = getMaxIndices(fre_read);
    max_objects_per_class = ;
    */
   // /*
    std::vector<ClassStats> classes(M);
    std::string line;
    int time_slice = 0; // 当前时间片（行号）
    
    std::vector<ivector> fre_del(M, ivector());
    std::vector<ivector> fre_write;
    std::vector<int> max_objects_per_class(M + 1, 0); // Vector to store max objects per class, with a dummy 0 at the start
    for (int time_slice = 1; time_slice <= (T + 105) / FRE_PER_SLICING; time_slice++) {
        // Read delete frequencies for all classes
        for (int class_idx = 0; class_idx < M; class_idx++) {
            int del = 0;
            if (scanf("%d", &del) != 1) {
                std::cerr << "Invalid input format for delete at time " << time_slice << " for class " << class_idx << std::endl;
                continue; // Skip if reading fails
            }
            fre_del[class_idx].push_back(del);
        }
        
        // Read write frequencies for all classes
        for (int class_idx = 0; class_idx < M; class_idx++) {
            int write = 0;
            if (scanf("%d", &write) != 1) {
                std::cerr << "Invalid input format for write at time " << time_slice << " for class " << class_idx << std::endl;
                continue; // Skip if reading fails
            }

            // Record the total write count for the current class
            classes[class_idx].total_write += write;
            if (fre_write.size() <= class_idx) {
                fre_write.resize(class_idx + 1);
            }
            fre_write[class_idx].push_back(write);
        }

        

        // Read read frequencies for all classes
        for (int class_idx = 0; class_idx < M; class_idx++) {
            int read = 0;
            if (scanf("%d", &read) != 1) {
                std::cerr << "Invalid input format for read at time " << time_slice << " for class " << class_idx << std::endl;
                continue; // Skip if reading fails
            }

            // Update maximum read statistics
            if (read > classes[class_idx].max_read) {
                classes[class_idx].max_read = read;
                classes[class_idx].max_read_time = time_slice;
            }
        }
        std::cerr << "fre_del[" << "] = ";
    }
    // Update the maximum number of objects existing simultaneously for each class
    for (int class_idx = 1; class_idx <= M; class_idx++) {
        int current_objects = 0; // Track current objects for this class

        // Iterate through all time slices up to the current one
        for (int t = 0; t < fre_del[class_idx - 1].size(); t++) {
            current_objects += fre_write[class_idx - 1][t]; // Add written objects
            current_objects -= fre_del[class_idx - 1][t]; // Subtract deleted objects

            // Ensure current_objects doesn't go below zero
            current_objects = std::max(0, current_objects);

            // Update the maximum objects for this class
            max_objects_per_class[class_idx] = std::max(max_objects_per_class[class_idx], current_objects);
        }
    }
     // 按max_read_time排序（未发生读取的排最后）
     std::vector<int> class_indices(16);
     for (int i = 0; i < 16; ++i) class_indices[i] = i;
 
     std::sort(class_indices.begin(), class_indices.end(), 
         [&classes](int a, int b) {
             // 处理从未读取的情况（max_read_time = -1）
             if (classes[a].max_read_time == -1) return false;
             if (classes[b].max_read_time == -1) return true;
             return classes[a].max_read_time < classes[b].max_read_time;
         });
 
    // 保存排序结果到vector
    std::vector<int> sorted_classes_by_max_read_time;
    sorted_classes_by_max_read_time.push_back(0); // 0 is a dummy value
     for (int idx : class_indices) {
         sorted_classes_by_max_read_time.push_back(idx + 1);
     }
    // std::cerr << "begin to set disk group" << std::endl;
    // */
    // ⚠️ Declare disk_group_maneger as a global variable
    // normalize the total_writes_per_class
    disk_group_maneger.set(
    sorted_classes_by_max_read_time,
    max_objects_per_class,
    G / 2, N, V, M);

    // std::cerr << "begin to say ok" << std::endl;
    std::cout << "OK";
    std::cout.flush();
    // std::cerr << "end to say ok" << std::endl;
}

void request_timestamp()
{
    int timestamp = 0;
    std::cerr << "begin to scan stamp" << std::endl;
    scanf_s("%*s%d", &timestamp);
    std::cerr << "scaned stamp: " << timestamp << std::endl;
    std::cout << "TIMESTAMP " << timestamp;
    std::cout.flush();
    std::cerr << "end to scan stamp" << std::endl;
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
            std::cout << obj.replica[i];
            for (int j = 1; j <= obj.size; j++)
                std::cout << ' ' << obj.unit[i][j];
            std::cout << '\n';
        }
    }

    std::cout.flush();
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

    std::cout.flush();
}

void request_read() {
    int n_read;
    ivector req_ids;
    ivector object_ids;
    scanf_s("%d", &n_read);

    for (int i = 1; i <= n_read; i++) {
        int object_id;
        int req_id;
        scanf_s("%d%d", &req_id, &object_id);

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

    std::cout.flush();

}