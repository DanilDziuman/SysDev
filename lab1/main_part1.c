#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // type 'mode_t' [3rd parameter of open()]
#include <sys/stat.h> // values for rights [flags] for 'mode_t' [3rd parameter of open()]
#include <fcntl.h> // open(); creat();
#include <unistd.h> // close(); read(); write(); fsync(); fseek();
#include <errno.h> // errno;
#include <string.h> // strerror()
#include <sys/select.h> // select()
#include <poll.h> // poll()
void processErrorWithFile(char * arg)
{
    fprintf(stderr, "%s: '%s'\n", strerror(errno), arg);
    exit(1);
}

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        errno = EINVAL; // errno doesn't know that argc < 3 is an EINVAL error.
        fprintf(stderr, "%s: provide input file and output file\n", strerror(errno));
        return 1;
    }
    else
    {
        const size_t buffer_size = 32;
        int fd_input;
        int fd_output;
        int flags_input = O_RDONLY;
        int flags_output = O_WRONLY | O_TRUNC | O_CREAT; // O_CREAT flag instead of creat()
        mode_t mode = 0644;
        ssize_t read_bytes;
        ssize_t written_bytes;
        size_t total_bytes = 0;
        char buffer[buffer_size];
        fd_input = open(argv[1], flags_input, mode);
        // diapasone lower letters: 97-122;
        // shift: -32 bit [a -> A, b -> B]
        if (fd_input < 0)
        {
            processErrorWithFile(argv[1]);
        }
        fd_output = open(argv[2], flags_output, mode);
        if (fd_output < 0)
        {
            processErrorWithFile(argv[2]);
        }
        while ((read_bytes = read(fd_input, buffer, buffer_size)) > 0)
        {
            for (size_t i = 0; i < buffer_size; i++)
            {
                if (buffer[i] >= 97 && buffer[i] <= 122)
                {
                    buffer[i] -= 32;
                }
            }
            written_bytes = write(fd_output, buffer, read_bytes);
            if (written_bytes != read_bytes)
            {
                processErrorWithFile(argv[2]);
            }
            total_bytes += written_bytes;
        }
        if (read_bytes < 0 || close(fd_input) < 0)
        {
            processErrorWithFile(argv[1]);
        }
        if (close(fd_output) < 0)
        {
            processErrorWithFile(argv[2]);
        }
        printf("Total number of bytes written: %li\n", total_bytes);
    }
    return 0;
}