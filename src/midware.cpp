#include <array>
#include <vector>
#include <stdexcept>
#include <iostream> // For std::cout

#include <libs.hpp>
#define SIZE_OF_PARTITION -1234
// ivector tags_sorted_by_busy_time(M);
// ivector sizes_sorted_by_tag(M);

class DiskGroup {
public:

    /*
        * Constructor for DiskGroup
        * @param groupSizes: Vector of group sizes
        * @param tags_sorted_by_busy_time: Vector of tags sorted by busy time
        * @param sizes_sorted_by_tag: Vector of sizes sorted by tag
        * @param token_size: Size of token
        * @param tag_size: Size of tag
        * @param dummy: Dummy parameter
        
    */
    DiskGroup(
            const ivector& tags_sorted_by_busy_time,
            const ivector& sizes_sorted_by_tag,
            int disk_num, int token_size, int tag_size, int): block_size(token_size)
        
    {
        /*
        logic for DiskGroup: for arranged tags allocate partitions in raided disks(all 3-sized disk groups) for them, 
        if cannot contain all the partitions, allocate the rest in the next disk
        */
        // check is_fallback_available
        if ( disk_num % REP_NUM != 0) 
        {
            is_fallback_available = true;
            fallback_disk_size = (disk_num % REP_NUM)+REP_NUM;
        } else {
            is_fallback_available = false;
            fallback_disk_size = 0;
        }
        
        // allocate partitions in raided disks(all 3-sized disk groups) for them
        
        // allocate the rest in the next disk
        // nothing to do here
        
    }
    
    void writeToGroup(int object_id, int object_tag, int object_size) {
        // traverse all partitions for this tag in the groups and write the object in first empty unit
        // code here
        // if no empty unit found, throw an exception
        // throw std::runtime_error("No empty unit found");
        /*
        for (int i = 0; i < groupSize; ++i) {
            if (groupSize == 3) {
                
                disk[startDisk + i][position] = value + i; // Example logic for size 3
            } else if (groupSize == 4) {
                disk[startDisk + i][position] = value * (i + 1); // Example logic for size 4
            } else if (groupSize == 5) {
                disk[startDisk + i][position] = value - i; // Example logic for size 5
            } else {
                disk[startDisk + i][position] = value; // Default logic
            }
        }
        
    }


    void printDiskState() const {
        for (int i = 0; i < MAX_DISK_NUM; ++i) {
            std::cout << "Disk " << i << ": ";
            for (int j = 0; j < MAX_DISK_SIZE; ++j) {
                if (disk[i][j] != 0) {
                    std::cout << "[" << j << "]=" << disk[i][j] << " ";
                }
            }
            std::cout << "\n";
        }
    }

private:
    // groupSizes: Vector of group sizes :: int[10]
    // ivector groupSizes;
    // ivector groupStartId; // Stores start IDs for each group, generated from groupSizes😅
    bool is_fallback_available;
    int fallback_disk_size;
    int block_size;
    ivector start_block_sorted_by_tag;
    ivector block_amount_sorted_by_tag;

}; // Closing brace for DiskGroup class

