#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<errno.h>
#include	<mbstring.h>
#include	<windows.h>

#define		__TMC_NOCONSOLE__
#include	"../tmctools.h"

#define		REQUIRE_NONE	0
#define		REQUIRE_BIBTEX	1
#define		REQUIRE_MKIDX	2
#define		REQUIRE_PDFTEX	4
#define		REQUIRE_COMPILE	8
#define		REQUIRE_ERROR	16

static unsigned char	szSecName[] = DYN_MODULECMD,
						szKeyName[] = "require_apps",
						szIniFileName[MAX_LINE],
						*extptr[2] = {".aux",".idx"},
						*outextptr[2] = {".dvi",".pdf"},
						*lpszScanString[3] = { "\\bibliography", "\\makeindex", "\\pdfoutput" };

int		isStringExist( LPTSTR );
time_t	gettimecountof( LPTSTR );
int		CompareTimeStamp( FNAME *, int );
int		Return_Result( int );

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
			LPTSTR lpCmdLine, int nCmdShow )
{
	int i, ret;
	static TCHAR buf[16] = "ShortName";
	static FNAME	fname;

	/*	INI ファイル名の取得	*/
	getinifilename(szIniFileName);

	/*	TeX ファイル名からショートファイル名を取得	*/
	if( __argc < 2 || !set_FNAME(__argv[1],&fname) ) return REQUIRE_ERROR;

	/*	shortname を書き込む	*/
	for( i=1; i>=0; i-- ){
		if( !i ) _mbscpy(buf+9,"_Old");
		fname.filename[fname.ishortname[i]] = '\0';
		WritePrivateProfileString(
				"CurrentProjectInfo",
				buf,
				_mbsrchr(fname.filename,'\\')+1,
				szIniFileName
			);
		fname.filename[fname.ishortname[i]] = '.';
	}

	if( (ret = isStringExist(fname.filename)) >= REQUIRE_ERROR )
		return REQUIRE_ERROR;

	return Return_Result( ret|CompareTimeStamp(&fname,ret) );
}

int isStringExist( LPTSTR filename )
{
	int				i, ret = REQUIRE_NONE;
	FILE			*stream;
	LPTSTR			s_str;
	static TCHAR	buf[MAX_LINE];

	if( (stream = fopen(filename,"rt")) == NULL ) return REQUIRE_ERROR;

	while( fgets(buf,MAX_LINE,stream) != NULL ){
		if( (s_str = _mbschr(buf,'%')) != NULL ){
			if( s_str == buf ) continue;	/*	zero length	*/
			*s_str = '\0';
		}
		for( i=0; i<2; i++ ){
			/*	BibTeX, MakeIndex 関連文字列のスキャン	*/
			if( (s_str = _mbsstr(buf,lpszScanString[i])) != NULL )
					ret |= (i+1);
		}
		if( (s_str = _mbsstr(buf,lpszScanString[2])) != NULL ){
			/*	PDFTeX 関連文字列のスキャン	*/
			s_str += strlen(lpszScanString[2]);
			if( _mbschr(s_str,'1') != NULL ) ret |= REQUIRE_PDFTEX;
		}
		if( ret == (REQUIRE_BIBTEX|REQUIRE_MKIDX|REQUIRE_PDFTEX) ) break;
	}

	fclose(stream);

	return ret;
}

int	CompareTimeStamp( FNAME *fname, int request )
{
	int		i, n;
	static TCHAR	namebuf[MAX_LINE];
	static time_t	textime, othertime;

	if( !*(fname->filename)	||
		(textime = gettimecountof(fname->filename)) == -1L ){
		MessageBox(NULL,"Failed to get time stamp.",NULL,MB_OK);
		return REQUIRE_ERROR;
	}

	for( n=1; n>=0; n-- ){
		_mbsncpy(namebuf,fname->filename,fname->ishortname[n]);
		_mbscpy(namebuf+(fname->ishortname[n]),outextptr[(request&REQUIRE_PDFTEX)?1:0]);
		if( (othertime = gettimecountof(namebuf)) != -1L )
			return (textime>othertime)?REQUIRE_COMPILE:REQUIRE_NONE;
		for( i=0; i < 2; i++ ){
			if( !((i+1)&request) ) continue;
			_mbscpy(namebuf+(fname->ishortname[n]),extptr[i]);
			if( (othertime = gettimecountof(namebuf)) != -1L )
				return (textime>othertime)?REQUIRE_COMPILE:REQUIRE_NONE;
		}
	}

	return REQUIRE_COMPILE;
}

time_t	gettimecountof( LPTSTR filename )
{
	static struct _stat fstat;

	if( _stat( filename, &fstat ) == -1 ) return -1L;
	else return fstat.st_mtime;
}

int Return_Result( int ret )
/*
	指定された値を ini file に書き込みます。

	ret		:	書き込む値
	return	:	ret をそのまま返す。
*/
{
	static TCHAR buf[8];

	if( szIniFileName == NULL ) return REQUIRE_ERROR;
	else {
		_itoa(ret,buf,10);
		WritePrivateProfileString(szSecName,szKeyName,buf,szIniFileName);
	}
	return ret;
}

