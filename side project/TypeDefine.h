#ifndef _TYPEDEFINE_H_
#define _TYPEDEFINE_H_

#define DATA_LIMIT ( 256 )
#define RECORD_LIMIT ( 200 )

enum EBankName {
// Exchange kind

	// 玉山銀行
	EBN_E_SUN = 0,
	// 中信銀行
	EBN_CTBC,
	// 花旗銀行
	EBN_CITI,
	// 第一銀行
	EBN_FCB,
	// 台新銀行
	EBN_TSIB,
	EBN_Num,
};

enum EExchangeKind {
// Exchange kind

	EEK_CASHBUY = 0,
	EEK_CASHSALE,
	EEK_Num,
};

enum EMoneyKind {
// Money kind

	EMK_NTD = 0,
	EMK_US,
	EMK_Num,
};

enum EMainMenu {
// Exchange kind

	EMM_EXIT = 0,
	// 玉山銀行
	EMM_E_SUN,
	// 中信銀行
	EMM_CTBC,
	// 花旗銀行
	EMM_CITI,
	// 第一銀行
	EMM_FCB,
	// 台新銀行
	EMM_TSIB,
	EMM_Num,
};

enum EMainMenuFunction {
// Use in main menu

	EMMF_EXIT = 0,
	EMMF_CREATEACCOUNT,
	EMMF_LOGINACCOUNT,
	EMMF_ADMIN,
	EMMF_Num,
};

enum ELogInMenuFunction {
// Use in log in menu

	ELMF_EXIT = 0,
	ELMF_INQUIREBALANCE,
	ELMF_DEPOSIT,
	ELMF_WITHDRAW,
	ELMF_EXCHANGE,
	ELMF_TRANSFER,
	ELMF_TRANSACTION_RECORD,
	ELMF_BANKTRANSFER,
	ELMF_Num,
};

static CHAR *BankName[ EBN_Num ] = {
	// Bank name
	"E_SUN",
	"CTBC",
	"CITI",
	"FCB",
	"TSIB",
};

static CHAR *MoneyKindList[ EMK_Num ] = {
	// Money kind
	"NTD",
	"US",
};

struct TTransactionRecord {

	// Operation
	CHAR szOperation[ DATA_LIMIT ];

	// SlaveAccount
	CHAR szSlaveBankName[ DATA_LIMIT ];

	// SlaveAccount
	CHAR szSlaveAccountID[ DATA_LIMIT ];

	// Money kind
	CHAR szMoneyKind[ DATA_LIMIT ];

	// Transfer money
	INT nTransferMoney;

	// Current balance
	INT nCurrentBalance;

	TTransactionRecord( void )
	// Constructor
	{
		szOperation[ 0 ] = '\0';
		szSlaveBankName[ 0 ] = '\0';
		szSlaveAccountID[ 0 ] = '\0';
		szMoneyKind[ 0 ] = '\0';
		nTransferMoney = 0;
		nCurrentBalance = 0;
	}
};

struct TAccount {
// Build account unit include ID, password, balance and pointer of next account.

	// AccountID
	CHAR szAccountID[ DATA_LIMIT ];

	// Account password
	CHAR szAccountPassword[ DATA_LIMIT ];

	// TND money
	INT nBalance[ EMK_Num ];

	// Remeber record count
	INT nRecordNum;

	// Transcition record
	TTransactionRecord Record[ RECORD_LIMIT ];

	// Point to next account
	TAccount *pNextAccount;

	TAccount( void )
	// Constructor
	{
		szAccountID[ 0 ] = '\0';
		szAccountPassword[ 0 ] = '\0';
		memset( nBalance, 0, sizeof( nBalance ) );
		nRecordNum = 0;
		pNextAccount = NULL;
	}
};

struct TTransferData {
// Build transfer data unit.

	// Transfer out ID
	CHAR szTransferOutID[ DATA_LIMIT ];

	// Log in bank index
	INT nLogInBankIdx;

	// Transfer in ID
	CHAR szTransferInID[ DATA_LIMIT ];

	// Select bank index
	INT nSelectBankIdx;

	// Money kind
	INT nMoneyKind;

	// Money
	INT nMoney;

	// Transfer out money
	INT nTransferOutMoney;

	TTransferData( void )
	// Constructor
	{
		szTransferOutID[ 0 ] = '\0';
		szTransferInID[ 0 ] = '\0';
		nLogInBankIdx = 0;
		nSelectBankIdx = 0;
		nMoneyKind = 0;
		nMoney = 0;
		nTransferOutMoney = 0;
	}
};

struct TAdminData {
// Build admin data unit.

	// Admin ID
	CHAR szAdminID[ DATA_LIMIT ];

	// Admin password
	CHAR szAdminPassword[ DATA_LIMIT ];

	TAdminData( void )
	// Constructor
	{
		szAdminID[ 0 ] = '\0';
		szAdminPassword[ 0 ] = '\0';
	}
};

struct TBankSettingData {
// Build admin data unit.

	// Admin data
	TAdminData AdminData;

	// Bank name
	CHAR szBankName[ DATA_LIMIT ];

	// Bank fee
	DOUBLE HandlingFee[ EBN_Num ];

	TBankSettingData( void )
	// Constructor
	{
		szBankName[ 0 ] = '\0';
		memset( HandlingFee, 0, sizeof( HandlingFee ) );
	}
};

#endif
