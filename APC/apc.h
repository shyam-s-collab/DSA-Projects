#ifndef APC_H
#define APC_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE -1
#define BASE 10000
#define WIDTH 4

typedef int data_t;
typedef struct node
{
	struct node *prev;
	data_t data;
	struct node *next;
}Dlist;

/* Include the prototypes here */
int insert_first(Dlist **head, Dlist **tail, data_t data);
int insert_last(Dlist **head, Dlist **tail, data_t data);
void free_list(Dlist **head, Dlist **tail);
void print_list(Dlist *head);
int create_list_from_digits(const char *digits, Dlist **head, Dlist **tail);
int compare_lists(Dlist *head1, Dlist *head2);
void trim_leading_zero_nodes(Dlist **head, Dlist **tail);

int addition(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR);
int subtraction(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR);
int multiplication(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR);
int division(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR);
#endif
