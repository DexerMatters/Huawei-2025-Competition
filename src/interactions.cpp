#include <libs.hpp>

#if defined(__linux__) || defined(__APPLE__)
#include <math.h>
#endif


void request_initalization()
{
    scanf_s("%d %d %d %d %d", &T, &M, &N, &V, &G);
    partition_size = V / M;

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
void request_freq_init()
{
    // Read fre_del
    for (int i = 1; i <= M; i++)
        scan_numbers(fre_del[i]);

    // Read fre_write
    for (int i = 1; i <= M; i++)
        scan_numbers(fre_write[i]);

    // Read fre_read
    for (int i = 1; i <= M; i++)
        scan_numbers(fre_read[i]);
}

void request_timestamp()
{
    int timestamp;
    scanf_s("TIMESTAMP %d", &timestamp);
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
        auto obj = write(object_id, object_tag, object_size);
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