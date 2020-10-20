#include	<stdlib.h>
#include	<mbstring.h>
#include	<windows.h>
#define	__TMC_NODECLARE_FUNC__
#include	"tmctools.h"

/*	Declarations	*/
FNAME	*set_FNAME( LPTSTR, FNAME * );
extern	LPTSTR	getshortname( LPTSTR, int );

/*	Definitions	*/
FNAME*	set_FNAME( LPTSTR lpCmdLine, FNAME *fname_buf )
/*
	TeX ファイル名から FNAME 構造体を作成する

	引数：
		LPTSTR lpCmdLine	:	TeX ファイル名を含む文字列へのポインタ
		FNAME *fname_buf	:	結果を格納する FNAME 構造体へのポインタ
	戻り値：
		FNAME*	:	格納に成功したら = fname_buf, 失敗したら = NULL
*/
{
	int		i;

	_mbscpy(fname_buf->filename,lpCmdLine);
	for(i=0;i<2;i++){
		if( (fname_buf->ishortname[i]
			= getshortname(fname_buf->filename,i) - fname_buf->filename)
			<= 0 ) return NULL;
	}
	if( fname_buf->ishortname[0] > fname_buf->ishortname[1] ) return NULL;

	return fname_buf;
}

