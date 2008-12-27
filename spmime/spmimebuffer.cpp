/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spmimebuffer.hpp"

SP_MimeBuffer :: SP_MimeBuffer( size_t allocSize )
{
	mAllocSize = allocSize;
	mBuffer = NULL;
	mReadPos = mWritePos = 0;

	if( mAllocSize > 0 ) {
		mBuffer = (char*)malloc( mAllocSize + 1 );
		mBuffer[ mAllocSize ] = '\0';
	}
}

SP_MimeBuffer :: ~SP_MimeBuffer()
{
	if( NULL != mBuffer ) free( mBuffer );
	mBuffer = NULL;

	reset();
}

void SP_MimeBuffer :: reset()
{
	mReadPos = mWritePos = 0;
}

int SP_MimeBuffer :: ensureWriteSpace( size_t writeSpace )
{
	size_t len = mAllocSize - mWritePos;

	if( writeSpace > len ) {
		mAllocSize = mAllocSize + ( writeSpace - len ) + 1;
		mAllocSize = ( mAllocSize + 7 ) & (~7);

		if( NULL != mBuffer ) {
			mBuffer = (char*)realloc( mBuffer, mAllocSize );
		} else {
			mBuffer = (char*)malloc( mAllocSize );
		}

		assert( NULL != mBuffer );

		mBuffer[ --mAllocSize ] = '\0';
	}

	return 0;
}

size_t SP_MimeBuffer :: getReadPos()
{
	return mReadPos;
}

const char * SP_MimeBuffer :: getReadPtr()
{
	assert( mReadPos <= mWritePos );
	return mBuffer + mReadPos;
}

void SP_MimeBuffer :: moveReadPos( int len )
{
	mReadPos += len;
	assert( mReadPos <= mWritePos );
}

int SP_MimeBuffer :: read( char * buffer, size_t len )
{
	int ret = mWritePos - mReadPos;

	if( ret > (int)len ) ret = len;

	if( ret > 0 ) {
		memcpy( buffer, mBuffer + mReadPos, ret );
		mReadPos += ret;
	}

	return ret;
}

size_t SP_MimeBuffer :: getWritePos()
{
	return mWritePos;
}

char * SP_MimeBuffer :: getWritePtr()
{
	assert( mWritePos <= mAllocSize );
	return mBuffer + mWritePos;
}

void SP_MimeBuffer :: moveWritePos( int len )
{
	mWritePos += len;
	assert( mWritePos <= mAllocSize );
}

int SP_MimeBuffer :: write( const char * buffer, size_t len )
{
	ensureWriteSpace( len );

	memcpy( getWritePtr(), buffer, len );
	moveWritePos( len );

	return 0;
}

size_t SP_MimeBuffer :: getAllocSize()
{
	return mAllocSize;
}

char * SP_MimeBuffer :: getBuffer()
{
	return mBuffer;
}

void SP_MimeBuffer :: moveTo( SP_MimeBuffer * other )
{
	if( NULL != other->mBuffer ) free( other->mBuffer );

	other->mAllocSize = mAllocSize;
	other->mBuffer = mBuffer;
	other->mReadPos = mReadPos;
	other->mWritePos = mWritePos;

	mAllocSize = 0;
	mBuffer = NULL;
	mReadPos = mWritePos = 0;
}

int SP_MimeBuffer :: getc()
{
	char ret = 0;

	if( read( &ret, 1 ) <= 0 ) ret = -1;

	return ret;
}

