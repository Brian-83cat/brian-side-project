#include <stdio.h>
#include <afx.h>

#include "TypeDefine.h"
#include "UI.h"

// Core process message list
CHAR *BankOPMsgList[] = {
	// Show main menu.
	"========= 主選單 =========\n"
	"0.退出主選單\n"
	"1.玉山銀行\n"
	"2.中信銀行\n"
	"3.花旗銀行\n"
	"4.第一銀行\n"
	"5.台新銀行\n"
	"============================\n"
	"請輸入您的選擇(0-5):",

	// Show bank menu.
	"========= 銀行選單 =========\n"
	"0.退出銀行\n"
	"1.新增帳戶\n"
	"2.登入\n"
	"============================\n"
	"請輸入您的選擇(0-2):",

	// Show log in menu.
	"========= 帳戶選單 =========\n"
	"0.退出帳戶\n"
	"1.查詢帳戶餘額\n"
	"2.存款\n"
	"3.提款\n"
	"4.幣別兌換\n"
	"5.轉帳\n"
	"6.顯示交易明細\n"
	"7.跨行轉帳\n"
	"============================\n"
	"請輸入您的選擇(0-7):",

	// Main input error
	"輸入並不在範圍內，請重新輸入:",
	"未連線該銀行的資料，無法進行操作\n",
};

// Create account Core process message list.
CHAR *CreateAccountMsgList[] = {
	// Invalid account Info
	"無效的帳戶資訊",

	// Message list about account ID
	"請輸入註冊帳號(4~6位數字):",
	"帳號已申請過，請重新輸入:",
	"帳號並非4~6位數，請重新輸入:",

	// Message list about account password
	"密碼並非4~6位數，請重新輸入:",
	"請輸入帳戶密碼(4~6位數字):",

	// Message list about account NTD balance
	"請輸入開戶金額(0~999999NTD):",
	"台幣金額不在範圍內，請重新輸入金額:",

	// Message list about account US balance
	"請輸入開戶金額(0~999999US):",
	"美金金額不在範圍內，請重新輸入金額:",

	// Create account success message
	"--------- 開戶成功 ---------\n",
};

CHAR *InquireList[] = {
	// Inquire message
	"台幣餘額為:",
	"美金餘額為:",
};

// Log in process message list.
CHAR *LogInMsgList[] = {
	// Log in message
	"還未登入",
	"請輸入您的帳號(4~6位數字):",
	"查無此帳號\n",
	"請輸入您的帳戶密碼(4~6位數字):",
	"密碼輸入錯誤，請重新輸入:",
	"--------- 登入成功 ---------\n",

	// Inquire message
	"請選擇想查詢的幣別(0.台幣, 1.美金):",

	// Deposit message
	"請選擇想要存款的幣別(0.台幣, 1.美金):",
	"請輸入存款金額:",
	"輸入錯誤或金額過大，請重新輸入:",
	"美金輸入錯誤或金額過大，請重新輸入:",
	"--------- 存款成功 ---------\n",

	// Whithdraw message
	"請選擇想要提款的幣別(0.台幣, 1.美金):",
	"請輸入提款金額:",
	"輸入錯誤或金額過大，請重新輸入:",
	"美金輸入錯誤或金額過大，請重新輸入:",
	"帳戶無餘額提供提款",
	"--------- 提款成功 ---------\n",

	// Transfer message
	"請選擇想要轉帳的幣別(0.台幣, 1.美金):",
	"請輸入轉入帳戶:",
	"查無此轉入帳戶，請重新輸入轉入帳號",
	"請輸入轉帳金額:",
	"輸入錯誤或金額過大，請重新輸入:",
	"轉入帳戶的台幣金額為:",
	"美金輸入錯誤或金額過大，請重新輸入:",
	"轉入帳戶的美金金額為:",
	"轉出帳戶的台幣金額為:",
	"轉出帳戶的美金金額為:",
	"帳戶無餘額提供轉帳",
	"--------- 轉帳成功 ---------\n",

	// Transaction record message
	"未登入帳號，無法查詢交易明細",
	"[操作]",
	"[對象銀行]",
	"[對象]",
	"[幣別]",
	"[交易金額]",
	"[餘額]\n",

	// Exchange money message
	"請選擇想要的服務(0.換台幣, 1.換外幣):",
	"請選擇幣別(1.美金):",
	"想用多少金額兌換:",
	"帳戶餘額不夠兌換一個貨幣\n",
	"使用金額不夠兌換一個貨幣或大於帳戶餘額\n"
	"使用金額最少1元，最多只能使用",
	"使用金額不夠兌換一個貨幣或使用金額大於帳戶餘額\n"
	"使用金額最少29元，最多只能使用",
	"實際使用的台幣為",
	"--------- 兌換成功 ---------\n",

	// Bank transfer
	"想要跨行轉帳的銀行(0.玉山, 1.中信, 2.花旗, 3.第一, 4.台新):",
	"請輸入該銀行轉入帳戶:",
	"請選擇想要轉帳的幣別(0.台幣, 1.美金):",
	"請輸入轉帳金額:",
	"無效的帳戶資訊",
	"不可選本銀行，請重新選轉帳銀行(0.玉山, 1.中信, 2.花旗, 3.第一, 4.台新):",
	"查詢不到該銀行的帳戶\n",
	"帳戶無餘額可以進行轉帳\n",
	"轉帳金額最多只能輸入",
	"------- 跨行轉帳成功 -------\n",
};

CHAR *AdminMsgList[] = {
	// Admin message list
	"[帳戶ID]",
	"[台幣]",
	"[美金]\n",
	"---- 歡迎來到管理員模式 ----\n"
	"請輸入管理員帳號:",
	"請輸入管理員密碼:",
	"您並非管理員，請離開\n",
	"未登入管理員帳號，無法取得銀行資訊",
	"管理員您好，以下為本銀行的帳戶資訊及總資金:\n",
	"本銀行目前沒有帳戶\n",
};

CHAR *BankMoneyList[] = {
	// Inquire message
	"台幣總資金為:",
	"美金總資金為:",
};

CUI::CUI( void )
// Constructor
{
}

CUI::~CUI(void)
// Destructor
{
}

void CUI::GetInteger( INT &nInput )
// Get user integer input
{
	scanf( "%d", &nInput );
}

void CUI::GetString( CHAR* pInput )
// Get user string input
{
	ASSERT( pInput != NULL );
	scanf( "%s", pInput );
}

void CUI::ShowInteger( INT nOutput )
// Shoe integer output
{
	printf( "%d\n", nOutput );
}

void CUI::ShowATMOPMsg( EATMOPMsg EMsg )
// Show bank operator message
{
	printf( "%s", BankOPMsgList[ EMsg ] );
}

void CUI::ShowCreateAccountMsg( ECreateAccountMsg EMsg )
// Show create account process message
{
	printf( "%s", CreateAccountMsgList[ EMsg ] );
}

void CUI::ShowInquireMsg( INT nMsg, INT nMoney )
// Show log in process message
{
	printf( "%s%d\n", InquireList[ nMsg ], nMoney );
}

void CUI::ShowLogInMsg( ELogInMsg EMsg )
// Show log in process message
{
	printf( "%s", LogInMsgList[ EMsg ] );
}

void CUI::ShowAdminMsg( EAdminMsg EMsg )
// Show log in process message
{
	printf( "%s", AdminMsgList[ EMsg ] );
}

void CUI::ShowTransactionRecord( TTransactionRecord* pRecord, INT nNum )
// Show transaction record message
{
	printf( "%20s", LogInMsgList[ ELIM_SHOW_TRANSACTION_OPERATION ] );
	printf( "%20s", LogInMsgList[ ELIM_SHOW_TRANSACTION_SELECT_BANK ] );
	printf( "%20s", LogInMsgList[ ELIM_SHOW_TRANSACTION_SELECT ] );
	printf( "%20s", LogInMsgList[ ELIM_SHOW_TRANSACTION_KIND ] );
	printf( "%20s", LogInMsgList[ ELIM_SHOW_TRANSACTION_MONEY ] );
	printf( "%20s", LogInMsgList[ ELIM_SHOW_TRANSACTION_BALANCE ] );
	for( INT i = 0; i < nNum; i++ ) {
		printf( "%20s", pRecord[ i ].szOperation );
		printf( "%20s", pRecord[ i ].szSlaveBankName );
		printf( "%20s", pRecord[ i ].szSlaveAccountID );
		printf( "%20s", pRecord[ i ].szMoneyKind );
		printf( "%20d", pRecord[ i ].nTransferMoney );
		printf( "%20d\n", pRecord[ i ].nCurrentBalance );
	}
}

void CUI::ShowBankRecord( TAccount* pAllAccount, INT* pTotalMoney )
// Show bank record
{
	printf( "%20s", AdminMsgList[ EAM_ADMIN_ACCOUNTID ] );
	printf( "%20s", AdminMsgList[ EAM_ADMIN_NTD ] );
	printf( "%20s", AdminMsgList[ EAM_ADMIN_US ] );

	while( pAllAccount != NULL ) {
		printf( "%20s", pAllAccount->szAccountID );
		printf( "%20d", pAllAccount->nBalance[ EMK_NTD ] );
		printf( "%20d\n", pAllAccount->nBalance[ EMK_US ] );
		pAllAccount = pAllAccount->pNextAccount;
	}

	// Show bank money
	for( INT i = 0; i < EMK_Num; i++ ) {
		printf( "%s%d\n", BankMoneyList[ i ], *( pTotalMoney + i ) );
	}
}
