#include <stdio.h>
#include <stdlib.h>
#include "TelBook.h"


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

