//
// Created by 60381 on 2019/11/23.
//

#include "sortFile.h"
#include <string.h>

int processFile(plugin_manager *pm, char *filename, int type, int order) {
    char *slash = strrchr(filename, '.');
    if (slash == NULL) {
        return -1;
    }
    char *role = slash + 1;
    return plugin_manager_apply_role_hooks(pm, filename, role, type, order);
}
