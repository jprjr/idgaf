#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

extern char** environ;

int main(int argc, char* argv[]) {

    int pid = 0;
    int fd  = 0;
    char t[512];
    char *p = getenv("PATH");
    char *q = 0;
    int plen = 0;

    if(argc <= 1) {
        printf("Usage: %s args...\n",argv[0]);
        return 1;
    }

    argv++;

    pid = fork();
    if(pid > 0) {
        return 0;
    }

    for(fd = getdtablesize() - 1; fd>=0; --fd) {
        close(fd);
    }

    fd = open("/dev/tty", O_RDWR);
    ioctl(fd, TIOCNOTTY, 0);
    close(fd);
    setpgid(0,0);
    fd = open("/dev/null", O_RDWR);
    dup(fd);
    dup(fd);

    if(argv[0][0] == '/') {
        return execve(argv[0],argv,environ);
    }
    else {
        plen = strlen(argv[0]);
        if(!p) {
            p = "/bin:/usr/bin:/usr/local/bin";
        }
        while(p) {
            strcpy(t,p);
            q = strchr(t,':');

            if(q) {
                *q = 0; /* replace t's colon with null */
            }

            printf("t = %s\n",t);
            if(strlen(t)) {
                strcat(t,"/");
            }

            if( (strlen(t) + plen) > sizeof(t)) {
                return 1;
            }
            strcat(t,argv[0]);
            printf("Trying %s\n",t);
            execve(t,argv,environ);
            if(errno != ENOENT) {
                return 1;
            }
            q = strchr(p,':');
            if(q) {
                p = q + 1;
            }
            else {
                p = 0;
            }
        }
        return 1;
    }
}
