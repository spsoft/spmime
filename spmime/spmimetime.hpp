/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimetime_hpp__
#define __spmimetime_hpp__

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

time_t sp_mime_parse_time_string( const char *string, int default_to_gmt );

const char * sp_mime_gen_time_string( char * result, int size, time_t * src_time = NULL );

#ifdef __cplusplus
}
#endif

#endif

