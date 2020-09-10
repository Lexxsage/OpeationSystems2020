#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void main()
{
    // Automatically called by the
    // other time conversion functions
    // that depend on the timezone.
    //
    // tzset();

    // Does not depend on the timezone.
    time_t t_sec = time(NULL);

    // LA timezone mathes for Californa.
    if (putenv("TZ=:America/Los_Angeles")) {
        // Insufficient space to allocate new environment
        perror("Putenv returned a failure code. Aborting.");
        return;
    }

    // Man: The return value points to a
    // statically allocated struct.
    struct tm *tm_ptr = localtime(&t_sec);

    printf("California local time is: %s", asctime(tm_ptr));
}