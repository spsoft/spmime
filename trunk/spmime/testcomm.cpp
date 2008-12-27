/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <sys/stat.h>

#include "spmimeheader.hpp"
#include "spmimepart.hpp"
#include "spmimeutils.hpp"
#include "spmimebuffer.hpp"
#include "spmimecodec.hpp"

#include "testcomm.hpp"

void printPart( const SP_MimePart * part )
{
	SP_MimeHeaderList * headerList = part->getHeaderList();

	SP_MimePartList * partList = part->getBodyPartList();

	for( int i = 0; i < headerList->getCount(); i++ ) {
		const SP_MimeHeader * header = headerList->getItem( i );

		if( NULL == strstr( header->getName(), "HMM" ) ) {
			printf( "%s: %s\r\n", header->getName(), header->getValue() );
		} else {
			printf( "%s:%s\r\n", header->getName(), header->getValue() );
		}
	}

	printf( "\r\n" );

	char boundary[ 256 ] = { 0 };
	headerList->getBoundary( boundary, sizeof( boundary ) );

	if( part->getContent()->getWritePos() > 0 ) {
		printf( "%s", part->getContent()->getBuffer() );
	}

	if( part->getPreamble()->getWritePos() > 0 ) {
		printf( "%s", part->getPreamble()->getBuffer() );
	}

	for( int i = 0; i < partList->getCount(); i++ ) {
		printf( "\r\n--%s\r\n", boundary );
		printPart( partList->getItem( i ) );
	}

	if( partList->getCount() > 0 ) {
		printf( "\r\n--%s--\r\n", boundary );
	}

	if( part->getEpilogue()->getWritePos() > 0 ) {
		printf( "%s", part->getEpilogue()->getBuffer() );
	}
}

void printDecodedPart( const SP_MimePart * part, const char * destCharset )
{
	SP_MimePart decodePart;
	SP_MimeCodec::decodePart( part, destCharset, &decodePart );

	printPart( &decodePart );

	printf( "\n\nGetShow\n\n" );

	SP_MimeBuffer text;
	SP_MimePartUtils::getTextMessage( &decodePart, &text );
	if( NULL != text.getBuffer() ) printf( "%s\n", text.getBuffer() );

	printf( "\n\nAttachments\n\n" );
	SP_MimePartVector attachmentList;
	SP_MimePartUtils::getAttachmentList( &decodePart, &attachmentList );

	for( int i = 0; i < (int)attachmentList.size(); ++i ) {
		SP_MimePart * part = attachmentList[ i ];
		const SP_MimeHeader * header = part->getHeaderList()->find( "Content-Disposition" );

		char filename[ 256 ] = { 0 };

		if( NULL != header ) {
			if( 0 == header->getParam( "filename", filename, sizeof( filename ) ) ) {
				printf( "%d: %s\n", i, filename );
			} else {
				printf( "%d: %s\n", i, header->getValue() );
			}
		}
	}
}

void printLine( const char * tag, const char * str )
{
	printf( "%s : ", tag );
	for( ; '\0' != *str && '\r' != *str && '\n' != *str; str++ ) {
		printf( "%c", *str );
	}
	printf( "\n" );
}

