// #include "sandbox.h"

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

const char* const short_opts = "d:p:";

void SandboxInit() __attribute__((constructor));
void SandboxInit(int argc, char** argv)
{
}

int main(int argc, char** argv)
{
    int opt;
    int cmdStartIndex = 1;
    bool isAllowArgs = false;
    char *sopath = "./sandbox.so";
    char *basedir = ".";

    for (int index = 1; index < argc; index++)
    {
        // TODO may have bug(with -- and other options): ./sandbox -- -d / ls /
        if (strcmp(argv[index], "--") == 0)
        {
            isAllowArgs = true;
            cmdStartIndex += 1;
            break;
        }
    }

    while ((opt = getopt(argc, argv, short_opts)) != -1)
    {
        switch(opt)
        {
            case 'd':
                basedir = optarg;
                cmdStartIndex += 2;
                break;

            case 'p':
                sopath = optarg;
                cmdStartIndex += 2;
                break;

            default: /* '?' */
                if (!isAllowArgs)
                {
                    fprintf(stderr, "Usage: %s [-p sopath] [-d basedir] [--] cmd [cmd args ...]\n"
                        "\t-p: set the path to sandbox.so, default = ./sandbox.so\n"
                        "\t-d: the base directory that is allowed to access, default = .\n"
                        "\t--: separate the arguments for sandbox and for the executed command\n", __FILE__);

                    exit(EXIT_FAILURE);
                }
                break;
        }
    }
    setenv("BASEDIR", basedir, 1);
    setenv("LD_PRELOAD", sopath, 1);

    pid_t pid;
    int status;
    if ((pid = fork()) < 0)
    {
        perror("fork error");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)  /* child */
    {
        char *cmd[argc - cmdStartIndex + 1];
        for (int index = cmdStartIndex; index < argc; index++)
        {
            cmd[index - cmdStartIndex] = argv[index];
        }
        cmd[argc - cmdStartIndex] = NULL;

        if (execvp(cmd[0], cmd) == -1)
        {
            perror("execvp error");
            exit(EXIT_FAILURE);
        }
    }
    else  /* parent */
    {
        waitpid(pid, &status, 0);
    }

    return 0;
}
