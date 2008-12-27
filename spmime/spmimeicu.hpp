/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimeicu_hpp__
#define __spmimeicu_hpp__

class SP_MimeBuffer;

class SP_MimeICU {
public:
	static int conv( const char * srcCharset, const char * src, size_t srcLen,
			const char * destCharset, SP_MimeBuffer * dest );

private:
	SP_MimeICU();
};

#endif

