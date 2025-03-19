#include <libs.hpp>

Object new_object(int object_id, int size) {

}

void jump(int disk_id, int dest) {
    disk_point[disk_id] = dest;
}

int read(int disk_id) {
    return disk[disk_id][disk_point[disk_id]++];
}

void pass(int disk_id) {
    disk_point[disk_id]++;
}

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