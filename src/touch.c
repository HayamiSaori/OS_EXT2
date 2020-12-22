#include "defs.h"
#include "disk.h"
#include <stdio.h>
#include <string.h>
int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        printf("Syntax error!The number of arguments must be 2 : touch [path/filename]\n");
        return SYNTAX_ERR;
    }
    char *path = argv[1];
}