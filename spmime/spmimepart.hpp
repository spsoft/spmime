/*
 * Copyright 2008 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spmimepart_hpp__
#define __spmimepart_hpp__

#include <vector>

class SP_MimePart;
class SP_MimePartList;
class SP_MimeHeaderList;

class SP_MimeBuffer;

typedef std::vector< SP_MimePart * > SP_MimePartVector;

class SP_MimePart {
public:
	SP_MimePart();
	~SP_MimePart();

	SP_MimeHeaderList * getHeaderList() const;

	bool isMultipart() const;

	bool isMessage() const;

	bool isTextPart() const;

	bool isAttachment() const;

	SP_MimePartList * getBodyPartList() const;

	SP_MimeBuffer * getContent() const;
	SP_MimeBuffer * getPreamble() const;
	SP_MimeBuffer * getEpilogue() const;

	void setParent( SP_MimePart * parent );
	SP_MimePart * getParent() const;

private:
	SP_MimeHeaderList * mHeaderList;
	SP_MimePartList * mBodyPartList;
	SP_MimePart * mParent;

	SP_MimeBuffer * mContent;
	SP_MimeBuffer * mPreamble;
	SP_MimeBuffer * mEpilogue;
};

class SP_MimePartList {
public:
	SP_MimePartList();
	~SP_MimePartList();

	int getCount() const;

	SP_MimePart * getItem( int index ) const;

	void append( SP_MimePart * part );

private:
	SP_MimePartVector mList;
};

class SP_MimePartUtils {
public:
	static int getAttachmentList( SP_MimePart * root, SP_MimePartVector * attachmentList );

	static int getTextList( SP_MimePart * root, SP_MimePartVector * textList );

	static int getTextMessage( SP_MimePart * root, SP_MimeBuffer * text );

private:
	SP_MimePartUtils();
};

#endif

