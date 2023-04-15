#ifndef ASSUMPTIONS_H
#define ASSUMPTIONS_H

#define MAX_FILE_SIZE_BYTES     1000000                     // maximum supported file
#define MAX_MSG_SIZE_BYTES      (MAX_FILE_SIZE_BYTES)   // safe assumption for max message size to be received by 
#define MAX_NODES_IN_NETWORK    64                          // maximum number of peers allowed in network at one time
#define MAX_FILES_PER_NODE      64                          // maximum number of files that each node can offer

#endif // ASSUMPTIONS_H
