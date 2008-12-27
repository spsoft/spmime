/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "spmimeutils.hpp"

int main( int argc, char * argv[] )
{
	char * sampleArray [] = {
		"From:from", "From: from", "From : from", "From:  from   ",
		"From-ign: from-ign\r\nFrom\r\nFrom: from",
		"From-ign: from-ign\r\nFrom: \r\nFrom: from",
		"Before: before\r\nFrom: from",
		"From:from\r\nAfter: after",
		"Before: before\r\nFrom: from\r\nAfter: after",
		"Before:before\r\n\r\n",
		"\r\n\r\n"
	};

	for( size_t i = 0; i < sizeof( sampleArray ) / sizeof( sampleArray[0] ); i++ )
	{
		char from[ 128 ] = { 0 };
		SP_MimeUtils::getValue( sampleArray[i], "From", from, sizeof( from ) );
		printf( "%d -- From: <%s>\n", i, from );

		size_t len = 0;
		const char * ptr = SP_MimeUtils::getHeader( sampleArray[i], "From", &len );
		printf( "%p %d\n", ptr, len );
		for( size_t j = 0; j < len; j++ )
		{
			printf( "%c", ptr[j] );
		}
		printf( "\n" );
	}

	for( size_t i = 0; i < sizeof( sampleArray ) / sizeof( sampleArray[0] ); i++ )
	{
		char * newCopy = strdup( sampleArray[i] );

		SP_MimeUtils::removeHeader( newCopy, "From" );

		printf( "\nOrigin ------\n%s", sampleArray[i] );
		printf( "\nRemove ------\n%s", newCopy );

		free( newCopy );
	}

	return 0;
}


