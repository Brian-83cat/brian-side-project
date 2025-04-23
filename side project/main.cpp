#include <stdio.h>
#include <stdlib.h>
#include <afx.h>

#include "SafeMemory.h"
#include "TypeDefine.h"
#include "UI.h"

#include "IBank.h"
#include "Bank.h"
#include "BankManager.h"
#include "ATMOP.h"

// Bank handling fee table
DOUBLE HandlingFeeTable[ EBN_Num ][ EBN_Num ] = {
	{ 0.0, 0.05, 0.10, 0.10, 0.10 },
	{ 0.05, 0.0, 0.15, 0.10, 0.10 },
	{ 0.10, 0.15, 0.0, 0.10, 0.10 },
	{ 0.10, 0.15, 0.10, 0.0, 0.10 },
	{ 0.10, 0.15, 0.10, 0.10, 0.0 }
};

// Admin ID table
CHAR *AdminIDTable[ EBN_Num ] = {
	"1234",
	"1234",
	"1234",
	"1234",
	"1234"
};

// Admin password table
CHAR *AdminPasswordTable[ EBN_Num ] = {
	"1234",
	"1234",
	"1234",
	"1234",
	"1234"
};

INT main( void )
{
	// Set bank setting data
	TBankSettingData BankSettingData[ EBN_Num ];
	for( INT i = 0; i < EBN_Num; i++ ) {
		strcpy( BankSettingData[ i ].szBankName, BankName[ i ] );
		strcpy( BankSettingData[ i ].AdminData.szAdminID, AdminIDTable[ i ] );
		strcpy( BankSettingData[ i ].AdminData.szAdminPassword, AdminPasswordTable[ i ] );
		memcpy( BankSettingData[ i ].HandlingFee, HandlingFeeTable[ i ], sizeof( DOUBLE ) * EBN_Num );
	}

	// New all the bank with bank name, bank rate, admin ID and admin Password
	CBank* pBank[ EBN_Num ];
	for( INT i = 0; i < EBN_Num; i++ ) {
		pBank[ i ] = NULL;
		SAFENEW( pBank[ i ], CBank( BankSettingData[ i ] ) );
	}

	CBankManager* pBankManager;
	CATMOP* pATMOP;

	pBankManager = NULL;
	pATMOP = NULL;

	// New bank manager
	SAFENEW( pBankManager, CBankManager( pBank ) );

	//New ATMOP
	SAFENEW( pATMOP, CATMOP( pBankManager ) );

	// Start application
	pATMOP->StartApplication();

	// Delete BankManager and ATMOP
	SAFEDELETE( pATMOP );
	SAFEDELETE( pBankManager );

	// Delete all the bank
	for( INT i = 0; i < EBN_Num; i++ ) {
		SAFEDELETE( pBank[ i ] );
	}

	return 0;
}
