#include "../src/disk.h"
#include "../src/defs.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    char path[] = "a/b/cd/ef.h";
    char *filename;
    char div[2] = "/";
    char *dirname = strtok(path,div);
    while (dirname != NULL)
    {
        printf("%s\n",dirname);
        filename = dirname;
        dirname = strtok(NULL,div);
        
    }
    printf("%s\n",filename);
}
