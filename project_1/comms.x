/* article service interface */

program Article 
{
    version VERSION 
    {
        int join(string, int) = 1;
        int leave(string, int) = 2;
        int subscribe(string, int, string) = 3;
        int unsubscribe(string, int, string) = 4;
        int publish(string, int, string) = 5;
        int ping(void) = 6;
    } = 1;
} = 9999;