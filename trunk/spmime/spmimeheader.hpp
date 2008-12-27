/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimeheader_hpp__
#define __spmimeheader_hpp__

#include <vector>

class SP_MimeHeader {
public:
	SP_MimeHeader();
	~SP_MimeHeader();

	void setName( const char * name );
	const char * getName() const;

	void setValue( const char * value );
	void directSetValue( char * value );
	const char * getValue() const;

	int getParam( const char * name, char * value, int len ) const;

private:

	SP_MimeHeader & operator=( const SP_MimeHeader & );
	SP_MimeHeader( SP_MimeHeader & );

	char * mName;
	char * mValue;
};

class SP_MimeHeaderList {
public:
	SP_MimeHeaderList();
	~SP_MimeHeaderList();

	int getCount() const;

	const SP_MimeHeader * getItem( int index ) const;

	const SP_MimeHeader * find( const char * name ) const;

	void append( SP_MimeHeader * header );

	int getBoundary( char * boundary, int len ) const;

	int getCharset( char * charset, int len ) const;

private:
	typedef std::vector< SP_MimeHeader * > HeaderList;
	HeaderList mList;
};

#endif

