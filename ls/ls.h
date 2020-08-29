/* Hi All *$* this app created be mohammad heib at 26/10/2018
   this app trying to simulate  ls -al command in Unix Os
 */

#ifndef _LS_H
#define _LS_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/types.h>
#include <string.h>
#include <grp.h>
#include <time.h>
// text colors
#define GREN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define RESET "\x1B[0m"
#define KYEL  "\x1B[33m"
#define RED  "\x1B[31m"
typedef struct stat * fistat;
char get_path_type(fistat st);
/*
   Get path type : this function get file inode contaent and return the type
   of the file contant as char.
   Return value : (d = dirctory), (- = file), (l = soft link), (o = Other)
 */

char * get_permission(fistat st);
/*
   Get permission : this function get file inode contaent and return the permission of the
   file contant as char *.
   return value :
   first 3 bytes in the string contaent the owner parmission
   next 3 bytes contaent the group parmission
   theard 3 bytes contaent the world parmission
 */

int get_hard_link_num(fistat st);
/*
get_hard_link_num : this function get file inode contaent and return the number of the hard link
to the file contaent.
return value : number of hard linkes.
 */

char * get_user_name(fistat st);
/*
get_user_name : this function get file inode contaent and return the file owner name.
return value : file owner name .
 */


char * get_group_name(fistat st);
/*
get_group_name : this function get file inode contaent and return the file group name.
return value : file group name .
 */

int get_file_size(fistat st);
/*
get_file_size : this function return the size of the contaent of st pointer
return value : sizr of the contaent in bytes
 */
char * last_modifi(fistat st);
/*
last_modifi : this function get file stat and return the last modifing time
for the file
return value : last modifi date as string (month/day_of_month/time_of day)
 */

char * get_file_name(fistat st, char * path);
/*
   get file name : this function return the file name that refrenced via giving inode.
   if the giving indoe is refrence to symbolc link the return value will be symbolc_name -> contant_name

   return value char *
 */

void ls_all(char * path, int recursion, int cwd);
/* main function
cwd : say if we work in the curent dirctory or not */

#endif
