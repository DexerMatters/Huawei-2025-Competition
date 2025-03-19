#include <iostream>


#define MAX_DISK_NUM (10 + 1)
#define MAX_DISK_SIZE (16384 + 1)
#define MAX_REQUEST_NUM (30000000 + 1)
#define MAX_OBJECT_NUM (100000 + 1)
#define REP_NUM (3)
#define FRE_PER_SLICING (1800)
#define EXTRA_TIME (105)

struct Request
{
    int object_id;
    int prev_id;
    bool is_done;
};

struct Object
{
    int replica[REP_NUM + 1];
    int* unit[REP_NUM + 1];
    int size;
    int last_request_point;
    bool is_delete;
};

Request request[MAX_REQUEST_NUM];
Object object[MAX_OBJECT_NUM];


// Count of timestamps in the input
int T;

// Count of object tags
int M;

// Disk count
int N;

// Unit count per disk
int V;

// Maximum token consumption per timestamp
int G;


// Disk data
int disk[MAX_DISK_NUM][MAX_DISK_SIZE];

// Disk data pointer
int disk_point[MAX_DISK_NUM];



/****************************************************************************************
 * Interaction functions
 ****************************************************************************************/

 // Initialize the environment
void start_interaction();

// Request a timestamp
void request_timestamp();


/****************************************************************************************
 * Action functions
 ****************************************************************************************/

 /**
  * Create a new object with the specified size
  * @param object_id Object ID
  * @param size Object size
  */
Object new_object(int object_id, int size);

/**
 * Jump to the specified location in the disk
 * @param disk_id Disk ID
 * @param dest Destination unit
 * @warning A jump costs `G` tokens
 */
void jump(int disk_id, int dest);

/**
 * Read the data in the current location from the disk.
 * Pointer will be moved to the next unit after reading.
 * @param disk_id Disk ID
 * @return Data read from the disk
 * @warning A read costs `1` token
 */
int read(int disk_id);

/**
 * Pass the disk to the next unit.
 * Pointer will be moved to the next unit after passing.
 * @param disk_id Disk ID
 * @warning A pass costs `1` token
 */
void pass(int disk_id);

/**
 * Clean up all the allocated memory
 */
void cleanAll();