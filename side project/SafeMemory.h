#define _AFXDLL
#include "afx.h"

#if !defined(AFX_SAFEMEMORY_H__30E2CE1A_8CA4_4030_812F_1F8DC907765F__INCLUDED_)
	#define AFX_SAFEMEMORY_H__30E2CE1A_8CA4_4030_812F_1F8DC907765F__INCLUDED_

	// check for success of newing a object
	#define SAFENEW( ptr, type )							\
	if( ( ptr ) != NULL ) {									\
		printf( "Attempt to new a non-null pointer\n" );	\
	}														\
	else if( ( ( ptr ) = new type ) == NULL ) {				\
		printf( "New failed\n" );							\
		ASSERT( FALSE );									\
	}

	// check pointer not null for deleting, then delete it and point it to null
	#define SAFEDELETE( ptr )								\
	if( ( ptr ) != NULL ) {									\
		delete ( ptr );										\
		( ptr ) = NULL;										\
	}

	// check array pointer not null for deleting, then delete it and point it to null
	#define SAFEDELETE_ARRAY( ptr )							\
	if( ( ptr ) != NULL ) {									\
		delete [] ( ptr );									\
		( ptr ) = NULL;										\
	}
#endif
