/**
 * History:
 * ================================================================
 * 2019-08-14 qing.zou created
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <sys/wait.h>

#include "load.h"

#include "pbrpc_server.h"
#include "pbrpc_client.h"

#define SW_VERSION  "v1.0"

static void handler(int num)
{
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    VMP_LOGW("waitpid, ret = %d", pid);

    if (WIFEXITED(status)) {
        VMP_LOGI("The child %d exit with code %d\n", pid, WEXITSTATUS(status));
    }
    signal(SIGCHLD, handler);
}

int main(int argc, char **argv)
{
    int opt = 0;

    fprintf(stdout, "\n****************************** ******************************\n");
    fprintf(stdout, "*             vmp-rl daemon starting...                     *\n");
    fprintf(stdout, "*             version %s                                  *", SW_VERSION);
    fprintf(stdout, "\n****************************** ******************************\n\n\n");

    char *conf = NULL;
    char *type = NULL;
    while ((opt = getopt(argc, argv, "hvclt:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            //help();
            return 0;
        case 'v':
            //version();
            return 0;
        case 'c':
            conf = optarg;
            break;
        case 't':
            type = optarg;
            break;
        case 'l':
            //showlog = 1;
        default:
            break;
        }
    }
    printf("type: %s\n", type);
    signal(SIGCHLD, handler);

    load_init(argc, argv);

    load_start();

    // if (strcmp(type, "client") == 0)
    //     client();
    // else
    //     server(1, argv);

    load_done();

    return (0);
}
