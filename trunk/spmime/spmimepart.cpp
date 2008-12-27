/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spmimepart.hpp"
#include "spmimeheader.hpp"
#include "spmimeutils.hpp"

#include "spmimebuffer.hpp"

SP_MimePart :: SP_MimePart()
{
	mHeaderList = new SP_MimeHeaderList();
	mBodyPartList = new SP_MimePartList();
	mParent = NULL;

	mContent = new SP_MimeBuffer();
	mPreamble = new SP_MimeBuffer();
	mEpilogue = new SP_MimeBuffer();
}

SP_MimePart :: ~SP_MimePart()
{
	delete mHeaderList;
	mHeaderList = NULL;

	delete mBodyPartList;
	mBodyPartList = NULL;

	mParent = NULL;

	delete mContent;
	mContent = NULL;

	delete mPreamble;
	mPreamble = NULL;

	delete mEpilogue;
	mEpilogue = NULL;
}

SP_MimeHeaderList * SP_MimePart :: getHeaderList() const
{
	return mHeaderList;
}

bool SP_MimePart :: isMultipart() const
{
	const char * ctype = NULL;
	{
		const SP_MimeHeader * header = mHeaderList->find( "Content-Type" );
		if( NULL != header ) ctype = header->getValue();
	}

	if( NULL != ctype && SP_MimeUtils::startsCaseWith( ctype, "multipart/" ) ) {
		return true;
	}

	return false;
}

bool SP_MimePart :: isMessage() const
{
	const char * ctype = NULL;
	{
		const SP_MimeHeader * header = mHeaderList->find( "Content-Type" );
		if( NULL != header ) ctype = header->getValue();
	}

	if( NULL != ctype && SP_MimeUtils::startsCaseWith( ctype, "message/" ) ) {
		return true;
	}

	return false;
}

bool SP_MimePart :: isTextPart() const
{
	const char * ctype = NULL, * cdisp = NULL;
	{
		const SP_MimeHeader * header = mHeaderList->find( "Content-Type" );
		if( NULL != header ) ctype = header->getValue();

		header = mHeaderList->find( "Content-Disposition" );
		if( NULL != header ) cdisp = header->getValue();
	}

	if( ( NULL != ctype && SP_MimeUtils::startsCaseWith( ctype, "text/" ) )
			&& ( NULL == cdisp || 0 == strcasecmp( cdisp, "inline") ) ) {
		return true;
	}

	return false;
}

bool SP_MimePart :: isAttachment() const
{
	const char * ctype = NULL, * cdisp = NULL;
	{
		const SP_MimeHeader * header = mHeaderList->find( "Content-Type" );
		if( NULL != header ) ctype = header->getValue();

		header = mHeaderList->find( "Content-Disposition" );
		if( NULL != header ) cdisp = header->getValue();
	}

	if( ( ! isMultipart() ) && /* NULL != ctype && */ NULL != cdisp ) {
		return true;
	}

	return false;
}

SP_MimePartList * SP_MimePart :: getBodyPartList() const
{
	return mBodyPartList;
}

void SP_MimePart :: setParent( SP_MimePart * parent )
{
	mParent = parent;
}

SP_MimePart * SP_MimePart :: getParent() const
{
	return mParent;
}

SP_MimeBuffer * SP_MimePart :: getContent() const
{
	return mContent;
}

SP_MimeBuffer * SP_MimePart :: getPreamble() const
{
	return mPreamble;
}

SP_MimeBuffer * SP_MimePart :: getEpilogue() const
{
	return mEpilogue;
}

//---------------------------------------------------------

SP_MimePartList :: SP_MimePartList()
{
}

SP_MimePartList :: ~SP_MimePartList()
{
	SP_MimePartVector::iterator it = mList.begin();
	for( ; mList.end() != it; ++it ) {
		delete (*it);
	}
	mList.clear();
}

int SP_MimePartList :: getCount() const
{
	return mList.size();
}

SP_MimePart * SP_MimePartList :: getItem( int index ) const
{
	if( 0 <= index && index < (int)mList.size() ) return mList[ index ];

	return NULL;
}

void SP_MimePartList :: append( SP_MimePart * part )
{
	mList.push_back( part );
}

//---------------------------------------------------------

int SP_MimePartUtils :: getAttachmentList( SP_MimePart * root, SP_MimePartVector * attachmentList )
{
	SP_MimePartList * partList = root->getBodyPartList();

	if( root->isMultipart() ) {
		for( int i = 0; i < partList->getCount(); ++i ) {
			getAttachmentList( partList->getItem( i ), attachmentList );
		}
		return 0;
	}

	if( root->isAttachment() ) {
		attachmentList->push_back( root );
	}

	return 0;
}

int SP_MimePartUtils :: getTextList( SP_MimePart * root, SP_MimePartVector * textList )
{
	SP_MimePartList * partList = root->getBodyPartList();

	if( root->isMultipart() ) {
		for( int i = 0; i < partList->getCount(); ++i ) {
			getTextList( partList->getItem( i ), textList );
		}
		return 0;
	}

	if( root->isTextPart() ) {
		textList->push_back( root );
	}

	return 0;
}

int SP_MimePartUtils :: getTextMessage( SP_MimePart * root, SP_MimeBuffer * text )
{
	SP_MimePartVector textList;

	getTextList( root, &textList );

	if( textList.size() > 0 ) {
		SP_MimePart * part = textList[0];

		text->write( part->getContent()->getBuffer(), part->getContent()->getWritePos() );
	}

	return textList.size() > 0 ? 0 : -1;
}

