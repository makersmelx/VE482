# VE482 HW3

<center>Wu Jiayao 517370910257
</center>

## 1. POSIX

Specified by the IEEE Computer Society, The Portable Operating System Interface (POSIX) is a family of standards for maintaining compatibility between operating systems. POSIX defines the application programming interface (API), command line shells and utility interfaces, for software compatibility with variants of Unix and other operating systems.

Unix was first selected as the basis for a standard system interface. However, later versions of Unix, including macOS, Linux, do not have compatibility with each other. Hence, there is a need to develop a new standard between different operating system.

POSIX includes such things:

- General terms, concepts, and interfaces
- Definitions for system service functions and subroutines, language-specific system services for the C programming language, function issues
- Definitions for a standard source code-level interface to command interpretation services, namely a shell, and common utility programs for application program

## 2. General Questions

1. Acutally, it is the programmer that writes the code to make all threads working in cooperation. Programmers make the thread voluntarily give up its CPU resources when the resources are considered not needed.
2.  The biggest advantage is the efficiency. No traps to the kernel are needed to switch threads. The biggest disadvantage is that if one thread blocks, the entire process blocks.
3.  No,. For a single-threaded process, it cannot fork when waiting for keyboard input.
4. Such system calls will be changed into combination of Win 32 APIs or other codes.

## 3. C programming

See /src

Use in terminal the shell script to test all steps.

~~~shell
./app.sh
~~~

## Reference

Information technology — Portable Operating System Interface (POSIX®) Base Specifications, Issue 7, www.iso.org/standard/50516.html

