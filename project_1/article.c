#include "article.h"
#include <stdio.h>
#include <string.h>

// decodes raw bytes into article struct
int articleDecode(char* bytes, Article* output)
{
    int semicolon_count = 0;
    int semicolon_1_pos;
    int semicolon_2_pos;
    int semicolon_3_pos;

    // clear out structure
    memset(output, 0, sizeof(Article));
    
    // find locations of each semi-colon
    for(int i = 0; i < strlen(bytes); i++)
    {
        if(bytes[i] == ';')
        {
            semicolon_count++;

            if(semicolon_count == 1)
                semicolon_1_pos = i;
            else if(semicolon_count == 2)
                semicolon_2_pos = i;
            else if(semicolon_count == 3)
                semicolon_3_pos = i;
        }
    }
    if(semicolon_count != 3)
    {
        fprintf(stderr, "ERROR: Invalid number of semi-colons found in article. Found %d, expected 3\n", semicolon_count);
        return 1;
    }

    // copy string into article struct
    memcpy(output->type, bytes, semicolon_1_pos);
    memcpy(output->originator, bytes + semicolon_1_pos + 1, semicolon_2_pos - semicolon_1_pos - 1);
    memcpy(output->org, bytes + semicolon_2_pos + 1, semicolon_3_pos - semicolon_2_pos - 1);
    memcpy(output->contents, bytes + semicolon_3_pos + 1, strlen(bytes) - semicolon_3_pos - 1);

    // validate type
    if(strcmp(output->type, "Sports") == 0){}
    else if(strcmp(output->type, "Lifestyle") == 0){}
    else if(strcmp(output->type, "Entertainment") == 0){}
    else if(strcmp(output->type, "Business") == 0){}
    else if(strcmp(output->type, "Technology") == 0){}
    else if(strcmp(output->type, "Science") == 0){}
    else if(strcmp(output->type, "Politics") == 0){}
    else if(strcmp(output->type, "Health") == 0){}
    else
    {
        fprintf(stderr, "ERROR: Invalid type \"%s\" found in article.\n", output->type);
        return 1;
    }

    // set okay flag if article meets publish requirements
    if((output->type != NULL || output->originator != NULL || output->org != NULL))
    {
        if(output->contents == NULL)
        {
            output->subscribe_ok = 1;
        }
        else
        {
            output->publish_ok = 1;
        }
    }
    else
    {
        fprintf(stderr, "ERROR: Article does not meet either publish or subscribe/unsubscribe message requirements\n", output->type);
        return 1;
    }

    //// DEBUG
    //printf("output.type == %s\n", output->type);
    //printf("output.originator == %s\n", output->originator);
    //printf("output.org == %s\n", output->org);
    //printf("output.contents == %s\n", output->contents);

    // success
    return 0;
}
