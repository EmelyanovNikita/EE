#include <stdio.h>
#include <stdlib.h>
// #include <sys/wait.h>
// #include <sys/types.h>
// #include <unistd.h>
// #include <errno.h>


void square(int i, char *argv[], int chet_nechet)
{
	for ( int y = 1; y < i; ++y)
	{
		
		if ( y%2 == chet_nechet)
		{
			if( chet_nechet == 0 ) printf("PARENT WORK: ");
			else if ( chet_nechet == 1 ) printf("CHILD WORK: ");
			printf("Площадь квадрата %d, со стороной %d\n", (atoi( argv[y] )) * (atoi(argv[y])), atoi( argv[y] ) );
		}
	}
}
 int main (int argc, char * argv[])
{
	pid_t pid;
	//square(argc, argv, 1);
	
	
	if (argc < 2)
	{
		perror("Error argc < 2\n");
		exit(EXIT_FAILURE);
	}
	pid = fork();
	switch ( pid )
	{
		case -1:
		{
			perror(" pid = -1 ");
			exit(EXIT_FAILURE);
		}
		case 0:
		{
			square(argc, argv, 1);
			exit(EXIT_SUCCESS);
		}
		default:
		{
			square(argc, argv, 0);
			exit(EXIT_SUCCESS);
		}
		
	}
	
	
	exit(EXIT_SUCCESS);
}

