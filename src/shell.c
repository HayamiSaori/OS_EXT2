#include "disk.h"
#include "defs.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
// super block:1024B=1KiB (disk block 0 to 1)
// inodes:32768B=32KiB (disk block 2 to 65)
// data blocks:4096-1-32=4063 (disk block 66 to ...)
#define MAX_CMD_LEN (512)
#define MAX_ARG_LEN (64)
#define MAXARGS (10)
#define DATA_START  (66)
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
// input a direction,return the inode id of it
int ScanDir(char *dirname,int cur_id)
{
    int i,j,block_num;
    struct dir_item *cur_dir;
    char buf[512];
    printf("from scandir,parent id of %s:%x\n",dirname,cur_id);
    // char singal_item[DIR_ITEM_SIZE];
    /*if(inodes[cur_id].file_type != TYPE_DIR)
    {
        printf("Path error! No such directory name %s\n",dirname);
        return PATH_ERR;
    }*/
    open_disk();
    for(i=0;i<inodes[cur_id].link;i++)
    {
        block_num = inodes[cur_id].block_point[i];
        disk_read_block(DATA_START + block_num * 2, buf);
        // memcpy(singal_item,buf,DIR_ITEM_SIZE);
        cur_dir = (struct dir_item *)buf;
        if((cur_dir -> type == TYPE_DIR) && (!strcmp(dirname,cur_dir->name)) && (cur_dir->valid == 1))
        {
            
            return cur_dir->inode_id;
        }
    }
    close_disk();
}
int AllocateInode(void)
{
    int i,j,temp_map,result;
    for(i=0;i<32;i++)
    {
        if(SP_BLK.inode_map[i] != 0xffffffff) // means that there has available inode.
        {
            temp_map = SP_BLK.inode_map[i];
            for(j=0;j<32;j++)
            {
                if(temp_map % 2 == 0)   // j-th bit of this map is available (from right to left)
                {
                    result = i * 32 + (31 - j);
                    SP_BLK.inode_map[i] = SP_BLK.inode_map[i] | (1 << j);
                    UpdateSuperBlock();
                    return result;
                }
                temp_map = temp_map >> 1;
            }
        }
    }
}
int AllocateBlock(void)
{
    int i,j,temp_map,result;
    for(i=0;i<128;i++)
    {
        // printf("blockmap of %d:%x\n",i,SP_BLK.block_map[i]);
        if(SP_BLK.block_map[i] != 0xffffffff) // means that there has available block.
        {
            temp_map = SP_BLK.block_map[i];
            for(j=0;j<32;j++)
            {
                if(temp_map % 2 == 0)   // j-th bit of this map is available (from right to left)
                {
                    result = i * 32 + (31 - j);
                    SP_BLK.block_map[i] = SP_BLK.block_map[i] | (1 << j);
                    UpdateSuperBlock();
                    return result;
                }
                temp_map = temp_map >> 1;
            }
        }
    }
}
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
    /*if(inodes[2].file_type != TYPE_DIR) 
    {
        inodes[2].file_type = TYPE_DIR;
        UpdateInode();
    }*/
    if(close_disk() == -1)
    {
        return CLOSE_ERR;
    }
}
int ls(int argc,char* argv[])
{
    if(argc != 2)
    {
        printf("Syntax error!The number of arguments must be 2 : ls [path]\n");
        return SYNTAX_ERR;
    }
    if(argv[1][0] == '/' || argv[1][ strlen(argv[1]) - 1 ] != '/')
    {
        printf("Path error!The path must be like \"dir1/dir2/.../\" or \"root/\"\n");
        return PATH_ERR;
    }
    uint32_t cur_id = 2;    // start at root directory:inode id 2
    char path[MAX_ARG_LEN];
    char *dirname;
    char div[2] = "/";
    strcpy(path,argv[1]);
    if(!strcmp(path,"root/"))
    {
        cur_id = 2;
    }
    else
    {
        dirname = strtok(path,div);
        while (dirname != NULL)
        {
            // printf("file type:%d\n",inodes[cur_id + 1].file_type);
            cur_id = ScanDir(dirname,cur_id);
            if(cur_id == PATH_ERR)
            {
                return PATH_ERR;
            }
            dirname = strtok(NULL,div);
        }
    }
    
    /*if(inodes[cur_id].file_type != TYPE_DIR)
    {
        printf("Path error! %s is not a path of directory\n",argv[1]);
        return PATH_ERR;
    }*/
    int i,j,block_num;
    char buf[512] = {0};
    struct dir_item *pdir = (struct dir_item *)buf;
    open_disk();
    for(i=0;i<inodes[cur_id].link;i++)
    {
        block_num = inodes[cur_id].block_point[i];
        disk_read_block(DATA_START + block_num * 2,buf);
        pdir = (struct dir_item *)buf;
        if(pdir->valid = ITEM_VALID)
            printf("%s\n",pdir->name);
    }
    close_disk();
}
int mkdir(int argc,char* argv[])
{
    if(SP_BLK.free_inode_count <1)
    {
        printf("No more available inodes.\n");
        return NO_INODE;
    }
    if(argc != 3)
    {
        printf("Syntax error!The number of arguments must be 3 : mkdir [path] [dirname]\n");
        return SYNTAX_ERR;
    }
    if(argv[1][0] == '/' || argv[1][ strlen(argv[1]) - 1 ] != '/')
    {
        printf("Path error!The path must be like \"dir1/dir2/.../\" or \"root/\"\n");
        return PATH_ERR;
    }
    if(strchr(argv[2],'/') != NULL)
    {
        printf("Folder name error!The new folder can't include '/'\n");
        return DIR_ERR;
    }
    uint32_t cur_id = 2;    // start at root directory:inode id 2
    char path[MAX_ARG_LEN];
    char new_folder[MAX_ARG_LEN];
    char *dirname;
    char div[2] = "/";
    strcpy(path,argv[1]);
    strcpy(new_folder,argv[2]);
    if(!strcmp(path,"root/"))
    {
        cur_id = 2;
    }
    else
    {
        dirname = strtok(path,div);
        while (dirname != NULL)
        {
            printf("dirname:%s,cur_id:%d\n",dirname,cur_id);
            cur_id = ScanDir(dirname,cur_id);
            if(cur_id == PATH_ERR)
            {
                return PATH_ERR;
            }
            dirname = strtok(NULL,div);
        }
    }
    int new_inode_id,new_block_num,cur_pos;
    char wbuf[512] = {0};
    struct dir_item *pdir = (struct dir_item *)wbuf;
    SP_BLK.free_inode_count--;
    SP_BLK.dir_inode_count++;
    SP_BLK.free_block_count--;
    new_inode_id = AllocateInode();
    new_block_num = AllocateBlock();
    // init the new inode.
    inodes[new_inode_id].link++;
    cur_pos = inodes[new_inode_id].link;
    inodes[new_inode_id].block_point[cur_pos - 1] = new_block_num;
    printf("The block of %s is : %x\n",new_folder,new_block_num);
    // set the directory inode.
    new_block_num = AllocateBlock();
    printf("The block item of %s is : %x\n",new_folder,new_block_num);
    inodes[cur_id].link++;
    cur_pos = inodes[cur_id].link;
    inodes[cur_id].block_point[cur_pos - 1] = new_block_num;
    pdir -> inode_id = new_inode_id;
    strcpy(pdir->name,argv[2]);
    pdir -> type = TYPE_DIR;
    pdir -> valid = ITEM_VALID;
    open_disk();
    disk_write_block(DATA_START+new_block_num*2,wbuf);
    close_disk();
    UpdateSuperBlock();
    UpdateInode();
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
            UpdateSuperBlock();
            UpdateInode();
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
