# LAB5

<center>Wu Jiayao, 517370910257
</center>

## 1. Layer programming

### The program can be divided into three layers, what are they?

The kernel layer, the logic/api layer and the interface layer.

### Split the program into files according to the defined layers. Create the appropriate corresponding header files.

The kernel layer in **list.c/.h**, the logic layer in **logic.c/.h** and the interface layer in **interface.c/.h**

### Write a “Menu interface”

see **interface.c/.h**

### Write two main functions,

see **main.c**

## 2. Libraries

### What are the three stages performed when compiling a file?

preprocess, compilation and link.

### Briefly describe each of them.

- preprocess: interpret definitions, join continued lines (lines ending with a `\`) and strip comments. 
- compilation: translate code to assembly instructions specific to the target processor architecture, then translate the assembly instructions to object code. 
- link: rearrange and link binary files to ensure successful function calling

### Search more details on how to proceed

"add_library" command in CMake

### Create two static libraries, one for each of the two lowest layers in the previous program

~~~cmake
add_library(list_static STATIC list.c)
add_library(logic_static STATIC logic.c)
~~~

### Compile the command line version of the program using these two static libraries.

~~~cmake
add_executable(cli_static cli.c interface.c)
target_link_libraries(cli_static logic_static list_static)
~~~

### Generate two dynamic libraries, one for each of the two lowest layers in the previous program

~~~cmake
add_library(list_dynamic SHARED list.c)
add_library(logic_dynamic SHARED logic.c)
target_link_libraries(logic_dynamic list_dynamic)
~~~

### Compile the whole program

~~~cmake
add_executable(cli cli.c interface.c logic.c list.c)
add_executable(ui ui.c interface.c logic.c list.c)
~~~

### Compile the Menu version of the program using these two dynamic libraries.

~~~cmake
add_executable(ui_dynamic ui.c interface.c)
target_link_libraries(ui_dynamic logic_dynamic list_dynamic)
~~~

### What is the difference between a library and the API.

API is implemented for operating systems, but library are specified for C/C++ language.