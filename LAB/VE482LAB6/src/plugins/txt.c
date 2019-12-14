//
// Created by 60381 on 2019/11/23.
//

#include "../plugin_manager.h"
#include "logic.h"
static int txt_role_hook(char*filename,char*rolename,int type,int order)
{
    return sortFile(filename,type,order);
}

int init_txt(PluginManager*pm)
{
    //printf("!!!!!");
    char* rolename = "txt";
    PluginManager_register_role_hook(pm,rolename,txt_role_hook);
    return 1;
}