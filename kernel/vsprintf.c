/*
 * vsprintf.c
 *
 *  Created on: 2012-11-10
 *      Author: desperedo
 *
 *	vsprintf() implementation for Magix
 */

#include <vsprintf.h>

#include <string.h>

#define V_ZEROPAD			0x01
#define V_SIGN				0x02
#define V_PLUS				0x04
#define V_SPACE				0x08
#define V_LEFT				0x10
#define V_SPECIAL			0x20
#define V_SMALL				0x40

#define DIV_BASE(n, base)					\
	({										\
		int __res;							\
		__asm__								\
		(									\
			"divl	%4"						\
			: "=a"(n), "=d"(__res)			\
			: "0"(n), "1"(0), "r"(base)		\
			:								\
		);									\
		__res;								\
	})

static int skip_atoi(const char **s)
{
	int x = 0;

	while (is_digit(**s)) x = x * 10 + *((*s)++) - '0';

	return x;
}

static char *number(char *str, int num, int base, int size, int precision, int type)
{
	if (base < 2 || base > 36) return NULL;

	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (type & V_LEFT) type &= ~V_ZEROPAD;
	if (type & V_SMALL) digits = "0123456789abcdefghijklmnopqrstuvwxyz";

	char c = type & V_ZEROPAD ? '0' : ' ';
	char sign = type & V_SIGN ? (num < 0 ? '-' : NULL) : (type & V_PLUS ? '+' : (type & V_SPACE ? ' ' : NULL));

	if (num < 0 && (type & V_SIGN)) num = -num;

	if (sign) size--;

	if (type & V_SPECIAL)
	{
		if (base == 8) size--;
		if (base == 16) size -= 2;
	}

	int i = 0;
	char buffer[256] = {0};

	if (num == 0) buffer[i++] = '0';

	while (num) buffer[i++] = digits[DIV_BASE(num, base)];

	if (i > precision && precision >= 0) i = precision;

	size -= i;

	while (!(type & (V_LEFT | V_ZEROPAD)) && (size-- > 0)) *str++ = ' ';

	if (sign) *str++ = sign;

	if (type & V_SPECIAL)
	{
		if (base == 8) *str++ = '0';
		if (base == 16) { *str++ = '0'; *str++ = 'x'; }
	}

	while (!(type & V_LEFT) && (size-- > 0)) *str++ = c;

	while (i < precision--) *str++ = '0';

	while (i-- > 0) *str++ = buffer[i];

	while (size-- > 0) *str++ = ' ';

	return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char *str = buf;

	for (; *fmt; fmt++)
	{
		if (*fmt != '%')
		{
			*str++ = *fmt;

			continue;
		}

		int loop = 1;
		int flags = 0;

		while (loop)
		{
			switch (*(++fmt))
			{
				case '-': flags |= V_LEFT; break;
				case '+': flags |= V_PLUS; break;
				case ' ': flags |= V_SPACE; break;
				case '#': flags |= V_SPECIAL; break;
				case '0': flags |= V_ZEROPAD; break;

				default: loop = 0; break;
			}
		}

		int field_width = -1;

		if (is_digit(*fmt))
		{
			field_width = skip_atoi(&fmt);
		}
		else
		{
			if (*fmt == '*')
			{
				field_width = va_arg(args, int);

				if (field_width < 0)
				{
					flags |= V_LEFT;
					field_width = -field_width;
				}
			}
		}

		int precision = -1;

		if (*fmt == '.')
		{
			fmt++;

			if (is_digit(*fmt))
			{
				precision = skip_atoi(&fmt);
			}
			else
			{
				if (*fmt == '*') precision = va_arg(args, int);
			}

			if (precision < 0) precision = 0;
		}

		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') fmt++;

		switch (*fmt)
		{
			case 'n': *va_arg(args, int *) = str - buf; break;

			case 'o': str = number(str, va_arg(args, ulong), 8, field_width, precision, flags); break;

			case 'x': flags |= V_SMALL;
			case 'X': str = number(str, va_arg(args, ulong), 16, field_width, precision, flags); break;

			case 'd':
			case 'i': flags |= V_SIGN;
			case 'u': str = number(str, va_arg(args, ulong), 10, field_width, precision, flags); break;

			case 'p':
			{
				if (field_width == -1)
				{
					field_width = 8;

					flags |= V_ZEROPAD;
				}

				str = number(str, (int)va_arg(args, void *), 16, field_width, precision, flags);

				break;
			}

			case 'c':
			{
				while (!(flags & V_LEFT) & (--field_width > 0)) *str++ = ' ';

				*str++ = (uchar)va_arg(args, char);

				while (--field_width > 0) *str++ = ' ';

				break;
			}

			case 's':
			{
				char *s = va_arg(args, char *);

				int len = strlen(s);

				if (precision < 0) precision = len;
				if (len > precision) len = precision;

				while (!(flags & V_LEFT) && (len < field_width--)) *str++ = ' ';

				for (int i = 0; i < len; i++) *str++ = *s++;

				while (len < field_width--) *str++ = ' ';

				break;
			}

			default:
			{
				if (*fmt != '%') *str++ = '%';

				if (*fmt) *str++ = *fmt; else fmt--;

				break;
			}
		}
	}

	*str = 0;

	return str - buf;
}