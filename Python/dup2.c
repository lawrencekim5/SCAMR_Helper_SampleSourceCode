/*
 * Public domain dup2() lookalike
 * by Curtis Jackson @ AT&T Technologies, Burlington, NC
 * electronic address:  burl!rcj
 *
 * dup2 performs the following functions:
 *
 * Check to make sure that fd1 is a valid open file descriptor.
 * Check to see if fd2 is already open; if so, close it.
 * Duplicate fd1 onto fd2; checking to make sure fd2 is a valid fd.
 * Return fd2 if all went well; return BADEXIT otherwise.
 */

#include <errno.h>                // errno
#include <fcntl.h>                // fcntl()
#include <unistd.h>               // close()

#define BADEXIT -1

int
dup2(int fd1, int fd2)
{
    if (fd1 != fd2) {
#ifdef F_DUPFD
        if (fcntl(fd1, F_GETFL) < 0)
            return BADEXIT;
        if (fcntl(fd2, F_GETFL) >= 0)
            close(fd2);
        if (fcntl(fd1, F_DUPFD, fd2) < 0)
            return BADEXIT;
#else
        errno = ENOTSUP;
        return BADEXIT;
#endif
    }
    return fd2;
}
/*In this example,the code does not account for the terminating null character, 
and it writes one byte beyond the end of the buffer.The first call to strncat() appends up to 20 characters 
plus a terminating null character to fullname[]. There is plenty of allocated space for this, 
and there is no weakness associated with this first call. However, the second call to strncat() potentially 
appends another 20 characters. The code does not account for the terminating null character that is automatically added by strncat(). 
This terminating null character would be written one byte beyond the end of the fullname[] buffer. 
Therefore an off-by-one error exists with the second strncat() call, as the third argument should be 19.*/
#include <stdio.h>
#include <string.h>
int main(){
    char firstname[20];
    char lastname[20];
    char fullname[40];
    firstname[0]='\0';
    strncat(fullname, firstname, 20);
    strncat(fullname, lastname, 20);
    return 0;
}