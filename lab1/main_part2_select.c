#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> // type 'mode_t' [3rd parameter of open()]
#include <sys/stat.h> // values for rights [flags] for 'mode_t' [3rd parameter of open()]
#include <fcntl.h> // open(); creat();
#include <unistd.h> // close(); read(); write(); fsync(); fseek();
#include <errno.h> // errno;
#include <string.h> // strerror()
#include <sys/select.h> // select()
#include <poll.h> // poll()
#include <time.h>
int processError()
{
    fprintf(stderr, "Error: %s", strerror(errno));
    exit(1);
}

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        errno = EINVAL; // errno doesn't know that argc < 3 is an EINVAL error.
        fprintf(stderr, "%s: provide string identifier\n", strerror(errno));
        return 1;
    }
    else
    {
        int n_fds = 1;
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        struct timeval read_timeout;
        size_t buffer_size = 1025;
        while (1)
        {
            read_timeout.tv_sec = 5;
            read_timeout.tv_usec = 0;
            int select_result = select(n_fds, &read_fds, NULL, NULL, &read_timeout);
            if (select_result > 0) // ok
            {
                char buffer[buffer_size + 1];
                ssize_t read_bytes;
                if ((read_bytes = read(STDIN_FILENO, buffer, buffer_size - 1)) >= 0)
                {
                    buffer[read_bytes] = '\0';
                    printf("[%s] %s", argv[1], buffer);
                    break;
                }
                else
                {
                    processError();
                }
            }
            else if (select_result == 0) // timeout
            {
                fprintf(stderr, "\nTimeout for [%s]\n", argv[1]);
                FD_SET(STDIN_FILENO, &read_fds);
            }
            else // error
            {
                processError();
            }
        }
    }
    return 0;
}