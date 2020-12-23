#include "../src/disk.h"
#include "../src/defs.h"
#include <stdio.h>
#include <string.h>
int AllocateInode(void)
{
    int i=0,j,temp_map,result;
    int cur_map = 0x00ff000f;
    temp_map = 0x00ff000f;
    for(j=0;j<32;j++)
    {
        if(temp_map % 2 == 0)   // j-th bit of this map is available
        {
            result = i * 32 + (31 - j);
            cur_map = cur_map | (1 << j);
            printf("%x\n",cur_map);
            break;
        }
        temp_map = temp_map >> 1;
    }
    return result;
}
int main(void)
{
    char *filename;
    char div[2] = "/";
    char path[] = "abcd.h";
    filename = strtok(path,div);
    printf("%s\n",filename);
}
