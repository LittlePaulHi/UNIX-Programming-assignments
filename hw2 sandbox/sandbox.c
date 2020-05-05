#include "sandbox.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void SandboxInit() __attribute__((constructor));

void SandboxInit()
{
    
}

const char* const short_opts = "p:d:";
char *basedir = ".";

void foo (int, ...);

int main(int argc, char** argv)
{
    bool isAllowArgs = false;
    int opt;
    char *sopath = "./sandbox.so";

    for (int index = 1; index < argc; index++)
    {
        if (strcmp(argv[index], "--") == 0)
        {
            isAllowArgs = true;
            break;
        }
    }

    while ((opt = getopt(argc, argv, short_opts)) != -1)
    {
        switch(opt)
        {
            case 'p':
                sopath = optarg;
                break;
            case 'd':
                basedir = optarg;
                break;
            default: /* '?' */
                if (!isAllowArgs)
                {
                    // TODO
                    fprintf(stderr, "Usage: %s [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n", __FILE__);
                }
                break;
        }
    }

    char *ldPreload = malloc(strlen("LD_PRELOAD=") + strlen(sopath) + 1);
    strcat(ldPreload, "LD_PRELOAD=");
    strcat(ldPreload, sopath);
    putenv(ldPreload);

    // TODO split the input
    pid_t pid;
    int status;
    if ((pid = fork()) < 0)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)  /* child */
    {
        char *argvs[] = { "ls", "-la", "/", "Makefile", NULL };
        if (execvp("ls", argvs) == -1)
        {
            perror("execvp error");
            exit(EXIT_FAILURE);
        }
    }
    else  /* parent */
    {
        waitpid(pid, &status, 0);
    }

    // system("ls -la / Makefile");

    return 0;
}
