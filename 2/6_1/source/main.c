#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#define MAXLINE 30 /* максимальный размер вводимой строки */
#define MAXTELBOOKSIZE 50 /* максимальный размер вводимой строки */


//объявили структуры и сигнатуры фукнкций
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
typedef struct node
{
	Contact *contact_;
	struct node *prev_;
	struct node *next_;
} node;
node* add_node (node *head, char *firstname, char *lastname, char *patronymic, char *company, char *position,char *number, char *email, char *links);
bool is_empty(node * temp);
node* find_contact(node * Head, char *firstname, char *lastname, char *patronymic);
void print_list(node *head);
node* delete_contact(node * Head, char *firstname, char *lastname, char *patronymic);
node* update_contact(node * Head, char *firstname, char *lastname, char *patronymic, char *firstname_new, char *lastname_new,
 char *patronymic_new, char *company_new, char *position_new, char *number_new, char *email_new, char *links_new);
 

 int main ()
 {
	//добавить возможность добавлять и другие поля и сделать функцию вывода одного контакта и если сделать так чтобы если поле не заполнено тогда хранится "-" и если в поле хранится "-" тогда при выводе ничего не выводится
	//изменится add delete upgrade show
	
	
	char *firstname = "N";
	char *lastname = "Первый";
	char *patronymic = "S";
	char *company = "-";
	char *position= "-";
	char *number= "-";
	char *email= "-";
	char *links= "-";
	node * head = add_node(NULL, firstname, lastname, patronymic, company, position, number, email, links );
	
	printf("Добавление контактов с упорядочеванием:\n");
	print_list(head);
	
	char *firstname_1 = "E";
	char *lastname_1 = "Второй";
	char *patronymic_1 = "C";
	head = add_node( head, firstname_1, lastname_1, patronymic_1, company, position, number, email, links );
	
	print_list(head);
	
	
	char *firstname_2 = "N";
	char *lastname_2 = "Третий";
	char *patronymic_2 = "R";
	head = add_node( head, firstname_2, lastname_2, patronymic_2, company, position, number, email, links );
	
	print_list(head);
	
	
	firstname_2 = "N";
	lastname_2 = "Рука";
	patronymic_2 = "R";
	
	head = add_node( head, firstname_2, lastname_2, patronymic_2, company, position, number, email, links );
	print_list( head);
	
	firstname = "DA";
	head = add_node(head, firstname, lastname, patronymic, company, position, number, email, links );
	print_list(head);
	
	
	printf("is empty: %s\n", is_empty( head) ? "true" : "false");
	
	printf("\nИзменение контактов с упорядочеванием, меняем контакт:\n");
	
	node * Node = find_contact( head, firstname_1, lastname_1, patronymic_1);
	
	if (Node == NULL) printf("Контакт не найден\n");
	else 
	{
		printf("Имя контакта: %s\n", Node->contact_ -> name_.firstname_);
		printf("Фамилия контакта: %s\n", Node->contact_ -> name_.lastname_);
		printf("Отчество контакта: %s\n", Node->contact_ -> name_.patronymic_);
	}
	
	char *firstname_i = "NEw_name";
	char *lastname_i = "Четвёртый";
	char *patronymic_i = "Sergey";
	char *company_i = "-";
	char *position_i= "-";
	char *number_i= "-";
	char *email_i= "-";
	char *links_i= "-";
	
	head = update_contact(head, firstname_1, lastname_1, patronymic_1, firstname_i, lastname_i, patronymic_i, company_i, position_i, number_i, email_i, links_i);
	
	printf("Измененный контакт:\n");
	Node = find_contact( head, firstname_i, lastname_i, patronymic_i);
	if (Node == NULL) printf("Контакт не найден\n");
	else 
	{
		printf("Имя контакта: %s\n", Node->contact_ -> name_.firstname_);
		printf("Фамилия контакта: %s\n", Node->contact_ -> name_.lastname_);
		printf("Отчество контакта: %s\n", Node->contact_ -> name_.patronymic_);
	}
	
	printf("Все контакты:\n");
	print_list(head);
	
	printf("\nПоиска контакта:\n");
	Node = find_contact( head, firstname, lastname, patronymic);
	
	if (Node == NULL) printf("КОнтакт не найден\n");
	else 
	{
		printf("Имя контакта: %s\n", Node->contact_ -> name_.firstname_);
		printf("Фамилия контакта: %s\n", Node->contact_ -> name_.lastname_);
		printf("Отчество контакта: %s\n", Node->contact_ -> name_.patronymic_);
	}
	
	printf("\nУдаление контактов :\n");
	head = delete_contact(head, firstname_i, lastname_i, patronymic_i);
	print_list( head);
	
	head = delete_contact(head, firstname, lastname, patronymic);
	print_list( head);
	
	firstname_2 = "N";
	lastname_2 = "Третий";
	patronymic_2 = "R";
	
	head = delete_contact(head, firstname_2, lastname_2, patronymic_2);
	
	print_list(head);
	
	firstname_2 = "N";
	lastname_2 = "Рука";
	patronymic_2 = "R";
	
	head = delete_contact(head, firstname_2, lastname_2, patronymic_2);
	print_list( head);
	
	firstname = "N";
	head = delete_contact(head, firstname, lastname, patronymic);
	print_list( head);
	//strcmp(new_node -> contact_->name_.lastname_, temp_head -> prev_ -> contact_-> name_.lastname_)
	
	
	printf("is empty: %s\n", is_empty( head) ? "true" : "false");
	
	
}

