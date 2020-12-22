#include "disk.h"
#include "defs.h"
#include "util.c"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
// super block:1024B=1KiB (disk block 0 to 1)
// inodes:32768B=32KiB (disk block 2 to 65)
// data blocks:4096-1-32=4063
#define MAX_CMD_LEN (512)
#define MAX_ARG_LEN (64)
#define MAXARGS (10)
int InitSuperBlock(void)
{
    uint8_t buf[512] = {0};
    uint16_t i;
    uint32_t *pbuf;
    uint8_t wbuf[512] = {0};
    if(open_disk() == -1)
    {
        return OPEN_ERR;
    }
    disk_read_block(0,buf);
    pbuf = (uint32_t *)buf;
    if(pbuf[0] != SYSTEM_CODE)
    {
        pbuf = (uint32_t *)wbuf;
        pbuf[0] = SYSTEM_CODE;
        pbuf[1] = DATA_BLOCK_NUM;
        pbuf[2] = INODE_NUM;
        pbuf[3] = 0;
        disk_write_block(0,wbuf);
        disk_read_block(0,buf);
        pbuf = (uint32_t *)buf;
    }
    SP_BLK.magic_num = pbuf[0];
    SP_BLK.free_block_count = pbuf[1];
    SP_BLK.free_inode_count = pbuf[2];
    SP_BLK.dir_inode_count = pbuf[3];
    for(i=4;i<128;i++)
    {
        SP_BLK.block_map[i - 4] = pbuf[i];
    }
    disk_read_block(1,buf);
    pbuf = (uint32_t *)buf;
    for(i=0;i<4;i++)
    {
        SP_BLK.block_map[124 + i] = pbuf[i];
    }
    for(i=4;i<36;i++)
    {
        SP_BLK.inode_map[i - 4] = pbuf[i];
    }
    if(close_disk() == -1)
    {
        return CLOSE_ERR;
    }
}
int InitInode(void)
{
    uint16_t i,j,k,count;
    uint16_t *p16;
    uint32_t *p32;
    uint8_t buf[512] = {0};
    uint8_t wbuf[512] = {0};
    if(open_disk() == -1)
    {
        return OPEN_ERR;
    }
    count = 0;
    for(i=2;i<=65;i++)
    {
        disk_read_block(i,buf);
        for(j=0;j<16;j++)
        {
            p32 = (uint32_t *)(buf + j * 32);
            p16 = (uint16_t *)(buf + j * 32);
            inodes[count].size = p32[0];
            inodes[count].file_type = p16[2];
            inodes[count].link = p16[3];
            for(k=0;k<6;k++)
            {
                inodes[count].block_point[k] = p32[2 + k];
            }
        }
    }
    // The inode of root direction has not been initialized.It means that the disk is just created
    if(inodes[2].file_type != TYPE_DIR) 
    {
        inodes[2].file_type = TYPE_DIR;
        UpdateInode();
    }
    if(close_disk() == -1)
    {
        return CLOSE_ERR;
    }
}


int getcmd(char *buf, int nbuf)
{
    printf("@ ");
    fgets(buf, nbuf,stdin);
    if(buf[0] == 0) // EOF
        return -1;
    return 0;
}
int isBlank(char c)
{
	if(c==' ' || c=='\t')
		return 1;
	else
		return 0;
}
void ScanCmd(char *cmd,char* argv[],int* argc)
{
	int i,j;
	i = 0;
	j = 0;
	for(i=0;cmd[i]!='\n' && cmd[i]!='\0' && j<MAXARGS;i++)
	{
		while(isBlank(cmd[i]))
		{
			i++;
		}
		argv[j] = cmd + i;
		j++;
		while(!isBlank(cmd[i]) && cmd[i]!='\0' && cmd[i]!='\n')
		{
			i++;
		}
		cmd[i] = '\0';	
	}
	argv[j] = '\0';
	*argc = j;
}
int main(void)
{
    printf("Initialing the disk...");
    InitSuperBlock();
    InitInode();
    printf("Done\nWelcome to use LDY's EXT2 File System Lite!\n");
    int fd,argc,i,j;
    pid_t cur_pid,child_pid;
    char buf[128] = {0};
    char *argv[MAXARGS];
    while(getcmd(buf,sizeof(buf)) >= 0)
    {
        buf[strlen(buf) - 1] = 0;
        if(!strcmp(buf,"shutdown"))
        {
            return 0;
        }
        ScanCmd(buf,argv,&argc);
        cur_pid = fork();
        if(cur_pid == 0)
        {
            execv(argv[0],argv);
        }
        else
        {
            child_pid = wait(NULL);
        }
    }
    
}
