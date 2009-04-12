/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "spmimeutils.hpp"
#include "spmimebuffer.hpp"

const char * SP_MimeUtils :: getHeader( const char * mailData,
		const char * name, size_t * len )
{
	const char * ret = NULL;

	const char * next = mailData;
	int nameLen = strlen( name );

	for( ; ; )
	{
		if( 0 == strncasecmp( next, name, nameLen ) )
		{
			const char * colon = next + nameLen;

			for( ; isspace( *colon ) && '\n' != *colon; ) colon++;

			if( ':' == *colon )
			{
				colon++;

				ret = next;

				// find the end of the multi-line header
				for( ; ; )
				{
					next = strchr( next, '\n' );
					if( NULL != next )
					{
						next++;

						// reach the end of headers
						if( '\r' == *next || '\n' == *next ) break;

						// reach the next header
						if( ! isspace( *next ) ) break;
					} else {
						break;
					}
				}

				if( NULL != next )
				{
					*len = next - ret;
				} else {
					*len = strlen( ret );
				}

				break;
			}
		}

		next = strchr( next, '\n' );
		if( NULL != next )
		{
			next++;
			// reach the end of headers
			if( '\r' == *next || '\n' == *next ) break;
		} else {
			break;
		}
	}

	return ret;
}

const char * SP_MimeUtils :: getValue( const char * mailData,
		const char * name, char * value, size_t len )
{
	int found = 0;

	size_t headerLen = 0;
	const char * header = getHeader( mailData, name, &headerLen );

	if( NULL != header )
	{
		found = 1;

		char * valptr = strchr( header, ':' ) + 1;
		const char * endptr = header + headerLen;

		for( ; valptr < endptr && isspace( *valptr ); ) valptr++;

		char * outptr = value;
		for( size_t i = 0; i < ( len - 1 ) && valptr < endptr; i++ )
		{
			*outptr++ = *valptr++;
		}

		*outptr-- = '\0';

		// remove trailing space
		for( ; outptr >= value && isspace( *outptr ); )
		{
			*outptr-- = '\0';
		}
	}

	return found ? value : NULL;
}

char * SP_MimeUtils :: dupValue( const char * mailData, const char * name )
{
	char * ret = NULL;

	size_t len = 0;
	const char * ptr = getHeader( mailData, name, &len );

	if( NULL != ptr && len > 0 )
	{
		ret = (char*)malloc( len + 1 );
		getValue( ptr, name, ret, len + 1 );
	}

	return ret;
}

int SP_MimeUtils :: removeHeader( char * mailData, const char * name )
{
	int ret = -1;

	size_t len = 0;
	char * ptr = (char*)getHeader( mailData, name, &len );

	if( NULL != ptr && len > 0 )
	{
		size_t moveLen = strlen( ptr ) + 1 - len;
		memmove( ptr, ptr + len, moveLen );

		ret = 0;
	}

	return ret;
}

const char * SP_MimeUtils :: getFirstEmpLine( const char * mailData )
{
	const char * next = mailData;

	for( ; ; )
	{
		next = strchr( next, '\n' );
		if( NULL != next )
		{
			next++;
			// reach the end of headers
			if( '\r' == *next || '\n' == *next ) break;
		} else {
			next = strchr( mailData, '\0' );
			break;
		}
	}

	return next;
}

char * SP_MimeUtils :: dupHeader( const char * mailData )
{
	const char * firstEmpLine = getFirstEmpLine( mailData );

	size_t headerLen = firstEmpLine - mailData;
	char * header = (char*)malloc( headerLen + 1 );
	memmove( header, mailData, headerLen );
	header[ headerLen ] = '\0';

	return header;
}

int SP_MimeUtils :: getBoundary( const char * mailData, char * boundary, size_t len )
{
	int ret = -1;

	char * type = dupValue( mailData, "Content-Type" );
	if( NULL == type ) return ret;

	ret = getParam( type, "boundary", boundary, len );

	free( type );

	return ret;
}

int SP_MimeUtils :: getCharset( const char * mailData, char * charset, size_t len )
{
	int ret = -1;

	char * type = dupValue( mailData, "Content-Type" );
	if( NULL == type ) return ret;

	ret = getParam( type, "charset", charset, len );

	free( type );

	return ret;
}

bool SP_MimeUtils :: startsCaseWith( const char *haystack, const char *needle )
{
	return 0 == strncasecmp( haystack, needle, strlen( needle ) );
}

int SP_MimeUtils :: readFile( const char * path, SP_MimeBuffer * buffer )
{
	int ret = -1;

#ifndef WIN32
	int fd = ::open( path, O_RDONLY );
#else
	int fd = _open( path, O_RDONLY );
#endif

	if( fd >= 0 ) {
		struct stat fileStat;
		if( 0 == fstat( fd, &fileStat ) ) {
			buffer->ensureWriteSpace( fileStat.st_size );
			if( fileStat.st_size == read( fd, buffer->getWritePtr(), fileStat.st_size ) ) {
				buffer->moveWritePos( fileStat.st_size );
				ret = 0;
			}
		}

		close( fd );
	}

	return ret;
}

#define _ISSPECIAL(c) ( \
    ((c) == '(') || ((c) == ')') || ((c) == '<') || ((c) == '>') \
    || ((c) == '@') || ((c) == ',') || ((c) == ';') || ((c) == ':') \
    || ((c) == '\\') || ((c) == '.') || ((c) == '[') \
    || ((c) == ']') )

int SP_MimeUtils :: getParam( const char * value, const char * name,
		char * param, size_t len )
{
	int ret = -1;

	const char * pos = strchr( value, ';' );
	for( ; NULL != pos; ) {
		pos++;
		for( ; isspace( *pos ); ) pos++;

		const char * val = strchr( pos, '=' );
		if( NULL == val ) break;

		int realLen = 0;

		const char * end = ++val;
		int quoted = 0, isQuoted = 0;

		for( ; '\0' != *end && ( quoted || ( !_ISSPECIAL( *end ) && ! isspace( *end ) ) ); ) {
			if( '\"' == *end ) {
				quoted = ~quoted, isQuoted = 1;
			} else {
				realLen++;
			}
			end++;
		}

		if( 0 == strncasecmp( pos, name, strlen( name ) ) ) {
			if( isQuoted ) val++;
			len = ( realLen + 1 ) > (int)len ? len : ( realLen + 1 );
			strncpy( param, val, len );
			param[ len - 1 ] = '\0';

			ret = 0;
			break;
		} else {
			pos = strchr( end, ';' );
			continue;
		}
	}

	return ret;
}

int SP_MimeUtils :: getToken ( const char * src, int index, 
		char * dest, int len, char delimiter, const char ** next )
{
	int ret = 0;

	const char * pos1 = src, * pos2 = NULL;

	if( isspace( delimiter ) ) delimiter = 0;

	for( ; isspace( *pos1 ); ) pos1++;

	for ( int i = 0; i < index; i++ ) {
		if( 0 == delimiter ) {
			for( ; '\0' != *pos1 && !isspace( *pos1 ); ) pos1++;
			if( '\0' == *pos1 ) pos1 = NULL;
		} else {
			pos1 = strchr ( pos1, delimiter );
		}
		if ( NULL == pos1 ) break;

		pos1++;
		for( ; isspace( *pos1 ); ) pos1++;
	}

	*dest = '\0';
	if( NULL != next ) *next = NULL;

	if ( NULL != pos1 && '\0' != * pos1 ) {
		if( 0 == delimiter ) {
			for( pos2 = pos1; '\0' != *pos2 && !isspace( *pos2 ); ) pos2++;
			if( '\0' == *pos2 ) pos2 = NULL;
		} else {
			pos2 = strchr ( pos1, delimiter );
		}
		if ( NULL == pos2 ) {
			strncpy ( dest, pos1, len );
			if ( ((int)strlen(pos1)) >= len ) ret = -2;
		} else {
			if( pos2 - pos1 >= len ) ret = -2;
			len = ( pos2 - pos1 + 1 ) > len ? len : ( pos2 - pos1 + 1 );
			strncpy( dest, pos1, len );

			for( pos2++; isspace( *pos2 ); ) pos2++;
			if( NULL != next && '\0' != *pos2 ) *next = pos2;
		}
	} else {
		ret = -1;
	}

	dest[ len - 1 ] = '\0';
	len = strlen( dest );

	// remove tailing space
	for( ; len > 0 && isspace( dest[ len - 1 ] ); ) len--;
	dest[ len ] = '\0';

	return ret;
}

