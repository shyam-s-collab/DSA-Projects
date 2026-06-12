#include "apc.h"

int insert_first(Dlist **head, Dlist **tail, data_t data)
{
	Dlist *new = malloc(sizeof(Dlist));
	if (new == NULL)
		return FAILURE;

	new->prev = NULL;
	new->data = data;
	new->next = *head;

	if (*head)
		(*head)->prev = new;
	else
		*tail = new;

	*head = new;
	return SUCCESS;
}

int insert_last(Dlist **head, Dlist **tail, data_t data)
{
	Dlist *new = malloc(sizeof(Dlist));
	if (new == NULL)
		return FAILURE;

	new->prev = *tail;
	new->data = data;
	new->next = NULL;

	if (*tail)
		(*tail)->next = new;
	else
		*head = new;

	*tail = new;
	return SUCCESS;
}

void free_list(Dlist **head, Dlist **tail)
{
	Dlist *temp = *head;
	while (temp)
	{
		Dlist *next = temp->next;
		free(temp);
		temp = next;
	}
	*head = NULL;
	*tail = NULL;
}

void trim_leading_zero_nodes(Dlist **head, Dlist **tail)
{
	while (*head && (*head)->data == 0 && (*head)->next)
	{
		Dlist *temp = *head;
		*head = (*head)->next;
		(*head)->prev = NULL;
		free(temp);
	}

	if (*head == NULL)
		*tail = NULL;
}

void print_list(Dlist *head)
{
	if (head == NULL)
	{
		printf("0");
		return;
	}

	printf("%d", head->data);
	for (head = head->next; head; head = head->next)
		printf("%0*d", WIDTH, head->data);
}

int create_list_from_digits(const char *digits, Dlist **head, Dlist **tail)
{
	size_t len, first, i;

	*head = NULL;
	*tail = NULL;

	while (*digits == '0' && digits[1] != '\0')
		digits++;

	len = strlen(digits);
	first = len % WIDTH;
	if (first == 0)
		first = WIDTH;

	for (i = 0; i < len; )
	{
		char chunk[WIDTH + 1];
		size_t take = (i == 0) ? first : WIDTH;
		memcpy(chunk, digits + i, take);
		chunk[take] = '\0';
		if (insert_last(head, tail, atoi(chunk)) == FAILURE)
			return FAILURE;
		i += take;
	}

	if (*head == NULL)
		return insert_last(head, tail, 0);

	return SUCCESS;
}

int compare_lists(Dlist *head1, Dlist *head2)
{
	int len1 = 0, len2 = 0;
	Dlist *temp;

	for (temp = head1; temp; temp = temp->next)
		len1++;
	for (temp = head2; temp; temp = temp->next)
		len2++;

	if (len1 != len2)
		return (len1 > len2) ? 1 : -1;

	while (head1 && head2)
	{
		if (head1->data != head2->data)
			return (head1->data > head2->data) ? 1 : -1;
		head1 = head1->next;
		head2 = head2->next;
	}

	return 0;
}
