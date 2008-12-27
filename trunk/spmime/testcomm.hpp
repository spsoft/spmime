/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __testcomm_hpp__
#define __testcomm_hpp__

#ifdef __cplusplus
extern "C" {
#endif

class SP_MimePart;

void printPart( const SP_MimePart * part );

void printDecodedPart( const SP_MimePart * part, const char * destCharset );

void printLine( const char * tag, const char * str );

#ifdef __cplusplus
}
#endif

#endif

