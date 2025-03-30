#ifndef LIBS_HPP
#define LIBS_HPP

// #define DEBUG_MODE

#include <iostream>
#include <vector>
#include <array>
#include <queue>
#include <list>

template <size_t N>
using iarray = std::array<int, N>;

using ivector = std::vector<int>;
using bvector = std::vector<bool>;

constexpr auto MAX_DISK_NUM = (10 + 1);
constexpr auto MAX_DISK_SIZE = (16384 + 1);
constexpr auto MAX_REQUEST_NUM = (30000000 + 1);
constexpr auto MAX_OBJECT_NUM = (100000 + 1);
constexpr auto REP_NUM = (3);
constexpr auto FRE_PER_SLICING = (1800);
constexpr auto EXTRA_TIME = (105);
constexpr iarray<128> READING_COSTS =
{
    0, 64, 52, 42, 34, 28, 23, 19, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
};

#define rep_char(c, n) std::string(n, c)

#if defined(DEBUG_MODE)
#define scanf_s debug_scanf
#elif defined(__linux__) || defined(__APPLE__)
#define scanf_s scanf
#endif

struct Request
{
    int object_id;
    bvector process;
    bool is_done;
};

struct Object
{
    int replica[REP_NUM + 1];
    ivector unit[REP_NUM + 1];
    int size;
    bool is_delete;
};


extern Request request[MAX_REQUEST_NUM];
extern Object object[MAX_OBJECT_NUM];

extern ivector tokens;

extern int request_count;

// Count of timestamps in the input
extern int T;

// Count of object tags
extern int M;

// Disk count
extern int N;

// Unit count per disk
extern int V;

// Maximum token consumption per timestamp
extern int G;

// Disk data
extern int disk[MAX_DISK_NUM][MAX_DISK_SIZE];

// Disk data pointer
extern int disk_point[MAX_DISK_NUM];

// Disk partition size
extern int partition_size;


// Ordered requests

extern std::vector<std::list<int>> ordered_requests;

extern std::vector<ivector> fre_del;

extern std::vector<ivector> fre_write;

extern std::vector<ivector> fre_read;

/*
extern std::vector<int> tag_sort_by_busy_time;

extern std::vector<int> sizes_sorted_by_tag;
*/

/****************************************************************************************
 * Interaction functions
 ****************************************************************************************/

void request_initalization();

void request_freq_init();

void skip_freq_init();

// Request a timestamp
void request_timestamp();

// Request a write operation
void request_write();

void request_delete();

void request_read();

/****************************************************************************************
 * Action functions
 ****************************************************************************************/

void cleanAll();

// Write action

Object write(int object_id, int object_tag, int object_size);

iarray<REP_NUM + 1> alloc_replica_disk_ids(int object_id, int object_tag);

ivector alloc_unit_indices(int object_id, int object_tag, int object_size, int disk_id);

// Delete action

ivector del(int object_id);

void release_unit(int object_id);

// Read action

void make_read_request(int req_id, int object_id);

int find_closest_request(int disk_id);

std::pair<std::vector<std::string>, ivector> read();

/****************************************************************************************
 * Auxiliary fuctions
 ****************************************************************************************/

void scan_numbers(ivector& vector);

void reset_tokens();

int calculate_distance(int point, int dest);

int calculate_distance(int disk_id, int point, int object_id);

bool is_farther_than(int dest1, int dest2);

void record_request(int req_id);

void delete_recorded_request(int req_id);

bool process_request(int disk_id, int req_id);

int which_replica(int disk_id, int object_id);

/****************************************************************************************
 * Debug functions
 ****************************************************************************************/

int debug_scanf(const char* format, ...);
std::vector<int> getMaxIndices(const std::vector<std::vector<int>>& matrix);

std::vector<int> getRowSums(const std::vector<std::vector<int>>& matrix);

/****************************
 * middleware functions
**************************/
template <size_t REP_NUM> int count_zero(const iarray<REP_NUM> &vec);
struct disk_group_unit_info;
struct disk_unit_info;

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
   DiskGroup();
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
        );
    Object write_to_group(int object_id, int object_tag, int object_size);
    void set(// tag level info, for disk-group-to-disk
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
        );
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
    * shape[0] begins with **1**, shape[1] begins with **1**
    */
   std::vector<ivector> tag_block_start_index_sorted_by_tag_for_each_disk;

   /*
   * `tag_block_amount_sorted_by_tag_for_each_disk`
   * layer: tag block
   * size: [disk_group_amount][tag_amount]
   * shape[0] begins with **1**, shape[1] begins with **1**
   */
   std::vector<ivector> tag_block_amount_sorted_by_tag_for_each_disk; 
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
    int read_disk_group_unit_usage(int disk_group_id, int unit_id);
    disk_unit_info read_disk_group_position(int disk_group_id, int unit_id, int blind_unit_amount);
    disk_unit_info write_disk_group_position(int disk_group_id, int unit_id, int blind_unit_amount, int value);
    /* ****************************
    * operation functions for object-disk group layer
    ******************************** */
   disk_group_unit_info find_available_disk_space(int object_id, int object_tag, int object_size);
   disk_group_unit_info read_2unit(int disk_group_id, int unit_id, int object_size);
   //********************************************* *//
   iarray<REP_NUM + 1> alloc_replica_disk_ids_in_fallback(int object_id, int object_tag);
};


// Disk group manager
extern DiskGroup disk_group_maneger;

#endif // !LIBS_HPP