#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#define MAX_ARGS 100


void parse(char * command, char *arguments[])
{
	char *token = strtok(command, " ");
    int i = 0;
    while (token != NULL && i < MAX_ARGS - 1) {
        arguments[i++] = token;
        token = strtok(NULL, " ");
    }
    arguments[i] = NULL; // Завершение списка аргументов
}


 int main (int argc, char * argv[])
{
	
	
	printf("Чтобы выйти введите exit\n");
	while (1)
	{
		char command[100];
		char * arguments[MAX_ARGS];
		
		
		printf(">> ");
		if (fgets(command, sizeof(command), stdin) == NULL) {
            perror("Ошибка ввода");
            continue;
        }
		
		
		command[strcspn(command, "\n\t\v\f\r")] = '\0'; //если строка оканчивается управляющие символы заменяем их на ноль тогда
		
		
		if( isspace(command[0]) || command[0] == '\0' ) // если была передана пустая строка переходим к следующей итерации цикла
		{
			continue;
		}
		
		
		if ( strcmp (command, "exit") == 0 )
		{		
			break;
		}
		
		
		parse(command, arguments);
		
		
		pid_t pid = fork();
		int rv;
		switch ( pid )
		{
			case -1:
			{
				perror(" pid = -1 ");
				exit(EXIT_FAILURE);
			}
			case 0:
			{
				if(execvp(arguments[0], arguments) == -1)
				{
					perror("Ошибка execv");
					exit(EXIT_FAILURE);
				}
				exit(EXIT_SUCCESS);
			}
			default:
			{
				wait(NULL);
			}
			
		}
		
		
	}
	
	
	exit(EXIT_SUCCESS);
}

