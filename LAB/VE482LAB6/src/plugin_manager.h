//
// Created by 60381 on 2019/11/23.
//

#ifndef SRC_PLUGIN_MANAGER_H
#define SRC_PLUGIN_MANAGER_H

typedef int (*plugin_hook)(char *, char *, int, int);


typedef struct plugin_manager_t plugin_manager;

plugin_manager *create_new_plugin_manager();

void free_plugin_manager(plugin_manager *);

void plugin_manager_register_role_hook(plugin_manager *, char *, plugin_hook);

int plugin_manager_apply_role_hooks(plugin_manager *, char *, char *, int, int);


#endif //SRC_PLUGIN_MANAGER_H
