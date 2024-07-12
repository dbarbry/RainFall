#include <stdlib.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int main(int ac, char **av) {
    int     nbr;
    gid_t   gid;
    uid_t   uid;
    char    *execv_args[2];

    nbr = atoi(av[1]);
    if (nbr == 423) {
        execv_args[0] = strdup("/bin/sh");
        execv_args[1] = '\0';
    
        gid = getegid();
		uid = geteuid();
		setresgid(gid, gid, gid);
		setresuid(uid, uid, uid);

        execv("/bin/sh", execv_args);
    }
    else {
        write(2, "No !\n", 5);
    }
    return 0;
}