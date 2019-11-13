# VE482 LAB5

## Introduction

It is the directory for the code of Lab 5, which requires to build two program to read and sort data from files using linked lists. And libraries need to be built and linked in compiling.

## Instruction on Compiling and Running

Following steps can be a reference of compiling this code.

1. Make a directory named "build" and get into it.
2. Run 

```shell
cmake build ..
make
```

In the directory, then there are 4 executables created. 

**cli** and **cli_static**: for command line interface, and 

**ui** and **ui_dynamic** : for user interface. 

**cli_static** and **ui_dynamic** are compiled with static and dynamic libraries linking respectively.

## File Manifest

### list.c/.h

The implementation of linked lists. 

### logic.c/.h

The implementation for specific operations. 

### interface.c/.h

The interface dispatch functions

### user.c/cli.c

Main funcitons for UI/CLI program

### rand_int.txt/rand_string.txt/rand_double.txt

Test cases for the two programs

### CMakeLists.txt