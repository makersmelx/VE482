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


typedef struct plugin_dl_node_t {
    void *handle;
    struct plugin_dl_node_t *next;
} plugin_dl_node;

typedef struct plugin_dl_list_t {
    plugin_dl_node *handle_list;
} plugin_dl_list;

typedef int (*plugin_init_func)(plugin_manager *);

static char *get_plugin_name(char *filename) {
    // Find where the file plugin_name starts, and where it ends before the extension starts
    char *last_slash = strrchr(filename, '/');
    char *name_start = last_slash ? last_slash + 1 : filename;
    char *last_dot = strrchr(filename, '.');

    // We only care about file names that have a .so extension
    if (!last_dot || strcmp(last_dot, ".so") != 0)
        return NULL;

    *last_dot = '\0';
    char *plugin_name = malloc(sizeof(char *));
    strcpy(plugin_name, name_start);
    *last_dot = '.';
    return plugin_name;
}

static void *load_plugin(char *plugin_name, char *full_path, plugin_manager *pm) {
    // Make sure the path to dlopen has a slash, for it to consider it an actual filesystem path and not just a lookup plugin_name.
    char *slashed_path = malloc(sizeof(char *));
    strcpy(slashed_path, "./");
    strcat(slashed_path, full_path);

    // Attempt to open the plugin DSO
    void *dl_fd = dlopen(slashed_path, RTLD_NOW);
    free(slashed_path);
    if (!dl_fd) {
        printf("Error loading DSO: %s\n", dlerror());
        return NULL;
    }

    // Attempt to find the init function and then call it
    char *init_func_name = malloc(sizeof(char *));
    strcpy(init_func_name, "init_");
    strcat(init_func_name, plugin_name);

    plugin_init_func init_func = (plugin_init_func) (intptr_t) dlsym(dl_fd, init_func_name);
    free(init_func_name);

    if (!init_func) {
        printf("Error loading init function: %s\n", dlerror());
        dlclose(dl_fd);
        return NULL;
    }

    int ret = init_func(pm);
    if (ret < 0) {
        printf("Error: Plugin init function returned %d\n", ret);
        dlclose(dl_fd);
        return NULL;
    }

    printf("Successfully load plugin from: '%s'\n", full_path);
    return dl_fd;
}

void *discover_plugins_in_directory(char *dir_name, plugin_manager *pm) {
    DIR *dir = opendir(dir_name);
    if (!dir) {
        perror("Unable to open path.");
        return NULL;
    }

    plugin_dl_list *dl_list = malloc(sizeof(*dl_list));
    dl_list->handle_list = NULL;

    // Look at all DSOs in the plugin directory and attempt to load them.
    struct dirent *dir_entry;
    while ((dir_entry = readdir(dir))) {
        char *plugin_name = get_plugin_name(dir_entry->d_name);
        if (!plugin_name)
            continue;
        char *full_path = malloc(sizeof(char *));
        strcpy(full_path, dir_name);
        strcat(full_path, "/");
        strcat(full_path, dir_entry->d_name);

        // Load the plugin, get the DSO handle and add it to the list
        void *plugin_dl = load_plugin(plugin_name, full_path, pm);
        if (plugin_dl) {
            plugin_dl_node *new_dl_node = malloc(sizeof(plugin_dl_node));
            new_dl_node->handle = plugin_dl;
            new_dl_node->next = dl_list->handle_list;
            dl_list->handle_list = new_dl_node;
        }
        free(plugin_name);
        free(full_path);
    }

    closedir(dir);

    // Return a state if plugins were found.
    if (dl_list->handle_list)
        return (void *) dl_list;
    else {
        free(dl_list);
        return NULL;
    }
}

void delete_all_plugins(void *void_plugin_state) {
    plugin_dl_list *plugin_state = (plugin_dl_list *) void_plugin_state;
    if (plugin_state == NULL) {
        return;
    }
    plugin_dl_node *node = plugin_state->handle_list;
    while (node) {
        plugin_dl_node *next = node->next;
        dlclose(node->handle);
        free(node);
        node = next;
    }
    free(plugin_state);
}

