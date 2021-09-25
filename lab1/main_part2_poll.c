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

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        errno = EINVAL;
        perror("Provide string identifier");
        exit(1);
    }
    else
    {
        int n_fds = 1;
        struct pollfd read_fd;
        read_fd.fd = STDIN_FILENO;
        read_fd.events = POLLIN;
        int read_timeout;
        size_t buffer_size = 1025;
        while (1)
        {
            read_timeout = 5000;
            int poll_result = poll(&read_fd, n_fds, read_timeout);
            if (poll_result > 0) // ok
            {
                char buffer[buffer_size];
                ssize_t read_bytes;
                while (1) // for EINTR processing
                {
                    if ((read_bytes = read(STDIN_FILENO, buffer, buffer_size - 1)) < 0)
                    {
                        if (errno = EINTR)
                        {
                            continue;
                        }
                        else
                        {
                            perror("Error while reading stdin");
                            exit(1);
                        }
                    }
                    buffer[read_bytes] = '\0';
                    printf("[%s] %s", argv[1], buffer);
                    break;
                }
                break;
            }
            else if (poll_result == 0) // timeout
            {
                fprintf(stderr, "\nTimeout for [%s]\n", argv[1]);
            }
            else // error
            {
                perror("Error while poll()");
                exit(1);
            }
        }
    }
    return 0;
}