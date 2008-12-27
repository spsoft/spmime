/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "spmimecodec.hpp"
#include "spmimebuffer.hpp"
#include "spmimecharset.hpp"

int main( int argc, char * argv[] )
{
	{
		const char * src = "0123456789";

		SP_MimeBuffer dest;
		SP_MimeCodec::encodeBase64( src, strlen( src ), &dest );

		printf( "encodeBase64 %s -> %s\n", src, dest.getBuffer() );

		SP_MimeBuffer plain;

		SP_MimeCodec::decodeBase64( dest.getBuffer(), dest.getWritePos(), &plain );

		printf( "decodeBase64 %s -> %s\n", dest.getBuffer(), plain.getBuffer() );
	}

	{
		const char * qp = "Re:Re:_=C7=EB=BD=CCspserver=B5=C4=CE=CA=CC=E2";

		SP_MimeBuffer plain;

		SP_MimeCodec::decodeQuoted( qp, strlen( qp ), &plain );

		printf( "decodeQuoted %s -> %s\n", qp, plain.getBuffer() );
	}

	{
		const char * hdr = "=?gbk?Q?Re:Re:_=C7=EB=BD=CCspserver=B5=C4=CE=CA=CC=E2?=";

		SP_MimeBuffer plain;

		SP_MimeCodec::decodeHeader( hdr, strlen( hdr ), "utf8", &plain );
		printf( "decodeHeader(utf8) %s -> %s\n", hdr, plain.getBuffer() );

		hdr = "=?UTF-8?B?U09B5Lit5Zu95oqA5pyv6K665Z2bMjAwOOmXqOelqOmZkOmHj+WPkemAgQ==?=";
		plain.reset();
		SP_MimeCodec::decodeHeader( hdr, strlen( hdr ), "gbk", &plain );
		printf( "decodeHeader(gbk) %s -> %s\n", hdr, plain.getBuffer() );

		hdr = "=?GB2312?B?IsbV1KrI7bz+Ig==?= <enewsletter@enewsletter.com>";
		plain.reset();
		SP_MimeCodec::decodeHeader( hdr, strlen( hdr ), "gbk", &plain );
		printf( "decodeHeader(gbk) %s -> %s\n", hdr, plain.getBuffer() );

		hdr = "=?ISO-8859-1?B?SWYgeW91IGNhbiByZWFkIHRoaXMgeW8=?="
				"=?ISO-8859-2?B?dSB1bmRlcnN0YW5kIHRoZSBleGFtcGxlLg==?=";
		plain.reset();
		SP_MimeCodec::decodeHeader( hdr, strlen( hdr ), "utf8", &plain );
		printf( "decodeHeader(utf8) %s -> %s\n", hdr, plain.getBuffer() );
	}

	{
		const char * src = "ÖÐÎÄ";

		SP_MimeBuffer utf8;
		int ret = SP_MimeCharset::conv( "gbk", src, strlen( src ), "utf8", &utf8 );
		if( 0 != ret ) printf( "conv %d, errno %d, %s\n", ret, errno, strerror( errno ) );
		printf( "conv gbk/utf8 %s -> %s\n", src, utf8.getBuffer() );

		SP_MimeBuffer gb;
		ret = SP_MimeCharset::conv( "utf8", utf8.getBuffer(), utf8.getWritePos(), "gbk", &gb );
		if( 0 != ret ) printf( "conv %d, errno %d, %s\n", ret, errno, strerror( errno ) );
		printf( "conv utf8/gbk %s -> %s\n", utf8.getBuffer(), gb.getBuffer() );
	}

	return 0;
}

