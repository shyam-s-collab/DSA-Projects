/**************************************************************************************************************************************************************
*Name    :Shyam S Suryavanshi
*Project :APC
*Title		: main function(Driver function)
*Description	: Command line arbitrary precision calculator.
***************************************************************************************************************************************************************/
#include "apc.h"

#define DIVISION_PRECISION 20

int _CRT_glob = 0;

typedef struct
{
	int sign;
	char *digits;
	int scale;
} Decimal;

static char *xstrdup(const char *s)
{
	char *copy = malloc(strlen(s) + 1);
	if (copy)
		strcpy(copy, s);
	return copy;
}

static int is_zero_str(const char *s)
{
	while (*s)
	{
		if (*s != '0')
			return 0;
		s++;
	}
	return 1;
}

static char *strip_leading_zeros_owned(char *s)
{
	char *p = s;
	char *out;

	while (*p == '0' && p[1] != '\0')
		p++;

	out = xstrdup(p);
	free(s);
	return out;
}

static char *append_zeros(const char *s, int count)
{
	size_t len = strlen(s);
	char *out = malloc(len + count + 1);
	if (out == NULL)
		return NULL;
	strcpy(out, s);
	memset(out + len, '0', count);
	out[len + count] = '\0';
	return out;
}

static int compare_abs(const char *a, const char *b)
{
	while (*a == '0' && a[1] != '\0')
		a++;
	while (*b == '0' && b[1] != '\0')
		b++;

	if (strlen(a) != strlen(b))
		return (strlen(a) > strlen(b)) ? 1 : -1;
	return strcmp(a, b) > 0 ? 1 : (strcmp(a, b) < 0 ? -1 : 0);
}

static char *add_abs(const char *a, const char *b)
{
	int i = (int)strlen(a) - 1, j = (int)strlen(b) - 1, k, carry = 0;
	int max = (i > j ? i : j) + 2;
	char *out = malloc(max + 1);
	if (out == NULL)
		return NULL;

	out[max] = '\0';
	k = max - 1;
	while (i >= 0 || j >= 0 || carry)
	{
		int sum = carry;
		if (i >= 0)
			sum += a[i--] - '0';
		if (j >= 0)
			sum += b[j--] - '0';
		out[k--] = (char)('0' + (sum % 10));
		carry = sum / 10;
	}
	while (k >= 0)
		out[k--] = '0';

	return strip_leading_zeros_owned(out);
}

static char *sub_abs(const char *a, const char *b)
{
	int i = (int)strlen(a) - 1, j = (int)strlen(b) - 1, k = (int)strlen(a);
	int borrow = 0;
	char *out = malloc(strlen(a) + 1);
	if (out == NULL)
		return NULL;

	out[k--] = '\0';
	while (i >= 0)
	{
		int diff = (a[i--] - '0') - borrow - (j >= 0 ? b[j--] - '0' : 0);
		if (diff < 0)
		{
			diff += 10;
			borrow = 1;
		}
		else
			borrow = 0;
		out[k--] = (char)('0' + diff);
	}
	return strip_leading_zeros_owned(out);
}

static char *mul_abs(const char *a, const char *b)
{
	int len1 = (int)strlen(a), len2 = (int)strlen(b), i, j;
	int *res;
	char *out;
	int k = 0, start = 0;

	if (is_zero_str(a) || is_zero_str(b))
		return xstrdup("0");

	res = calloc(len1 + len2, sizeof(int));
	out = malloc(len1 + len2 + 1);
	if (res == NULL || out == NULL)
	{
		free(res);
		free(out);
		return NULL;
	}

	for (i = len1 - 1; i >= 0; i--)
		for (j = len2 - 1; j >= 0; j--)
			res[i + j + 1] += (a[i] - '0') * (b[j] - '0');

	for (i = len1 + len2 - 1; i > 0; i--)
	{
		res[i - 1] += res[i] / 10;
		res[i] %= 10;
	}

	while (start < len1 + len2 - 1 && res[start] == 0)
		start++;
	for (i = start; i < len1 + len2; i++)
		out[k++] = (char)('0' + res[i]);
	out[k] = '\0';

	free(res);
	return out;
}

static char *mul_digit(const char *a, int digit)
{
	char d[2] = { (char)('0' + digit), '\0' };
	return mul_abs(a, d);
}

static int divmod_abs(const char *dividend, const char *divisor, char **quotient, char **remainder)
{
	size_t len = strlen(dividend), qi = 0, idx;
	char *q = malloc(len + 1);
	char *rem = xstrdup("0");

	if (q == NULL || rem == NULL || is_zero_str(divisor))
	{
		free(q);
		free(rem);
		return FAILURE;
	}

	for (idx = 0; idx < len; idx++)
	{
		size_t rlen = strlen(rem);
		char *next = malloc(rlen + 2);
		int digit;

		if (next == NULL)
		{
			free(q);
			free(rem);
			return FAILURE;
		}
		strcpy(next, rem);
		next[rlen] = dividend[idx];
		next[rlen + 1] = '\0';
		free(rem);
		rem = strip_leading_zeros_owned(next);

		for (digit = 9; digit >= 1; digit--)
		{
			char *prod = mul_digit(divisor, digit);
			int cmp = compare_abs(prod, rem);
			if (cmp <= 0)
			{
				char *new_rem = sub_abs(rem, prod);
				free(rem);
				free(prod);
				rem = new_rem;
				break;
			}
			free(prod);
		}
		if (digit == 0)
			q[qi++] = '0';
		else
			q[qi++] = (char)('0' + digit);
	}
	q[qi] = '\0';

	*quotient = strip_leading_zeros_owned(q);
	*remainder = strip_leading_zeros_owned(rem);
	return SUCCESS;
}

static void free_decimal(Decimal *d)
{
	free(d->digits);
	d->digits = NULL;
}

static int parse_decimal(const char *text, Decimal *out)
{
	size_t len = strlen(text), i, used = 0;
	int seen_dot = 0;
	char *digits = malloc(len + 2);

	if (digits == NULL || len == 0)
	{
		free(digits);
		return FAILURE;
	}

	out->sign = 1;
	out->scale = 0;
	if (*text == '-' || *text == '+')
	{
		if (*text == '-')
			out->sign = -1;
		text++;
	}

	if (*text == '\0')
	{
		free(digits);
		return FAILURE;
	}

	for (i = 0; text[i]; i++)
	{
		if (text[i] == '.')
		{
			if (seen_dot)
			{
				free(digits);
				return FAILURE;
			}
			seen_dot = 1;
		}
		else if (isdigit((unsigned char)text[i]))
		{
			digits[used++] = text[i];
			if (seen_dot)
				out->scale++;
		}
		else
		{
			free(digits);
			return FAILURE;
		}
	}

	if (used == 0)
	{
		free(digits);
		return FAILURE;
	}

	while (out->scale > 0 && used > 0 && digits[used - 1] == '0')
	{
		used--;
		out->scale--;
	}
	digits[used] = '\0';

	out->digits = strip_leading_zeros_owned(digits);
	if (is_zero_str(out->digits))
	{
		out->sign = 1;
		out->scale = 0;
	}
	return SUCCESS;
}

static char *format_decimal(int sign, char *digits, int scale)
{
	size_t len;
	char *clean, *out;

	digits = strip_leading_zeros_owned(digits);
	if (is_zero_str(digits))
	{
		free(digits);
		return xstrdup("0");
	}

	while (scale > 0 && strlen(digits) > 0 && digits[strlen(digits) - 1] == '0')
	{
		digits[strlen(digits) - 1] = '\0';
		scale--;
	}

	len = strlen(digits);
	if (scale == 0)
	{
		out = malloc(len + (sign < 0 ? 2 : 1));
		if (out == NULL)
		{
			free(digits);
			return NULL;
		}
		sprintf(out, "%s%s", sign < 0 ? "-" : "", digits);
		free(digits);
		return out;
	}

	if ((size_t)scale >= len)
	{
		size_t zeros = (size_t)scale - len;
		clean = malloc(2 + zeros + len + 1);
		if (clean == NULL)
		{
			free(digits);
			return NULL;
		}
		clean[0] = '0';
		clean[1] = '.';
		memset(clean + 2, '0', zeros);
		strcpy(clean + 2 + zeros, digits);
	}
	else
	{
		size_t int_len = len - scale;
		clean = malloc(len + 2);
		if (clean == NULL)
		{
			free(digits);
			return NULL;
		}
		memcpy(clean, digits, int_len);
		clean[int_len] = '.';
		strcpy(clean + int_len + 1, digits + int_len);
	}

	out = malloc(strlen(clean) + (sign < 0 ? 2 : 1));
	if (out)
		sprintf(out, "%s%s", sign < 0 ? "-" : "", clean);
	free(clean);
	free(digits);
	return out;
}

static char *decimal_add_values(Decimal a, Decimal b)
{
	int scale = a.scale > b.scale ? a.scale : b.scale;
	char *ad = append_zeros(a.digits, scale - a.scale);
	char *bd = append_zeros(b.digits, scale - b.scale);
	char *digits;
	int sign = 1;

	if (ad == NULL || bd == NULL)
	{
		free(ad);
		free(bd);
		return NULL;
	}

	if (a.sign == b.sign)
	{
		digits = add_abs(ad, bd);
		sign = a.sign;
	}
	else
	{
		int cmp = compare_abs(ad, bd);
		if (cmp >= 0)
		{
			digits = sub_abs(ad, bd);
			sign = a.sign;
		}
		else
		{
			digits = sub_abs(bd, ad);
			sign = b.sign;
		}
	}

	free(ad);
	free(bd);
	return format_decimal(sign, digits, scale);
}

static char *decimal_add(const char *left, const char *right)
{
	Decimal a, b;
	char *out;
	if (parse_decimal(left, &a) == FAILURE || parse_decimal(right, &b) == FAILURE)
		return NULL;
	out = decimal_add_values(a, b);
	free_decimal(&a);
	free_decimal(&b);
	return out;
}

static char *decimal_subtract(const char *left, const char *right)
{
	Decimal a, b;
	char *out;
	if (parse_decimal(left, &a) == FAILURE || parse_decimal(right, &b) == FAILURE)
		return NULL;
	b.sign *= -1;
	out = decimal_add_values(a, b);
	free_decimal(&a);
	free_decimal(&b);
	return out;
}

static char *decimal_multiply(const char *left, const char *right)
{
	Decimal a, b;
	char *digits, *out;
	int sign;

	if (parse_decimal(left, &a) == FAILURE || parse_decimal(right, &b) == FAILURE)
		return NULL;

	digits = mul_abs(a.digits, b.digits);
	sign = a.sign * b.sign;
	out = format_decimal(sign, digits, a.scale + b.scale);
	free_decimal(&a);
	free_decimal(&b);
	return out;
}

static char *decimal_divide(const char *left, const char *right)
{
	Decimal a, b;
	char *num, *den, *quot, *rem, *out;
	int shift, sign;

	if (parse_decimal(left, &a) == FAILURE || parse_decimal(right, &b) == FAILURE)
		return NULL;
	if (is_zero_str(b.digits))
	{
		free_decimal(&a);
		free_decimal(&b);
		return NULL;
	}

	shift = DIVISION_PRECISION + b.scale - a.scale;
	if (shift >= 0)
	{
		num = append_zeros(a.digits, shift);
		den = xstrdup(b.digits);
	}
	else
	{
		num = xstrdup(a.digits);
		den = append_zeros(b.digits, -shift);
	}

	if (divmod_abs(num, den, &quot, &rem) == FAILURE)
	{
		free(num);
		free(den);
		free_decimal(&a);
		free_decimal(&b);
		return NULL;
	}

	sign = a.sign * b.sign;
	out = format_decimal(sign, quot, DIVISION_PRECISION);
	free(rem);
	free(num);
	free(den);
	free_decimal(&a);
	free_decimal(&b);
	return out;
}

static char *decimal_modulo(const char *left, const char *right)
{
	Decimal a, b;
	int scale;
	char *ad, *bd, *quot, *rem, *out;

	if (parse_decimal(left, &a) == FAILURE || parse_decimal(right, &b) == FAILURE)
		return NULL;
	if (is_zero_str(b.digits))
	{
		free_decimal(&a);
		free_decimal(&b);
		return NULL;
	}

	scale = a.scale > b.scale ? a.scale : b.scale;
	ad = append_zeros(a.digits, scale - a.scale);
	bd = append_zeros(b.digits, scale - b.scale);

	if (divmod_abs(ad, bd, &quot, &rem) == FAILURE)
	{
		free(ad);
		free(bd);
		free_decimal(&a);
		free_decimal(&b);
		return NULL;
	}

	out = format_decimal(a.sign, rem, scale);
	free(quot);
	free(ad);
	free(bd);
	free_decimal(&a);
	free_decimal(&b);
	return out;
}

static int parse_exponent(const char *text, unsigned long long *exp)
{
	unsigned long long value = 0;
	if (*text == '+')
		text++;
	if (*text == '\0')
		return FAILURE;
	while (*text)
	{
		if (!isdigit((unsigned char)*text))
			return FAILURE;
		if (value > 1000000ULL)
			return FAILURE;
		value = value * 10 + (unsigned)(*text - '0');
		text++;
	}
	*exp = value;
	return SUCCESS;
}

static char *decimal_power(const char *left, const char *right)
{
	Decimal base;
	unsigned long long exp, i;
	char *digits = xstrdup("1");
	int scale = 0, sign = 1;

	if (parse_decimal(left, &base) == FAILURE || parse_exponent(right, &exp) == FAILURE)
		return NULL;

	for (i = 0; i < exp; i++)
	{
		char *tmp = mul_abs(digits, base.digits);
		free(digits);
		digits = tmp;
		scale += base.scale;
		sign *= base.sign;
		if (digits == NULL)
		{
			free_decimal(&base);
			return NULL;
		}
	}

	free_decimal(&base);
	return format_decimal(sign, digits, scale);
}

static char *calculate(const char *left, char operator, const char *right)
{
	switch (operator)
	{
		case '+':
			return decimal_add(left, right);
		case '-':
			return decimal_subtract(left, right);
		case '*':
			return decimal_multiply(left, right);
		case '/':
			return decimal_divide(left, right);
		case '%':
			return decimal_modulo(left, right);
		case '^':
			return decimal_power(left, right);
		default:
			return NULL;
	}
}

int main(int argc, char *argv[])
{
	char *result;

	if (argc != 4 || strlen(argv[2]) != 1)
	{
		printf("Usage: %s <number1> <operator> <number2>\n", argv[0]);
		printf("Operators: + - * / %% ^\n");
		return FAILURE;
	}

	result = calculate(argv[1], argv[2][0], argv[3]);
	if (result == NULL)
	{
		printf("Invalid Input:-( Try again...\n");
		return FAILURE;
	}

	printf("%s\n", result);
	free(result);
	return SUCCESS;
}
