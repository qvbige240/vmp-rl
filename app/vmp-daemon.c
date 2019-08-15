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

#include "mod_log.h"

#include "pbrpc_server.h"
#include "pbrpc_client.h"

#define SW_VERSION  "v1.0"

int main(int argc, char **argv)
{
    int opt = 0;

	mod_log_init(PROCESS_DAEMON, NULL);

	fprintf(stdout, "\n****************************** ******************************\n");
	fprintf(stdout, "  *             vmp-rl daemon starting...                     *\n");
	fprintf(stdout, "  *             version %s                                  *\n", SW_VERSION);
	fprintf(stdout, "\n****************************** ******************************\n\n\n");

    char *type = NULL;
	while ((opt = getopt(argc, argv, "hvlt:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			//help();
            return 0;
        case 'v':
            //version();
            return 0;
        case 't':
            type = optarg;
            break;
        case 'l':
            //showlog = 1;
        default:
            break;
        }
	}

    if (strcmp(type, "server") == 0)
        server(1, argv);
    else
        client();

    return (0);
}
