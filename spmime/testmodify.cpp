/*
 * Copyright 2009 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "spmimemodify.hpp"

int main( int argc, char * argv[] )
{
	const char * mail = "From: sender@foo.bar\r\n"
			"To: to1@foo.bar\r\n"
			"To: to2@foo.bar\r\n"
			"To: to3@foo.bar\r\n"
			"Cc: cc1@foo.bar\r\n"
			"Subject: test modify\r\n";

	SP_MimeSimpleEditor editor( mail );

	editor.addHeader( "X-Spam-Flag", "true" );
	editor.removeHeader( "To", 2 );
	editor.addHeader( "To", "cc2_1@foo.bar" );

	char * newMail = editor.getNewData();

	printf( "%s\n", newMail );

	free( newMail );

	return 0;
}


