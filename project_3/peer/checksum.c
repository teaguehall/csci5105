#include <string.h>
#include <stdio.h>

#include "checksum.h"

int checksum(FileInfo* file_info_to_modify)
{
    FILE *file;

    file = fopen(file_info_to_modify->name, "r");
    
    // TODO

    // success
    return 0;
}