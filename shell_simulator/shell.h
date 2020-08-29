/*
 * Hi all the app trying to impelamint linux shell 
 * created by Mohamad Heib Id=316373562
 */

#ifndef _SHELL_H
#define _SHELL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>
#define KYEL  "\x1B[33m"
#define RED  "\x1B[31m"
#define RESET "\x1B[0m"
#define GREAN  "\x1B[1;32m"
char * get_prompt();
int cd(char * path);
int shell_main();
void say_hello();
#endif

