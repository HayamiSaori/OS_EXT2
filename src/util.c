// #include "disk.h"
// #include "defs.h"
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
int UpdateInode(void)
{
    char *p = (char *)inodes;
    char wbuf[512] = {0};
    int i,j=0,cur_block;
    open_disk();
    for(cur_block = 2;cur_block<=65;cur_block++)
    {
        for(i=0;i<512;i++)
        {
            wbuf[i] = p[i + j * 512];
        }
        disk_write_block(cur_block,wbuf);
    }
    close_disk();
}
// input a path,return the inode id of it
int ScanPath(char *path)
{
    uint8_t i,j;
    uint32_t cur_inode_id = ROOT_DIR_ID;
    if((path[0] == '/') || (path[strlen(path) - 1] == '/'))
    {
        return PATH_ERR;   
    }
    for(i=0;i<strlen(path);i++)
    {

    }
}