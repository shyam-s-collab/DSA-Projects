/*******************************************************************************************************************************************************************
*Title			: Division
*Description		: This function performs division of two given large numbers and store the result in the resultant list.
*Prototype		: int division(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR);
*Input Parameters	: head1: Pointer to the first node of the first double linked list.
			: tail1: Pointer to the last node of the first double linked list.
			: head2: Pointer to the first node of the second double linked list.
			: tail2: Pointer to the last node of the second double linked list.
			: headR: Pointer to the first node of the resultant double linked list.
*Output			: Status (SUCCESS / FAILURE)
*******************************************************************************************************************************************************************/
#include "apc.h"

static int multiply_list_by_int(Dlist *head, int num, Dlist **headR, Dlist **tailR)
{
	Dlist *tail = NULL;
	long long carry = 0;

	*headR = NULL;
	*tailR = NULL;
	for (tail = head; tail && tail->next; tail = tail->next)
		;

	while (tail || carry)
	{
		long long prod = carry;
		if (tail)
		{
			prod += (long long)tail->data * num;
			tail = tail->prev;
		}
		carry = prod / BASE;
		if (insert_first(headR, tailR, (int)(prod % BASE)) == FAILURE)
			return FAILURE;
	}

	if (*headR == NULL)
		return insert_last(headR, tailR, 0);

	trim_leading_zero_nodes(headR, tailR);
	return SUCCESS;
}

static int append_remainder_chunk(Dlist **head, Dlist **tail, int data)
{
	if (*head && (*head)->data == 0 && (*head)->next == NULL)
	{
		(*head)->data = data;
		return SUCCESS;
	}
	return insert_last(head, tail, data);
}

int division(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR)
{
	Dlist *remH = NULL, *remT = NULL, *resT = NULL;
	Dlist *prodH = NULL, *prodT = NULL, *newRemH = NULL, *newRemT = NULL;
	Dlist *cur;

	(void)tail1;
	*headR = NULL;

	if ((*head2)->data == 0 && (*head2)->next == NULL)
		return FAILURE;

	if (create_list_from_digits("0", &remH, &remT) == FAILURE)
		return FAILURE;

	for (cur = *head1; cur; cur = cur->next)
	{
		int low = 0, high = BASE - 1, qdigit = 0;

		if (append_remainder_chunk(&remH, &remT, cur->data) == FAILURE)
			return FAILURE;
		trim_leading_zero_nodes(&remH, &remT);

		while (low <= high)
		{
			int mid = low + (high - low) / 2;
			free_list(&prodH, &prodT);
			if (multiply_list_by_int(*head2, mid, &prodH, &prodT) == FAILURE)
				return FAILURE;

			if (compare_lists(prodH, remH) <= 0)
			{
				qdigit = mid;
				low = mid + 1;
			}
			else
				high = mid - 1;
		}

		if (insert_last(headR, &resT, qdigit) == FAILURE)
			return FAILURE;

		free_list(&prodH, &prodT);
		if (multiply_list_by_int(*head2, qdigit, &prodH, &prodT) == FAILURE)
			return FAILURE;

		free_list(&newRemH, &newRemT);
		if (subtraction(&remH, &remT, &prodH, &prodT, &newRemH) == FAILURE)
			return FAILURE;

		free_list(&remH, &remT);
		remH = newRemH;
		for (remT = remH; remT && remT->next; remT = remT->next)
			;
		newRemH = newRemT = NULL;
	}

	free_list(&prodH, &prodT);
	free_list(&remH, &remT);
	trim_leading_zero_nodes(headR, &resT);

	(void)tail2;
	return SUCCESS;
}
