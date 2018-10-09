/* Copyright (C) 2004       Manuel Novoa III    <mjn3@codepoet.org>
 *
 * GNU Library General Public License (LGPL) version 2 or later.
 *
 * Dedicated to Toni.  See uClibc/DEDICATION.mjn3 for details.
 */

#include "_stdio.h"
#include <stdarg.h>
#include <wchar.h>

libc_hidden_proto(vfwprintf)

int vwprintf(const wchar_t * __restrict format, va_list arg)
{
	return vfwprintf(stdout, format, arg);
}
