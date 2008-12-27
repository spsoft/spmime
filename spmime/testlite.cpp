/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spmimelite.hpp"
#include "spmimeutils.hpp"
#include "spmimebuffer.hpp"

#include "testcomm.hpp"

int main( int argc, char * argv[] )
{
	if( argc < 2 ) {
		printf( "Usage: %s <eml> [charset]\n", argv[0] );
		exit( 0 );
	}

	const char * charset = NULL;
	if( argc > 2 ) charset = argv[2];

	const char * filename = argv[1];

	SP_MimeBuffer buffer;

	if( 0 != SP_MimeUtils::readFile( filename, &buffer ) ) {
		printf( "cannot not read %s\n", filename );
		exit( -1 );
	}

	SP_MimeLiteParser parser;
	parser.parse( buffer.getBuffer(), buffer.getWritePos() );

	if( NULL != charset ) {
		printDecodedPart( parser.getRootPart(), charset );
	} else {
		printPart( parser.getRootPart() );
	}

	return 0;
}

