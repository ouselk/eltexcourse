#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{

    char *ch_name = NULL;
    char* filenames[argc];
    int file_count = 0;
    for (int i = 1; i<argc; i++)
    {
        if (strcmp(argv[i], "-p")==0)
        {
            if (ch_name!=NULL)
            {
                fprintf(stderr, "Так не красиво поступать\n");
                exit(EXIT_FAILURE);
            }
            if (i+1>=argc)
            {
                fprintf(stderr, "Не указано название канала после -p\n");
                exit(EXIT_FAILURE);
            }
            else
            {
                ch_name = argv[++i];
            }
        }
        else
        {
            filenames[file_count++]=argv[i];
        }
    }

    if (file_count==0)
    {
        fprintf(stderr, "Не введено имен файлов\n");
        exit(EXIT_FAILURE);
    }

    for (int i=0; i<file_count; i++)
    {
        FILE *fp = fopen(filenames[i],);

        if (fp!=NULL)

        pid_t pid;
        int rv; 
        switch (pid = fork())
        {
            case -1:
                perror("fork");
                exit(EXIT_FAILURE);
            case 0:
                printf("CHILD: PID - %d\n", getpid());
                printf("CHILD: PPID - %d\n", getppid());
                _exit(EXIT_SUCCESS);
            default:
                printf("PARENT: PID -%d\n", getpid());
                printf("PARENT: CHILD PID - %d\n", pid);
                wait(&rv);
                printf("PARENT: RETURN STATUS FOR CHILD - %d\n", WEXITSTATUS(rv));
                exit(EXIT_SUCCESS);
        }
    }

}