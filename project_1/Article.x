/* article service interface */

program Article 
{
    version VERSION 
    {
        int join(string, int) = 1;
        int leave(string, int) = 1;
        int subscribe(string, int, string) = 1;
        int unsubscribe(string, port, string) = 1;
        int publish(string, int, string) = 1;
        int ping(void) = 1;
    } = 1;
} = 9999;