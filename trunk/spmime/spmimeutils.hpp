/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimeutils_hpp__
#define __spmimeutils_hpp__

#include <sys/types.h>

class SP_MimeBuffer;

class SP_MimeUtils {
public:

	/**
	 * @brief get the value of a header
	 * @return NOT NULL : found, NULL : not found
	 */
	static const char * getValue( const char * mailData,
			const char * name, char * value, size_t len );

	/**
	 * @brief duplicate the value of a header, the trailing CRLF is removed
	 * @return NOT NULL : found, NULL : not found
	 * @note caller must free the return value
	 */
	static char * dupValue( const char * mailData, const char * name );

	/**
	 * @brief find the position of a header
	 * @return the begin position of the header name
	 * @param len : how many bytes of the header, include name and trailing CRLF
	 */
	static const char * getHeader( const char * mailData,
			const char * name, size_t * len );

	/**
	 * @brief remove a header
	 * @return 0 : found and remove, -1 : not found the header
	 */
	static int removeHeader( char * mailData, const char * name );

	/**
	 * @brief find the position of the first empty line
	 * @return if not empty line, then return the end of the data
	 */
	static const char * getFirstEmpLine( const char * mailData );

	/**
	 * @brief duplicate the whole mail header
	 */
	static char * dupHeader( const char * mailData );

	static int getParam( const char * value, const char * name,
			char * param, size_t len );

	static int getBoundary( const char * mailData, char * boundary, size_t len );

	static int getCharset( const char * mailData, char * charset, size_t len );

	static bool startsCaseWith( const char *haystack, const char *needle );

	static int readFile( const char * path, SP_MimeBuffer * buffer );

private:
	SP_MimeUtils();
};

#endif

