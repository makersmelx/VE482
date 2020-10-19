//
// Created by 60381 on 2019/11/23.
//

#include "../plugin_manager.h"
#include "logic.h"

static int txt_role_hook(char *filename, char *role_name, int type, int order) {
    return sortFile(filename, type, order);
}

int init_txt(plugin_manager *pm) {
    char *role_name = "txt";
    plugin_manager_register_role_hook(pm, role_name, txt_role_hook);
    return 1;
}