//
// Created by 60381 on 2019/11/23.
//

#include "plugin_manager.h"
#include <stdlib.h>
#include <string.h>

typedef struct plugin_hook_node_t {
    // The role for which this hook is registered, and the hook itself
    char *role;
    plugin_hook hook;
    struct plugin_hook_node_t *next;
} plugin_hook_node;


struct plugin_manager_t {
    plugin_hook_node *role_hook_list;
};

plugin_manager *create_new_plugin_manager() {
    plugin_manager *pm = malloc(sizeof(plugin_manager));
    pm->role_hook_list = NULL;
    return pm;
}

void free_plugin_manager(plugin_manager *pm) {
    plugin_hook_node *role_plugin = pm->role_hook_list;
    while (role_plugin) {
        plugin_hook_node *next = role_plugin->next;
        free(role_plugin->role);
        free(role_plugin);
        role_plugin = next;
    }
    free(pm);
}

void plugin_manager_register_role_hook(plugin_manager *pm, char *role_name, plugin_hook hook) {
    plugin_hook_node *node = malloc(sizeof(plugin_hook_node));
    node->role = malloc(sizeof(char *));
    strcpy(node->role, role_name);
    node->hook = hook;
    node->next = pm->role_hook_list;
    pm->role_hook_list = node;
}

int plugin_manager_apply_role_hooks(plugin_manager *pm, char *filename, char *role_name, int type, int order) {
    plugin_hook_node *role_plugin = pm->role_hook_list;
    while (role_plugin) {
        if (strcmp(role_name, role_plugin->role) == 0) {
            return role_plugin->hook(filename, role_name, type, order);
        }
        role_plugin = role_plugin->next;
    }
    return 0;
}
