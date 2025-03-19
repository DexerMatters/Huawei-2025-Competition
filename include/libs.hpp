#define MAX_DISK_NUM (10 + 1)
#define MAX_DISK_SIZE (16384 + 1)
#define MAX_REQUEST_NUM (30000000 + 1)
#define MAX_OBJECT_NUM (100000 + 1)
#define REP_NUM (3)
#define FRE_PER_SLICING (1800)
#define EXTRA_TIME (105)

struct Request {
  int object_id;
  int prev_id;
  bool is_done;
};

struct Object {
  int replica[REP_NUM + 1];
  int *unit[REP_NUM + 1];
  int size;
  int last_request_point;
  bool is_delete;
};

Request request[MAX_REQUEST_NUM];
Object object[MAX_OBJECT_NUM];

namespace specs {

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

} // namespace specs

namespace disk {

// Disk data
int disks[MAX_DISK_NUM][MAX_DISK_SIZE];

// Disk data pointer
int ptrs[MAX_DISK_NUM];

} // namespace disk

int initialize();