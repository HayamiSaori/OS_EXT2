#include "../src/disk.h"
#include "../src/defs.h"
#include <stdio.h>
#include <string.h>
// int main(void)
// {
    
//     if(!open_disk())
//     {
//         printf("successfully open!\n");
//     }
//     char buf[512];
//     uint16_t *pbuf;
//     pbuf = (uint16_t *)buf;
//     int i;
//     for(i=0;i<512;i++)
//     {
//         pbuf[i] = i;
//     }
        
//     printf("now write\n");
//     disk_write_block(0,buf);
//     if(!close_disk())
//     {
//         printf("Successfully close!\n");
//     }
//     open_disk();
//     char bbuf[512];
//     disk_read_block(0,bbuf);
//     for(i=0;i<512;i++)
//     {
//         if((i+1)%16 == 0)
//             printf("\n");
//         printf("%x ",bbuf[i]);
//     }
//     close_disk();
// }
int main()
{
    char buf[128];
    fgets(buf,128,stdin);
    printf("%s\n",buf);
}