/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimeaddr_hpp__
#define __spmimeaddr_hpp__

#include <vector>

class SP_MimeMailbox;
class SP_MimeAddress;
class SP_MimeGroup;
class SP_MimeAddressList;

typedef std::vector< SP_MimeMailbox * > SP_MimeMailboxVector;
typedef std::vector< SP_MimeAddress * > SP_MimeAddressVector;

class SP_MimeAddress {
public:
	SP_MimeAddress( int isGroup );
	virtual ~SP_MimeAddress();

	virtual void dump() = 0;

	int isGroup() const;

	SP_MimeMailbox * toMailbox() const;

	SP_MimeGroup * toGroup() const;

public:

	static int parse( const char * str, SP_MimeAddressList * addrList );

protected:
	int mIsGroup;

	void safeFree( void * ptr );
	static char * mNull;
};

class SP_MimeAddressList {
public:
	SP_MimeAddressList();
	~SP_MimeAddressList();

	void dump();

	int getCount();

	SP_MimeAddress * getItem( int index );

	void append( SP_MimeAddress * address );

	/**
	 * Returns a flat list of all mailboxes represented
	 * in this address list. Use this if you don't care
	 * about grouping. 
	 *
	 * Note: Returns value is an internal list, cannot
	 *       been deleted.
	 */
	SP_MimeMailboxVector * getFlatList();

private:
	SP_MimeAddressVector mList;
	SP_MimeMailboxVector mFlatList;
};

class SP_MimeMailbox : public SP_MimeAddress {
public:
	SP_MimeMailbox();
	virtual ~SP_MimeMailbox();

	virtual void dump();

	void setMailbox( const char * mailbox );
	const char * getMailbox() const;

	void setDomain( const char * domain );
	const char * getDomain() const;

	void setName( const char * name );
	const char * getName() const;

	void setRoute( const char * route );
	const char * getRoute() const;

private:
	char * mMailbox, * mDomain, * mName, * mRoute;
};

class SP_MimeGroup : public SP_MimeAddress {
public:
	SP_MimeGroup();
	virtual ~SP_MimeGroup();

	virtual void dump();

	void setName( const char * name );
	const char * getName() const;

	SP_MimeMailboxVector * getMailboxVector();

private:
	SP_MimeMailboxVector mList;
	char * mName;
};

#endif

