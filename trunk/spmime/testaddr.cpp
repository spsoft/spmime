/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spmimebuffer.hpp"
#include "spmimeutils.hpp"

#include "spmimeaddr.hpp"

void parseField( const char * mailData, const char * field )
{
	char * value = 	SP_MimeUtils::dupValue( mailData, field );

	if( NULL == value ) return;

	printf( "\n%s: %s\n", field, value );

	SP_MimeAddressList addrList;

	SP_MimeAddress::parse( value, &addrList );

	addrList.dump();

	free( value );
}

int main( int argc, char * argv[] )
{
	if( argc < 2 ) {
		printf( "Usage: %s <file>\n", argv[0] );
		exit( 0 );
	}

	const char * filename = argv[1];

	SP_MimeBuffer buffer;

	if( 0 != SP_MimeUtils::readFile( filename, &buffer ) ) {
		printf( "cannot not read %s\n", filename );
		exit( -1 );
	}

	parseField( buffer.getBuffer(), "From" );
	parseField( buffer.getBuffer(), "To" );
	parseField( buffer.getBuffer(), "Cc" );

	return 0;
}

