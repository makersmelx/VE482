//
// Created by 60381 on 2019/11/5.
//

#include "interface.h"
#include "plugins/logic.h"
#include <time.h>
#include <stdlib.h>
#include "plugin_manager.h"
#include "plugin_discovery.h"
#include "sortFile.h"

int dispatch_cli(int argc, char **argv) {
    srand(time(NULL));
    if (argc < 3) {
        return -1;
    }
    plugin_manager *pm = create_new_plugin_manager();
    char *dirname = malloc(sizeof(char *));
    dirname = "plugins";
    void *state = discover_plugins_in_directory(dirname, pm);
    processFile(pm, argv[1], getType(argv[1]), getSort(argv[2]));
    free_plugin_manager(pm);
    delete_all_plugins(state);
    return 0;
}
