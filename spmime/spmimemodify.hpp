/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimemodify_hpp__
#define __spmimemodify_hpp__

class SP_MimeSimpleEditor {
public:
	SP_MimeSimpleEditor( const char * data );
	~SP_MimeSimpleEditor();

	void addHeader( const char * name, const char * value );

	int removeHeader( const char * name, int index );

	char * getNewData();

private:
	char * mHeader;
	const char * mBody;
};

#endif

