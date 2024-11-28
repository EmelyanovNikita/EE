#include "TelephoneBook.h"

bool is_empty(TelephoneBook* telbook)
{
	if (telbook == 0 || telbook->filled_contacts == 0 ) return true;
	return false;
}


int search_contact(TelephoneBook *telbook, char *firstname, char *lastname, char *patronymic)
{
	if (telbook == 0 || is_empty(telbook)) return -1;
	
	for (int i = 0, end = telbook->filled_contacts; i<end; ++i)
	{
		if ( !(strcmp ( telbook->contacts[i].name_.firstname_, firstname)) &&
		!(strcmp( telbook->contacts[i].name_.lastname_, lastname) ) &&
		!(strcmp( telbook->contacts[i].name_.patronymic_, patronymic) ) ) return i;
	}
	return -1;
}


int delete_contact(TelephoneBook *telbook, char *firstname, char *lastname, char *patronymic)
{
	int found;
	found = search_contact(telbook, firstname, lastname, patronymic);
	
	if (found == -1) return found;
	
	
	for (int i = 0, end = telbook->filled_contacts; i<end; ++i)
	{
		if ( i > found ) 
		{
			strcpy(telbook->contacts[i-1].name_.firstname_, telbook->contacts[i].name_.firstname_);
			strcpy(telbook->contacts[i-1].name_.lastname_, telbook->contacts[i].name_.lastname_);
			strcpy(telbook->contacts[i-1].name_.patronymic_, telbook->contacts[i].name_.patronymic_);
			
			strcpy(telbook->contacts[i-1].dop_info_.company_, telbook->contacts[i].dop_info_.company_);
			strcpy(telbook->contacts[i-1].dop_info_.position_, telbook->contacts[i].dop_info_.position_);
			strcpy(telbook->contacts[i-1].dop_info_.number_, telbook->contacts[i].dop_info_.number_);
			strcpy(telbook->contacts[i-1].dop_info_.email_, telbook->contacts[i].dop_info_.email_);
			strcpy(telbook->contacts[i-1].dop_info_.links_, telbook->contacts[i].dop_info_.links_);
		}
		if (i==end-1)
		{
			telbook->contacts[i].name_.firstname_[i] = '\0';
			telbook->contacts[i].name_.lastname_[i] = '\0';
			telbook->contacts[i].name_.patronymic_[i] = '\0';
			telbook->contacts[i].dop_info_.company_[i] = '\0';
			telbook->contacts[i].dop_info_.position_[i] = '\0';
			telbook->contacts[i].dop_info_.number_[i] = '\0';
			telbook->contacts[i].dop_info_.email_[i] = '\0';
			telbook->contacts[i].dop_info_.links_[i] = '\0';
		}
	}
	telbook->filled_contacts --;
	
	return 0;
}


int add_contact(TelephoneBook *telbook, char *firstname, char *lastname, char *patronymic, char *company, char *position,char *number, char *email, char *links)
{
	if ( search_contact(telbook, firstname, lastname, patronymic) != -1) return -1;
	if ( telbook->size_book == telbook->filled_contacts) return -2;
	
	
	if (telbook->filled_contacts < telbook->size_book)
	{
		strcpy(telbook->contacts[telbook->filled_contacts].name_.firstname_, firstname);
		strcpy(telbook->contacts[telbook->filled_contacts].name_.lastname_, lastname);
		strcpy(telbook->contacts[telbook->filled_contacts].name_.patronymic_, patronymic);
		
		strcpy(telbook->contacts[telbook->filled_contacts].dop_info_.company_, company);
		strcpy(telbook->contacts[telbook->filled_contacts].dop_info_.position_, position);
		strcpy(telbook->contacts[telbook->filled_contacts].dop_info_.number_, number);
		strcpy(telbook->contacts[telbook->filled_contacts].dop_info_.email_, email);
		strcpy(telbook->contacts[telbook->filled_contacts].dop_info_.links_, links);
		
		telbook->filled_contacts++;
	}
	else printf ("telbook->filled_contacts: %d", telbook->filled_contacts);
	
	
	return (telbook->filled_contacts-1);
}


int upgrade_contact(TelephoneBook *telbook, char *firstname, char *lastname, char *patronymic,
char *new_firstname, char *new_lastname, char *new_patronymic, char *new_company, char *new_position,char *new_number, char *new_email, char *new_links)
{
	int found;
	found = search_contact(telbook, firstname, lastname, patronymic);
	
	
	if ( found == -1) return found;
	
	//сравнить строки, если строка такая же как "." тогда оставляем прежнее значение иначе вставляем новое итого  всё вместе - строка не равна . не равна - не равна
	if (telbook->filled_contacts < telbook->size_book)
	{
		if ( (strcmp ( new_firstname, ".")))strcpy(telbook->contacts[found].name_.firstname_, new_firstname);
		
		if ( (strcmp ( new_lastname, ".")))strcpy(telbook->contacts[found].name_.lastname_, new_lastname);
		
		if ( (strcmp ( new_patronymic, ".")))strcpy(telbook->contacts[found].name_.patronymic_, new_patronymic);
		
		if (  !(strcmp ( new_company, "-")) ) strcpy(telbook->contacts[found].dop_info_.company_, "");
		else if ( (strcmp ( new_company, ".")))strcpy(telbook->contacts[found].dop_info_.company_, new_company);
		
		if (  !(strcmp ( new_position, "-")) ) strcpy(telbook->contacts[found].dop_info_.position_, "");
		else if ( (strcmp ( new_position, ".")))strcpy(telbook->contacts[found].dop_info_.position_, new_position);
		
		if (  !(strcmp ( new_number, "-")) ) strcpy(telbook->contacts[found].dop_info_.number_, "");
		else if ( (strcmp ( new_number, ".")))strcpy(telbook->contacts[found].dop_info_.number_, new_number);
		
		if (  !(strcmp ( new_email, "-")) ) strcpy(telbook->contacts[found].dop_info_.email_, "");
		else if ( (strcmp ( new_email, ".")))strcpy(telbook->contacts[found].dop_info_.email_, new_email);
		
		if (  !(strcmp ( new_links, "-")) ) strcpy(telbook->contacts[found].dop_info_.links_, "");
		else if ( (strcmp ( new_links, ".")))strcpy(telbook->contacts[found].dop_info_.links_, new_links);
	}

	return found;
}


int show_contacts(TelephoneBook *telbook)
{
	if (telbook->filled_contacts == 0) return -1;
	for (int i=0, end = telbook->filled_contacts; i<end ; ++i)
	{
	printf("\nКонтакт: %d\n", i);
	printf("Имя контакта: %s\n", telbook->contacts[i].name_.firstname_);
	printf("Фамилия контакта: %s\n", telbook->contacts[i].name_.lastname_);
	printf("Отчество контакта: %s\n", telbook->contacts[i].name_.patronymic_);
	
	printf("Копания: %s\n", telbook->contacts[i].dop_info_.company_);
	printf("Позиция: %s\n", telbook->contacts[i].dop_info_.position_);
	printf("Номер: %s\n", telbook->contacts[i].dop_info_.number_);
	printf("Email: %s\n", telbook->contacts[i].dop_info_.email_);
	printf("Ссылки: %s\n", telbook->contacts[i].dop_info_.links_);
	}
	return 0;
}

//the function showing my capabilities to work with a function pointer
int funk_choose( int funk(TelephoneBook *, char *, char *, char *), TelephoneBook *telbook, int num_funk)
{
	int result;
	
	printf("Введите имя контакта: ");
	char *firstname = malloc (sizeof(char) * MAXLINE);
	scanf("%s", firstname);
	printf("Введите фамилию контакта: ");
	char *lastname = malloc (sizeof(char) * MAXLINE);
	scanf("%s", lastname);
	printf("Введите отчество контакта: ");
	char *patronymic = malloc (sizeof(char) * MAXLINE);
	scanf("%s", patronymic);
	
	
	result = funk(telbook, firstname, lastname, patronymic);
	
	
	free (firstname);
	free (lastname);
	free (patronymic);
	
	
	return result;
}


//исправлено считывание и записывание, было вся структура теперь каждый контакт по отдельности
int readData(char* filename, TelephoneBook *telbook)
{
	Contact temp_contact = {0};
	memset(telbook, 0, sizeof(TelephoneBook)); // Очистка структуры
	telbook->filled_contacts = 0;
	
	
	int fd = open(filename, O_RDONLY);
    if (fd == -1) return -1;
	
	
    // ssize_t bytesRead = read(fd, telbook, sizeof(TelephoneBook));
    // close(fd);


	while (read(fd, &temp_contact, sizeof(Contact)) > 0) 
	{
        if (telbook->filled_contacts < MAXTELBOOKSIZE) 
		{
            telbook->contacts[telbook->filled_contacts++] = temp_contact;
        } 
		else 
		{
            break; // Превышен лимит контактов
		}
    }
	telbook->size_book = MAXTELBOOKSIZE;
	
	close(fd);
	
	
	return 0;
}


int writeData(char* filename, TelephoneBook *telbook, int i)
{
	if ( i == 0 ) 
	{
		int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC | O_SYNC, 0744);
		if (fd == -1) return -1;
		
		
		for (int j = 0; j < telbook->filled_contacts; ++j) 
		{
			if (write(fd, &telbook->contacts[j], sizeof(Contact)) == -1) 
			{
				close(fd);
				return -1; // Ошибка записи
			}
		}
		close(fd);
	}
	else if ( i == 1 )
	{
		TelephoneBook telbook_temp;
		TelephoneBook * ptr_tel = &telbook_temp;
		
		int res = readData(filename, ptr_tel);
		if (res == -1) return -1;
		
		add_new_contacts(telbook, ptr_tel);
		writeData(filename, ptr_tel, 0);
	}
	
	
	return 0;
}


int add_new_contacts(TelephoneBook *telbook, TelephoneBook *temp)
{
	printf("До: ");
	for ( int a = 0; a < temp->filled_contacts; ++a) printf("%s %s %s - ", temp->contacts[a].name_.firstname_, temp->contacts[a].name_.lastname_, temp->contacts[a].name_.patronymic_);
	printf("\n");
	for ( int i = 0 ; i < telbook->filled_contacts; ++i)
	{
		printf("%d: %s %s %s - ", i, telbook->contacts[i].name_.firstname_, telbook->contacts[i].name_.lastname_, telbook->contacts[i].name_.patronymic_);
		int found = search_contact(temp, telbook->contacts[i].name_.firstname_, telbook->contacts[i].name_.lastname_, telbook->contacts[i].name_.patronymic_);
		if ( found == -1 )
		{
			printf("Такого у нас нет контакта");
			add_contact( temp, telbook->contacts[i].name_.firstname_, telbook->contacts[i].name_.lastname_, telbook->contacts[i].name_.patronymic_,  
			telbook->contacts[i].dop_info_.company_, telbook->contacts[i].dop_info_.position_, telbook->contacts[i].dop_info_.number_, 
			telbook->contacts[i].dop_info_.email_, telbook->contacts[i].dop_info_.links_);
		}
		else printf("Да: %s %s %s", temp->contacts[found].name_.firstname_, temp->contacts[found].name_.lastname_, temp->contacts[found].name_.patronymic_);
		printf("\n");
	}
	printf("После: ");
	for ( int a = 0; a < temp->filled_contacts; ++a) printf("%s %s %s - ", temp->contacts[a].name_.firstname_, temp->contacts[a].name_.lastname_, temp->contacts[a].name_.patronymic_);
	printf("\n");
}