/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>

#include "spmimeaddr.hpp"

#include "spparseaddr.h"

char * SP_MimeAddress :: mNull = "";

void SP_MimeAddress :: safeFree( void * ptr )
{
	if( mNull != ptr ) free( ptr );
}

SP_MimeAddress :: SP_MimeAddress( int isGroup )
{
	mIsGroup = isGroup;
}

SP_MimeAddress :: ~SP_MimeAddress()
{
}

int SP_MimeAddress :: isGroup() const
{
	return mIsGroup;
}

SP_MimeMailbox * SP_MimeAddress :: toMailbox() const
{
	if( 0 == mIsGroup ) return (SP_MimeMailbox*)this;

	return NULL;
}

SP_MimeGroup * SP_MimeAddress :: toGroup() const
{
	if( mIsGroup ) return (SP_MimeGroup*)this;

	return NULL;
}

int SP_MimeAddress :: parse( const char * str, SP_MimeAddressList * addrList )
{
	struct sp_address * addrPtr = NULL;
	sp_parseaddr_list( str, &addrPtr );

	for( struct sp_address * iter = addrPtr; NULL != iter; iter = iter->next ) {
		if( NULL == iter->domain && NULL == iter->name && NULL == iter->route
				&& NULL != iter->mailbox ) {
			SP_MimeGroup * group = new SP_MimeGroup();
			group->setName( iter->mailbox );
			addrList->append( group );

			for( iter = iter->next; NULL != iter; iter = iter->next ) {
				if( NULL == iter->domain && NULL == iter->name
						&& NULL == iter->route && NULL == iter->mailbox ) break;	

				SP_MimeMailbox * mailbox = new SP_MimeMailbox();
				mailbox->setMailbox( iter->mailbox );
				mailbox->setDomain( iter->domain );
				mailbox->setName( iter->name );
				mailbox->setRoute( iter->route );

				group->getMailboxVector()->push_back( mailbox );
			}
		} else {
			SP_MimeMailbox * mailbox = new SP_MimeMailbox();
			mailbox->setMailbox( iter->mailbox );
			mailbox->setDomain( iter->domain );
			mailbox->setName( iter->name );
			mailbox->setRoute( iter->route );

			addrList->append( mailbox );
		}
	}

	if( NULL != addrPtr ) sp_parseaddr_free( addrPtr );

	return 0;
}

//---------------------------------------------------------

SP_MimeAddressList :: SP_MimeAddressList()
{
}

SP_MimeAddressList :: ~SP_MimeAddressList()
{
	std::vector< SP_MimeAddress * >::iterator iter = mList.begin();
	for( ; mList.end() != iter; ++iter ) {
		delete *iter;
	}
	mList.clear();
}

void SP_MimeAddressList :: dump()
{
	printf( "\n" );

	printf( "list [%d]\n", mList.size() );
	std::vector< SP_MimeAddress * >::iterator iter = mList.begin();
	for( ; mList.end() != iter; ++iter ) {
		(*iter)->dump();
	}

	printf( "\n" );

	SP_MimeMailboxVector * list = getFlatList();
	printf( "flatlist [%d]\n", list->size() );
	for( int i = 0; i < (int)list->size(); i++ ) {
		(*list)[i]->dump();
	}

	printf( "\n" );
}

int SP_MimeAddressList :: getCount()
{
	return mList.size();
}

SP_MimeAddress * SP_MimeAddressList :: getItem( int index )
{
	SP_MimeAddress * ret = NULL;

	if( index >= 0 && index < (int)mList.size() ) ret = mList[ index ];

	return ret;
}

void SP_MimeAddressList :: append( SP_MimeAddress * address )
{
	mList.push_back( address  );
	mFlatList.clear();
}

SP_MimeMailboxVector * SP_MimeAddressList :: getFlatList()
{
	if( mFlatList.size() <= 0 ) {
		std::vector< SP_MimeAddress * >::iterator iter = mList.begin();
		for( ; mList.end() != iter; ++iter ) {
			if( (*iter)->isGroup() ) {
				SP_MimeMailboxVector * tmp = (*iter)->toGroup()->getMailboxVector();
				mFlatList.insert( mFlatList.end(), tmp->begin(), tmp->end() );
			} else {
				mFlatList.push_back( (*iter)->toMailbox() );
			}
		}
	}

	return &mFlatList;
}

//---------------------------------------------------------

SP_MimeMailbox :: SP_MimeMailbox()
	: SP_MimeAddress( 0 )
{
	mMailbox = mDomain = mName = mRoute = mNull;
}

SP_MimeMailbox :: ~SP_MimeMailbox()
{
	safeFree( mMailbox );
	safeFree( mDomain );
	safeFree( mName );
	safeFree( mRoute );

	mMailbox = mDomain = mName = mRoute = mNull;
}

void SP_MimeMailbox :: dump()
{
	printf( "name [%s], mailbox [%s], domain [%s], route [%s]\n",
		mName, mMailbox, mDomain, mRoute );
}

void SP_MimeMailbox :: setMailbox( const char * mailbox )
{
	char * tmp = mMailbox;
	mMailbox = mailbox ? strdup( mailbox ) : mNull;
	safeFree( tmp );
}

const char * SP_MimeMailbox :: getMailbox() const
{
	return mMailbox;
}

void SP_MimeMailbox :: setDomain( const char * domain )
{
	if( 0 == strcmp( domain, "unspecified-domain" ) ) domain = NULL;

	char * tmp = mDomain;
	mDomain = domain ? strdup( domain ) : mNull;
	safeFree( tmp );
}

const char * SP_MimeMailbox :: getDomain() const
{
	return mDomain;
}

void SP_MimeMailbox :: setName( const char * name )
{
	char * tmp = mName;
	mName = name ? strdup( name ) : mNull;
	safeFree( tmp );
}

const char * SP_MimeMailbox :: getName() const
{
	return mName;
}

void SP_MimeMailbox :: setRoute( const char * route )
{
	char * tmp = mRoute;
	mRoute = route ? strdup( route ) : mNull;
	safeFree( tmp );
}

const char * SP_MimeMailbox :: getRoute() const
{
	return mRoute;
}

//---------------------------------------------------------

SP_MimeGroup :: SP_MimeGroup()
	: SP_MimeAddress( 1 )
{
	mName = mNull;
}

SP_MimeGroup :: ~SP_MimeGroup()
{
	SP_MimeMailboxVector::iterator iter = mList.begin();
	for( ; mList.end() != iter; ++iter ) {
		delete (*iter);
	}

	mList.clear();

	safeFree( mName );
	mName = NULL;
}

void SP_MimeGroup :: dump()
{
	printf( "Group: %s\n", mName );

	SP_MimeMailboxVector::iterator iter = mList.begin();
	for( ; mList.end() != iter; ++iter ) {
		(*iter)->dump();
	}
}

void SP_MimeGroup :: setName( const char * name )
{
	char * tmp = mName;
	mName = name ? strdup( name ) : mNull;
	safeFree( tmp );
}

const char * SP_MimeGroup :: getName() const
{
	return mName;
}

SP_MimeMailboxVector * SP_MimeGroup :: getMailboxVector()
{
	return &mList;
}

