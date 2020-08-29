#include "ls.h"
int main(int argc , char * argv[]){

        if (argc == 1){
                // work in the curent dirctory
                char cwd[1024];
                if(getcwd( cwd, 1024) == -1){
                        fprintf(stderr, "Failed to get curent work dirctory\n");
                        exit(-1);
                }

                // print some detailes about wich file we work on now
                printf(RED"details about (file/folder)"BLU " %s "RED" is:\n"RESET, cwd);
                ls_all(cwd, 1, 1 );
        }
        else {
                int i =0 ;
                for (i=1; i<argc; i++){
                        printf(RED"details about (file/folder)"BLU " %s "RED" is:\n"RESET, argv[i]);
                        ls_all(argv[i], 1, 0);
                }
        }
        return 0 ;
}