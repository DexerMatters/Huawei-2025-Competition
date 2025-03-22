#include <libs.hpp>
#include <algorithm>


void cleanAll() {
    for (auto& obj : object) {
        for (int i = 1; i <= REP_NUM; i++) {
            if (obj.unit[i] != nullptr) {
                delete obj.unit[i];
                obj.unit[i] = nullptr;
            }
        }
    }
}

Object write(int object_id, int object_tag, int object_size) {
    Object obj{};
    obj.size = object_size;
    obj.is_delete = false;

    auto replica_disk_ids = alloc_replica_disk_ids(object_id, object_tag);

    std::copy(replica_disk_ids.begin(), replica_disk_ids.end(), obj.replica);
    for (int i = 1; i <= REP_NUM; i++) {
        ivector unit_indices =
            alloc_unit_indices(object_id, object_tag, object_size, obj.replica[i]);
        int* tmp = new int[object_size + 1];
        std::copy(unit_indices.begin(), unit_indices.end(), tmp);
        obj.unit[i] = tmp;
    }

    return obj;
}

iarray<REP_NUM + 1> alloc_replica_disk_ids(int object_id, int object_tag) {
    static int replica_disk_start = 0;
    int disk_start = replica_disk_start++;
    iarray<REP_NUM + 1> arr {
        0,
        disk_start == 0 ? 1 : disk_start % N + 1,
        (disk_start + 1) % N + 1,
        (disk_start + 2) % N + 1
    };
    return arr;
}

ivector alloc_unit_indices(int object_id, int object_tag, int object_size, int disk_id) {
    ivector unit_indices = { 0 };
    int start = (object_tag - 1) * partition_size;
    // Disk is a circular array
    for (int i = start;; i = (i + 1) % V) {
        if (disk[disk_id][i + 1] == 0) {
            disk[disk_id][i + 1] = object_id;
            unit_indices.push_back(i + 1);
        }
        if (unit_indices.size() > object_size) {
            break;
        }
    }
    return unit_indices;
}