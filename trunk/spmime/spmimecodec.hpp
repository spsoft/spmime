/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimecodec_hpp__
#define __spmimecodec_hpp__

#include <sys/types.h>

class SP_MimeBuffer;
class SP_MimePart;

class SP_MimeCodec {
public:
	static int decodePart( const SP_MimePart * src,
			const char * destCharset, SP_MimePart * dest );

	static int decodeHeader( const char * src, size_t len,
			const char * destCharset, SP_MimeBuffer * dest );

	static int decodeBase64( const char * src, size_t len, SP_MimeBuffer * dest );

	static int encodeBase64( const char * src, size_t len, SP_MimeBuffer * dest );

	static int decodeQuoted( const char * src, size_t len, SP_MimeBuffer * dest );

	enum { eUnknown, eBase64, eQP, e8Bit, e7Bit, eUuEncode, eBinary };
	static int getEncodingType( const char * name );

	static const char * getEncodingName( int type );

private:
	SP_MimeCodec();
	~SP_MimeCodec();

	typedef struct tagEncodingInfo {
		int mType;
		const char * mName;
		int mLen;
	} EncodingInfo_t;

	static EncodingInfo_t mEncodingArray[];
};

#endif

