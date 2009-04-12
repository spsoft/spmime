/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "spmimemodify.hpp"
#include "spmimeutils.hpp"

SP_MimeSimpleEditor :: SP_MimeSimpleEditor( const char * data )
{
	mBody = SP_MimeUtils::getFirstEmpLine( data );
	mHeader = SP_MimeUtils::dupHeader( data );
}

SP_MimeSimpleEditor :: ~SP_MimeSimpleEditor()
{
	free( mHeader ), mHeader = NULL;
}

void SP_MimeSimpleEditor :: addHeader( const char * name, const char * value )
{
	int headerLen = strlen( mHeader );

	int nameLen = strlen( name );
	int valueLen = strlen( value );

	mHeader = (char*)realloc( mHeader, headerLen + nameLen + valueLen + 128 );

	memcpy( mHeader + headerLen, name, nameLen );
	memcpy( mHeader + headerLen + nameLen, ": ", 2 );
	memcpy( mHeader + headerLen + nameLen + 2, value, valueLen );
	if( '\n' != value[ valueLen - 1 ] ) {
		memcpy( mHeader + headerLen + nameLen + 2 + valueLen, "\r\n", 2 );
		valueLen += 2;
	}

	mHeader[ headerLen + nameLen + 2 + valueLen ] = '\0';
}

int SP_MimeSimpleEditor :: removeHeader( const char * name, int index )
{
	size_t len = 0;
	char * pos = mHeader;

	for( int i = 0; i < index; i++ ) {
		pos = (char*)SP_MimeUtils::getHeader( pos + len, name, &len );

		if( NULL == pos ) break;
	}

	if( NULL != pos ) strcpy( pos, pos + len );

	return NULL == pos ? -1 : 0;
}

char * SP_MimeSimpleEditor :: getNewData()
{
	int headerLen = strlen( mHeader );
	int bodyLen = strlen( mBody );

	char * ret = (char*)malloc( headerLen + bodyLen + 1 );

	memcpy( ret, mHeader, headerLen );
	memcpy( ret + headerLen, mBody, bodyLen );

	ret[ headerLen + bodyLen ] = '\0';

	return ret;
}

