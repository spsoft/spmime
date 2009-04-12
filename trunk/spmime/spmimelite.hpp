/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimelite_hpp__
#define __spmimelite_hpp__

class SP_MimePart;
class SP_MimeHeaderList;

class SP_MimeLiteParser {
public:
	SP_MimeLiteParser();
	~SP_MimeLiteParser();

	int parse( const char * buffer, int len );

	SP_MimePart * getRootPart();

	static int parseHeader( const char * buffer, SP_MimeHeaderList * list );

private:
	static int parse( const char * buffer, int len, SP_MimePart * part );

	SP_MimePart * mRootPart;
	SP_MimePart * mCurrent;
};

#endif

