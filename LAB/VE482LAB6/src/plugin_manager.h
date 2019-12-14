//
// Created by 60381 on 2019/11/23.
//

#ifndef SRC_PLUGIN_MANAGER_H
#define SRC_PLUGIN_MANAGER_H

typedef int (*PluginRoleHook)(char*,char*,int,int);


typedef struct PluginManager_t PluginManager;

PluginManager* PluginManager_new();

void PluginManager_free(PluginManager* pm);

void PluginManager_register_role_hook(PluginManager* pm, char* rolename, PluginRoleHook hook);

int PluginManager_apply_role_hooks(PluginManager* pm, char*filename, char* rolename, int type,int order);




#endif //SRC_PLUGIN_MANAGER_H
