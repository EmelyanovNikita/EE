#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <dirent.h> 
#include <ctype.h>


typedef struct FileSettings{ //структура для более удобной работы с несколькими файлами
	mode_t mode_;
	char * filename_;
}FileSettings;


FileSettings* make_struct(char * filename)
{
	struct stat buf; //буффер для найденной информации о файле
	int result; //результат выполнения stat
	FileSettings* file_set = malloc(sizeof(FileSettings)); //указатель с выделенной памятью
	
	result = stat( filename, &buf ); //получаем права у файла
    if( result != 0 )
    {
		printf( "Проблема с получением информации" );
	}
	else 
	{
		file_set->mode_ = buf.st_mode;
		file_set->filename_ = strdup(filename);
	}
	
	
	return file_set;
}


void mode_t_to_bit(mode_t mode){
	
	printf("Битовое представление: ");
	printf((mode & S_IRUSR) ? "1" : "0");
	printf((mode & S_IWUSR) ? "1" : "0");
	printf((mode & S_IXUSR) ? "1" : "0");
	printf((mode & S_IRGRP) ? "1" : "0");
	printf((mode & S_IWGRP) ? "1" : "0");
	printf((mode & S_IXGRP) ? "1" : "0");
	printf((mode & S_IROTH) ? "1" : "0");
	printf((mode & S_IWOTH) ? "1" : "0");
	printf((mode & S_IXOTH) ? "1" : "0");
	printf("\n");
	
}


mode_t modern_mode(mode_t mode_cur, char who, char mode_set, char operate)
{
	mode_t itog_mask = 0;
	mode_t temp_mask;
	
	
	switch(mode_set)
	{
		case 'r':
			temp_mask = S_IRUSR | S_IRGRP | S_IROTH;
		break;
		case 'w':
			temp_mask = S_IWUSR | S_IWGRP | S_IWOTH;
		break;
		case 'x':
			temp_mask = S_IXUSR | S_IXGRP | S_IXOTH;
		break;
		default:
			printf("Ошибка");
			exit(EXIT_FAILURE);
	}
	
	
	if(who == 'a' || who == 'u') itog_mask |= temp_mask & (S_IRUSR | S_IWUSR | S_IXUSR);
	if(who == 'a' || who == 'g') itog_mask |= temp_mask & (S_IRGRP | S_IWGRP | S_IXGRP);
	if(who == 'a' || who == 'o') itog_mask |= temp_mask & (S_IROTH | S_IWOTH | S_IXOTH);
	
	
	switch (operate) {
		case '+':
			mode_cur |= itog_mask;
		break;
		case '-':
			mode_cur &= ~itog_mask;
		break;
		case '=':
			if (who == 'a') mode_cur = (mode_cur & ~0777) | itog_mask;
			else
			{
				if (who == 'u') mode_cur = (mode_cur & ~0777) | (itog_mask & 0700);
				if (who == 'g') mode_cur = (mode_cur & ~0777) | (itog_mask & 0070);
				if (who == 'o') mode_cur = (mode_cur & ~0777) | (itog_mask & 0007);
			}
		break;
	}
	
	
	return mode_cur;
}

mode_t update_mode_from_str(mode_t mode, char * str_mode)
{
	char * str_who = (char*) malloc (sizeof(char) * 3); //для кого будут применены права
	char * str_mode_set = (char*) malloc (sizeof(char) * 3); //какие будут применены права
	char operate = '\0'; //операция над правами
	
	
	for( int i = 0, i_who=0, i_mode =0; str_mode[i] != '\0'; ++i) //строку на составляющие
	{
		if (operate!='\0' && ispunct(str_mode[i]) ) 
		{
			printf("Ошибка, знак должен быть только один");
			break;
		}
		else if (operate=='\0'  && ispunct(str_mode[i]) ) { //к кому применяем 
			operate = str_mode[i];
		}
		else if (operate=='\0' && isalpha(str_mode[i])) { //какие будут права
			str_who[i_who] = str_mode[i];
			++i_who;
		}
		else if(operate!='\0' && isalpha(str_mode[i]) ) //какая будет операция
		{
			str_mode_set[i_mode] = str_mode[i];
			++i_mode;
		}
	}
	
	for( int i = 0; str_who[i] != '\0'; ++i) { 
		for(int y=0; str_mode_set[y] != '\0'; ++y)
		{
			if(operate == '=' && y > 0)//из-за тонкостей реализации при равенстве мы всегда заново присваиваем и поэтому старое значение затирается и для того что этого не было мы сохраняем  старое значние и делаем или
			{
				mode_t temp_mode = mode;
				mode = modern_mode(mode, str_who[i], str_mode_set[y], operate);
				mode |= temp_mode;
			}
			else mode = modern_mode(mode, str_who[i], str_mode_set[y], operate);
		}
	}
	//print_FileSettings(mode);
	
	free(str_who);
	free(str_mode_set);
	return mode;
}



mode_t str_to_mode_t(const char* str_mode){
	
	mode_t mode = 0;
	
	if(str_mode[0] == 'r')
		mode = mode | S_IRUSR;
	if(str_mode[1] == 'w')
		mode = mode | S_IWUSR;
	if(str_mode[2] == 'x')
		mode = mode | S_IXUSR;
	if(str_mode[3] == 'r')
		mode = mode | S_IRGRP;
	if(str_mode[4] == 'w')
		mode = mode | S_IWGRP;
	if(str_mode[5] == 'x')
		mode = mode | S_IXGRP;
	if(str_mode[6] == 'r')
		mode = mode | S_IROTH;
	if(str_mode[7] == 'w')
		mode = mode | S_IWOTH;
	if(str_mode[8] == 'x')
		mode = mode | S_IXOTH;
	
	
	return mode;
}


mode_t update_mode_from_int(mode_t mode, int int_mode)
{
	mode |= int_mode;
	print_FileSettings(mode);
	return mode;
}

void print_FileSettings(mode_t mode)
{
	printf( (S_ISDIR(mode)) ? "\td" : "\t-");
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
	
	printf(" \tdec: %o", mode & 0777 );
	
	printf(" \tbin: ");
	mode_t_to_bit(mode);
	
    printf("\n");
}


void take_files_from_cur_dir(char * str_dir)
{
	struct dirent *de;
	
	
	DIR *dr = opendir(str_dir); 
    if (dr == NULL)
    { 
        printf("Не удалось открыть директорию" ); 
        return 0; 
    } 
	
	printf("Все файлы из папки %s\n", str_dir);
	
    for( int i=0; (de = readdir(dr)) != NULL; ++i) 
	{
		printf("%s\n", de->d_name);
	}
    closedir(dr);
    return ;
}


menu()
{
	int n;
	
	while(n!=NULL)
	{
		printf("Получить права после ввода прав в буквенном или цифровом обозначении - 1\n");
		printf("Вывести все файлы из переданной папки - 2\n");
		printf("Выбрать файл для вывода прав - 3\n");
		printf("Изменить права файла - 4\n");
		printf("Введите номер операции: ");
		scanf("%d", &n);
		printf("\n");
		
		
		switch(n)
		{
			case 1:
				int n_1;
				printf("Ввести права в буквенном обозначении - 1\n");
				printf("Ввести права в цифровом обозначении - 2\n");
				scanf("%d", &n_1);
				mode_t test = 0;
				switch(n_1)
				{
					case 1:
						char str_1[10];
						printf("Введите права в буквенном виде(пример ввода: rwx-w-rw-): ");
						scanf("%s", str_1);
						test = str_to_mode_t(str_1);
						printf("\nПрава: ");
						print_FileSettings(test);
					break;
					case 2:
						mode_t temp_1;
						printf("Введите права в цифровом виде: ");
						scanf("%o", &temp_1);
						print_FileSettings(temp_1);
					break;
				}
			break;
			
			
			case 2:
				char str_dir[30];
				printf("Введите путь и название папки для получения файлов (пример открытия папки test: ./test/)(. это текущая папка)\n");
				scanf("%s", str_dir);
				printf("\n");
				take_files_from_cur_dir(str_dir);
			break;
			
			
			case 3:
				char p_str[30];
				printf("Введите путь и имя файла (пример для файла main.c из папки source: source/main.c)\n");
				scanf("%s", p_str);
				FileSettings *file_sett = make_struct(p_str);
				
				printf("Права файла: %s", file_sett->filename_);
				print_FileSettings(file_sett->mode_);
				free(file_sett);
			break;
			
			
			case 4: 
				char p_str_new[30];
				int num;
				mode_t mode_to_operate = 0;
				printf("Введите путь и имя файла (пример для файла main.c из папки source: source/main.c)\n");
				scanf("%s", p_str_new);
				FileSettings *file_sett_new = make_struct(p_str_new);
				
				printf("Права файла: %s", file_sett_new->filename_);
				print_FileSettings(file_sett_new->mode_);
				
				
				printf("Изменить права можно разными способами:\nНаписав права в виде rwx-rw-rw - 1\n");
				printf("Написав (как в chmod) u+rwx - 2\n");
				printf("Написав цифрами 777 - 3\n");
				scanf("%d", &num);
				switch(num)
				{
					case 1:
						char str_mode[10];
						printf("Введите новые права в буквенном виде(пример ввода: rwx-w-rw-): ");
						scanf("%s", str_mode);
						mode_to_operate = str_to_mode_t(str_mode);
						file_sett_new->mode_ = 0;
						file_sett_new->mode_ |= mode_to_operate; //изменяем права у файла
					break;
					
					
					case 2:
						char str_mode_chmod[10];
						printf("Введите новые права в буквенном виде(пример ввода: u+rwx): ");
						scanf("%s", str_mode_chmod);
						file_sett_new->mode_ = update_mode_from_str(file_sett_new->mode_, str_mode_chmod); //изменяем права у файла
					break;
					
					
					case 3:
						printf("Введите права в цифровом виде: ");
						scanf("%o", &mode_to_operate);
						file_sett_new->mode_ = 0;
						file_sett_new->mode_ |= mode_to_operate; //изменяем права у файла
						
						///
					break;
					
					
					default:
						printf("Операция не выбрана\n");
						free(file_sett_new);
						return;
					break;
				}
				
				
				printf("Новые права файла: %s", file_sett_new->filename_);
				print_FileSettings(file_sett_new -> mode_);
				free(file_sett_new);
				
				
			break;
			
			
			default: 
				return;
			
			
			test = 0;
		}
		
	}
		
}
 int main ()
{
	printf("\nПример получения прав файла\n");
	char * p_str = "source/main.c";
	FileSettings *file_main = make_struct(p_str);
	
	printf("%s", file_main->filename_);
	print_FileSettings(file_main->mode_);
	
	printf("\nПример измененния прав файла\nsource/main.c");
	char * a = "a-r";
	mode_t temp = update_mode_from_str(file_main->mode_, a);
	
	printf("\nещё раз измененние прав файла\n");
	printf("source/main.c");
	update_mode_from_int(temp, 255);
	printf("\n\n");
	
	// printf("Введите название файла: ");
	// scanf("%s", p_str);

	
	// mode_t test=0;
	// char *str_1 = malloc(sizeof(char) * 10);
	// printf("\nВведите права в символьном виде: \n");
	// scanf("%s", str_1);
	// test = str_to_mode_t(str_1);
	// printf("\nПрава: ");
	// print_FileSettings(test);
	
	// mode_t test_mode=0;
	// int test_1;
	// printf("\nВведите права в числовом виде: \n");
	// scanf("%d", &test_1);
	// printf("\nПрава: ");
	// test_mode = update_mode_from_int(test_mode, test_1);
	
	
	// free(str_1);
	menu();
	return 0;
}