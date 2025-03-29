#include <array>
#include <vector>
#include <stdexcept>
#include <iostream> // For std::cout

#include <libs.hpp>
#include <vector> // Ensure std::vector is included

using ivector = std::vector<int>; // Define ivector as an alias for std::vector<int>

// Constants
#define SIZE_OF_PARTITION -1234
#define INVALIDE_NUM -1234

/*
 * utility functions
 */
template <size_t REP_NUM>
int count_zero(const iarray<REP_NUM> &vec)
{
    int count = 0;
    for (int i = 0; i < vec.size(); i++)
    {
        if (vec[i] == 0)
        {
            count++;
        }
    }
    return count;
}

struct disk_group_unit_info
{
    int disk_group_id;
    int disk_group_unit_id;
    bool is_fallback_disk_group;
    bool is_fail_lookup;

    disk_group_unit_info()
        : disk_group_id(0), disk_group_unit_id(0), is_fallback_disk_group(false), is_fail_lookup(true)
    {
    }
    disk_group_unit_info(int disk_group_id, int disk_group_unit_id)
        : disk_group_id(disk_group_id), disk_group_unit_id(disk_group_unit_id), is_fallback_disk_group(false), is_fail_lookup(false)
    {
    }
    disk_group_unit_info(bool is_fallback_disk_group)
        : disk_group_id(INVALIDE_NUM), disk_group_unit_id(INVALIDE_NUM), is_fallback_disk_group(true), is_fail_lookup(false)
    {
    }
    disk_group_unit_info(int disk_group_id, int disk_group_unit_id, bool is_fallback_disk_group, bool is_fail_lookup)
        : disk_group_id(disk_group_id), disk_group_unit_id(disk_group_unit_id), is_fallback_disk_group(is_fallback_disk_group), is_fail_lookup(is_fail_lookup)
    {
    }
};

struct disk_unit_info
{
    // `disk_group_unit` is a 2D array of pairs, where each pair contains the disk group id and the unit id
    // shape: [disk_group_amount][REP_NUM]
    // pair <disk_group_id, unit_id>
    std::array<std::array<std::pair<int, int>, REP_NUM>, REP_NUM> disk_group_unit;
    int blind_unit_amount;

    disk_unit_info(std::array<std::array<std::pair<int, int>, REP_NUM>, REP_NUM> disk_group_unit, int blind_unit_amount) {}
};

DiskGroup::DiskGroup(
    // tag level info, for disk-group-to-disk
    const ivector &tags_sorted_by_busy_time,

    // index begin with **1**
    const ivector &sizes_sorted_by_tag,

    // disk level info, for disk-to-disk
    int repica_block_size,

    // index begin with **1**
    int disk_amount,

    // index begin with **1**
    int disk_size,

    // index begin with **0**
    int tag_amount

    ) : repica_block_size(repica_block_size),
        tag_block_size(repica_block_size * REP_NUM),
        disk_amount(disk_amount),
        disk_size(disk_size),
        tag_amount(tag_amount)
{
    /***************************************
    *  initialize fallback disk settings
    // `is_fallback_available` is set to true if the disk amount is not a multiple of REP_NUM
    // `fallback_disk_size` is set to the next multiple of REP_NUM
    *********************************************/
    if (disk_amount % REP_NUM != 0)
    {
        is_fallback_available = true;

        // `fallback_disk_start_index` is start with **1**
        fallback_disk_size = disk_size - (disk_amount % REP_NUM) - REP_NUM;
        fallback_disk_start_index = disk_amount - fallback_disk_size;
    }
    else
    {
        is_fallback_available = false;
        fallback_disk_size = INVALIDE_NUM;       // Invalid value
        fallback_disk_start_index = disk_amount; // Invalid value
    }

    /***************************************
     *  initialize disk group settings
     * ***************************************/
    // `limit_disk_amount` is set to the maximum number of disks allowed
    // [[tag_block1], [tag_block2], [tag_block3], [tag_block4]... [free_tag_block]]
    tag_block_limit_amount_for_disk = disk_size / tag_block_size;

    // `disk_group_id_head_disk_mapping_list` is initialized to hold the disk group mapping
    // e.g. [1, 4, 7, 10, ...]
    // ⚠️ `fallback_disk_start_index` is the head disk of the fallback disk group
    // ⚠️ `fallback_disk_size` >= i
    for (int disk_id = 1; disk_id < fallback_disk_start_index; disk_id += REP_NUM)
    {
        disk_group_id_head_disk_mapping_list.push_back(disk_id);
    }
    disk_group_amount = disk_group_id_head_disk_mapping_list.size();

    /***************************************
     *  initialize tag_tag_block settings
     * ***************************************/
    // first line is [0, 0, 0, 0, ...](disk 0)
    tag_block_start_index_sorted_by_tag_for_each_disk.push_back(ivector(tag_amount, 0));
    tag_block_amount_sorted_by_tag_for_each_disk.push_back(ivector(tag_amount, 0));

    /***************************************
     *  initialize tag_tag_block settings::set zeros to block related variables
     * ***************************************/
    // index `current_group_id` begin with **1**
    for (int current_group_id = 1; current_group_id <= disk_group_amount; current_group_id++)
    {
        // `tag_block_start_index_sorted_by_tag_for_each_disk` is initialized to hold the start tag_blocks for each tag
        // size: [tag_amount]
        // [0, 0, 0, 0, ...](disk i)
        // start tag_block
        ivector start_tag_block_sorted_by_tag_temp = ivector(tag_amount + 1, 0);
        ivector tag_block_amount_sorted_by_tag_temp = ivector(tag_amount + 1, 0);
        tag_block_amount_sorted_by_tag_temp[0] = 1;

        // tag_block_start_index_sorted_by_tag_for_each_disk is filled and pushed back
        tag_block_start_index_sorted_by_tag_for_each_disk.push_back(start_tag_block_sorted_by_tag_temp);

        // `tag_block_amount_sorted_by_tag_for_each_disk` is initialized to hold the tag_block amounts for each tag
        tag_block_amount_sorted_by_tag_for_each_disk.push_back(tag_block_amount_sorted_by_tag_temp);
    }

    /***************************************
     *  initialize tag_tag_block settings::set block related variables
     * ***************************************/
    for (int current_tag = 1; current_tag <= tag_amount; current_tag++)
    {
        // e.g.:
        // 1 1
        // 1 1
        // 1
        // remaining_tag_block_count = sizes_sorted_by_tag[current_tag] % repica_block_size;
        int remaining_tag_block_count = sizes_sorted_by_tag[current_tag] % repica_block_size;
        int basic_tag_block_amount = sizes_sorted_by_tag[current_tag] / repica_block_size;
        for (int current_disk_group = 1; current_disk_group <= disk_group_amount; current_disk_group++)
        {
            // `finally_tag_block_amount` if is not full
            // reduce overflow
            int finally_tag_block_amount = current_disk_group > remaining_tag_block_count ? basic_tag_block_amount : basic_tag_block_amount + 1;
            int stimulational_tag_block_amount =
                tag_block_start_index_sorted_by_tag_for_each_disk[current_disk_group][current_tag - 1] +
                tag_block_amount_sorted_by_tag_for_each_disk[current_disk_group][current_tag - 1] + finally_tag_block_amount;
            if (stimulational_tag_block_amount > tag_block_limit_amount_for_disk)
            {
                finally_tag_block_amount -= (stimulational_tag_block_amount - tag_block_limit_amount_for_disk);
                if (current_disk_group + 1 <= disk_group_amount)
                {
                    // this operation is 十分甚至九分的离谱
                    tag_block_amount_sorted_by_tag_for_each_disk[current_disk_group + 1][current_tag] +=
                        (stimulational_tag_block_amount - tag_block_limit_amount_for_disk);
                }
            } // reduced overflow

            tag_block_amount_sorted_by_tag_for_each_disk[current_disk_group][current_tag] = finally_tag_block_amount;
            // `tag_block_start_index_sorted_by_tag_for_each_disk` begin with **1**
            // 0     1 2 3 4 5 6 7 8 9 10
            // null  s i z e 1 s i z e 2
            // tag_block_size is the **6th**
            tag_block_start_index_sorted_by_tag_for_each_disk[current_disk_group][current_tag] =
                tag_block_start_index_sorted_by_tag_for_each_disk[current_disk_group][current_tag - 1] +
                tag_block_amount_sorted_by_tag_for_each_disk[current_disk_group][current_tag - 1];
        }
    }
}

Object DiskGroup::write_to_group(int object_id, int object_tag, int object_size)
{
    Object obj{};
    int write_times = object_size / REP_NUM;
    int blind_unit_amount = object_size % REP_NUM;
    disk_group_unit_info group_pos = find_available_disk_space(object_id, object_tag, object_size);
    group_pos = find_available_disk_space(object_id, object_tag, object_size);
    if (group_pos.is_fallback_disk_group == false)
    {
        if (write_times == 0)
        {
            // write to disk group
            // `disk_group_id` begin with **1**
            // `unit_id` begin with **1**
            disk_unit_info disk_pos = write_disk_group_position(group_pos.disk_group_id, group_pos.disk_group_unit_id, blind_unit_amount, object_id);

            obj.size = object_size;
            obj.is_delete = false;

            obj.replica[0] = group_pos.disk_group_id;
            for (int i = 1; i <= REP_NUM; i++)
            {
                // warring: `disk_group_id` begin with **1**
                // magical number 0 is the first replica
                obj.replica[i] = disk_pos.disk_group_unit[i][0].first;
            }

            obj.unit[0] = new int[object_size + 1];
            for (int current_rep = 1; current_rep <= REP_NUM; current_rep++)
            {
                int *unit_tmp = new int{1};
                for (int current_object_block = 0; current_object_block < REP_NUM; current_object_block++)
                {
                    if (disk_pos.disk_group_unit[current_object_block][current_rep].second != INVALIDE_NUM)
                    {
                        unit_tmp[current_object_block] = disk_pos.disk_group_unit[current_object_block][current_rep].second;
                    }
                }

                obj.unit[current_rep] = unit_tmp;
            }
        }
        else if (write_times == 1)
        {
            // write to disk group
            // `disk_group_id` begin with **1**
            // `unit_id` begin with **1**
            disk_unit_info disk_pos1 = write_disk_group_position(group_pos.disk_group_id, group_pos.disk_group_unit_id, 0, object_id);
            disk_unit_info disk_pos2 = write_disk_group_position(group_pos.disk_group_id, group_pos.disk_group_unit_id + 1, blind_unit_amount, object_id);
        }
        else
        {
            throw std::runtime_error("object size is not in the range of 1-5");
        }

        return obj;
    }
    else
    {

        auto replica_disk_ids = alloc_replica_disk_ids(object_id, object_tag);

        std::copy(replica_disk_ids.begin(), replica_disk_ids.end(), obj.replica);
        for (int i = 1; i <= REP_NUM; i++)
        {
            ivector unit_indices =
                alloc_unit_indices(object_id, object_tag, object_size, obj.replica[i]);
            int *tmp = new int[object_size + 1];
            std::copy(unit_indices.begin(), unit_indices.end(), tmp);
            obj.unit[i] = tmp;
        }

        return obj;
    }
}

/* ****************************
 * operation functions for disk group-disk layer
 ******************************** */

// * @name: `read_disk_group_unit_usage`
// * layer: disk group-disk
// * int -> int -> int
// * return: how many non-zero ints are in the disk group unit
// * @params:
// * disk_group_id: the id of the disk group
// * trac_unit_id: the id of the unit
// * @return: how many zero ints are in the disk group unit
int DiskGroup::read_disk_group_unit_usage(int disk_group_id, int unit_id)
{
    iarray<REP_NUM> disk_group_unit;
    for (size_t disk_offset = 0; disk_offset < REP_NUM; disk_offset++)
    {
        int disk_index = disk_group_id_head_disk_mapping_list[disk_group_id] + disk_offset;
        disk_group_unit[disk_offset] = disk[disk_index][unit_id];
    }

    return count_zero(disk_group_unit);
}

disk_unit_info DiskGroup::read_disk_group_position(int disk_group_id, int unit_id, int blind_unit_amount)
{

    std::array<std::array<std::pair<int, int>, REP_NUM>, REP_NUM> disk_group_unit;
    int blind_offset = REP_NUM - blind_unit_amount;
    for (size_t unit_offset = 0; unit_offset < REP_NUM; unit_offset++)
    {
        std::array<std::pair<int, int>, REP_NUM> rep_position;
        for (size_t rep_offset = 0; rep_offset < REP_NUM; rep_offset++)
        {
            std::pair<int, int> new_unit;
            if (unit_offset < blind_offset)
            {
                new_unit = std::make_pair(disk_group_id_head_disk_mapping_list[disk_group_id] + ((unit_offset + rep_offset) % REP_NUM),
                                          unit_id + rep_offset * repica_block_size);
            }
            else
            {
                new_unit = std::make_pair(INVALIDE_NUM, INVALIDE_NUM);
            }
            rep_position[rep_offset] = new_unit;
        }
        disk_group_unit[unit_offset] = rep_position;
    }
    return disk_unit_info(disk_group_unit, blind_unit_amount);
}

disk_unit_info DiskGroup::write_disk_group_position(int disk_group_id, int unit_id, int blind_unit_amount, int value)
{
    disk_unit_info actual_pos = read_disk_group_position(disk_group_id, unit_id, blind_unit_amount);

    for (size_t unit_offset = 0; unit_offset < REP_NUM; unit_offset++)
    {
        for (size_t rep_offset = 0; rep_offset < REP_NUM; rep_offset++)
        {
            if (actual_pos.disk_group_unit[unit_offset][rep_offset].first != INVALIDE_NUM)
            {
                int disk_index = actual_pos.disk_group_unit[unit_offset][rep_offset].first;
                int unit_index = actual_pos.disk_group_unit[unit_offset][rep_offset].second;
                disk[disk_index][unit_index] = value;
            }
        }
    }
    return actual_pos;
}

/* ****************************
 * operation functions for object-disk group layer
 ******************************** */
disk_group_unit_info DiskGroup::find_available_disk_space(int object_id, int object_tag, int object_size)
{

    for (int disk_group_id = 1; disk_group_id <= disk_group_amount; disk_group_id++)
    {
        // `disk_group_id` begin with **1**
        int disk_index = disk_group_id_head_disk_mapping_list[disk_group_id - 1];
        int tag_block_start_index =
            tag_block_start_index_sorted_by_tag_for_each_disk[disk_group_id][object_tag];
        // `tag_block_size` begin with **1**
        // 0     1 2 3 4 5 6 7 8 9 10
        // null  s i z e 1 s i z e 2
        // tag_block_size is the **6th**
        int former_tag_block_total_size = (tag_block_start_index - 1) * tag_block_size;
        int first_tag_block_header_position = former_tag_block_total_size + 1;
        int tag_block_amount_in_this_disk_group =
            tag_block_amount_sorted_by_tag_for_each_disk[disk_group_id][object_tag];

        for (int offset_of_tag_block = 0; offset_of_tag_block < tag_block_amount_in_this_disk_group; offset_of_tag_block++)
        {
            int tag_block_position =
                first_tag_block_header_position + offset_of_tag_block * tag_block_size;

            // ⚠️ unit_position < tag_block_position+repica_block_size **- 1**;
            for (int unit_id = tag_block_position; unit_id < tag_block_position + repica_block_size - 1; unit_id++)
            {
                auto maybe_position = read_2unit(disk_group_id, unit_id, object_size);
                if (maybe_position.is_fail_lookup == false)
                {
                    // `disk_group_id` begin with **1**
                    // `unit_id` begin with **1**
                    return maybe_position;
                }
            }
        }
    }
    return disk_group_unit_info(true);
}

disk_group_unit_info DiskGroup::read_2unit(int disk_group_id, int unit_id, int object_size)
{
    auto pos = disk_group_unit_info();
    int current_unit_usage = read_disk_group_unit_usage(disk_group_id, unit_id);
    int later_unit_usage = read_disk_group_unit_usage(disk_group_id, unit_id);
    if (object_size / REP_NUM == 0)
    {
        if (current_unit_usage + later_unit_usage == object_size || current_unit_usage + later_unit_usage == 6)
        {
            // ⚠️ unit_id+1 is the next unit
            // ⚠️ magical number 6 is the size of the tag block
            auto pos = disk_group_unit_info(disk_group_id, unit_id + 1, false, false);
            return pos;
        }
        return disk_group_unit_info();
    }
    else if (object_size / REP_NUM == 1)
    {
        if (current_unit_usage + later_unit_usage == object_size)
        {
            auto pos = disk_group_unit_info(disk_group_id, unit_id, false, false);
            return pos;
        }
        return disk_group_unit_info();
    }
    else
    {
        throw std::runtime_error("object size is in the range of 1-5");
    }
}

//********************************************* *//
iarray<REP_NUM + 1> DiskGroup::alloc_replica_disk_ids_in_fallback(int object_id, int object_tag)
{
    static int replica_disk_start = 0;
    replica_disk_start += 3;
    int disk_start = replica_disk_start;
    iarray<REP_NUM + 1> arr{
        0,
        disk_start % fallback_disk_size + fallback_disk_start_index,
        (disk_start + 1) % fallback_disk_size + fallback_disk_start_index,
        (disk_start + 2) % fallback_disk_size + fallback_disk_start_index};
    return arr;
}

// magical number G/2 is replica_block_size
// initialize the disk group manager
DiskGroup disk_group_manager(
    tag_sort_by_busy_time,
    sizes_sorted_by_tag,
    G / 2, N, V, M);