#include	<mbstring.h>
#include	<windows.h>
#define	__TMC_NODECLARE_FUNC__
#include	"tmctools.h"

/*	Declarations	*/
LPTSTR	getshortname( LPTSTR, int );

/*	Definitions	*/
LPTSTR	getshortname( LPTSTR filename, int ptexversion )
/*
	TeX ファイル名から LOG, AUX, DVI ファイルetc. のファイルノード名を
	得るための終端ポインタを取得

	引数：
		LPTSTR filename	:	TeX ファイル名へのポインタ
		int ptexversion	:	pTeX のマイナーバージョン - 4
							(ex. 2.1.4 -> = 0, 2.1.5 -> = 1)
	戻り値：
		LPTSTR	:	ファイルベース名の
						ptexversion = 0	:	最後の
						ptexversion = 1	:	(ファイルベース名先頭を除く)最初の
					ピリオドのアドレス
*/
{
	LPTSTR	s;

	if( (s = _mbsrchr(filename,'\\')) == NULL ) s = filename;
	else s++;
	if( *s == '.' ) s++;
	return ptexversion?_mbsrchr(s,'.'):_mbschr(s,'.');
}

