#include <stdio.h>
#include <afx.h>

#include "TypeDefine.h"
#include "UI.h"

// Core process message list
CHAR *BankOPMsgList[] = {
	// Show main menu.
	"========= �D��� =========\n"
	"0.�h�X�D���\n"
	"1.�ɤs�Ȧ�\n"
	"2.���H�Ȧ�\n"
	"3.��X�Ȧ�\n"
	"4.�Ĥ@�Ȧ�\n"
	"5.�x�s�Ȧ�\n"
	"============================\n"
	"�п�J�z�����(0-5):",

	// Show bank menu.
	"========= �Ȧ��� =========\n"
	"0.�h�X�Ȧ�\n"
	"1.�s�W�b��\n"
	"2.�n�J\n"
	"============================\n"
	"�п�J�z�����(0-2):",

	// Show log in menu.
	"========= �b���� =========\n"
	"0.�h�X�b��\n"
	"1.�d�߱b��l�B\n"
	"2.�s��\n"
	"3.����\n"
	"4.���O�I��\n"
	"5.��b\n"
	"6.��ܥ������\n"
	"7.�����b\n"
	"============================\n"
	"�п�J�z�����(0-7):",

	// Main input error
	"��J�ä��b�d�򤺡A�Э��s��J:",
	"���s�u�ӻȦ檺��ơA�L�k�i��ާ@\n",
};

// Create account Core process message list.
CHAR *CreateAccountMsgList[] = {
	// Invalid account Info
	"�L�Ī��b���T",

	// Message list about account ID
	"�п�J���U�b��(4~6��Ʀr):",
	"�b���w�ӽйL�A�Э��s��J:",
	"�b���ëD4~6��ơA�Э��s��J:",

	// Message list about account password
	"�K�X�ëD4~6��ơA�Э��s��J:",
	"�п�J�b��K�X(4~6��Ʀr):",

	// Message list about account NTD balance
	"�п�J�}����B(0~999999NTD):",
	"�x�����B���b�d�򤺡A�Э��s��J���B:",

	// Message list about account US balance
	"�п�J�}����B(0~999999US):",
	"�������B���b�d�򤺡A�Э��s��J���B:",

	// Create account success message
	"--------- �}�ᦨ�\ ---------\n",
};

CHAR *InquireList[] = {
	// Inquire message
	"�x���l�B��:",
	"�����l�B��:",
};

// Log in process message list.
CHAR *LogInMsgList[] = {
	// Log in message
	"�٥��n�J",
	"�п�J�z���b��(4~6��Ʀr):",
	"�d�L���b��\n",
	"�п�J�z���b��K�X(4~6��Ʀr):",
	"�K�X��J���~�A�Э��s��J:",
	"--------- �n�J���\ ---------\n",

	// Inquire message
	"�п�ܷQ�d�ߪ����O(0.�x��, 1.����):",

	// Deposit message
	"�п�ܷQ�n�s�ڪ����O(0.�x��, 1.����):",
	"�п�J�s�ڪ��B:",
	"��J���~�Ϊ��B�L�j�A�Э��s��J:",
	"������J���~�Ϊ��B�L�j�A�Э��s��J:",
	"--------- �s�ڦ��\ ---------\n",

	// Whithdraw message
	"�п�ܷQ�n���ڪ����O(0.�x��, 1.����):",
	"�п�J���ڪ��B:",
	"��J���~�Ϊ��B�L�j�A�Э��s��J:",
	"������J���~�Ϊ��B�L�j�A�Э��s��J:",
	"�b��L�l�B���Ѵ���",
	"--------- ���ڦ��\ ---------\n",

	// Transfer message
	"�п�ܷQ�n��b�����O(0.�x��, 1.����):",
	"�п�J��J�b��:",
	"�d�L����J�b��A�Э��s��J��J�b��",
	"�п�J��b���B:",
	"��J���~�Ϊ��B�L�j�A�Э��s��J:",
	"��J�b�᪺�x�����B��:",
	"������J���~�Ϊ��B�L�j�A�Э��s��J:",
	"��J�b�᪺�������B��:",
	"��X�b�᪺�x�����B��:",
	"��X�b�᪺�������B��:",
	"�b��L�l�B������b",
	"--------- ��b���\ ---------\n",

	// Transaction record message
	"���n�J�b���A�L�k�d�ߥ������",
	"[�ާ@]",
	"[��H�Ȧ�]",
	"[��H]",
	"[���O]",
	"[������B]",
	"[�l�B]\n",

	// Exchange money message
	"�п�ܷQ�n���A��(0.���x��, 1.���~��):",
	"�п�ܹ��O(1.����):",
	"�Q�Φh�֪��B�I��:",
	"�b��l�B�����I���@�ӳf��\n",
	"�ϥΪ��B�����I���@�ӳf���Τj��b��l�B\n"
	"�ϥΪ��B�̤�1���A�̦h�u��ϥ�",
	"�ϥΪ��B�����I���@�ӳf���ΨϥΪ��B�j��b��l�B\n"
	"�ϥΪ��B�̤�29���A�̦h�u��ϥ�",
	"��ڨϥΪ��x����",
	"--------- �I�����\ ---------\n",

	// Bank transfer
	"�Q�n�����b���Ȧ�(0.�ɤs, 1.���H, 2.��X, 3.�Ĥ@, 4.�x�s):",
	"�п�J�ӻȦ���J�b��:",
	"�п�ܷQ�n��b�����O(0.�x��, 1.����):",
	"�п�J��b���B:",
	"�L�Ī��b���T",
	"���i�糧�Ȧ�A�Э��s����b�Ȧ�(0.�ɤs, 1.���H, 2.��X, 3.�Ĥ@, 4.�x�s):",
	"�d�ߤ���ӻȦ檺�b��\n",
	"�b��L�l�B�i�H�i����b\n",
	"��b���B�̦h�u���J",
	"------- �����b���\ -------\n",
};

CHAR *AdminMsgList[] = {
	// Admin message list
	"[�b��ID]",
	"[�x��]",
	"[����]\n",
	"---- �w��Ө�޲z���Ҧ� ----\n"
	"�п�J�޲z���b��:",
	"�п�J�޲z���K�X:",
	"�z�ëD�޲z���A�����}\n",
	"���n�J�޲z���b���A�L�k���o�Ȧ��T",
	"�޲z���z�n�A�H�U�����Ȧ檺�b���T���`���:\n",
	"���Ȧ�ثe�S���b��\n",
};

CHAR *BankMoneyList[] = {
	// Inquire message
	"�x���`�����:",
	"�����`�����:",
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
