#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <TelephoneBook.h>


void menu (TelephoneBook *telbook)
{
	int number;
	int found;
	
	
	do
	{
		char *firstname_1 = malloc (sizeof(char) * MAXLINE);
		char *lastname_1 = malloc (sizeof(char) * MAXLINE);
		char *patronymic_1 = malloc (sizeof(char) * MAXLINE);
		char *company_1 = malloc (sizeof(char) * MAXLINE);
		char *position_1 = malloc (sizeof(char) * MAXLINE);
		char *number_1 = malloc (sizeof(char) * MAXLINE);
		char *email_1 = malloc (sizeof(char) * MAXLINE);
		char *links_1 = malloc (sizeof(char) * MAXLINE);
	
	
		printf("Найти контакт - 1\n");
		printf("Удалить контакт - 2\n");
		printf("Добавить контакт - 3\n");
		printf("Изменить контакт - 4\n");
		printf("Вывести все контакты - 5\n");
		printf("Сохранить телефонную книгу в файл (заново переписать файл) - 6\n");
		printf("Взять телефонную книгу из файла (нынешние данные будут удалены) - 7\n");
		printf("Сохранить телефонную книгу в файл (добавить новые элементы в файл) - 8\n");
		printf("Выйти - 0\n");
		scanf("%d", &number);
		switch (number)
		{
			case 1:
				printf("Введите данные для поиска контакта\n");
				found = funk_choose( search_contact, telbook, number);
				
				if (found == -1)
				{
					printf("Контакт не найден\n");
				}
				else 
				{
					printf("Имя найденного контакта: %s\n", telbook->contacts[found].name_.firstname_);
					printf("Фамилия найденного контакта: %s\n", telbook->contacts[found].name_.lastname_);
					printf("Отчество найденного контакта: %s\n", telbook->contacts[found].name_.patronymic_);
				}
				
				break;
			case 2:
				printf("Введите данные для удаления контакта\n");
				found = funk_choose( delete_contact, telbook, number);
				
				if (found == -1)
				{
					printf("Контакт не найден\n");
				}
				else 
				{
					printf("Контакт удалён\n");
				}
				
				break;
			case 3:
				printf("Введите данные для добавления контакта (- если поле не нужно заполнять, первые три заполняются обязательно)\n");
				
		
				printf("Введите имя контакта: ");
				scanf("%s", firstname_1);
				while ( (strcmp ( firstname_1, "-") ) == 0 ) 
				{
					printf("Вы не можете ввести -\nВведите имя контакта: ");
					strcpy(firstname_1, "");
					scanf("%s", firstname_1);
				}
				printf("Введите фамилию контакта: ");
				scanf("%s", lastname_1);
				while ( (strcmp ( lastname_1, "-")) == 0 ) 
				{
					printf("Вы не можете ввести -\nВведите имя контакта: ");
					strcpy(lastname_1, "");
					scanf("%s", lastname_1);
				}
				printf("Введите отчество контакта: ");
				scanf("%s", patronymic_1);
				while ( (strcmp ( patronymic_1, "-")) == 0 ) 
				{
					printf("Вы не можете ввести -\nВведите имя контакта: ");
					strcpy(patronymic_1, "");
					scanf("%s", patronymic_1);
				}
				
				
				printf("Введите компанию: ");
				scanf("%s", company_1);
				if( !(strcmp ( company_1, "-")) ) strcpy(company_1, "");
				printf("Введите должность: ");
				scanf("%s", position_1);
				if( !(strcmp ( position_1, "-")) ) strcpy(position_1, "");
				printf("Введите номер: ");
				scanf("%s", number_1);
				if( !(strcmp ( number_1, "-")) ) strcpy(number_1, "");
				printf("Введите email: ");
				scanf("%s", email_1);
				if( !(strcmp ( email_1, "-")) ) strcpy(email_1, "");
				printf("Введите ссылки: ");
				scanf("%s", links_1);
				if( !(strcmp ( links_1, "-")) ) strcpy(links_1, "");
	
	
				found = add_contact(telbook, firstname_1, lastname_1, patronymic_1, company_1, position_1, number_1, email_1, links_1);
				if (found == -1)
				{
					printf("Контакт уже существует \n");
				}
				else if ( found == -2 )
				{
					printf("Добавлено максимальное количество контактов\n");
					
				}
				else
				{
					printf("Контакт добавлен\n");
				}
				break;
			case 4:
				printf("Введите данные контакта, который нужно изменить\n");
				
				printf("Введите имя контакта: ");
				scanf("%s", firstname_1);
				printf("Введите фамилию контакта: ");
				scanf("%s", lastname_1);
				printf("Введите отчество контакта: ");
				scanf("%s", patronymic_1);
				
				if ( search_contact(telbook, firstname_1, lastname_1, patronymic_1) == -1 ) 
				{
					printf("Контакт не найден\n");
					break;
				}
				printf("Введите данные для изменения контакта (- если поле не нужно заполнять (первые три заполняются обязательно), . если поле оставить прежнее)\n");
				
				char *new_firstname = malloc (sizeof(char) * MAXLINE);
				char *new_lastname = malloc (sizeof(char) * MAXLINE);
				char *new_patronymic = malloc (sizeof(char) * MAXLINE);
				
				printf("Введите имя контакта: ");
				scanf("%s", new_firstname);
				printf("Введите фамилию контакта: ");
				scanf("%s", new_lastname);
				printf("Введите отчество контакта: ");
				scanf("%s", new_patronymic);
				
				printf("Введите компанию: ");
				scanf("%s", company_1);
				if( !(strcmp ( company_1, "-")) ) strcpy(company_1, "");
				printf("Введите должность: ");
				scanf("%s", position_1);
				if( !(strcmp ( position_1, "-")) ) strcpy(position_1, "");
				printf("Введите номер: ");
				scanf("%s", number_1);
				if( !(strcmp ( number_1, "-")) ) strcpy(number_1, "");
				printf("Введите email: ");
				scanf("%s", email_1);
				if( !(strcmp ( email_1, "-")) ) strcpy(email_1, "");
				printf("Введите ссылки: ");
				scanf("%s", links_1);
				if( !(strcmp ( links_1, "-")) ) strcpy(links_1, "");
				
				found = upgrade_contact(telbook, firstname_1, lastname_1, patronymic_1, new_firstname, new_lastname, new_patronymic, company_1, position_1, number_1, email_1, links_1);
				if (found == -1)
				{
					printf("Контакт не найден\n");
				}
				else 
				{
					printf("Данные изменены\n");
					printf("Имя контакта: %s\n", telbook->contacts[found].name_.firstname_);
					printf("Фамилия контакта: %s\n", telbook->contacts[found].name_.lastname_);
					printf("Отчество контакта: %s\n", telbook->contacts[found].name_.patronymic_);
				}
				free (new_firstname);
				free (new_lastname);
				free (new_patronymic);
				break;
				
			case 5:
				int result = show_contacts(telbook);
				if (result == -1) printf("Контакты не найдены\n");
				else printf("Все контакты показаны\n");
				break;
			case 6:
				char filename[30];
				printf("Введите имя файла: ");
				scanf("%s", filename);
			
			
				if ( writeData(filename, telbook, 0) == -1)
				{
					perror("Ошибка открытия файла для записи");
				}
				
				break;
			case 7:
				char filename_read[30];
				printf("Введите имя файла: ");
				scanf("%s", filename_read);
			
			
				if ( readData(filename_read, telbook) == -1)
				{
					perror("Ошибка открытия файла для чтения");
				}
				
				break;
			case 8:
				char filename_dop[30];
				printf("Введите имя файла: ");
				scanf("%s", filename);
			
			
				if ( writeData(filename, telbook, 1) == -1)
				{
					perror("Ошибка открытия файла для записи");
				}
				
				break;
			case 0:
				break;
		}
		printf("\n");
		
		
		free (firstname_1);
		free (lastname_1);
		free (patronymic_1);
		free (company_1);
		free (position_1);
		free (number_1);
		free (email_1);
		free (links_1);
		
		
	} while (number != 0);
	
	
}

 int main (int argc, char * argv[])
{
	//добавить возможность добавлять и другие поля и сделать функцию вывода одного контакта и если сделать так чтобы если поле не заполнено тогда хранится "-" и если в поле хранится "-" тогда при выводе ничего не выводится
	//изменится add delete upgrade show
	
	TelephoneBook telbook;
	telbook.size_book = MAXTELBOOKSIZE;
	telbook.filled_contacts = 0;

	//три контакта для теста
	strcpy(telbook.contacts[0].name_.firstname_, "Nik");
	strcpy(telbook.contacts[0].name_.lastname_, "Emel");
	strcpy(telbook.contacts[0].name_.patronymic_, "Serge");
	strcpy(telbook.contacts[0].dop_info_.company_, "ec");
	strcpy(telbook.contacts[0].dop_info_.position_, "developer");
	strcpy(telbook.contacts[0].dop_info_.number_, "89236458777");
	strcpy(telbook.contacts[0].dop_info_.email_, "emeli@mail.ru");
	strcpy(telbook.contacts[0].dop_info_.links_, "https://ec.ru");
	
	strcpy(telbook.contacts[1].name_.firstname_, "Kris");
	strcpy(telbook.contacts[1].name_.lastname_, "Bov");
	strcpy(telbook.contacts[1].name_.patronymic_, "Serge");
	strcpy(telbook.contacts[1].dop_info_.email_, "kristina@mail.ru");
	strcpy(telbook.contacts[1].dop_info_.links_, "https://vk/kristina");
	
	strcpy(telbook.contacts[2].name_.firstname_, "Mark");
	strcpy(telbook.contacts[2].name_.lastname_, "Bov");
	strcpy(telbook.contacts[2].name_.patronymic_, "Art");
	
	
	telbook.filled_contacts = 3;
	
	
	menu(&telbook);
	
}

