#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
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
typedef struct node
{
	Contact *contact_;
	struct node *left_;
	struct node *right_;
} node;


bool is_empty(node * temp);

node* add_node (node *head, char *firstname, char *lastname, char *patronymic, char *company, char *position,char *number, char *email, char *links);

node* find_contact(node * Head, char *firstname, char *lastname, char *patronymic);
node* find_child(node * Node);
node* find_prev_node(node* head, node * Node);

node* delete_contact(node * Head, char *firstname, char *lastname, char *patronymic);
node* update_contact(node * Head, char *firstname, char *lastname, char *patronymic, char *firstname_new, char *lastname_new,
 char *patronymic_new, char *company_new, char *position_new, char *number_new, char *email_new, char *links_new);
 
void print_contact(node *Node);
void print_bin_tree(node *head);
//node * add_contact ( node * Head, char *firstname, char *lastname, char *patronymic, char *company, char *position,char *number, char *email, char *links);
