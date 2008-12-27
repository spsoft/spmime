/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>

#include "spmimetime.hpp"

int main ( int argc, char * argv[] )
{
	char * strList[] = {
		"14 Apr 89 03:20:12",
		"14 Apr 89 03:20 GMT",
		"Fri, 17 Mar 89 4:01:33",
		"Fri, 17 Mar 89 4:01 GMT",
		"Mon Jan 16 16:12 PDT 1989",
		"Mon Jan 16 16:12 +0130 1989",
		"6 May 1992 16:41-JST (Wednesday)",
		"22-AUG-1993 10:59:12.82",
		"22-AUG-1993 10:59pm",
		"22-AUG-1993 12:59am",
		"22-AUG-1993 12:59 PM",
		"Friday, August 04, 1995 3:54 PM",
		"06/21/95 04:24:34 PM",
		"20/06/95 21:07",
		"95-06-08 19:32:48 EDT",
		"Tue, 5 Jan 1999 11:33:33 +0800 (CST)",
		"Tue, 5 Jan 1999 11:14:47 +0800",
		"8 Jan 1999 11:10:52 -0000",
		"Thu, 17 Dec 1998 22:55:05 (PST)",
		"06 Feb 99 21:30:22 CST",
		"Thu, 15 Nov 2001 17:2:0 +0800",
		NULL } ;

	for( int i = 0; NULL != strList[i]; i++ ) {
		time_t ret = sp_mime_parse_time_string( strList[i], 0 );

		char str[ 128 ] = { 0 };
		sp_mime_gen_time_string( str, sizeof( str ), &ret );

		struct tm gmt;
		gmtime_r( &ret, &gmt );

		printf( "%s\t\t\t%s\t%s\t\t%s", strList[i], ctime( &ret ), str, asctime( &gmt ) );
	}

	return 0;
}

