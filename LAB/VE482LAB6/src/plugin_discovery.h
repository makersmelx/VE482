//
// Created by 60381 on 2019/11/23.
//

#ifndef SRC_PLUGIN_DISCOVERY_H
#define SRC_PLUGIN_DISCOVERY_H

#include "plugin_manager.h"

void* discover_plugins(char* dirname, PluginManager* pm);

void cleanup_plugins(void*);

#endif //SRC_PLUGIN_DISCOVERY_H
