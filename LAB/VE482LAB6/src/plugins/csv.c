//
// Created by 60381 on 2019/11/23.
//

#include "../plugin_manager.h"
#include "logic.h"
static int csv_role_hook(char*filename,char*rolename,int type,int order)
{
    return sortCSVFile(filename);
}

int init_csv(PluginManager*pm)
{
    //printf("!!!!!");
    char* rolename = "csv";
    PluginManager_register_role_hook(pm,rolename,csv_role_hook);
    return 1;
}