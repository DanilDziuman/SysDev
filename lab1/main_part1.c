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

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        errno = EINVAL; // 'custom' EINVAL error.
        perror("Provide input and output file");
        exit(1);
    }
    else
    {
        const size_t buffer_size = 32;
        int fd_input;
        int fd_output;
        int flags_input = O_RDONLY;
        int flags_output = O_WRONLY | O_TRUNC | O_CREAT;
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
            perror("Error while opening input file");
            exit(1);
        }
        fd_output = open(argv[2], flags_output, mode);
        if (fd_output < 0)
        {
            perror("Error while opening output file");
            exit(1);
        }
        while ((read_bytes = read(fd_input, buffer, buffer_size)) != 0)
        {
            if (read_bytes < 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    perror("Error while reading input file");
                    exit(1);
                }
            }
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
                perror("Error while writing to output file");
                exit(1);
            }
            total_bytes += written_bytes;
        }
        if (close(fd_input) < 0)
        {
            perror("Error while closing input file");
            exit(1);
        }
        if (fsync(fd_output) < 0)
        {
            perror("Error while syncing output file");
            exit(1);
        }
        if (close(fd_output) < 0)
        {
            perror("Error while closing output file");
            exit(1);
        }
        printf("Total number of bytes written: %li\n", total_bytes);
    }
    return 0;
}