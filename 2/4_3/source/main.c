#include <stdio.h>
#include <stdlib.h>
#include "TelBook.h"
#include <assert.h>
#define MAXCHARARRAY 30


void menu(node* head)
{
	int n;
	while (n!=0)
	{
		printf("Поиск элемента - 1\n");
		printf("Вставка элемента - 2\n");
		printf("Удаление элемента - 3\n");
		printf("Пустой ли список - 4\n");
		printf("Изменить элемент - 5\n");
		printf("Вывести элементы - 6\n");
		printf("Узнать родителя элемента - 7\n");
		printf("Вывести всю информацию о элементе - 8\n");
		printf("Введите операцию: ");
		scanf("%d", &n);
		printf("\n");
		
		char firstname [MAXCHARARRAY], lastname[MAXCHARARRAY], patronymic[MAXCHARARRAY];
		char  company[MAXCHARARRAY], position[MAXCHARARRAY], number[MAXCHARARRAY], email[MAXCHARARRAY], links[MAXCHARARRAY];
		node * Node;
		switch(n)
		{
			case(1): 
			{
				printf("Введите имя: ");
				scanf("%s", firstname);
				printf("Введите фамилию: ");
				scanf("%s", lastname);
				printf("Введите отчество: ");
				scanf("%s", patronymic);
				
				Node = find_contact(head, firstname, lastname, patronymic);
				printf("\n");
				if (Node != NULL) print_contact(Node);
				else printf("Эдемент не найден\n");
				printf("\n");
			break;
			}
			case(2):
			{
				printf("Введите имя: ");
				scanf("%s", firstname);
				printf("Введите фамилию: ");
				scanf("%s", lastname);
				printf("Введите отчество: ");
				scanf("%s", patronymic);
				printf("Введите компанию (здесь и далее '-' если хотите ввести пустую строку): ");
				scanf("%s", company);
				printf("Введите должность: ");
				scanf("%s", position);
				printf("Введите номер: ");
				scanf("%s", number);
				printf("Введите email: ");
				scanf("%s", email);
				printf("Введите ссылки: ");
				scanf("%s", links);
				
				Node = add_node (head, firstname, lastname, patronymic, company, position, number, email, links);
				if (Node != NULL)
				{
					printf("Контакт добавлен\n");
					head = Node;
				}
			break;
			}
			case (3):
			{
				printf("Введите имя: ");
				scanf("%s", firstname);
				printf("Введите фамилию: ");
				scanf("%s", lastname);
				printf("Введите отчество: ");
				scanf("%s", patronymic);
				head = delete_contact( head,  firstname, lastname, patronymic);
				Node = find_contact(head, firstname, lastname, patronymic);
				// if (Node != NULL) printf("\nЭлемент не удалён\n");
				// else printf("Элемент удалён\n");
				printf("\n");
			break;
			}
			case(4):
			{
				printf("Список пустой ? : %s\n", is_empty(head) == 0 ? "ДА" : "НЕТ");
			break;
			}
			case(5):
			{
				printf("Введите имя: ");
				scanf("%s", firstname);
				printf("Введите фамилию: ");
				scanf("%s", lastname);
				printf("Введите отчество: ");
				scanf("%s", patronymic);
				
				
				char firstname_new [MAXCHARARRAY], lastname_new[MAXCHARARRAY], patronymic_new[MAXCHARARRAY];
				char  company_new[MAXCHARARRAY], position_new[MAXCHARARRAY], number_new[MAXCHARARRAY], email_new[MAXCHARARRAY], links_new[MAXCHARARRAY];
				
				
				printf("Введите новое имя: ");
				scanf("%s", firstname_new);
				printf("Введите новую фамилию: ");
				scanf("%s", lastname_new);
				printf("Введите новое отчество: ");
				scanf("%s", patronymic_new);
				printf("Введите новую компанию (здесь и далее '-' если хотите ввести пустую строку): ");
				scanf("%s", company_new);
				printf("Введите новую должность: ");
				scanf("%s", position_new);
				printf("Введите новый номер: ");
				scanf("%s", number_new);
				printf("Введите новый email: ");
				scanf("%s", email_new);
				printf("Введите новые ссылки: ");
				scanf("%s", links_new);
				
				
				update_contact(head, firstname, lastname, patronymic, firstname_new, lastname_new, patronymic_new, company_new, position_new, position_new, email_new, links_new);
				printf("\n");
			break;
			}
			case(6):
			{
				print_bin_tree( head );
			break;
			}
			case(7):
			{
				// Конструкция для нахождения предка элемента
				printf("Введите имя: ");
				scanf("%s", firstname);
				printf("Введите фамилию: ");
				scanf("%s", lastname);
				printf("Введите отчество: ");
				scanf("%s", patronymic);
				
				node * temp = find_contact(head, firstname,  lastname, patronymic);
				Node = find_prev_node(head, temp);
				if (Node -> left_ == temp) printf("%s -> (левый) %s\n", Node->contact_->name_.lastname_, temp->contact_->name_.lastname_ );
				else if (Node -> right_ == temp) printf("%s -> (правый) %s\n", Node->contact_->name_.lastname_, temp->contact_->name_.lastname_ );
			break;
			}
			case(8):
			{
				printf("Введите имя: ");
				scanf("%s", firstname);
				printf("Введите фамилию: ");
				scanf("%s", lastname);
				printf("Введите отчество: ");
				scanf("%s", patronymic);
				
				
				Node = find_contact(head, firstname, lastname, patronymic);
				
				
				print_contact(Node);
				
				
			}
		}
	}
}
 int main ()
 {
	//добавить возможность добавлять и другие поля и сделать функцию вывода одного контакта и если сделать так чтобы если поле не заполнено тогда хранится "-" и если в поле хранится "-" тогда при выводе ничего не выводится
	//изменится add delete upgrade show
	
	
	char *firstname = "E";
	char *lastname = "Первый";
	char *patronymic = "E";
	char *company = "-";
	char *position= "-";
	char *number= "-";
	char *email= "fvgu";
	char *links= "-";
	node * head = add_node(NULL, firstname, lastname, patronymic, company, position, number, email, links );
	
	printf("Добавление контактов:\n");
	printf("Добавлен 1-ый контакт\n");
	print_bin_tree(head);
	
	char *company_1 = "-";
	char *position_1 = "-";
	char *number_1 = "-";
	char *email_1 = "-";
	char *links_1 = "-";
	
	head = add_node( head, "Nikita", "Второй", "Sergeevich", company_1, position_1, number_1, email_1, links_1 );
	printf("Добавлен 2-ой контакт\n");
	print_bin_tree(head);
	
	head = add_node( head, "Kristina", "Шесть", "Rvfsv", "eltex", "-", "-", "-", "-" );
	printf("Добавлен 3-ий контакт\n");
	print_bin_tree(head);

	
	char *firstname_2 = "N";
	char *lastname_2 = "Третий";
	char *patronymic_2 = "R";
	
	head = add_node( head, firstname_2, lastname_2, patronymic_2, "Roskosmos", "Prisident", "-", "-", "-" );
	
	
	printf("Добавлен 4-ый контакт\n");
	print_bin_tree( head );
	
	
	firstname_2 = "N";
	lastname_2 = "Рука";
	patronymic_2 = "R";
	
	head = add_node( head, firstname_2, lastname_2, patronymic_2, "-", "-", "-", "-", "-" );
	
	printf("Добавлен 5-ый контакт\n");
	print_bin_tree( head );
	
	
	head = add_node( head, firstname_2, "Траааа" , patronymic_2, "-", "-", "89236888777", "-", "-" );
	
	printf("Добавлен  контакт\n");
	print_bin_tree( head );
	
	
	head = add_node( head, "N", "Фух", "jnddn", "-", "-", "-", "-", "-" );
	printf("Добавлен 6-ой контакт\n");
	print_bin_tree( head );
	
	
	head = add_node( head, "Nvver", "Ура", "Rvnae", "-", "-", "-", "-", "-" );
	
	
	printf("Добавлен 7-ой контакт\n");
	print_bin_tree( head );
	
	
	printf("Удаление контактов:\n");
	head = delete_contact( head,  firstname, lastname, patronymic);
	
	
	print_bin_tree( head );
	
	
	printf("Обновление контактов:\n");
	head = update_contact( head,  "N", "Рука" , "R", "N", "Ручка", "R", "rv", "vevev", "vevv", "vrvervrv", "rverevrerv");
	print_bin_tree( head );
	
	// Конструкция для нахождения предка элемента
	// node * temp = find_contact(head, firstname_3,  "Ручка", patronymic_3);
	// printf("%s", temp->contact_->name_.lastname_);
	// temp = find_prev_node(head, temp);
	// printf("<-%s\n", temp->contact_->name_.lastname_);
	
	
	menu(head);
	return 0;
}

