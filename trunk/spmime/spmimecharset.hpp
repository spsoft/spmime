/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimecharset_hpp__
#define __spmimecharset_hpp__

#include <sys/types.h>

class SP_MimeBuffer;

class SP_MimeCharset {
public:
	typedef int ( * ConvFunc_t ) ( const char * srcCharset, const char * src, size_t srcLen,
			const char * destCharset, SP_MimeBuffer * dest );

	// set your own conv function to replace default function
	static void setConvFunc( ConvFunc_t func );

	static int conv( const char * srcCharset, const char * src, size_t srcLen,
			const char * destCharset, SP_MimeBuffer * dest );

	static int defaultImpl( const char * srcCharset, const char * src, size_t srcLen,
			const char * destCharset, SP_MimeBuffer * dest );

private:
	static ConvFunc_t mFunc;

private:
	SP_MimeCharset();
};

#endif

