#include	<stdlib.h>
#include	<mbstring.h>
#include	<windows.h>
#define	__TMC_NODECLARE_FUNC__
#include	"tmctools.h"

/*	Declarations	*/
unsigned char* getinifilename( unsigned char * );

/*	Definitions	*/
unsigned char* getinifilename( unsigned char *buf )
/*
	dyndata.ini のフルパス名を取得

	引数：
		unsigned char *buf	:	INI ファイル名を格納するバッファへのポインタ
	戻り値：
		unsigned char*	:	失敗したら = NULL, 成功なら = buf
*/
{
	HKEY	hKeyTMC;
	DWORD	len = (DWORD )MAX_LINE, dwType_SZ = REG_SZ;

	if( RegOpenKeyEx(	HKEY_CURRENT_USER,
						RK_LM_TMC_MACROCFG,
						0,
						KEY_ALL_ACCESS,
						&hKeyTMC ) != ERROR_SUCCESS ) return NULL;
	if( RegQueryValueEx(	hKeyTMC,
							RV_TMC_MACRODIR,
							0,
							&dwType_SZ,
							buf,
							&len ) != ERROR_SUCCESS ) buf = NULL;
	RegCloseKey(hKeyTMC);
	if( buf != NULL ) _mbscat(buf,DYN_INI);
	return buf;
}

