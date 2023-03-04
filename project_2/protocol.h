#define MSG_MAGIC_NUMBER                0x12AB34CD

#define MSG_TYPE_ERROR_RESPONSE         0x1000

// error response message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - error message (null terminated string)

#define MSG_TYPE_POST                   0x2000

// post message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - post author (null terminated string)
// - post title (null terminated string)
// - post contents (null terminated string)

#define MSG_TYPE_POST_RESPONSE          0x2001

// post response message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message size (uint32 4-bytes)

#define MSG_TYPE_READ                   0x3000

// read message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message size (uint32 4-bytes)

#define MSG_TYPE_READ_RESPONSE          0x3001

// read message structure:
// - magic number (uint32 4-bytes)
// - message type (uint32 4-bytes)
// - message size (uint32 4-bytes)
// - number of articles (uint32 4-bytes)
//      - article ID (uint32 4-bytes)
//      - parent article ID (uint32 4-bytes)
//      - article uthor (null-terminated string)
//      - article title (null-terminated string)
//      - article contents (null-terminated string)







#define MSG_TYPE_CHOOSE                 0x4000
#define MSG_TYPE_CHOOSE_RESPONSE        0x4001
#define MSG_TYPE_REPLY                  0x5000
#define MSG_TYPE_REPLY_RESPONSE         0x5001

// error 