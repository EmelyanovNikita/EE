#include "TelBook.h"


node* add_node(node *head, char *firstname, char *lastname, char *patronymic, char *company, char *position,char *number, char *email, char *links)
{
	node *temp_head = head;
	node *new_node = find_contact(temp_head, firstname, lastname, patronymic);
	
	if( new_node != NULL ) //если контакт существует вернётся указатель на него из find и мы его вернём дальше
	{
		printf("Контакт уже существует");
		return NULL;
	}
	
	new_node = (node *) malloc (sizeof(node)); //если контакт  не существует вернётся 0 и мы можем выделить память и создать новый элемент
	new_node -> contact_ = (Contact*) malloc (sizeof(Contact));
	
	strcpy(new_node -> contact_->name_.firstname_, firstname);
	strcpy(new_node -> contact_->name_.lastname_, lastname);
	strcpy(new_node -> contact_->name_.patronymic_, patronymic);
	
	
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
		new_node -> left_ = NULL;
		new_node -> right_ = NULL;
		return new_node;
	}
	else 
	{
		while( temp_head!=NULL )
		{
			if (  strcmp( new_node -> contact_-> name_.lastname_, temp_head -> contact_->name_.lastname_) <= 0 ) //если новая нода меньше или равна чем текущая тога влево
			{
				if (temp_head -> left_ == NULL) // если левый пустой тогда становимся им 
				{
					temp_head -> left_ = new_node;
					new_node -> left_ = NULL;
					new_node -> right_ = NULL;
					return head;
				}
				temp_head = temp_head ->left_;
				
			}
			else if (  strcmp( new_node -> contact_-> name_.lastname_, temp_head -> contact_->name_.lastname_) > 0 ) //если новая нода больше чем текущая тогда вправо
			{
				if (temp_head -> right_ == NULL) // если правый пустой тогда становимся им 
				{
					temp_head -> right_ = new_node;
					new_node -> left_ = NULL;
					new_node -> right_ = NULL;
					return head;
				}
				temp_head = temp_head ->right_;
			}
		}
	}
	
	
	return head;
}


bool is_empty(node * temp)
{
	
	while (temp != NULL)
	{
		if ( strlen (temp->contact_->name_.firstname_) != 0) return false;
		if (temp->left_ == NULL && temp->right_ == NULL) return true;
		else if (temp->left_ == NULL) temp = temp->right_;
		else if (temp->right_ == NULL) temp = temp->left_;
		
	}
	return true;
}


node* find_contact(node * Head, char *firstname, char *lastname, char *patronymic)
{
	if (Head == NULL || is_empty(Head)) 
	{
		return 0;
	}
	
	while(Head!= NULL)
	{
		if ( (strcmp(Head->contact_->name_.lastname_, lastname)) == 0 && (strcmp(Head->contact_->name_.firstname_, firstname)) == 0 
		&& (strcmp(Head->contact_->name_.patronymic_, patronymic)) == 0) 
		{
			return Head;
		}
		if( (strcmp(lastname, Head->contact_->name_.lastname_)) <= 0 ) 
		{
			Head = Head->left_;
		}
		else if( (strcmp(lastname, Head->contact_->name_.lastname_)) > 0 ) 
		{
			Head = Head->right_;
		}
	}
	return 0;
}


node* find_child(node * Node) // удаляемой ноды ищем новую node-потомка
{
	if (Node == NULL) return NULL;
	
	
	if (Node -> left_ == NULL && Node -> right_ == NULL)  return NULL; //если оба потомка NULL тогда и верём NULL
	else if (Node -> left_ != NULL && Node -> right_ == NULL) return Node -> left_; //если левый есть а правого нет тогда возвращаем его 
	else if (Node -> right_ != NULL && Node -> left_ == NULL) return Node -> right_; //если правый есть а левого нет тогда возвращаем его
	else // если есть оба потомка
	{
		if(Node -> right_ -> left_ == NULL ) // если у правого потомка нет младшего (или нет вообще никакого) возвращает (этого же) правого потомка
		{
			return Node -> right_;
		}
		else //если у правого потомка есть младший всё же ищем самого младшего из них
		{
			node* temp = Node -> right_ -> left_;
			while (temp -> left_ != NULL)  temp = temp -> left_;
			return temp;
			
		}
	}
}


node* find_prev_node(node* head, node * Node)
{
	if (head == NULL || is_empty(head) || Node == NULL) 
	{
		return 0;
	}
	
	node* prev_node = head;
while(prev_node -> left_ != NULL || prev_node -> right_ != NULL) //поиск предка целевой ноды
	{
		if ( prev_node ->left_ != NULL && ((strcmp(prev_node->left_->contact_->name_.lastname_, Node->contact_->name_.lastname_)) == 0 && (strcmp(prev_node->left_->contact_->name_.firstname_, Node->contact_->name_.firstname_)) == 0 
		&& (strcmp(prev_node->left_->contact_->name_.patronymic_, Node->contact_->name_.patronymic_)) == 0) )
		{
			return prev_node;
		} 
		if ( prev_node ->right_ != NULL && ((strcmp(prev_node->right_->contact_->name_.lastname_, Node->contact_->name_.lastname_)) == 0 && (strcmp(prev_node->right_->contact_->name_.firstname_, Node->contact_->name_.firstname_)) == 0 
		&& (strcmp(prev_node->right_->contact_->name_.patronymic_, Node->contact_->name_.patronymic_)) == 0) ) 
		{
			return prev_node;
		}
		if( (strcmp(Node->contact_->name_.lastname_, prev_node->contact_->name_.lastname_)) <= 0 ) 
		{
			prev_node = prev_node->left_;
		}
		else if( (strcmp(Node->contact_->name_.lastname_, prev_node->contact_->name_.lastname_)) > 0 ) 
		{
			prev_node = prev_node->right_;
		}
	}
	return 0;
}


node* delete_contact(node * head, char *firstname, char *lastname, char *patronymic)
{
	node *temp_head = head; //сохраним голову чтобы вернуть потом, использоваь будем вреемнную голову
	node *del_node = find_contact(temp_head, firstname, lastname, patronymic); //найдёи удаляемую node
	node* new_node; //и поставим указатель на новую node
	node* prev_node; //преллылущая нода чтобы изменить у неё указатель
	
	
	if( del_node == NULL ) //если контакт не существует вернётся NULL
	{
		printf("Контакта нет");
		return NULL;
	}
	
	
	new_node = find_child(del_node); //нашли новую node
	prev_node = find_prev_node(head, del_node); // нашли предыдущую ноду
	if( new_node == NULL && del_node == head) //если корень без потомков просто удаляем его
	{
		free( del_node -> contact_);
		free( del_node );
		return NULL;
	}
	else if(del_node == head) //если корень с потомками
	{
		node* prev_new_node = find_prev_node(head, new_node); // нашли потомка новой node
		
		
		if(prev_new_node -> left_ == new_node ) // если у новой ноды есть предки надо или занулить у них указатели или перевязать единственного возможного потомка
		{
			if(new_node -> right_ != NULL) prev_new_node -> left_ = new_node -> right_;
			else  prev_new_node -> left_ = NULL;
		}
		if(prev_new_node -> right_ == new_node )
		{
			if(new_node -> right_ != NULL) prev_new_node -> right_ = new_node -> right_;
			else  prev_new_node -> right_ = NULL;
		}
		
		
		new_node -> left_ = del_node -> left_; //нашли новую node и теперь присваиваем ей потомков от старой ноды
		new_node -> right_ = del_node -> right_; //нашли новую node и теперь присваиваем ей потомков от старой ноды
		
		
		free( del_node -> contact_);
		free( del_node );
		
		
		return new_node; // так как она теперь голова
	}
	
	else if (new_node == NULL)
	{
		if(del_node == prev_node -> left_) prev_node -> left_ = NULL; //у предка зануляем удалённого потомка
		else if (del_node == prev_node -> right_) prev_node -> right_ = NULL;
	}
	else
	{
		node* prev_new_node = find_prev_node(head, new_node); // нашли потомка новой node
		
		
		if(prev_new_node -> left_ == new_node ) // если у новой ноды есть предки надо или занулить у них указатели или перевязать единственного возможного потомка
		{
			if(new_node -> right_ != NULL) prev_new_node -> left_ = new_node -> right_;
			else  prev_new_node -> left_ = NULL;
		}
		if(prev_new_node -> right_ == new_node )
		{
			if(new_node -> right_ != NULL) prev_new_node -> right_ = new_node -> right_;
			else  prev_new_node -> right_ = NULL;
		}
		
		
		new_node -> left_ = del_node -> left_; //нашли новую node и теперь присваиваем ей потомков от старой ноды
		new_node -> right_ = del_node -> right_; //нашли новую node и теперь присваиваем ей потомков от старой ноды
		
		
		if(del_node == prev_node -> left_) prev_node -> left_ = new_node;
		else if (del_node == prev_node -> right_) prev_node -> right_ = new_node;
	}
	
	
	free(del_node -> contact_);
	free(del_node);
	
	
	return head;
}


node* update_contact(node * Head, char *firstname, char *lastname, char *patronymic, char *firstname_new, char *lastname_new, 
char *patronymic_new, char *company_new, char *position_new, char *number_new, char *email_new, char *links_new)
{
	node *update_node = find_contact(Head, firstname, lastname, patronymic);
	if( update_node == NULL ) //если контакт не существует вернётся NULL
	{
		printf("Контакта нет");
		return NULL;
	}
	
	
	if( (strcmp (firstname, firstname_new) == 0) && (strcmp (lastname, lastname_new) == 0) && (strcmp (patronymic, patronymic_new)  == 0) ) 
	{
		strcpy(update_node->contact_->dop_info_.company_, company_new);
		strcpy(update_node->contact_->dop_info_.position_, position_new);
		strcpy(update_node->contact_->dop_info_.number_, number_new);
		strcpy(update_node->contact_->dop_info_.email_, email_new);
		strcpy(update_node->contact_->dop_info_.links_, links_new);
		return Head;
	}
	Head = delete_contact(Head, firstname, lastname, patronymic);
	Head = add_node(Head, firstname_new, lastname_new, patronymic_new, company_new, position_new, number_new, email_new, links_new);
	return Head;
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

void print_bin_tree(node *head)
{
	if (head == NULL) return ;
	
	print_contact(head);
	printf("\n");
	if (head -> left_ != NULL) 
	{
		printf("Левый потомок\n");
		print_bin_tree(head -> left_);
	}
	if (head -> right_ != NULL)
	{
		printf("Правый потомок\n");
		print_bin_tree(head -> right_);
	}
	
}