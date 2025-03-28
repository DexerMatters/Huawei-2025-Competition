#include <array>
#include <vector>
#include <stdexcept>
#include <iostream> // For std::cout

#include <libs.hpp>
#include <vector> // Ensure std::vector is included

using ivector = std::vector<int>; // Define ivector as an alias for std::vector<int>

// Constants
#define SIZE_OF_PARTITION -1234

class DiskGroup
{
public:

    DiskGroup(
        // tag level info, for disk-group-to-disk
        const ivector &tags_sorted_by_busy_time,
        const ivector &sizes_sorted_by_tag,
        // disk level info, for disk-to-disk
        int token_size,
        int disk_amount,
        int disk_size,
        int tag_count)
        : block_size(token_size)
    {
        initializeFallback(disk_amount);
        calculateBlockLimits(disk_size);
        allocatePartitions(disk_amount, sizes_sorted_by_tag, tag_count);
    }


    void write_to_group(int object_id, int object_tag, int object_size)
    {
        // code
    }

private:
    // Member variables
    bool is_fallback_available;
    int fallback_disk_size;
    int block_size; // Size of each block
    int block_limit_for_disk;
    std::vector<ivector> start_block_sorted_by_tag; // Start blocks for each tag
    std::vector<ivector> block_amount_sorted_by_tag; // Block amounts for each tag

    /**
     * Initializes fallback disk settings.
     * @param disk_amount: Number of disks
     */
    void initializeFallback(int disk_amount)
    {
        if (disk_amount % REP_NUM != 0)
        {
            is_fallback_available = true;
            fallback_disk_size = (disk_amount % REP_NUM) + REP_NUM;
        }
        else
        {
            is_fallback_available = false;
            fallback_disk_size = 0;
        }
    }

    /**
     * Calculates the block limit for each disk.
     * throw all unfit space.e.g. [[block_1], [block_2], ...[free_space]]<br/>
     * count by 1<br>
     *  -> this.`block_limit_for_disk`
     * @param disk_size: Size of each disk
     * Block limit for each disk to class field `block_limit_for_disk`
     */
    void calculateBlockLimits(int disk_size)
    {
        block_limit_for_disk = disk_size / block_size;
    }

    /**
     * Allocates partitions across disks.
     * @param disk_amount: Number of disks
     * @param sizes_sorted_by_tag: Vector of sizes sorted by tag
     * @param tag_count: Number of tags
     */
    void allocatePartitions(int tag_amount, const ivector &sizes_sorted_by_tag, int tag_count)
    {
        // Initialize storage for each disk
        for (size_t current_disk = 0; current_disk < tag_amount; current_disk++)
        {
            start_block_sorted_by_tag.push_back(ivector(tag_count, 0));
            block_amount_sorted_by_tag.push_back(ivector(tag_count, 0));
        }

        // Allocate blocks for each tag
        for (int current_tag = 0; current_tag < tag_count; current_tag++)
        {
            int tag_s_block_amount = sizes_sorted_by_tag[current_tag] / block_size;
            int tag_remaining_block = sizes_sorted_by_tag[current_tag] % block_size;

            for (int current_disk = 0; current_disk < tag_amount; ++current_disk)
            {
                allocateBlocksForTag(current_disk, current_tag, tag_s_block_amount, tag_remaining_block, tag_amount);
            }
        }
    }

    /**
     * Allocates blocks for a specific tag on a specific disk.
     * @param current_disk: Current disk index
     * @param current_tag: Current tag index
     * @param tag_s_block_amount: Block amount for the tag
     * @param tag_remaining_block: Remaining blocks for the tag
     * @param disk_amount: Total number of disks
     */
    void allocateBlocksForTag(int current_disk, int current_tag, int tag_s_block_amount, int tag_remaining_block, int disk_amount)
    {
        block_amount_sorted_by_tag[current_disk][current_tag] += tag_s_block_amount;

        if (current_tag == 0)
        {
            start_block_sorted_by_tag[current_disk][current_tag] =
                (current_disk == 0 ? 0 : start_block_sorted_by_tag[current_disk - 1][current_tag] + block_amount_sorted_by_tag[current_disk - 1][current_tag]);
        }
        else
        {
            start_block_sorted_by_tag[current_disk][current_tag] =
                start_block_sorted_by_tag[current_disk][current_tag - 1] + block_amount_sorted_by_tag[current_disk][current_tag - 1];
        }

        if (current_disk < tag_remaining_block)
        {
            block_amount_sorted_by_tag[current_disk][current_tag] += 1;
        }

        adjustExcessBlocks(current_disk, current_tag, disk_amount);
    }

    /**
     * Adjusts excess blocks if they exceed the limit for the current disk.
     * @param current_disk: Current disk index
     * @param current_tag: Current tag index
     * @param disk_amount: Total number of disks
     */
    void adjustExcessBlocks(int current_disk, int current_tag, int disk_amount)
    {
        if (block_amount_sorted_by_tag[current_disk][current_tag] > block_limit_for_disk)
        {
            int excess_blocks = block_amount_sorted_by_tag[current_disk][current_tag] - block_limit_for_disk;
            block_amount_sorted_by_tag[current_disk][current_tag] = block_limit_for_disk;

            // Distribute the excess blocks to the next disk
            if (current_disk + 1 < disk_amount)
            {
                block_amount_sorted_by_tag[current_disk + 1][current_tag] += excess_blocks;
            }
        }
    }
};
