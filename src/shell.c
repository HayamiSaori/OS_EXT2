#include "disk.h"
#include "defs.h"
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
struct inode inodes[1024];
sp_block SP_BLK;
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
        j++;
        disk_write_block(cur_block,wbuf);
    }
    close_disk();
}
// input a path,return the inode id of it
/*int ScanPath(char *path)
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
}*/
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
        SP_BLK.magic_num = SYSTEM_CODE;
        SP_BLK.free_block_count = DATA_BLOCK_NUM;
        SP_BLK.free_inode_count = INODE_NUM - 1;
        SP_BLK.dir_inode_count = 1;
        SP_BLK.inode_map[0] = (1 << 29);
        close_disk();
        UpdateSuperBlock();
        open_disk();
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
void ls(int argc,char* argv[])
{

}
int mkdir(int argc,char* argv[])
{
    if(argc != 2)
    {
        printf("Syntax error!The number of arguments must be 2 : mkdir [path/dirname]\n");
        return SYNTAX_ERR;
    }
    if(argv[1][0] == '/' || argv[1][ strlen(argv[1]) - 1 ] == '/')
    {
        printf("Path error!The path must be like \"dir1/dir2/.../filename\" \n");
        return PATH_ERR;
    }
    char path[MAX_ARG_LEN];
    strcpy(path,argv[1]);
    printf("%s\n",path);
    char *filename;
    char *dirname;
    char div[2] = "/";
    dirname = strtok(path,div);
    while (dirname != NULL)
    {
        filename = dirname;
        dirname = strtok(NULL,div);
    }
    
}
void touch(int argc,char* argv[])
{

}
void cp(int argc,char* argv[])
{

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
        if(!strcmp(argv[0],"ls"))
        {
            ls(argc,argv);
        }
        else if(!strcmp(argv[0],"mkdir"))
        {
            mkdir(argc,argv);
        }
        else if(!strcmp(argv[0],"touch"))
        {
            touch(argc,argv);
        }
        else if(!strcmp(argv[0],"cp"))
        {
            cp(argc,argv);
        }
        else
        {
            printf("No command named \"%s\",please input again.\n",argv[0]);
        }
    }
}
