#include "src/disk.h"
#include "src/defs.h"
int main(void)
{
    printf("Welcome to use LDY's EXT2 File System Lite!\nNow opening the disk...\n");

    uint8_t flag = open_disk();
    if(flag == -1)
    {
        printf("Faild to open the disk!The file system will close in 5 seconds\n");
        /* sleep(5); */
        exit(OPEN_ERR);
    }
    // TODO
    uint16_t cur_pid;
    
    flag = close_disk();
    if(flag == -1)
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
