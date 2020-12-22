#define SYSTEM_CODE (0x18010421)    
#define DATA_BLOCK_SIZE (1024)
#define MAX_FILE_SIZE   (6144)
#define DATA_BLOCK_NUM  (4063)
#define INODE_NUM   (1024)
#define TYPE_FILE   (0)
#define TYPE_DIR    (1)
#define ROOT_DIR_ID (2)

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef struct super_block {
    int32_t magic_num;                  // 幻数
    int32_t free_block_count;           // 空闲数据块数
    int32_t free_inode_count;           // 空闲inode数
    int32_t dir_inode_count;            // 目录inode数
    uint32_t block_map[128];            // 数据块占用位图
    uint32_t inode_map[32];             // inode占用位图
} sp_block;
extern sp_block SP_BLK;
struct inode {
    uint32_t size;              // 文件大小
    uint16_t file_type;         // 文件类型（文件/文件夹）
    uint16_t link;              // 连接数
    uint32_t block_point[6];    // 数据块指针
};

extern struct inode inodes[1024];
// Error codes
#define OPEN_ERR    -1
#define CLOSE_ERR   -2
#define CODE_ERR    -3
#define SYNTAX_ERR  -4
#define NO_BLOCK    -5
#define PATH_ERR    -6
