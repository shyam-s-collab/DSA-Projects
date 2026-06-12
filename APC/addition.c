/*******************************************************************************************************************************************************************
*Name    :Shyam S Suryavanshi
*Project :APC
*Title			: Addition
*Description		: This function performs addition of two given large numbers and store the result in the resultant list.
*Prototype		: int addition(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR);
*Input Parameters	: head1: Pointer to the first node of the first double linked list.
			: tail1: Pointer to the last node of the first double linked list.
			: head2: Pointer to the first node of the second double linked list.
			: tail2: Pointer to the last node of the second double linked list.
			: headR: Pointer to the first node of the resultant double linked list.
*Output			: Status (SUCCESS / FAILURE)
*******************************************************************************************************************************************************************/
#include "apc.h"

int addition(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR)
{
	Dlist *t1 = *tail1, *t2 = *tail2, *tailR = NULL;
	int carry = 0;

	(void)head1;
	(void)head2;
	*headR = NULL;
	while (t1 || t2 || carry)
	{
		int sum = carry;
		if (t1)
		{
			sum += t1->data;
			t1 = t1->prev;
		}
		if (t2)
		{
			sum += t2->data;
			t2 = t2->prev;
		}
		carry = sum / BASE;
		if (insert_first(headR, &tailR, sum % BASE) == FAILURE)
			return FAILURE;
	}

	trim_leading_zero_nodes(headR, &tailR);
	return SUCCESS;
}
