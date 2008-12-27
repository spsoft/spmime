/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <unicode/ucnv.h>

#include "spmimeicu.hpp"
#include "spmimebuffer.hpp"

int SP_MimeICU :: conv( const char * srcCharset, const char * src, size_t srcLen,
			const char * destCharset, SP_MimeBuffer * dest )
{
	UErrorCode status = U_ZERO_ERROR;

	long destLen = ucnv_convert( destCharset, srcCharset, NULL, 0, src, srcLen, &status );

	if( U_BUFFER_OVERFLOW_ERROR == status ) {
		status = U_ZERO_ERROR;
	} else if( U_FAILURE( status ) ) {
		return -1;
	}

	dest->ensureWriteSpace( destLen + 1 );

	ucnv_convert( destCharset, srcCharset, dest->getWritePtr(), destLen + 1, src, srcLen, &status );
	dest->getWritePtr()[ destLen ] = '\0';
	dest->moveWritePos( destLen );

	return 0;
}

