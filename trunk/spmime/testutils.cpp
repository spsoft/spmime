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
		"\r\n\r\n",
		NULL,
	};

	for( int i = 0; ; i++ )
	{
		if( NULL == sampleArray[i] ) break;

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

	for( int i = 0; ; i++ )
	{
		if( NULL == sampleArray[i] ) break;

		char * newCopy = strdup( sampleArray[i] );

		SP_MimeUtils::removeHeader( newCopy, "From" );

		printf( "\nOrigin ------\n%s", sampleArray[i] );
		printf( "\nRemove ------\n%s", newCopy );

		free( newCopy );
	}

	{
		char * sampleToken [] = {
			"MAIL FROM: Pete <pete@silly.example>",
			"RCPT TO: A Group:Ed Jones <c@a.test>,joe@where.test,John <jdoe@one.test>;",
			NULL
		};

		for( int i = 0; ; i++ )
		{
			if( NULL == sampleToken[i] ) break;

			char part0[ 128 ] = { 0 }, part1[ 128 ] = { 0 };
			const char * part2 = NULL;

			SP_MimeUtils::getToken( sampleToken[i], 0, part0, sizeof( part0 ) );
			SP_MimeUtils::getToken( sampleToken[i], 1, part1, sizeof( part1 ), ' ', &part2 );

			printf( "[%s] -- [%s %s %s]\n", sampleToken[i], part0, part1, part2 );

			SP_MimeUtils::getToken( sampleToken[i], 0, part1, sizeof( part1 ), ':', &part2 );

			printf( "[%s] -- [%s %s]\n", sampleToken[i], part1, part2 );
		}
	}

	return 0;
}


