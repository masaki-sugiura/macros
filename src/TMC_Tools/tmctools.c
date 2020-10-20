#include	<stdio.h>
#include	<stdlib.h>
#include	<mbstring.h>
#include	<windows.h>
#define	__TMC_NODECLARE_FUNC__
#include	"tmctools.h"

typedef struct ERRMSG_tag {
	int		err_code;
	UCHAR	err_msg[64];
} ERRMSG;

static ERRMSG	err[] = {
							{ RETURN_EDIT, "? e" },
							{ RETURN_STOP, "? x" },
							{ RETURN_STOP, "! Emergency stop." },
							{ RETURN_CONTINUE, "LaTeX Warning: Label(s) may have changed." },
							{ RETURN_STOP, "No pages of output." },
							{ RETURN_ERROR, "" }
						};

UCHAR*	getinifilename( UCHAR * );
UCHAR*	getshortname( LPTSTR, int );
FNAME	*set_FNAME( LPTSTR, FNAME * );

UCHAR* getinifilename( UCHAR *buf )
{
	HKEY	hKeyTMC;
	DWORD	len = MAX_LINE,
			dwType_SZ = REG_SZ;

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
							&len ) != ERROR_SUCCESS ) return NULL;
	RegCloseKey(hKeyTMC);
	return strcat(buf,DYN_INI);
}

UCHAR*	getshortname( UCHAR *filename, int ptexversion )
{
	UCHAR	*s;

	if( (s = _mbsrchr(filename,'\\')) == NULL ) s = filename;
	else s++;
	if( *s == '.' ) s++;
	return ptexversion?_mbsrchr(s,'.'):_mbschr(s,'.');
}

FNAME*	set_FNAME( LPTSTR lpCmdLine, FNAME *fname_buf )
{
	int		ptexversion;
	UCHAR	*s;

	if( (s = (UCHAR*)_mbsstr(lpCmdLine,"-f:")) == NULL ) return NULL;
	strcpy(fname_buf->filename,s+3);
	ptexversion = (int)!(	(s = (UCHAR*)_mbsstr(lpCmdLine,"-v:"))==NULL ||
							*(s+3) < '5'	);
	fname_buf->ishortname = (int)(getshortname(fname_buf->filename,ptexversion) - fname_buf->filename);

	return fname_buf;
}

int isCompileComplete( FNAME *fname )
{
	FILE	*stream;
	UCHAR	buf[MAX_LINE];
	int		i, ret;

	strcpy(buf,fname->filename);
	strcpy(buf+fname->ishortname,".log");
	if( (stream = fopen(buf,"rt")) == NULL ) return RETURN_ERROR;
	ret = RETURN_COMPLETE;
	while( fgets(buf,MAX_LINE-1,stream) != NULL ){
		if( !buf[MAX_LINE-1] ){
			for( i=0; err[i].err_code != RETURN_ERROR; i++ )
				if( _mbsstr(buf,err[i].err_msg) != NULL ){
					ret = err[i].err_code;
					break;
			}
			if( ret != RETURN_COMPLETE ) break;
		} else buf[MAX_LINE-1] = '\0';	//	行がバッファに収まらなかった場合の処理
	}
	fclose(stream);
	return ret;
}

int Return_VirTeX( int ret )
/*
	指定された値を ini file に書き込みます。

	ret		:	書き込む値
	return	:	ret をそのまま返す。
*/
{
	UCHAR	buf[8], szIniFileName[MAX_LINE];

	_itoa(ret,buf,10);
	if( getinifilename(szIniFileName) != NULL )
		WritePrivateProfileString(DYN_MODULECMD,DYN_RETURN_VTEX,buf,szIniFileName);
	return ret;
}

