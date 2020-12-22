#include "../src/disk.h"
#include "../src/defs.h"
#include <stdio.h>
#include <string.h>
int UpdateSuperBlock(void)
{
    char *p = (char *)(&SP_BLK);
    char wbuf[512] = {0};
    int i,j;
    open_disk();
    for(i=0;i<512;i++)
    {
        wbuf[i] = p[i];
    }
    disk_write_block(0,wbuf);
    for(i=512;i<1024;i++)
    {
        wbuf[i - 512] = (i<656)?p[i]:0;
    }
    disk_write_block(1,wbuf);
    close_disk();
}
int main(void)
{
    SP_BLK.magic_num = 0x12345678;
    SP_BLK.inode_map[31] = 0xabcddcba;
    UpdateSuperBlock();
    char rbuf[512] = {0};
    open_disk();
    disk_read_block(0,rbuf);
    int i,j;
    for(i=0;i<512;i++)
    {
        printf("%x ",rbuf[i]);
        if(i%16 == 0)printf("\n");
    }
    disk_read_block(1,rbuf);
    for(i=0;i<512;i++)
    {
        printf("%x ",rbuf[i]);
        if(i%16 == 0)printf("\n");
    }
    close_disk();
}
