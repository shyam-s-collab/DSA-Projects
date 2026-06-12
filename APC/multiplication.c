/*******************************************************************************************************************************************************************
*Title			: Multiplication
*Description		: This function performs multiplication of two given large numbers and store the result in the resultant list.
*Prototype		: int multiplication(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR);
*Input Parameters	: head1: Pointer to the first node of the first double linked list.
			: tail1: Pointer to the last node of the first double linked list.
			: head2: Pointer to the first node of the second double linked list.
			: tail2: Pointer to the last node of the second double linked list.
			: headR: Pointer to the first node of the resultant double linked list.
*Output			: Status (SUCCESS / FAILURE)
*******************************************************************************************************************************************************************/
#include "apc.h"

int multiplication(Dlist **head1, Dlist **tail1, Dlist **head2, Dlist **tail2, Dlist **headR)
{
	int len1 = 0, len2 = 0, i, j;
	Dlist *temp, *tailR = NULL;
	long long *a, *b, *res;

	for (temp = *head1; temp; temp = temp->next)
		len1++;
	for (temp = *head2; temp; temp = temp->next)
		len2++;

	a = calloc(len1, sizeof(long long));
	b = calloc(len2, sizeof(long long));
	res = calloc(len1 + len2, sizeof(long long));
	if (a == NULL || b == NULL || res == NULL)
	{
		free(a);
		free(b);
		free(res);
		return FAILURE;
	}

	for (temp = *tail1, i = 0; temp; temp = temp->prev, i++)
		a[i] = temp->data;
	for (temp = *tail2, i = 0; temp; temp = temp->prev, i++)
		b[i] = temp->data;

	for (i = 0; i < len1; i++)
	{
		for (j = 0; j < len2; j++)
		{
			res[i + j] += a[i] * b[j];
			res[i + j + 1] += res[i + j] / BASE;
			res[i + j] %= BASE;
		}
	}

	*headR = NULL;
	for (i = len1 + len2 - 1; i > 0 && res[i] == 0; i--)
		;
	for (; i >= 0; i--)
	{
		if (insert_last(headR, &tailR, (int)res[i]) == FAILURE)
		{
			free(a);
			free(b);
			free(res);
			return FAILURE;
		}
	}

	free(a);
	free(b);
	free(res);
	trim_leading_zero_nodes(headR, &tailR);
	return SUCCESS;
}
