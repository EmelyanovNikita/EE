#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#define MAXLINE 30 /* максимальный размер вводимой строки */
#define MAXTELBOOKSIZE 50 /* максимальный размер вводимой строки */

// эта информация должна быть заполнена
typedef struct HumanName {
	char firstname_[MAXLINE];
	char lastname_[MAXLINE];
	char patronymic_[MAXLINE];
}HumanName;
// эта информация не обязательно должна быть заполнена
typedef struct HumanInfo {
	char company_[MAXLINE];
	char position_[MAXLINE];
	char number_[MAXLINE];
	char email_[MAXLINE];
	char links_[MAXLINE];
}HumanInfo;
//контакт это единица телефонной книги 
typedef struct Contact
{
	HumanName name_;
	HumanInfo dop_info_;
} Contact;
// телефонная книга это массив контактов (указатель для которого будет выделена память)
typedef struct TelephoneBook
{
	Contact contacts[MAXTELBOOKSIZE];
	int size_book;
	int filled_contacts;
} TelephoneBook;


bool is_empty(TelephoneBook* telbook);
int search_contact(TelephoneBook *telbook, char *firstname, char *lastname, char *patronymic);
int delete_contact(TelephoneBook *telbook, char *firstname, char *lastname, char *patronymic);
int add_contact(TelephoneBook *telbook, char *firstname, char *lastname, char *patronymic, char *company, char *position,char *number, char *email, char *links);
int upgrade_contact(TelephoneBook *telbook, char *firstname, char *lastname, char *patronymic,
char *new_firstname, char *new_lastname, char *new_patronymic, char *new_company, char *new_position,char *new_number, char *new_email, char *new_links);
int show_contacts(TelephoneBook *telbook);
int funk_choose( int funk(TelephoneBook *, char *, char *, char *), TelephoneBook *telbook, int num_funk);
int readData(char* filename, TelephoneBook *telbook);
int writeData(char* filename, TelephoneBook *telbook, int i);
int add_new_contacts(TelephoneBook* telbook, TelephoneBook* temp);