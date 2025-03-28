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
    int disk_id;
    int tag_block_id;
    int tag_block_size;
    int tag_block_start_index;
    int tag_block_amount;
};



class DiskGroup
{
public:
    /*
    * Constructor for DiskGroup
    * @param tags_sorted_by_busy_time: Vector of tags sorted by busy time
    * @param sizes_sorted_by_tag: Vector of sizes sorted by tag
    * @param token_size: Size of each token 
    * @param disk_amount: Number of disks
    * @param disk_size: Size of each disk
    * @param tag_amount: Number of tags
    */
    DiskGroup(
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


        ): repica_block_size(repica_block_size), 
            tag_block_size(repica_block_size* REP_NUM),
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
            fallback_disk_size = INVALIDE_NUM; // Invalid value
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
        for (int disk_id = 1; disk_id < fallback_disk_start_index; disk_id+= REP_NUM)
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
            ivector start_tag_block_sorted_by_tag_temp = ivector(tag_amount, 0);
            ivector tag_block_amount_sorted_by_tag_temp = ivector(tag_amount, 0);
            
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
                int finally_tag_block_amount = current_disk_group > remaining_tag_block_count? basic_tag_block_amount: basic_tag_block_amount+1;
                int stimulational_tag_block_amount = 
                    tag_block_start_index_sorted_by_tag_for_each_disk[current_disk_group][current_tag-1] + 
                    tag_block_amount_sorted_by_tag_for_each_disk[current_disk_group][current_tag-1] + finally_tag_block_amount;
                if (stimulational_tag_block_amount > tag_block_limit_amount_for_disk)
                {
                    finally_tag_block_amount -= (stimulational_tag_block_amount - tag_block_limit_amount_for_disk);
                    if (current_disk_group + 1 <= disk_group_amount) 
                    {   
                        // this operation is 十分甚至九分的离谱
                        tag_block_amount_sorted_by_tag_for_each_disk[current_disk_group + 1][current_tag] += 
                            (stimulational_tag_block_amount - tag_block_limit_amount_for_disk);
                    }
                }
                tag_block_amount_sorted_by_tag_for_each_disk[current_disk_group][current_tag] = finally_tag_block_amount;
                    
                tag_block_start_index_sorted_by_tag_for_each_disk[current_disk_group][current_tag] = 
                    tag_block_start_index_sorted_by_tag_for_each_disk[current_disk_group - 1][current_tag] + 
                    tag_block_amount_sorted_by_tag_for_each_disk[current_disk_group][current_tag];
            }
        }
    }


    void write_to_group(int object_id, int object_tag, int object_size)
    {
        // code
    }

private:
    /*****************
     * member variables
     ******************/
    // `is_fallback_available`
    // layer: external
    bool is_fallback_available;

    // `fallback_disk_start_index`
    // layer: external::phyical disk
    int fallback_disk_start_index;

    // `fallback_disk_size`
    // layer: external::phyical disk
    int fallback_disk_size;


    // `tag_amount`
    // layer: external::tag
    int tag_amount; 
    int repica_block_size; 
    int tag_block_size; 

    // `tag_block_limit_amount_for_disk`
    // layer: external::phyical disk
    int disk_amount;
    int disk_size; 
    int tag_block_limit_amount_for_disk;


    // `disk_group_amount`
    // layer: disk group
    int disk_group_amount;
    // `disk_group_id_head_disk_mapping_list`
    // layer: disk group - disk
    // size: [disk_group_amount]
    // shape[0] begins with **1**
    ivector disk_group_id_head_disk_mapping_list;
    /*
    * `tag_block_start_index_sorted_by_tag_for_each_disk`
    * layer: tag block
    * size: [disk_group_amount][tag_amount]
    * shape[0] begins with **1**, shape[1] begins with **0**
    */
    std::vector<ivector> tag_block_start_index_sorted_by_tag_for_each_disk;

    /*
    * `tag_block_amount_sorted_by_tag_for_each_disk`
    * layer: tag block
    * size: [disk_group_amount][tag_amount]
    * shape[0] begins with **1**, shape[1] begins with **0**
    */
    std::vector<ivector> tag_block_amount_sorted_by_tag_for_each_disk; 

    /* ****************************
    * operation functions for object-disk group layer
    ******************************** */
    disk_group_unit_info find_available_disk_space(int object_id, int object_tag, int object_size)
    {
        auto pos = disk_group_unit_info();
        return std::move(pos);
    }

    /* ****************************
    * operation functions for disk group-disk layer
    ******************************** */

    /*
    * @name: `read_disk_group_unit_usage`
    * layer: disk group-disk
    * int -> int -> int
    * return: how many non-zero ints are in the disk group unit
    * @params:
    * disk_group_id: the id of the disk group
    * trac_unit_id: the id of the unit
    * @return: how many non-zero ints are in the disk group unit
    */
    int read_disk_group_unit_usage(int disk_group_id, int trac_unit_id)
    {
        iarray<REP_NUM> disk_group_unit;
        for (size_t i = 0; i < REP_NUM; i++)
        {
            int disk_index = disk_group_id_head_disk_mapping_list[disk_group_id] + i;
            disk_group_unit[i] = disk[disk_index][trac_unit_id];
        }

        return count_zero(disk_group_unit);
    }
 
    
};

