//
// Created by 60381 on 2019/11/23.
//

#include "plugin_discovery.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>
#include <stdint.h>


typedef struct PluginHandleList_t {
    void* handle;
    struct PluginHandleList_t* next;
} PluginHandleList;

typedef struct PluginDiscoveryState_t {
    PluginHandleList* handle_list;
} PluginDiscoveryState;

typedef int (*PluginInitFunc)(PluginManager*);

static char* get_plugin_name(char* filename) {
    // Find where the file name starts, and where it ends before
    // the extension starts
    char* last_slash = strrchr(filename, '/');
    char* name_start = last_slash ? last_slash + 1 : filename;
    char* last_dot = strrchr(filename, '.');

    // We only care about file names that have a .so extension
    if (!last_dot || strcmp(last_dot, ".so"))
        return NULL;

    //free(filename);
    *last_dot = '\0';
    char* name = malloc(sizeof(char*));
    strcpy(name,name_start);
    *last_dot = '.';
    return name;
}

static void* load_plugin(char* name, char* fullpath, PluginManager* pm) {
    // Make sure the path to dlopen has a slash, for it to consider it
    // an actual filesystem path and not just a lookup name.
    char* slashedpath = malloc(sizeof(char*));
    strcpy(slashedpath,"./");
    strcat(slashedpath,fullpath);

    // Attempt to open the plugin DSO
    void* libhandle = dlopen(slashedpath, RTLD_NOW);
    free(slashedpath);
    if (!libhandle) {
        printf("Error loading DSO: %s\n", dlerror());
        return NULL;
    }

    // Attempt to find the init function and then call it
    char *initfunc_name = malloc(sizeof(char*));
    strcpy(initfunc_name,"init_");
    strcat(initfunc_name,name);
    //printf("Check: %s\n",initfunc_name);

    PluginInitFunc initfunc = (PluginInitFunc)
            (intptr_t) dlsym(libhandle, initfunc_name);
    free(initfunc_name);
    if (!initfunc) {
        printf("Error loading init function: %s\n", dlerror());
        dlclose(libhandle);
        return NULL;
    }

    int rc = initfunc(pm);
    if (rc < 0) {
        printf("Error: Plugin init function returned %d\n", rc);
        dlclose(libhandle);
        return NULL;
    }

    printf("Loaded plugin from: '%s'\n", fullpath);
    return libhandle;
}

void* discover_plugins(char* dirname, PluginManager* pm) {
    char* dirname_s = dirname;
    DIR* dir = opendir(dirname_s);
    if (!dir) {
        perror("Unable to open path.");
        return NULL;
    }

    PluginDiscoveryState* plugins_state = malloc(sizeof(*plugins_state));
    plugins_state->handle_list = NULL;

    // Look at all DSOs in the plugin directory and attempt to load them.
    struct dirent* direntry;
    while ((direntry = readdir(dir)))
    {
        char* name = get_plugin_name(direntry->d_name);
        if (!name)
            continue;
        char*fullpath = malloc(sizeof(char*));
        strcpy(fullpath,dirname_s);
        strcat(fullpath,"/");
        strcat(fullpath,direntry->d_name);
        // Load the plugin, get the DSO handle and add it to the list
        void* handle = load_plugin(name, fullpath, pm);
        if (handle)
        {
            PluginHandleList* handle_node = malloc(sizeof(*handle_node));
            handle_node->handle = handle;
            handle_node->next = plugins_state->handle_list;
            plugins_state->handle_list = handle_node;
        }

        //free(name);
        free(fullpath);
    }

    closedir(dir);
    // Return a state if plugins were found.
    if (plugins_state->handle_list)
        return (void*)plugins_state;
    else {
        free(plugins_state);
        return NULL;
    }
}

void cleanup_plugins(void* vpds) {
    PluginDiscoveryState* pds = (PluginDiscoveryState*)vpds;
    if(pds == NULL)
    {
        return;
    }
    PluginHandleList* node = pds->handle_list;
    while (node) {
        PluginHandleList* next = node->next;
        dlclose(node->handle);
        free(node);
        node = next;
    }
    free(pds);
}

