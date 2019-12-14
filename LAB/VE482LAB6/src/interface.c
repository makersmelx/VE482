//
// Created by 60381 on 2019/11/5.
//

#include "interface.h"
#include "plugins/logic.h"
#include <time.h>
#include <stdlib.h>
#include "plugin_manager.h"
#include "plugin_discovery.h"
#include "sortFile.h"

int dispatch_cli(int argc, char** argv)
{
    srand(time(NULL));
    if(argc < 3)
    {
        return -1;
    }
    PluginManager* pm = PluginManager_new();
    char* dirname = malloc(sizeof(char*));
    dirname = "plugins";
    void* state = discover_plugins(dirname,pm);
    //free(dirname);
    //DO SOMETHING?
    int result = processFile(pm,argv[1],getType(argv[1]),getSort(argv[2]));
    //sortFile(argv[1], getType(argv[1]), getSort(argv[2]));
    PluginManager_free(pm);
    cleanup_plugins(state);
    return 0;
}

//int dispatch_ui(int argc, char** argv)
//{
//    srand(time(NULL));
//    char buffer[MAXLINE];
//    char filename[MAXLINE];
//    printf("Welcome to the Menu Interface!\n");
//    int exit = 0;
//    while (1)
//    {
//        int varType = -1;
//        int sortType = -1;
//        printf("Decide the type of data:\n");
//        printf("- rand_int.txt (random integers)\n");
//        printf("- rand_double.txt (random doubles)\n");
//        printf("- rand_string.txt (random strings)\n");
//        printf("- exit (Exit)\n");
//        while (1) {
//            printf("> ");
//            fgets(buffer, MAXLINE - 1, stdin);
//            buffer[strlen(buffer) - 1] = '\0';
//            if (strcmp(buffer, "exit") == 0) {
//                exit = 1;
//                break;
//            }
//            varType = getType(buffer);
//            if (varType < 0) {
//                printf("Invalid input\n");
//            }
//            else
//            {
//                strcpy(filename,buffer);
//                break;
//            }
//        }
//        if (exit)
//            break;
//        printf("Decide the sorting rule:\n");
//        printf("- inc (sort in increasing order)\n");
//        printf("- dec (sort in decreasing order)\n");
//        printf("- rand (sort in random order)\n");
//        printf("- exit (Exit)\n");
//        while (1) {
//            printf("> ");
//            fgets(buffer, MAXLINE -1 , stdin);
//            buffer[strlen(buffer) - 1] = '\0';
//            if (strcmp(buffer, "exit") == 0) {
//                exit = 1;
//                break;
//            }
//            sortType = getSort(buffer);
//            if (sortType < 0) {
//                printf("Invalid input\n");
//            }
//            else
//            {
//                break;
//            }
//        }
//        if (exit)
//            break;
//        sortFile(filename, varType, sortType);
//        printf("Sort succeeds.\n\n");
//    }
//    return 0;
//}