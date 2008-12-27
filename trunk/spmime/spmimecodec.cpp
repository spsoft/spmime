/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>

#include "spmimecodec.hpp"

#include "spmimebuffer.hpp"
#include "spmimepart.hpp"
#include "spmimeheader.hpp"
#include "spmimeutils.hpp"
#include "spmimecharset.hpp"

int SP_MimeCodec :: decodePart( const SP_MimePart * src,
		const char * destCharset, SP_MimePart * dest )
{
	SP_MimeHeaderList * srcList = src->getHeaderList();
	SP_MimeHeaderList * destList = dest->getHeaderList();

	for( int i = 0; i < srcList->getCount(); i++ ) {
		SP_MimeBuffer tmp;

		const SP_MimeHeader * header = srcList->getItem( i );
		decodeHeader( header->getValue(), strlen( header->getValue() ),
				destCharset, &tmp );

		SP_MimeHeader * newHeader = new SP_MimeHeader();
		newHeader->setName( header->getName() );
		newHeader->setValue( tmp.getBuffer() );

		destList->append( newHeader );
	}

	// decode body
	if( NULL != src->getContent()->getBuffer() ) {
		const SP_MimeHeader * header = srcList->find( "Content-Transfer-Encoding" );

		SP_MimeBuffer tmpBuffer;

		if( NULL != header ) {
			int type = getEncodingType( header->getValue() );
			SP_MimeBuffer * content = src->getContent();

			if( eBase64 == type ) {
				decodeBase64( content->getBuffer(), content->getWritePos(), &tmpBuffer );
			} else if( eQP == type ) {
				decodeQuoted( content->getBuffer(), content->getWritePos(), &tmpBuffer );
			} else {
				tmpBuffer.write( content->getBuffer(), content->getWritePos() );
			}
		} else {
			tmpBuffer.write( src->getContent()->getBuffer(),
					src->getContent()->getWritePos() );
		}

		if( src->isTextPart() ) {
			char charset[ 32 ] = { 0 };
			srcList->getCharset( charset, sizeof( charset ) );

			if( 0 == strcasecmp( charset, destCharset ) ) {
				tmpBuffer.moveTo( dest->getContent() );
			} else {
				SP_MimeCharset::conv( charset, tmpBuffer.getBuffer(),
					tmpBuffer.getWritePos(), destCharset, dest->getContent() );
			}
		} else {
			tmpBuffer.moveTo( dest->getContent() );
		}
	}

	for( int i = 0; i < src->getBodyPartList()->getCount(); i++ ) {
		SP_MimePart * tmp = new SP_MimePart();
		dest->getBodyPartList()->append( tmp );
		decodePart( src->getBodyPartList()->getItem(i), destCharset, tmp );
	}

	return 0;
}

int SP_MimeCodec :: decodeHeader( const char * src, size_t len,
		const char * destCharset, SP_MimeBuffer * dest )
{
	dest->ensureWriteSpace( len );

	const char* start = src;
	const char* pos = src;
	const char* end = start + len;

	while( start < end ) {
		if( end - start > 8 && start[0] == '=' && start[1] == '?') {
			// start of encoded data, find the next position of '?','?' and '?='
			/// =?'charset'(*'language')?{B|Q}?'encoded_content'?=

			const char *q1, *q2, *q3;
			q1 = start + 2;
			while( q1 < end && *q1 != '?' ) q1++;
			q2 = q1 + 1;
			while( q2 < end && *q2 != '?' ) q2++;
			q3 = q2 + 1;
			while( q3 < end && *q3 != '?') q3++;

			if( end - q3 < 1 || q3[1] != '=') {
				start++;
				continue;
			}

			while( pos < start && isspace( *pos ) ) pos++;
			dest->write( pos, start - pos );

			start += 2; // point to charset
			q1++;       // point to encoding
			q2++;       // point to content

			char charset[ 32 ] = { 0 };
			strncpy( charset, start, sizeof( charset ) - 1 );
			if( NULL != strchr( charset, '?' ) ) *strchr( charset, '?' ) = '\0';

			// find the end
			if (*q1 == 'b' || *q1 == 'B') {
				if( 0 == strcasecmp( charset, destCharset ) ) {
					decodeBase64( q2, q3 - q2, dest );
				} else {
					SP_MimeBuffer temp;
					decodeBase64( q2, q3 - q2, &temp );
					SP_MimeCharset::conv( charset, temp.getBuffer(), temp.getWritePos(), destCharset, dest );
				}
			} else if ((*q1 == 'q' || *q1 =='Q') ) {
				if( 0 == strcasecmp( charset, destCharset ) ) {
					decodeQuoted( q2, q3 - q2, dest );
				} else {
					SP_MimeBuffer temp;
					decodeQuoted( q2, q3 - q2, &temp );
					SP_MimeCharset::conv( charset, temp.getBuffer(), temp.getWritePos(), destCharset, dest );
				}
			} else {
				start -= 1;
				continue;
			}

			// continue after the quoted data
			start = pos = q3 + 2;
		} else {
			start++;
		}
	}

	if (pos < end) dest->write( pos, end - pos );

	dest->getWritePtr()[ 0 ] = '\0';

	return 0;
}

int SP_MimeCodec :: decodeBase64( const char * src, size_t len, SP_MimeBuffer * dest )
{
	static int base64decode[] =
	{
		/*00*/    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
		/*08*/    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
		/*10*/    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
		/*18*/    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
		/*20*/    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
		/*28*/    -1,   -1,   -1,   62,   -1,   -1,   -1,   63,
		/*30*/    52,   53,   54,   55,   56,   57,   58,   59,
		/*38*/    60,   61,   -1,   -1,   -1,   -1,   -1,   -1,
		/*40*/    -1,    0,    1,    2,    3,    4,    5,    6,
		/*48*/     7,    8,    9,   10,   11,   12,   13,   14,
		/*50*/    15,   16,   17,   18,   19,   20,   21,   22,
		/*58*/    23,   24,   25,   -1,   -1,   -1,   -1,   -1,
		/*60*/    -1,   26,   27,   28,   29,   30,   31,   32,
		/*68*/    33,   34,   35,   36,   37,   38,   39,   40,
		/*70*/    41,   42,   43,   44,   45,   46,   47,   48,
		/*78*/    49,   50,   51,   -1,   -1,   -1,   -1,   -1
	};

	char inBytes[ 8 ] = { 0 };
	int inCount = 0, padCount = 0;

	dest->ensureWriteSpace( len * 3 / 4 + 16 );

	for( size_t i = 0; i < len; i++ ) {
		int ch = src[i];

		if( isspace( ch ) ) continue;

		if( '=' == ch ) {
			inBytes[ inCount++ ] = 0;
			padCount++;
		} else {
			int byteVal = base64decode[ ch & 0x7f ];

			if( byteVal >= 0 ) {
				inBytes[ inCount++ ] = byteVal;
			} else {
				// invalid base64 char
			}
		}

		if( inCount >= 4 ) {
			unsigned char b0 = ((inBytes[0]<<2) & 0xfc) | ((inBytes[1]>>4) & 0x03);
			unsigned char b1 = ((inBytes[1]<<4) & 0xf0) | ((inBytes[2]>>2) & 0x0f);
			unsigned char b2 = ((inBytes[2]<<6) & 0xc0) | ((inBytes[3]   ) & 0x3f);

			dest->write( (char*)&b0, 1 );
			if( padCount < 2 ) dest->write( (char*)&b1, 1 );
			if( padCount < 1 ) dest->write( (char*)&b2, 1 );

			inCount = 0;
		}
	}

	dest->getWritePtr()[0] = '\0';

	return 0;
}

int SP_MimeCodec :: encodeBase64( const char * src, size_t len, SP_MimeBuffer * dest )
{
	static char *base64encode =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	dest->ensureWriteSpace( len * 4 / 3 + 16 );

	const unsigned char * s = (unsigned char*)src;
	size_t n = len, i = 0;

	for( ; n > 2; n -= 3, s += 3 ) {
		unsigned long m = ( (unsigned long)s[0] << 16 ) | ( (unsigned long)s[1] << 8 ) | s[2];

		for( i = 4; i > 0; m >>= 6 ) {
			dest->getWritePtr()[ --i ] = base64encode[ m & 0x3f ];
		}

		dest->moveWritePos( 4 );
	}

	if( n > 0 ) {
		unsigned long m = 0;
		for( i = 0; i < n; i++ ) m = (m << 8) | *s++;
		for( ; i < 3; i++ ) m <<= 8;

		for( i++; i > 0; m >>= 6 ) {
			dest->getWritePtr()[ --i ] = base64encode[ m & 0x3F ];
		}

		for( i = 3; i > n; i-- ) {
			dest->getWritePtr()[ i ] = '=';
		}

		dest->moveWritePos( 4 );
	}

	dest->getWritePtr()[0] = '\0';

	return 0;
}

int SP_MimeCodec :: decodeQuoted( const char * src, size_t len, SP_MimeBuffer * dest )
{
	dest->ensureWriteSpace( len + 1 );

	for( size_t i = 0; i < len; i++ ) {
		if( '=' == src[i] && ( len - i ) > 2 && isxdigit( src[ i + 1 ] ) && isxdigit( src[ i + 2 ] ) ) {
			char c1 = tolower( src[ i + 1 ] ), c2 = tolower( src[ i + 2 ] );
			char c = ( c1 > '9' ? 10 + c1 - 'a' : c1 - '0' );
			c = c * 16 + ( c2 > '9' ? 10 + c2 - 'a' : c2 - '0' );

			dest->write( &c, 1 );
			i += 2;
		} else if( '_' == src[i] ) {
			dest->write( " ", 1 );
		} else {
			dest->write( src + i, 1 );
		}
	}

	dest->getWritePtr()[0] = '\0';

	return 0;
}

SP_MimeCodec::EncodingInfo_t SP_MimeCodec :: mEncodingArray[] = {
	{ eBase64,   "base64",           6  },
	{ eQP,       "quoted-printable", 16 },
	{ e8Bit,     "8bit",             4  },
	{ e7Bit,     "7bit",             4  },
	{ eUuEncode, "uuencode",         8  },
	{ eBinary,   "binary",           6  },
	{ eUnknown,  "",                 0  }
};

int SP_MimeCodec :: getEncodingType( const char * name )
{
	int len = strlen( name );

	int ret = eUnknown;
	for( int i = 0; ; i++ ) {
		EncodingInfo_t * info = &( mEncodingArray[i] );
		if( eUnknown != info->mType ) {
			if( len == info->mLen && 0 == strcasecmp( name, info->mName ) ) {
				ret = info->mType;
				break;
			}
		} else {
			break;
		}
	}

	return ret;
}

const char * SP_MimeCodec :: getEncodingName( int type )
{
	const char * ret = "";

	for( int i = 0; ; i++ ) {
		EncodingInfo_t * info = &( mEncodingArray[i] );
		if( eUnknown != info->mType ) {
			if( type == info->mType ) {
				ret = info->mName;
				break;
			}
		} else {
			break;
		}
	}

	return ret;
}

