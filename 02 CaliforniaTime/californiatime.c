#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void main()
{
    /* 
     * The function is auto called
     * by other time conversion functions. 
     * TCF depends on time zone
     */
    
    // tzset();

    // Doesn't depend on timezone
    time_t t_sec = time(NULL);

    // LA timezone
    if (putenv("TZ=:America/Los_Angeles")) {
        // No space for new environment selection
        perror("Putenv returned a failure code. Error.");
        return;
    }

    /*
     * Return a time value to a
     * statically allocated structure
     */
    struct tm *tm_ptr = localtime(&t_sec);

    printf("California local time is: %s", asctime(tm_ptr));
}
