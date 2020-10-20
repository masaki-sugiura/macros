/*
	Getting Long File Name (for 32bit Windows)
	Copyright (C) 1997, M. Sugiura(PXF05775@niftyserve.or.jp)
*/

#include	<string.h>
#include	<mbstring.h>
#include	<windows.h>

#if	0
#define	LFN_DEBUG	1
#endif

#define		LFN_ERROR_INVALIDARG		1
#define		LFN_ERROR_UNKNOWNOPT		2
#define		LFN_ERROR_MODULENAME		3
#define		LFN_ERROR_GETLONGFILENAME	4
/*
	LFN_ERROR_GETLONGFILENAME_FILENAME	=	 4
	LFN_ERROR_GETLONGFILENAME_FILENAME2	=	 8
	LFN_ERROR_GETLONGFILENAME_FILENAME3	=	16
*/

typedef struct LFN_tag {
	char	type;
	LPSTR	lastsep;
	UCHAR	fname[MAX_PATH];
} LFN;

UCHAR	szSecName[] = "Result",
		szIniFileName[MAX_PATH];

int		LFN_Error( int );
LFN*	getlongfilename( LPSTR, LFN* );
int		WriteINI( LFN* );

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
			LPSTR lpCmdLine, int nCmdShow )
{
	char	ftype;
	int		ret;
	LFN		szLongFileName;
#ifdef	LFN_DEBUG
	__argv[1] = "-d";
	__argv[2] = "c:\\winnt\\getlfn.ini";
	__argv[3] = "D:\\Program Files\\DevStudio\\MyProjects\\TMC_Tools\\GetLFN\\getlfn.c";
	__argc = 4;
#endif

	if( __argc <= 1 ) return LFN_Error(LFN_ERROR_INVALIDARG);
	else if( **(++__argv) == '-' || **__argv == '/' ){
		if( *(*__argv+1) == 'd' || *(*__argv+1) == 'D' ){
			if( *(*__argv+2) ) _mbscpy(szIniFileName,*(__argv++)+2);
			else if( *(++__argv) ) _mbscpy(szIniFileName,*(__argv++));
			else return LFN_Error(LFN_ERROR_INVALIDARG);
		} else return LFN_Error(LFN_ERROR_UNKNOWNOPT);
	} else {
		LPSTR s;
		if( (!GetModuleFileName(NULL,szIniFileName,MAX_PATH)) ||
			(s = _mbsrchr(szIniFileName,'.')) == NULL )
			return LFN_Error(LFN_ERROR_MODULENAME);
		else _mbscpy(s+1,"ini");
	}

	for(ret=0,ftype=0;ftype<3;ftype++){
		szLongFileName.type = ftype?('1'+ftype):'\0';
		ret |= (WriteINI(getlongfilename(*__argv,&szLongFileName))<<ftype);
	}
	return LFN_Error(ret);
}

LFN *getlongfilename( LPSTR dosname, LFN *lfn )
/*
	DOS名からロングファイル名を得ます。
*/
{
	short	len;
	WIN32_FIND_DATA fdata;
	HANDLE	hdata;
	UCHAR *s, *p, buf[MAX_PATH];

	if( !GetFullPathName(dosname,MAX_PATH,buf,&p) ) return NULL;
	strncpy(lfn->fname,buf,2);
	*((lfn->fname)+2) = '\0';
	_mbslwr(lfn->fname);
	len = strlen(buf);
	for( s=buf+3; s < buf+len; ){
		if( (s = _mbschr(s,'\\')) == NULL ) break;
		*s = '\0';
		hdata = FindFirstFile(buf,&fdata);
		if( hdata == INVALID_HANDLE_VALUE ) return NULL;
		else FindClose( hdata );
		if( s != NULL &&
			!(fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) )
			return NULL;
		_mbscat(lfn->fname,"\\");
		if( lfn->type != '3' || *fdata.cAlternateFileName == '\0' )
			_mbscat(lfn->fname,fdata.cFileName);
		else _mbscat(lfn->fname,fdata.cAlternateFileName);
		*(s++)='\\';
	}	/* end of for */
	_mbscat(lfn->fname,"\\");
	if( lfn->type == '3' ){
		hdata = FindFirstFile(buf,&fdata);
		if( hdata != INVALID_HANDLE_VALUE ){
			FindClose( hdata );
			if( *fdata.cAlternateFileName != '\0' )
				p = fdata.cAlternateFileName;
		}
	}
	_mbscat(lfn->fname,p);
	if( lfn->type != '2' ) _mbslwr(lfn->fname);
	lfn->lastsep = _mbsrchr(lfn->fname,'\\');
	return lfn;
}

int WriteINI( LFN *lfn )
{
	UCHAR	szValueName_FileName[16] = "filename",
			szValueName_Directory[16] = "directory",
			szValueName_BaseName[16] = "basename";

	if( lfn == NULL ) return LFN_ERROR_GETLONGFILENAME;

#ifndef	LFN_DEBUG
	szValueName_FileName[8] = lfn->type;
	szValueName_FileName[9] = '\0';
	WritePrivateProfileString(	szSecName,
								szValueName_FileName,
								lfn->fname,
								szIniFileName	);
	szValueName_BaseName[8] = lfn->type;
	szValueName_BaseName[9] = '\0';
	WritePrivateProfileString(	szSecName,
								szValueName_BaseName,
								(lfn->lastsep)+1,
								szIniFileName	);
	if( lfn->lastsep == lfn->fname + 2 ) (lfn->lastsep)++;
	*(lfn->lastsep) = '\0';
	szValueName_Directory[9] = lfn->type;
	szValueName_Directory[10] = '\0';
	WritePrivateProfileString(	szSecName,
								szValueName_Directory,
								lfn->fname,
								szIniFileName	);
#else
	MessageBox(NULL,szIniFileName,NULL,MB_OK);
	MessageBox(NULL,lfn->fname,NULL,MB_OK);
	*(lfn->lastsep) = '\0';
	MessageBox(NULL,lfn->fname,NULL,MB_OK);
	MessageBox(NULL,(lfn->lastsep)+1,NULL,MB_OK);
#endif
	return 0;
}

int LFN_Error( int err )
{
	UCHAR	ret[4];

	_itoa(err,ret,10);
	WritePrivateProfileString(szSecName,"returncode",ret,szIniFileName);
	return err;
}
