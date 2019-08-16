
/**
 * History:
 * ================================================================
 * 2019-08-16 qing.zou created
 *
 */

#include <getopt.h>

#include "module_config.h"

vmp_config_t* vmp_config_create()
{
    vmp_config_t* thiz = calloc(1, sizeof(vmp_config_t));
    if (thiz != NULL)
    {
        thiz->server = 0;
        strcpy(thiz->data_dir, DATA_DEFAULT_PATH);
    }

    return thiz;
}

int vmp_config_init(vmp_config_t* thiz, int argc, char* argv[])
{
    int opt = 0;
    char *conf = NULL;
    char *type = NULL;
    
// 	static const struct option long_options[] = {
// 		{ "help",			no_argument,			NULL, 'h' },
// 		{ "conf",		    no_argument,			NULL, 'c' },
// //		{ "sample",			required_argument,		NULL, 'd' },
// 		{ "type",			required_argument,		NULL, 't' },
// 		{ "log",			optional_argument,		NULL, 'l' },
// 		{ NULL, 0, NULL, 0 }
// 	};

    optind = 1;
    while ((opt = getopt(argc, argv, "hct:l")) != -1)
    //while ((opt = getopt_long(argc, argv, "hct:l", long_options, NULL)) >= 0)
	{
        // printf("opt = %c\n", opt);
        // printf("optarg = %s\n", optarg);
        // printf("optind = %d\n", optind);
        // printf("argv[optind - 1] = %s\n", argv[optind - 1]);
        switch (opt)
		{
		case 'h':
			//help();
            return 0;
        case 'c':
            conf = optarg;
            break;;
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
    if (type && strcmp(type, "server") == 0)
        thiz->server = 1;
    
    if (conf)
        vmp_config_load(thiz, conf);

    return 0;
}

int vmp_config_load_file(vmp_config_t* thiz, void *conf)
{
    return 0;
}

int vmp_config_load(vmp_config_t* thiz, void *conf)
{
    return vmp_config_load_file(thiz, conf);
}