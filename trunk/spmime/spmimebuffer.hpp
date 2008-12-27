/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimebuffer_hpp__
#define __spmimebuffer_hpp__

#include <ctype.h>

class SP_MimeBuffer {
public:
	SP_MimeBuffer( size_t allocSize = 0 );

	~SP_MimeBuffer();

	// make buffer empty
	void reset();

	size_t getReadPos();

	void moveReadPos( int len );

	// get pointer to read data
	const char * getReadPtr();

	// @return read bytes
	int read( char * buffer, size_t len );

	// ensure enough space for writing new data
	int ensureWriteSpace( size_t writeSpace );

	size_t getWritePos();

	void moveWritePos( int len );

	// get pointer to write data
	char * getWritePtr();

	// @return write bytes
	int write( const char * buffer, size_t len );

	// length of the internal buffer
	size_t getAllocSize();

	char * getBuffer();

	void moveTo( SP_MimeBuffer * other );

	int getc();

public:
	char * mBuffer;    // allocated storage pointer
	size_t mReadPos;   // index to data that is ready to be read
	size_t mWritePos;  // index to data that is still to be written
	size_t mAllocSize; // allocated size for data
};

#endif

