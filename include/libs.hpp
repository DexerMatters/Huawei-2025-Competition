#ifndef LIBS_HPP
#define LIBS_HPP

#include <iostream>
#include <vector>
#include <array>

constexpr auto MAX_DISK_NUM = (10 + 1);
constexpr auto MAX_DISK_SIZE = (16384 + 1);
constexpr auto MAX_REQUEST_NUM = (30000000 + 1);
constexpr auto MAX_OBJECT_NUM = (100000 + 1);
constexpr auto REP_NUM = (3);
constexpr auto FRE_PER_SLICING(1800);
constexpr auto EXTRA_TIME(105);

#define rep_char(c, n) std::string(n, c)

#if defined(__linux__) || defined(__APPLE__)
#define scanf_s scanf
#endif

template <size_t N>
using iarray = std::array<int, N>;

using ivector = std::vector<int>;

typedef struct Request_
{
    int object_id;
    int prev_req_id;
    bool is_done;
} Request;

typedef struct Object_
{
    int replica[REP_NUM + 1];
    int *unit[REP_NUM + 1];
    int size;
    bool is_delete;
} Object;

extern Request request[MAX_REQUEST_NUM];
extern Object object[MAX_OBJECT_NUM];

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

extern std::vector<ivector> fre_del;

extern std::vector<ivector> fre_write;

extern std::vector<ivector> fre_read;

/****************************************************************************************
 * Interaction functions
 ****************************************************************************************/

void request_initalization();

void request_freq_init();

// Request a timestamp
void request_timestamp();

// Request a write operation
void request_write();

void request_delete();

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

std::string read(int object_id, int req_id);

void move_point(int disk_id);

/****************************************************************************************
 * Auxiliary functions
 ****************************************************************************************/

void scan_numbers(ivector &vector);

#endif // LIBS_HPP