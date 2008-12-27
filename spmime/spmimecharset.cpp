/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "spmimecharset.hpp"
#include "spmimebuffer.hpp"

SP_MimeCharset::ConvFunc_t SP_MimeCharset :: mFunc = SP_MimeCharset::defaultImpl;

void SP_MimeCharset :: setConvFunc( ConvFunc_t func )
{
	mFunc = func;
}

int SP_MimeCharset :: conv( const char * srcCharset, const char * src, size_t srcLen,
			const char * destCharset, SP_MimeBuffer * dest )
{
	return mFunc( srcCharset, src, srcLen, destCharset, dest );
}

#ifndef WIN32

#include <iconv.h>

/*
--------------------------------------------------------------------
The PHP License, version 3.01
Copyright (c) 1999 - 2008 The PHP Group. All rights reserved.
--------------------------------------------------------------------

"This product includes PHP software, freely available from
<http://www.php.net/software/>".
*/

enum {
	PHP_ICONV_ERR_SUCCESS           = 0,
	PHP_ICONV_ERR_CONVERTER         = 1,
	PHP_ICONV_ERR_WRONG_CHARSET     = 2,
	PHP_ICONV_ERR_TOO_BIG           = 3,
	PHP_ICONV_ERR_ILLEGAL_SEQ       = 4,
	PHP_ICONV_ERR_ILLEGAL_CHAR      = 5,
	PHP_ICONV_ERR_UNKNOWN           = 6,
	PHP_ICONV_ERR_MALFORMED         = 7,
	PHP_ICONV_ERR_ALLOC             = 8
};

static int php_iconv_string(const char *in_p, size_t in_len,
		char **out, size_t *out_len, const char *in_charset, const char *out_charset)
{
	/*
	  iconv supports errno. Handle it better way.
	*/
	iconv_t cd;
	size_t in_left, out_size, out_left;
	char *out_p, *out_buf, *tmp_buf;
	size_t bsz, result = 0;
	int retval = PHP_ICONV_ERR_SUCCESS;

	*out = NULL;
	*out_len = 0;

	cd = iconv_open(out_charset, in_charset);

	if (cd == (iconv_t)(-1)) {
		if (errno == EINVAL) {
			return PHP_ICONV_ERR_WRONG_CHARSET;
		} else {
			return PHP_ICONV_ERR_CONVERTER;
		}
	}
	in_left= in_len;
	out_left = in_len + 32; /* Avoid realloc() most cases */ 
	out_size = 0;
	bsz = out_left;
	out_buf = (char *) malloc(bsz+1); 
	out_p = out_buf;

	while (in_left > 0) {

#if defined (__SVR4) && defined (__sun) 
		result = iconv(cd, (const char **) &in_p, &in_left, (char **) &out_p, &out_left);
#else
		result = iconv(cd, (char **) &in_p, &in_left, (char **) &out_p, &out_left);
#endif

		out_size = bsz - out_left;
		if (result == (size_t)(-1)) {
			if (errno == E2BIG && in_left > 0) {
				/* converted string is longer than out buffer */
				bsz += in_len;

				tmp_buf = (char*) realloc(out_buf, bsz+1);
				out_p = out_buf = tmp_buf;
				out_p += out_size;
				out_left = bsz - out_size;
				continue;	
			}
		}
		break;
	}

	if (result != (size_t)(-1)) {
		/* flush the shift-out sequences */ 
		for (;;) {
		   	result = iconv(cd, NULL, NULL, (char **) &out_p, &out_left);
			out_size = bsz - out_left;

			if (result != (size_t)(-1)) {
				break;
			}

			if (errno == E2BIG) {
				bsz += 16;
				tmp_buf = (char *) realloc(out_buf, bsz);
				
				out_p = out_buf = tmp_buf;
				out_p += out_size;
				out_left = bsz - out_size;
			} else {
				break;
			}
		}
	}

	iconv_close(cd);

	if (result == (size_t)(-1)) {
		switch (errno) {
			case EINVAL:
				retval = PHP_ICONV_ERR_ILLEGAL_CHAR;
				break;

			case EILSEQ:
				retval = PHP_ICONV_ERR_ILLEGAL_SEQ;
				break;

			case E2BIG:
				/* should not happen */
				retval = PHP_ICONV_ERR_TOO_BIG;
				break;

			default:
				/* other error */
				retval = PHP_ICONV_ERR_UNKNOWN;
				free(out_buf);
				return PHP_ICONV_ERR_UNKNOWN;
		}
	}
	*out_p = '\0';
	*out = out_buf;
	*out_len = out_size;
	return retval;
}

int SP_MimeCharset :: defaultImpl( const char * srcCharset, const char * src, size_t srcLen,
			const char * destCharset, SP_MimeBuffer * dest )
{
	char * result = NULL;
	size_t resultLen = 0;

	int ret = php_iconv_string( src, srcLen, &result, &resultLen, srcCharset, destCharset );

	if( 0 == ret )
	{
		dest->write( result, resultLen );
		free( result );
	}

	return 0 == ret ? 0 : -1;
}

#else

int SP_MimeCharset :: defaultImpl( const char * srcCharset, const char * src, size_t srcLen,
			const char * destCharset, SP_MimeBuffer * dest )
{
	return 0;
}

#endif

