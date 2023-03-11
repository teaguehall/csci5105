#ifndef SERVER_COORDINATE_H
#define SERVER_COORDINATE_H

// coordinator thread info
typedef struct CoordinatorInfo
{
    char coord_address[128];
    int coord_port;

} CoordinatorInfo;

void* funcCoordinator(void *vargp);
void* funcCoordinated(void *vargp);

#endif // SERVER_COORDINATE_H
