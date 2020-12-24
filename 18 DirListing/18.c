#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <string.h>
#include "header.h"

void printFileStatus(struct stat *status) {
    switch (status->st_mode & S_IFMT) { //маска для типа файла
        case S_IFREG:
            printf("-");
            break;
        case S_IFDIR:
            printf("d");
            break;
        default:
            printf("?");
    };
}

void printAccess(struct stat *status) {
    static char symbRights[] = {'r', 'w', 'x'};
    static mode_t rights[] = {
        S_IRUSR, S_IWUSR, S_IXUSR,
        S_IRGRP, S_IWGRP, S_IXGRP,
        S_IROTH, S_IWOTH, S_IXOTH
    };

    char str[] = "---------";

    for(int i = 0, count = sizeof(rights) / sizeof(mode_t); i < count; ++i) {
        if(status->st_mode & rights[i]) {
            str[i] = symbRights[i % COUNT_RIGHTS];
        }
    }

    printf("%s ", str);
}

int printUserInfo(struct stat *status) {
    struct passwd *userBuffer = getpwuid(status->st_uid);
    if (userBuffer == NULL) {
        perror("Error getpwuid");
        return EXIT_FAILURE;
    }
    printf(" %10s", userBuffer->pw_name);

    struct group *groupBuffer = getgrgid(status->st_gid);
    if (groupBuffer == NULL) {
        perror("Error getgrgid");
        return EXIT_FAILURE;
    }
    printf(" %10s", groupBuffer->gr_name);

    return EXIT_SUCCESS;
}

/*
struct passwd {
         char *pw_name;      // user's login name 
         char *pw_passwd;    // no longer used 
         uid_t pw_uid;       // user's uid 
         gid_t pw_gid;       // user's gid 
         char *pw_age;       // not used 
         char *pw_comment;   // not used 
         char *pw_gecos;     // typically user's full name 
         char *pw_dir;       // user's home dir 
         char *pw_shell;     // user's login shell 
     };
*/

void printFileSize(struct stat *status) {
    if ((status->st_mode & S_IFMT) == S_IFREG) {
        printf(" %10d", status->st_size);
    } else {
        printf(" %10s", "");
    }
}

int printTimes(struct stat *status) {
    char *time = ctime(&status->st_mtime);
    if (time == NULL) {
        perror("Error ctime");
        return EXIT_FAILURE;
    }
    printf(" %.24s ", time);

    return EXIT_SUCCESS;
}

void printFilename(char *filepath) {
    size_t index = strlen(filepath) - 1;
    while (filepath[index] != '/' && index != 0) {
        --index;
    }

    if (index != 0) {
        ++index;
    }

    printf("%s", &filepath[index]);
}

int printInfo(char *file) {
    struct stat statBuffer;

    if (stat(file, &statBuffer) == ERROR) {
        perror("Error stat");
        return EXIT_FAILURE;
    }

    printFileStatus(&statBuffer);
    printAccess(&statBuffer);

    if(printUserInfo(&statBuffer) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    printFileSize(&statBuffer);

    if(printTimes(&statBuffer) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    printFilename(file);

    printf("\n");

    return EXIT_SUCCESS;
}

 // mode_t   st_mode;          /* File mode */
 //       ino_t    st_ino;           /* Inode number */
 //       dev_t    st_dev;           /* ID of device containing */
 //                                 /* a directory entry for this file */
 //       dev_t    st_rdev;          /* ID of device */
 //                                 /* This entry is defined only for */
 //                                 /* char special or block special files */
 //       nlink_t  st_nlink;         /* Number of links */
 //       uid_t    st_uid;           /* User ID of the file's owner */
 //       gid_t    st_gid;           /* Group ID of the file's group */
 //       off_t    st_size;          /* File size in bytes */
 //       time_t   st_atime;         /* Time of last access */
 //       time_t   st_mtime;         /* Time of last data modification */
 //       time_t   st_ctime;         /* Time of last file status change */
 //                                 /* Times measured in seconds since */
 //                                 /* 00:00:00 UTC, Jan. 1, 1970 */
 //       long     st_blksize;       /* Preferred I/O block size */
 //       blkcnt_t st_blocks;        /* Number of 512 byte blocks allocated*/
 //       char     st_fstype[_ST_FSTYPSZ];
 //                                 /* Null-terminated type of filesystem */



int main(int argc, char *argv[]) {

    if(argc <= 1) {
        return printInfo(".");
    }

    for (int i = 1; i < argc; ++i) {
        if(printInfo(argv[i]) != EXIT_SUCCESS) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

