/*******************************************************************************************************************************************************************
*Title			: Subtraction
*Description		: This function performs subtraction of two given large numbers and store the result in the resultant list.
*Prototype		: int subtraction(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR);
*Input Parameters	: head1: Pointer to the first node of the first double linked list.
			: tail1: Pointer to the last node of the first double linked list.
			: head2: Pointer to the first node of the second double linked list.
			: tail2: Pointer to the last node of the second double linked list.
			: headR: Pointer to the first node of the resultant double linked list.
*Output			: Status (SUCCESS / FAILURE)
*******************************************************************************************************************************************************************/
#include "apc.h"

int subtraction(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR)
{
	Dlist *t1 = *tail1, *t2 = *tail2, *tailR = NULL;
	int borrow = 0;

	if (compare_lists(*head1, *head2) < 0)
		return FAILURE;

	*headR = NULL;
	while (t1 || t2)
	{
		int diff = (t1 ? t1->data : 0) - borrow - (t2 ? t2->data : 0);
		if (diff < 0)
		{
			diff += BASE;
			borrow = 1;
		}
		else
			borrow = 0;

		if (insert_first(headR, &tailR, diff) == FAILURE)
			return FAILURE;

		if (t1)
			t1 = t1->prev;
		if (t2)
			t2 = t2->prev;
	}

	trim_leading_zero_nodes(headR, &tailR);
	return SUCCESS;
}
