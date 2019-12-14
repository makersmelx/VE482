//
// Created by 60381 on 2019/11/23.
//

#include "plugin_manager.h"
#include <stdlib.h>
#include <string.h>

typedef struct PluginRoleHookList_t
{
    // The role for which this hook is registered, and the hook itself
    char* role;
    PluginRoleHook hook;
    struct PluginRoleHookList_t* next;
} PluginRoleHookList;


struct PluginManager_t
{
    PluginRoleHookList* role_hook_list;
};

PluginManager* PluginManager_new()
{
    PluginManager* pm = malloc(sizeof(pm));
    pm->role_hook_list = NULL;
    return pm;
}

void PluginManager_free(PluginManager* pm)
{
    PluginRoleHookList* role_plugin = pm->role_hook_list;
    while (role_plugin) {
        PluginRoleHookList* next = role_plugin->next;
        free(role_plugin->role);
        free(role_plugin);
        role_plugin = next;
    }
    free(pm);
}

void PluginManager_register_role_hook(PluginManager* pm, char* rolename,PluginRoleHook hook) {
    PluginRoleHookList* node = malloc(sizeof(node));
    node->role = malloc(sizeof(char*));
    strcpy(node->role,rolename);
    node->hook = hook;
    node->next = pm->role_hook_list;
    pm->role_hook_list = node;
}

int PluginManager_apply_role_hooks(PluginManager* pm, char*filename, char* rolename,int type,int order)
{
    PluginRoleHookList* role_plugin = pm->role_hook_list;
    while (role_plugin)
    {
        if (strcmp(rolename, role_plugin->role) == 0)
        {
            //printf("%s\n",rolename);
            return role_plugin->hook(filename,rolename,type,order);
        }

        role_plugin = role_plugin->next;
    }
    return 0;
}
