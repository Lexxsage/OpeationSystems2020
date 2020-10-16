#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h> 
#include <unistd.h>


int main (int argc, char **argv){

        printf("uid %ld \n",getuid());
        printf("eid %ld \n",geteuid());

        if (argc != 2) {
                fprintf(stderr, "Need file\n");
                return EXIT_FAILURE;
        }

        FILE * f1;
        f1 = fopen(argv[1], "r");
        if (NULL == f1){
        perror("File not open");
        }

        fclose(f1);

        uid_t uid = getuid();
        setuid(uid);
        printf("uid %ld \n",getuid());
        printf("eid %ld \n",geteuid());

        f1 = fopen(argv[1],"r");
        if (f1 == NULL){
                perror("File not open");
        }

        fclose(f1);

        return EXIT_SUCCESS;
}
