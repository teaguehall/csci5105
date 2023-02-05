/* article service interface */

program COMMUNICATE_PROG 
{
    version COMMUNICATE_VERSION 
    {
        int join(string ip, int port) = 1;
        int leave(string ip, int port) = 2;
        int subscribe(string ip, int port, string article) = 3;
        int unsubscribe(string ip, int port, string article) = 4;
        int publish(string ip, int port, string article) = 5;
        int ping(void) = 6;
    } = 1;
} = 9999;