/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "spmimelite.hpp"

#include "spmimepart.hpp"
#include "spmimeutils.hpp"
#include "spmimeheader.hpp"
#include "spmimebuffer.hpp"

SP_MimeLiteParser :: SP_MimeLiteParser()
{
	mRootPart = new SP_MimePart();
	mCurrent = mRootPart;
}

SP_MimeLiteParser :: ~SP_MimeLiteParser()
{
	delete mRootPart;
	mRootPart = NULL;

	mCurrent = NULL;
}
 
int SP_MimeLiteParser :: parseHeader( const char * buffer, SP_MimeHeaderList * list )
{
	const char * body = SP_MimeUtils::getFirstEmpLine( buffer );

	const char * pos = buffer;

	for( ; NULL != pos && pos < body; ) {
		char name[ 128 ] = { 0 };
		strncpy( name, pos, sizeof( name ) - 1 );

		if( NULL != strchr( name, '\n' ) ) *strchr( name, '\n' ) = '\0';

		char * colon = strchr( name, ':' );

		if( NULL == colon ) break;
		*colon = '\0';

		char * value = SP_MimeUtils::dupValue( pos, name );

		SP_MimeHeader * header = new SP_MimeHeader();
		header->setName( name );
		header->directSetValue( value );

		list->append( header );

		pos = strchr( pos, '\n' );
		for( ; NULL != pos && isspace( *( pos + 1 ) ); ) {
			pos = strchr( pos + 1, '\n' );
		}
		if( NULL != pos ) pos++;
	}

	return 0;
}

int SP_MimeLiteParser :: parse( const char * buffer, int len )
{
	return parse( buffer, len, mRootPart );
}

int SP_MimeLiteParser :: parse( const char * buffer, int len, SP_MimePart * part )
{
	char type[ 512 ] = { 0 };
	SP_MimeUtils::getValue( buffer, "Content-Type", type, sizeof( type ) );

	const char * body = SP_MimeUtils::getFirstEmpLine( buffer );
	if( '\n' == *body ) body++;

	parseHeader( buffer, part->getHeaderList() );

	if( 0 == strncasecmp( type, "multipart/", 10 ) ) {
		char boundary[ 256 ] = { 0 };
		if( 0 == SP_MimeUtils::getBoundary( buffer, boundary, sizeof( boundary ) ) ) {
			char key[ 256 ] = { 0 };
			snprintf( key, sizeof( key ), "\n--%s", boundary );

			char * pos = strstr( buffer, key );
			if( NULL != pos ) pos++;

			if( pos > body ) part->getPreamble()->write( body, pos - body - 1 );

			for( ; NULL != pos; ) {
				char * begin = strchr( pos, '\n' );
				if( NULL == begin ) break;
				begin++;

				char * end = strstr( begin, key );
				if( NULL == end ) break;

				if( end - buffer > len ) break;

				SP_MimePart * temp = new SP_MimePart();
				part->getBodyPartList()->append( temp );
				temp->setParent( part );

				parse( begin, end - begin, temp );

				pos = end + strlen( key );

				if( 0 == strncmp( pos, "--", 2 ) ) {
					pos += 3;
					break;
				}
			}

			if( NULL != pos ) {
				part->getEpilogue()->write( pos, len + buffer - pos );
			}
		} else {
			part->getPreamble()->write( body, buffer + len - body );
		}
	} else {
		if( buffer + len - body > 0 ) {
			part->getContent()->write( body, buffer + len - body );
		} else {
			if( part->getHeaderList()->getCount() <= 0 ) {
				// only has content, no header
				part->getContent()->write( buffer, len );

				SP_MimeHeader * header = new SP_MimeHeader();
				header->setName( "Content-Type" );
				header->setValue( "text/plain" );
				part->getHeaderList()->append( header );
			} else {
				// only has header, no content
			}
		}
	}

	return 0;
}

SP_MimePart * SP_MimeLiteParser :: getRootPart()
{
	return mRootPart;
}

