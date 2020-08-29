#include "ls.h"

char get_path_type(fistat st){

        int rc = 0;
        char err[1024];

        if(st == NULL){
                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }



        if(S_ISLNK(st->st_mode))
                return 'l';
        if(S_ISREG(st->st_mode))
                return '-';
        if(S_ISDIR(st->st_mode))
                return 'd';


        return 'o';


}

char * get_permission(fistat st){
        int rc = 0, i=0;
        char err[1024], * parr;

        if(st== NULL){
                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }
        parr = (char *) malloc(sizeof(char) * 10);
        if (parr == NULL)
        {
                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }

        for(i=0; i<3; i++){
                if(i == 0){
                        parr[0] = ((S_IRUSR & st->st_mode) == S_IRUSR)?'r':'-';
                        parr[1] = ((S_IWUSR & st->st_mode) == S_IWUSR)?'w':'-';
                        parr[2] = ((S_IXUSR & st->st_mode) == S_IXUSR)?'x':'-';

                }
                else if(i == 1){
                        parr[3] = ((S_IRGRP & st->st_mode) == S_IRGRP)?'r':'-';
                        parr[4] = ((S_IWGRP & st->st_mode) == S_IWGRP)?'w':'-';
                        parr[5] = ((S_IXGRP & st->st_mode) == S_IXGRP)?'x':'-';

                }
                else if(i == 2){
                        parr[6] = ((S_IROTH & st->st_mode) == S_IROTH)?'r':'-';
                        parr[7] = ((S_IWOTH & st->st_mode) == S_IWOTH)?'w':'-';
                        parr[8] = ((S_IXOTH & st->st_mode) == S_IXOTH)?'x':'-';

                }
        }
        parr[9] = '\0';
        return parr;


}

int get_hard_link_num(fistat st){
        int rc = 0;
        char err[1024];

        if(st == NULL){
                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }

        return st->st_nlink;

}

char * get_user_name(fistat st){

        struct passwd * passwd;
        char * username=NULL;
        if(st == NULL){
                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }

        passwd = getpwuid(st->st_uid);
        username = (char *) malloc(sizeof(char) * (strlen(passwd->pw_name)+1));

        if( username == NULL){

                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }
        strcpy(username, passwd->pw_name);
        return username;
}



char * get_group_name(fistat st){

        struct group * group;
        char * gname=NULL;
        if(st == NULL){
                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }

        group = getgrgid(st->st_gid);
        gname = (char *) malloc(sizeof(char) * (strlen(group->gr_name)+1));

        if( gname == NULL){

                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }
        strcpy(gname, group->gr_name);
        return gname;
}


int get_file_size(fistat st){


        if(st == NULL){
                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }

        return st->st_size;
}


char * last_modifi(fistat st){

        struct tm * mtime;
        char *lasttime;
        int j = 0;
        if(st == NULL){
                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }

        mtime = localtime(&st->st_mtime);
        lasttime = (char *) malloc(sizeof(char)*(strlen(asctime(mtime))+1));
        if(mtime == NULL || lasttime ==  NULL){


                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }
        strcpy(lasttime, asctime(mtime));
        lasttime[strlen(lasttime)-9]='\0'; 
        for(; *lasttime != ' '; lasttime++);
        return ++lasttime;
}


char * get_file_name(fistat st, char * path){

        char * file_name = NULL, type, cont_name[1024];
        file_name = (char *)malloc(sizeof(char)*1024);
        if(st == NULL || file_name ==NULL){
                fprintf(stderr, "Failed in Function = %s in Line = %d \n ", __FUNCTION__, __LINE__);
                exit(-1);
        }

        type = get_path_type(st);

        if (type == '-'){
                sprintf(file_name, GREN);
                strcpy(file_name + strlen(file_name), path);
                sprintf(file_name + strlen(file_name), RESET);
                return file_name;
        }
        else if (type == 'd'){
                sprintf(file_name, BLU);
                strcpy(file_name + strlen(file_name), path);
                sprintf(file_name + strlen(file_name), RESET);
                return file_name;
        }
        else if(type == 'l'){
                sprintf(file_name, MAG);
                strcpy(file_name + strlen(file_name), path);
                sprintf(file_name + strlen(file_name), RESET);
                strcpy(file_name + strlen(file_name), " -> ");
                if(readlink(path, cont_name, 1024) == -1 ){
                        perror("Failed to get symbolic contaent ");
                        exit(-1);
                }

                sprintf(file_name + strlen(file_name), KYEL);
                strcpy(file_name + strlen(file_name), cont_name);
                sprintf(file_name + strlen(file_name), RESET);
        }
        return file_name;
}

void ls_all(char * path, int recursion, int cwd ){

        char type;
        fistat st = (fistat)malloc(sizeof(struct stat)*1);

        if(st == NULL){
                fprintf(stderr, "Failed to create file stat \n" );
                exit(-1);
        }
        if(lstat(path, st) == -1){



                printf(" path = %s \n", path);
                perror("Failed to read file inode");
                exit(-1);
        }
        type = get_path_type(st);

        if(type == '-'||type == 'l'||(type == 'd' && recursion == 0)){

                printf("%c ",  get_path_type(st));
                printf("%s ",  get_permission(st));
                printf("%d ",  get_hard_link_num(st));
                printf("%s ",get_user_name(st));
                printf("%s ",get_group_name(st));
                printf("%d ",get_file_size(st));
                printf("%s ",last_modifi(st));
                printf("%s ",get_file_name(st, path));
                printf("\n");
                free(st);
                return;
        }

        else if(type == 'd' && recursion != 0 ){

                DIR * dirc = NULL;
                struct dirent * cdir=NULL;
                dirc = opendir(path);
                char patht[4048];
                if(dirc == NULL){
                        fprintf(stderr, "Failed to open dirctory :%s\n",path);
                        exit(-1);
                }
                while((cdir = readdir(dirc)) != NULL){
                        if( cwd == 0){
                                strcpy(patht, path);
                                if(path[strlen(path)-1] != '/')   // if the user enter durctory name without 
                                        strcpy((patht+strlen(patht)), "/"); //  '/' at the end of the path 
                                strcpy((patht+strlen(patht)), cdir->d_name);
                        }
                        else
                                strcpy(patht, cdir->d_name);

                        ls_all(patht, 0, 0);
                }
                return;
        }

}