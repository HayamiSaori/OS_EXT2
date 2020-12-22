#include "src/disk.h"
#include "src/defs.h"
// super block:1024B=1KiB
// inodes:32768B=32KiB
int InitSuperBlock(void)
{
    uint8_t i,j;
    uint8_t buf[512] = {0};
    if(open_disk() == -1)
    {
        return OPEN_ERR;
    }
    else
    {
        
    }
    
}
int main(void)
{
    printf("Welcome to use LDY's EXT2 File System Lite!\nNow opening the disk...\n");

    if(open_disk() == -1)
    {
        printf("Faild to open the disk!The file system will close in 5 seconds\n");
        /* sleep(5); */
        exit(OPEN_ERR);
    }
    // TODO
    uint16_t cur_pid;
    
    if(close_disk() == -1)
    {
        printf("Faild to close the disk!The file system will close without saving the disk files\n");
        exit(CLOSE_ERR);
    }
    else
    {
        printf("Success to save disk file,thanks to use!\n");
        exit(0);
    }
}
