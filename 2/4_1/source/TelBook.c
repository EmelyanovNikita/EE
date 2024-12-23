#include "TelBook.h"


node* add_node(node *head, char *firstname, char *lastname, char *patronymic, char *company, char *position,char *number, char *email, char *links)
{
	node *temp_head = head;
	node *new_node = find_contact(temp_head, firstname, lastname, patronymic);
	if( new_node != NULL ) //если контакт существует вернётся указатель на него из find и мы его вернём дальше
	{
		printf("Контакт уже существует");
		return new_node;
	}
	
	new_node = (node *) malloc (sizeof(node)); //если контакт  не существует вернётся 0 и мы можем выделить память и создать новый элемент
	new_node -> contact_ = (Contact*) malloc (sizeof(Contact));
	
	strcpy(new_node -> contact_->name_.firstname_, firstname);
	strcpy(new_node -> contact_->name_.lastname_, lastname);
	strcpy(new_node -> contact_->name_.patronymic_, patronymic);
	
	// strcpy(new_node -> contact_->dop_info_.company_, company);
	// strcpy(new_node -> contact_->dop_info_.position_, position);
	// strcpy(new_node -> contact_->dop_info_.number_, number);
	// strcpy(new_node -> contact_->dop_info_.email_, email);
	// strcpy(new_node -> contact_->dop_info_.links_, links);
	
	if (  !(strcmp ( company, "-")) ) strcpy(new_node -> contact_->dop_info_.company_, "");
	else strcpy(new_node -> contact_->dop_info_.company_, company);
	
	if (  !(strcmp ( position, "-")) ) strcpy(new_node -> contact_->dop_info_.position_, "");
	else strcpy(new_node -> contact_->dop_info_.position_, position);
	
	if (  !(strcmp ( number, "-")) ) strcpy(new_node -> contact_->dop_info_.number_, "");
	else strcpy(new_node -> contact_->dop_info_.number_, number);
	
	if (  !(strcmp ( email, "-")) ) strcpy(new_node -> contact_->dop_info_.email_, "");
	else strcpy(new_node -> contact_->dop_info_.email_, email);
	
	if (  !(strcmp ( links, "-")) ) strcpy(new_node -> contact_->dop_info_.links_, "");
	else strcpy(new_node -> contact_->dop_info_.links_, links);
	
	
	if (temp_head==NULL)
	{
		new_node -> prev_ = NULL;
		new_node -> next_ = NULL;
		return new_node;
	}
	else 
	{
		while( temp_head!=NULL )
		{
			if (  temp_head->next_ == NULL && strcmp(new_node -> contact_->name_.lastname_, temp_head -> contact_->name_.lastname_) >= 0 ) //если имя у текущей головы меньше чем твоё 
			{ //а следующее NULL тогда становимся новым next
				temp_head->next_ = new_node;
				new_node -> prev_ = temp_head;
				new_node -> next_ = NULL;
				return head;
			}
			else if ( temp_head->prev_ == NULL && strcmp(new_node -> contact_->name_.lastname_, temp_head -> contact_->name_.lastname_) <= 0 )//если имя у текущей головы больше чем твоё 
			{ //а предок NULL (первый элемент) тогда становимся новой головой
				temp_head -> prev_ = new_node;
				new_node -> next_ = temp_head;
				new_node -> prev_ = NULL;
				return new_node;
			}			
			else if ( strcmp(new_node -> contact_->name_.lastname_, temp_head -> contact_->name_.lastname_) >= 0 && //если имя у текущей головы меньше чем твоё 
			strcmp(new_node -> contact_->name_.lastname_, temp_head -> next_ -> contact_-> name_.lastname_) <= 0 ) // а у следующей больше тогда встаём между ними
			{
				temp_head -> next_ -> prev_ = new_node;
				new_node -> prev_ = temp_head; 
				new_node -> next_ = temp_head -> next_;
				temp_head -> next_ = new_node;
				return head;
			}
			temp_head = temp_head -> next_;
		}
	}
	
	
	while( new_node->prev_ != NULL ) new_node = new_node->prev_;
	
	
	return new_node;
}


bool is_empty(node * temp)
{
	while (temp != NULL)
	{
		if ( strlen (temp->contact_->name_.firstname_) != 0) return false;
		temp = temp->next_;
	}
	return true;
}


node* find_contact(node * Head, char *firstname, char *lastname, char *patronymic)
{
	if (Head == NULL || is_empty(Head)) return 0;
	
	while(Head!= NULL)
	{
		if ( (strcmp(Head->contact_->name_.lastname_, lastname)) == 0 && (strcmp(Head->contact_->name_.firstname_, firstname)) == 0 
		&& (strcmp(Head->contact_->name_.patronymic_, patronymic)) == 0) return Head;
		Head = Head->next_;
	}
	return 0;
}


void print_contact(node *Node)
{
	if (Node == NULL) return ;
	printf("ФИО: %s %s %s\n", Node -> contact_ -> name_.firstname_, Node -> contact_ -> name_.lastname_, Node -> contact_ -> name_.patronymic_);
	printf("Доп. Информация:\n");
	printf("Компания: %s\n", Node->contact_->dop_info_.company_);
	printf("Должность: %s\n", Node->contact_->dop_info_.position_);
	printf("Номер: %s\n", Node->contact_->dop_info_.number_);
	printf("Email: %s\n", Node->contact_->dop_info_.email_);
	printf("Ссылки: %s\n",Node->contact_->dop_info_.links_);
}

void print_list(node *head)
{
	while(head != NULL)
	{
		printf("%s %s %s -> ", head -> contact_ -> name_.firstname_, head -> contact_ -> name_.lastname_, head -> contact_ -> name_.patronymic_);
		if (head -> next_ == NULL ) printf("NULL\n");
		head = head -> next_;
	}
	
}


node* delete_contact(node * head, char *firstname, char *lastname, char *patronymic)
{
	node *temp_head = head;
	node *del_node = find_contact(temp_head, firstname, lastname, patronymic);
	
	if( del_node == NULL ) //если контакт существует вернётся указатель на него из find и мы его вернём дальше
	{
		printf("Контакта нет");
		return NULL;
	}
	
	
	if (del_node -> prev_ ==NULL && del_node -> next_ ==NULL)
	{
		free(del_node -> contact_);
		free(del_node);
		return NULL;
	}
	else if ( del_node -> prev_ == NULL)
	{
		del_node -> next_ -> prev_ = NULL;
		temp_head = del_node -> next_ ;
	}
	else if ( del_node -> next_ == NULL)
	{
		del_node -> prev_ -> next_ = NULL;
	}
	else 
	{
		del_node -> prev_ -> next_ = del_node -> next_;
		del_node -> next_ -> prev_ = del_node -> prev_;
		
	}
	
	free(del_node -> contact_);
	free(del_node);
	return temp_head;
	
	
}


node* update_contact(node * Head, char *firstname, char *lastname, char *patronymic, char *firstname_new, char *lastname_new, 
char *patronymic_new, char *company_new, char *position_new, char *number_new, char *email_new, char *links_new)
{
	Head = delete_contact(Head, firstname, lastname, patronymic);
	Head = add_node(Head, firstname_new, lastname_new, patronymic_new, company_new, position_new, number_new, email_new, links_new);
	return Head;
}