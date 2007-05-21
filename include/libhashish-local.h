
/*
** $Id$
**
** Copyright (C) 2006 - Hagen Paul Pfeifer <hagen@jauu.net>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef _LIB_HASHISH_LOCAL_H
#define	_LIB_HASHISH_LOCAL_H

#include "../config.h"

#ifdef DEBUG
# include <stdio.h>
# define DEBUGPRINTF( fmt, ... )  fprintf(stderr, "DEBUG: %s:%u - " fmt,  __FILE__, __LINE__, __VA_ARGS__)
# define NDEBUG
#include <assert.h>
#else
# define DEBUGPRINTF( fmt, ... )  do { } while(0)
#endif

/* Forces a function to be always inlined
** 'must inline' - so that they get inlined even
** if optimizing for size
*/
#undef __always_inline
#if __GNUC_PREREQ (3,2)
# define __always_inline __inline __attribute__ ((__always_inline__))
#else
# define __always_inline __inline
#endif

#if __GNUC__ >= 3
#if !defined likely && !defined unlikely
# define likely(x)   __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)
#endif
#endif

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/* use this macros for .so initialize code */
#define __init __attribute__ ((constructor))
#define __exit __attribute__ ((destructor))

#define min(x,y) ({ \
     typeof(x) _x = (x); \
     typeof(y) _y = (y); \
     (void) (&_x == &_y);        \
     _x < _y ? _x : _y; })

#define max(x,y) ({ \
     typeof(x) _x = (x); \
     typeof(y) _y = (y); \
     (void) (&_x == &_y);        \
     _x > _y ? _x : _y; })

#define min_t(type,x,y) \
  ({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
#define max_t(type,x,y) \
  ({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })

extern int __hi_error(int, const char *, unsigned int, const char *, \
	const char *, ...);

extern const char *hi_geterror(void);
extern void hi_perror(const char *);

#define hi_error(E, FMT,ARG...) \
	__hi_error(E, __FILE__, __LINE__, __FUNCTION__, FMT, ##ARG)

#define hi_errno(E) hi_error(E, NULL)


#endif /* _LIB_HASHISH_LOCAL_H */

/* vim:set ts=4 sw=4 sts=4 tw=78 ff=unix noet: */
