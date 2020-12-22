#include "defs.h"
#include "disk.h"
#include <stdio.h>
#include <string.h>
// path must be like "folder1/folder2/.../dirname"
int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        printf("Syntax error!The number of arguments must be 2 : mkdir [path/dirname]\n");
        return SYNTAX_ERR;
    }
    char *dir_name = argv[1];
    if(SP_BLK.free_block_count < 0)
    {
        printf("There are no more available blocks,faild to creat dirctory %s\n",dir_name);
        return NO_BLOCK;
    }
    
}