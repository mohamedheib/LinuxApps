#include "shell.h"

int main(){
	return shell_main();
}


int shell_main(){
	char * pro = get_prompt();
	int status,flag = 1, count = 1, rc = 0, pid;
	char cmd[1024],input, cdd[2]={"cd"}, path[1024],*p;
	char shexit[4] = "exit",history[1024][1024], history_count = 0;
	size_t cmd_len = 1024;
	int num_of_out = 0,bg , num_of_in = 0, num_of_pip = 0, num_of_space = 0, i = 0;
	say_hello();


	while(flag){
		pro = get_prompt();
		printf(KYEL"%s"RESET,pro);
		fflush(stdout);
		memset(cmd, 0 ,1024);
		count = 0;
		num_of_out = 0;
		num_of_in = 0;
		num_of_pip = 0;
		num_of_space = 0;
		read(0, &input, 1);
		while( input != '\n'){
			cmd[count] = input;
			switch(input){
				case ' ':
					++num_of_space;
					break;
				case '<':
					++num_of_in;
					break;
				case '>':
					++num_of_out;
					break;
				case '|':
					++num_of_pip;
					break;

			}
			read(0, &input, 1);
			++count;

		}
		cmd[count] = '\0';
		if(history_count >= 1024)  // start to write from index 0
			history_count = 0;
		strcpy(history[history_count], cmd);
		++history_count;

		if(strcmp(cmd, "exit") == 0){
			free(pro);   // created by malloc
			exit(0);

		}       
		if(strcmp(cmd, "history") == 0){
			for(i = 0; i < history_count; i++)
				printf("%s\n",history[i] );
			continue;
		}

		else{
			bg = 0;
			if(strrchr(cmd, '&')!=NULL){
				bg = 1;
				*strrchr(cmd, '&') = ' ';
			}
			if(strstr(cmd, cdd)!= NULL && num_of_pip != 0 ){
				fprintf(stderr,RED"syntax error `%s'\n",cmd);
				continue;
			}else if((p=strstr(cmd, "cd"))!= NULL){
				p = p +2;
				if(p == NULL){
					continue;}
				while(*p == ' ')++p;
				cd(p);
				continue;
			}



			rc = exec_cmd(cmd, num_of_out, num_of_in, num_of_pip, num_of_space);
			if( rc != 0 ){
				continue;
			}

			while(((pid = wait(&status))!= -1)&& bg == 0){
				if(WIFEXITED(status))
					printf(RED"Exit : %d\n", WEXITSTATUS(status));
					printf(RESET"");

			}

		}

	}
	return rc;
}



int exec_cmd(char * cmd, int out, int in ,int pip,int space){
	/*
	 * pip number : told us how match for we need.
	 * in/out told us if we need to get/send input/out from another place
	 */

	int process_num = (pip!=0)?pip+1:1, pips[pip][2];
	pid_t process[process_num];
	int status,rc = 0, count = 0, i = 0, rflag = 0, wflag = 0, j = 0, m =0, num =0;
	char cmds[process_num][1024];
	char input_file[1024], output_file[1024];
	char * argv[1024], *tmp,  * prog_name, *output;

	if(process_num == 1)
		strcpy(cmds[0],cmd);
	else
		strcpy(cmds[0],strtok(cmd,"|"));

	for(count = 1; count < process_num; count++){
		strcpy(cmds[count],strtok(NULL,"|"));
	}

	for(count = 0; count < pip; count++){ //create pipes
		rc = pipe(pips[count]);
		if( rc < 0){
			perror((" Error : pips "));
			return rc;
		}
	}

	for(count = 0; count < process_num; count++){ //create list of process
		process[count] = fork();

		if(process[count] ==  -1){
			perror("Fork Failed:");
			exit(-1);
		}            //start execute

		if(process[count] == 0){
			i = 0;
			memset(output_file, 0, 1024);
			memset(input_file, 0, 1024);

			if(strrchr(cmds[count], '>') != NULL && count != process_num-1){
				fprintf(stderr, " syntax error near unexpected token `%s'\n",cmds[count]);
				return -1;
			}
			if(strrchr(cmds[count], '<') != NULL && count != 0){
				fprintf(stderr, " syntax error near unexpected token `%s'\n",cmds[count]);
				return -1;
			}


			if((tmp = strrchr(cmds[count], '<' ))!= NULL){

				*tmp = ' ';
				++tmp;
				if(*tmp == '\0' || tmp == NULL){
					fprintf(stderr, "Bad input file \n");
					return -1;
				}
				for(j = 0, m = 0; *(tmp + j)!= '>' && *(tmp + j)!= '\0' && *(tmp + j)!= '\n'; j++){
					if( *(tmp + j) != ' '){
						input_file[m] = *(tmp + j);
						++m;
					}
					*(tmp + j) = ' ';

				}
				if(close(0) == -1 || open(input_file,O_RDONLY) == -1){
					perror("Error line 107");
					return -1;
				}

			}
			if((tmp = strrchr(cmds[count], '>' ))!= NULL){

				*tmp = ' ';
				++tmp;
				if(*tmp == '\0' || tmp == NULL){
					fprintf(stderr, "Bad output file \n");
					return -1;
				}

				for(j = 0, m = 0; *(tmp + j)!= '<' && *(tmp + j)!= '\0' && *(tmp + j)!= '\n' ; j++){
					if(*(tmp + j) != ' '){
						output_file[m] = *(tmp + j);
						++m;
					}
					*(tmp + j) = ' ';
				}
				if(close(1) == -1 || open(output_file,O_TRUNC| O_WRONLY | O_CREAT,0666) == -1){
					perror("Error 135 ");
					return -1;
				}


			}


			if(input_file[0] == 0 && count != 0){
				if(count != process_num ){
					if(close(0) == -1 || dup(pips[count -1][0]) == -1){
						perror("Error 146");
						return -1;
					}

				}
			}
			if(output_file[0] == 0 && count != process_num -1){
				if(close(1) == -1 || dup(pips[count][1]) == -1){
					perror("Error 156");
					return -1;
				}

			}
			for(j = 0; j<pip; j++){

				if(close(pips[j][0]) == -1 || close(pips[j][1]) == -1 ){
					return -1;
				}
			}
			tmp = strtok(cmds[count]," ");
			do{

				argv[i] = (char *)malloc(sizeof(char) * strlen(tmp) + 1);
				if (argv[i] == NULL){
					fprintf(stderr, RED "malloc failed to allocate memory \n"RESET);
					exit(-1);
				}
				strcpy(argv[i], tmp);
				argv[i][strlen(tmp)] = '\0';
				++i;
			}while((tmp = strtok(NULL," ")) != NULL);

			argv[i] == NULL;

			if(argv[0][0] == '/'){  //full path don't use execpv
				prog_name = (char *)malloc(sizeof(char) * strlen(argv[0]) + 1);

				strcpy(prog_name , argv[0]);
				strcpy(argv[0], strrchr(argv[0],'/') + 1);
				execv(prog_name, argv);
				perror("Failed to run process :");
				return -1;
			}else{
				prog_name = (char *)malloc(sizeof(char) * strlen(argv[0]) + 1);
				strcpy(prog_name , argv[0]);
				if(argv[0][0] == '.')
					argv[0] = argv[0] + 2;
				execvp(prog_name, argv);
				perror("Failed to run process :");
				return -1;

			}






		}
		else{

		}
	}

	for(j = 0; j<pip; j++){

		if(close(pips[j][0]) == -1 || close(pips[j][1]) == -1 ){
			return -1;
		}
	}
	return 0;
}

char * get_prompt(){
	char * pro_sin = "+:";
	char cwd[1024] = {0};
	const char s[2] = "/";
	char *token, *tmptoken;
	if(!getcwd(cwd,1024)){
		fprintf(stderr,RED"Failed to get curent work dirctory \n"RESET);
		exit(-1);
	}
	/* get the first token */
	tmptoken = strtok(cwd, s);

	/* walk through other tokens */
	while( tmptoken != NULL ) {
		token = tmptoken;

		tmptoken = strtok(NULL, s);
	}
	tmptoken = (char *) malloc(sizeof(char) * strlen(token) + 3);
	if(tmptoken == NULL){
		fprintf(stderr,RED"Failed to allocate memory \n"RESET);
		exit(-1);
	}
	strcpy(tmptoken, token);
	strcpy(tmptoken + strlen(token), pro_sin);
	tmptoken[strlen(tmptoken) ] = '\0';
	return tmptoken;

}


int cd(char * path){
	int rc = chdir(path);
	if(rc < 0){
		perror("cd :");
		return -1;
	}
	return 0;
}

void say_hello(){

	printf(KYEL" =============================================================\n");
	printf("|| "GREAN"Hello there welcome in Mohamad Heib shell "KYEL" ||\n");
	printf(KYEL"||"GREAN" this shell support all the normale shell cmd\t\t     "KYEL"|| \n||"GREAN" that implimnted by shell, and we have history cmd :) "KYEL"     ||\n"RESET  );

	printf(KYEL" =============================================================\n"RESET);

}
