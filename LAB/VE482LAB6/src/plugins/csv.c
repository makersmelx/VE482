//
// Created by 60381 on 2019/11/23.
//

#include "../plugin_manager.h"
#include "logic.h"

static int csv_role_hook(char *filename, char *role_name, int type, int order) {
    return sortCSVFile(filename);
}

int init_csv(plugin_manager *pm) {
    //printf("!!!!!");
    char *role_name = "csv";
    plugin_manager_register_role_hook(pm, role_name, csv_role_hook);
    return 1;
}