#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "ch_communicate.h"
#include "files.h"

#define MAX_FILE_NAME_LEN 128

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
            if (strlen(argv[i])<MAX_FILE_NAME_LEN)
            {
                filenames[file_count++]=argv[i];
            }
            else
            {
                fprintf(stderr, "Слишком длинное имя файла\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (file_count==0)
    {
        fprintf(stderr, "Не введено имен файлов\n");
        exit(EXIT_FAILURE);
    }



    pid_t pid;
    int m_to_s[2];
    pipe (m_to_s);
    int s_to_m[2];
    pipe(s_to_m);

    fflush(NULL);
    switch (pid = fork())
    {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);

        case 0:
            close(m_to_s[1]);
            close(s_to_m[0]);
            

            BorkaProtocolPackage answer = {UNKNOWN, '\0', 0};
            long long filesize = 0;
            long long currise = 0;
            char filename[MAX_FILE_NAME_LEN+6];
            int fd = -1;
            send_ready_rcv(s_to_m[1]);
            while(answer.type!=MASTER_NOTIFICATE_ABOUT_END)
            {
                if (read_package(m_to_s[0], &answer)!=0);
                {
                    fprintf(stderr, "Родитель оборвал передачу\n");
                    exit(EXIT_FAILURE);
                }
                if (answer.type==MASTER_SEND_NAME_AND_SIZE)
                {
                    if (fd>=0)
                    {
                        close(fd);
                        fd=-1;
                    }
                    parse_name_and_size(&answer, filename, &filesize);
                    printf("Я дочерний процесс и я принимаю файл %s размером %lld\n", filename, filesize);
                    strcat(filename, ".copy");
                    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd<0)
                    {
                        perror("копию не получилось создаать(");
                        exit(EXIT_FAILURE);
                    }
                }
                else if (answer.type==DATA_RECIVE)
                {
                    write_all(fd, answer.data, answer.lenght);
                }
            }
            close(fd);
            exit(EXIT_SUCCESS);
            


        default:
            close(m_to_s[0]);
            close(s_to_m[1]);

            BorkaProtocolPackage answ = {UNKNOWN, '\0', 0};
            while (answ.type!=SLAVE_READY_RECIEVE)
            {
                read_package(s_to_m[0], &answ);
            }
            for (int i=0; i<file_count; i++)
            {
                int fd = open(filenames[i], O_RDONLY);

                if (fd<0)
                {
                    fprintf(stderr, "Файла с именем %s не существует\n", filenames[i]);
                    continue;
                }
                char buf[PACKAGE_SIZE] = {'\0'};
                struct stat file_info;
                fstat(fd, &file_info);
                send_name_and_size(filenames[i], file_info.st_size, m_to_s[1]);
                
                BorkaProtocolPackage pkg = {0};
                pkg.type = DATA_RECIVE;
                while ((pkg.lenght = read_all(fd, pkg.data, PACKAGE_SIZE))>0)
                { send_package(&pkg, m_to_s[1]); }
                close(fd);
            }
            send_end(m_to_s[1]);
            wait(NULL);
    }
    

}