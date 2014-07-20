/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system. 
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

/*
 * ydirectenv.h: Environment wrappers for YAFFS direct.
 */

#ifndef __YDIRECTENV_H__
#define __YDIRECTENV_H__


//lepton modif
#include "kernel/core/kernelconf.h"

// Direct interface
#include "devextras.h"
//lepton modif
#ifdef __tauon_posix__
   #include "kernel/core/types.h"
   #include "kernel/core/system.h"
   #include "kernel/core/stat.h"
   #include "kernel/core/libstd.h"
   #include "lib/libc/string/string.h"

   extern int  abs(int);
#else
   #include <math.h>
#endif

#ifdef __tauon_posix__
   #define strnlen(__s__,__m__) strlen(__s__)
   #define S_ISSOCK(m) 0
   typedef unsigned long loff_t;
#endif

//
#include "yaffs_malloc.h"

//test
#ifdef __tauon_posix__
	#include "kernel/core/assert.h"
#else
	#include "assert.h"
#endif
//

#define YBUG() assert(0)
//#define YBUG() do { *((int *)0) =1;} while(0)


#define YCHAR char
#define YUCHAR unsigned char
#define _Y(x) x
#define yaffs_strcat(a,b)    strcat(a,b)
#define yaffs_strcpy(a,b)    strcpy(a,b)
#define yaffs_strncpy(a,b,c) strncpy(a,b,c)
#define yaffs_strncmp(a,b,c) strncmp(a,b,c)
#define yaffs_strnlen(s,m)	     strnlen(s,m)
#define yaffs_sprintf	     sprintf
#define yaffs_toupper(a)     toupper(a)

#define YAFFS_PATH_DIVIDERS  "/"

#ifdef NO_Y_INLINE
#define Y_INLINE
#else
#define Y_INLINE inline
#endif

#define YMALLOC(x) yaffs_malloc(x)
#define YFREE(x)   free(x)
#define YMALLOC_ALT(x) yaffs_malloc(x)
#define YFREE_ALT(x)   free(x)

#define YMALLOC_DMA(x) yaffs_malloc(x)

#define YYIELD()  do {} while(0)



//#define YINFO(s) YPRINTF(( __FILE__ " %d %s\n",__LINE__,s))
//#define YALERT(s) YINFO(s)


#define TENDSTR "\n"
#define TSTR(x) x
#define TCONT(x) x
//#define TOUT(p) printf p
#define TOUT(p)



#define YAFFS_LOSTNFOUND_NAME		"lost+found"
#define YAFFS_LOSTNFOUND_PREFIX		"obj"
//#define YPRINTF(x) printf x

//#include "yaffscfg.h"

#define Y_CURRENT_TIME yaffsfs_CurrentTime()
#define Y_TIME_CONVERT(x) x

#define YAFFS_ROOT_MODE				0666
#define YAFFS_LOSTNFOUND_MODE		0666

#define yaffs_SumCompare(x,y) ((x) == (y))
#define yaffs_strcmp(a,b) strcmp(a,b)

#endif


