/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spmimeheader.hpp"
#include "spmimeutils.hpp"

SP_MimeHeader :: SP_MimeHeader()
{
	mName = NULL;
	mValue = NULL;
}

SP_MimeHeader :: ~SP_MimeHeader()
{
	if( NULL != mName ) free( mName );
	mName = NULL;

	if( NULL != mValue ) free( mValue );
	mValue = NULL;
}

void SP_MimeHeader :: setName( const char * name )
{
	if( name != mName ) {
		free( mName );
		mName = strdup( name );
	}
}

const char * SP_MimeHeader :: getName() const
{
	return mName;
}

void SP_MimeHeader :: setValue( const char * value )
{
	if( value != mValue ) {
		free( mValue );
		mValue = strdup( value );
	}
}

void SP_MimeHeader :: directSetValue( char * value )
{
	if( value != mValue ) {
		free( mValue );
		mValue = value;
	}
}

const char * SP_MimeHeader :: getValue() const
{
	return mValue;
}

int SP_MimeHeader :: getParam( const char * name, char * value, int len ) const
{
	return SP_MimeUtils::getParam( mValue, name, value, len );
}

//---------------------------------------------------------

SP_MimeHeaderList :: SP_MimeHeaderList()
{
}

SP_MimeHeaderList :: ~SP_MimeHeaderList()
{
	HeaderList::iterator it = mList.begin();
	for( ; mList.end() != it; ++it ) {
		delete (*it);
	}

	mList.clear();
}

int SP_MimeHeaderList :: getCount() const
{
	return mList.size();
}

const SP_MimeHeader * SP_MimeHeaderList :: getItem( int index ) const
{
	if( 0 <= index && index < (int)mList.size() ) return mList[ index ];

	return NULL;
}

void SP_MimeHeaderList :: append( SP_MimeHeader * header )
{
	mList.push_back( header );
}

const SP_MimeHeader * SP_MimeHeaderList :: find( const char * name ) const
{
	const SP_MimeHeader * ret = NULL;

	HeaderList::const_iterator it = mList.begin();
	for( ; mList.end() != it; ++it ) {
		if( 0 == strcasecmp( (*it)->getName(), name ) ) {
			ret = *it;
			break;
		}
	}

	return ret;
}

int SP_MimeHeaderList :: getBoundary( char * boundary, int len ) const
{
	int ret = -1;

	const SP_MimeHeader * type = find( "Content-Type" );
	if( NULL == type ) return ret;

	return type->getParam( "boundary", boundary, len );
}

int SP_MimeHeaderList :: getCharset( char * charset, int len ) const
{
	int ret = -1;

	const SP_MimeHeader * type = find( "Content-Type" );
	if( NULL == type ) return ret;

	return type->getParam( "charset", charset, len );
}

