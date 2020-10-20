/*
	VirTeX Loader for "TeX Module Control with HIDEMARU MACRO Ver.3.X.X".
	Copyright (C) 1997, M. Sugiura(PXF05775@niftyserve.or.jp)
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<process.h>
#include	<errno.h>
#include	<mbstring.h>
#include	<windows.h>

#define		__TMC_NOWINAPP__
#define		__TMC_LOGREADER__
#include	"../tmctools.h"

#define		MAX_CMD		16

/*	Execute File Name	*/
static unsigned char	szAppName[MAX_PATH];

BOOL	GetTeXModuleName( LPTSTR, LPCTSTR );
void	usage( void );
void	help( void );
LPTSTR	getlongfilename( LPCTSTR, LPTSTR );
void	ChangeFileSep( LPTSTR, BOOL );
void	eprintf( char* );

extern	FNAME*	set_FNAME( LPTSTR, FNAME* );

#define	BStoS(str)	ChangeFileSep(str,TRUE);
#define	StoBS(str)	ChangeFileSep(str,FALSE);

int main( int argc, unsigned char **argv )
/*
	Command line:
		execvtex virtex_name [&|-fmt=|-progname=]format_name [other options]
		virtex_name を format_name.exe に変えた物が存在すれば
		そのモジュールを実行する。
		なければ argv[1] 以降をそのまま実行する。
*/
{
	int		ac, ret;
	static FNAME	fname;
	static TCHAR	*s, org_dir[MAX_PATH],
					module[MAX_PATH],
					texedit[MAX_PATH] = "TEXEDIT=\"",
					*av[MAX_CMD];

	_mbscpy(szAppName,*(argv++));	/*	Copy exe-file name to szAppName[].	*/

	/*	Currently argv points __argv[1].	*/

	/*	The number of parameters is invalid, show help/usage.	*/
	if( argc < 4 ){
		if( argv != NULL && !_mbsicmp(*argv,"-help") ){
			help();
		} else {
			if( argc > 1 ) eprintf("Too few arguments.");
			usage();
		}
		return Return_VirTeX(RETURN_ERROR);
	}

	/*	Copy TeX-Compiler's name to module[].	*/
	_mbscpy(module,*(argv++));

	/*	Currently argv points __argv[2].	*/

	/*	Save pointer to format_opt	*/
	s = *argv;

	/*	Check format specification	*/
	if( **argv == '&' ) ++*argv;
	else if( !_mbsnicmp(*argv,"-fmt=",5) ) *argv += 5;
	else if( !_mbsnicmp(*argv,"-progname=",10) ) *argv += 10;
	else {
		eprintf("Invalid format specification.");
		return Return_VirTeX(RETURN_ERROR);
	}

	/*	Merge compiler's name and the specified format name	*/
	if( GetTeXModuleName(module,*(argv++)) ) s = NULL;
	else if( !module[0] ){
		eprintf("Failed to get a module file name.");
		Return_VirTeX(RETURN_ERROR);
	}

	/*	Currently argv points __argv[3].	*/

	/*	Analyze other options	*/
	av[0] = module;
	for( ac=1; (ac<argc-1)&&(*argv!=NULL)&&(av[ac]=*(argv++)); ac++ ){
		if( *av[ac] != '-' ){
			/* Reach at first non-optional argument. */
			if( s != NULL ){
				/* Move format specification option
				   to the last of other options. */
				av[ac+1] = av[ac];
				av[ac++] = s;
				s = NULL;
			}
			if( *av[ac] != '\\' ){
				/*	convert filename which is given to (vir)(p)tex.exe.	*/
				if( getlongfilename(av[ac],fname.filename) == NULL ){
					eprintf("Failed to get long filename.");
					return Return_VirTeX(RETURN_ERROR);
				}
				set_FNAME(fname.filename,&fname);
				BStoS(fname.filename);	/*	Convert "\" to "/"	*/
				av[ac] = fname.filename;
			}
		}
	}
	av[ac] = NULL;

#if 0
	/*	Debug message	*/
	for( ret=0; ret<ac; ret++ ) MessageBox(NULL,av[ret],NULL,MB_OK);
#endif

	/*	環境変数 TEXEDIT を err_hook を呼び出すように設定	*/
	getinifilename(texedit+9);	/*	get directory name of dyndata.ini.	*/
	if( !texedit[9] || (s = _mbsrchr(texedit,'\\')) == NULL ){
		eprintf("Failed to find dyndata.ini.");
		return Return_VirTeX(RETURN_ERROR);
	}
	_mbscpy(++s,"tools\\err_hook\" %s %d");
	/*	Win95(for NEC PC98) だと "\" を受け付けない(らしい)ので
		セパレータを "/" に設定	*/
	if( getenv("KSH_NEC_WIN95") != NULL ) BStoS(texedit+9);
	_putenv(texedit);

	/*	カレントディレクトリの変更	*/
	s = NULL;
	if( *fname.filename && (s = _mbsrchr(fname.filename,'/')) != NULL ){
		GetCurrentDirectory(MAX_PATH,org_dir);
		*s = '\0';
		if( !SetCurrentDirectory(fname.filename) )
			return Return_VirTeX(RETURN_ERROR);
		*s = '/';
	}

	_flushall();	/*	flush all streams.	*/
	/*	argv[0] にフォーマットファイル名を入れるのは
		角藤さんがtex-loaderで使ったアイデアです。	*/
	ret = spawnvp(P_WAIT,av[0],av);
	if( s != NULL ) SetCurrentDirectory(org_dir);
	if( ret == -1 ){
		switch( errno )
		{
		case E2BIG:
			eprintf("Too long argument strings.");
			break;
		case ENOENT:
			eprintf("No such an executable file.");
			break;
		case ENOMEM:
			eprintf("No memory.");
			break;
		default:
			eprintf("Unknown Error.");
		}
		return Return_VirTeX(RETURN_ERROR);
	} else if( *fname.filename ){
		StoBS(fname.filename);
		return Return_VirTeX(isCompileComplete(&fname));
	} else return Return_VirTeX(RETURN_COMPLETE);
	/* NOT REACHED.*/
}


BOOL GetTeXModuleName( LPTSTR virtexname, LPCTSTR av_formatname )
/*
	If there exists a "format_name.exe" in the same directory of virtexname,
	replace virtexname[] into "virtex_dir\format_name.exe".
	NOTICE: size of virtexname[] should be equal to or greater than MAX_PATH.
*/
{
	BOOL			bRet = FALSE;
	LPTSTR			s, d;
	static TCHAR	bbuf[MAX_PATH], fbuf[MAX_PATH];

	/*	Copy virtex_dir to virtexname[].	*/
	if( (d = _mbsrchr(virtexname,'\\')) != NULL	||
		(d = _mbsrchr(virtexname,'/')) != NULL	){
		/*	Here it is assumed that virtexname is a full path name
			when it has a path separating character.	*/
		*d = '\0';
	} else if( SearchPath(NULL,virtexname,".exe",MAX_PATH,fbuf,&s) ){
		/*	virtexname doesn't have a directory name.	*/
		_mbscpy(virtexname,fbuf);
		*(d = virtexname + (--s - fbuf)) = '\0';
	} else {
		*virtexname = '\0';	/*	Could not find such an executable.	*/
		return bRet;
	}

	/*	Merge av_formatname to virtex_dir.	*/
	_mbscpy(bbuf,av_formatname);
	if( (s = _mbsrchr(bbuf,'.')) != NULL	&&
		_mbsicmp(s,".fmt") == 0 ) *s = '\0';

	/*	Search "virtex_dir\av_formatname.exe".	*/
	bRet = SearchPath(virtexname,bbuf,".exe",MAX_PATH,fbuf,&s);
	*(d++) = '\\';
	if( bRet ) _mbscpy(d,s);
	return bRet;
}

void usage( void )
{
	eprintf("\nUsage: execvtex virtex format_opt [options] texfile .\n"
			"       Type \"execvtex -help\" for more information.");
	return;
}

void help( void )
{
	fputs(
		"This is ExecVTeX, for TeX Module Control with HIDEMARU Macro.\n"
		"Copyright (C) 1997-1998, M. Sugiura.\n"
		"\n"
		"[Usage]\n"
		" execvtex virtex format_opt [options] texfile .\n"
		"\n"
		" virtex     : filename of TeX compiler,\n"
		"               e.g. \"c:\\usr\\local\\bin\\tex.exe\".\n"
		" format_opt : string to give the format file name to the above.\n"
		"              For example, \"&latex\" or \"-fmt=platex\" etc.\n"
		" [options]  : options for TeX compiler.\n"
		" texfile    : TeX source file name (or TeX command strings).\n"
		"\n"
		"[Rule]\n"
		" If \"virtex_dir\\format_filename.exe\" exists,\n"
		"  virtex is replaced to it and format_opt is removed.\n"
		" Else \"virtex format_opt ...\" is merely executed.\n"
		"\n"
	, stdout);
	return;
}

void ChangeFileSep( LPTSTR str, BOOL bsep )
{
	TCHAR	oldsep = bsep?'\\':'/',
			newsep = bsep?'/':'\\';

	while( str = _mbschr(str,oldsep) ) *str = newsep;

	return;
}

LPTSTR getlongfilename( LPCTSTR dosname, LPTSTR lfn_buf )
/*
	DOS名からロングファイル名を得ます。
*/
{
	LPTSTR	s, p;
	static TCHAR	buf[MAX_PATH];
	WIN32_FIND_DATA fdata;
	HANDLE	hdata;

	if( *(dosname+1) == ':'			&&
		_mbschr(dosname,'~') == NULL	) _mbscpy(lfn_buf,dosname);
	else {
		GetFullPathName(dosname,MAX_PATH,buf,&p);
		/*	この関数呼び出しでフルパス名に変換される(でもDOS名だよ…(--;)。	*/
		*lfn_buf = '\0';
		for( s=buf; ; ){
			p = s;
			if( (s = _mbschr(p,'\\')) != NULL ) *s = '\0';
			if( _mbschr(p,'~') != NULL ){
				if( (hdata = FindFirstFile(buf,&fdata)) == INVALID_HANDLE_VALUE )
					return NULL;
				FindClose( hdata );
				if( s==NULL || (fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) )
					p = (unsigned char *)fdata.cFileName;
				else return NULL;
			}
			if( *lfn_buf != '\0' ) _mbscat(lfn_buf,"\\");
			_mbscat(lfn_buf,p);
			if( s != NULL ) *(s++) = '\\';
			else break;
		}	/* end of for */
	}
	return _mbslwr(lfn_buf);
}

void eprintf( char *msg )
{
	fputs(szAppName,stderr);
	fputs(": ",stderr);
	fputs(msg,stderr);
	fputs("\n",stderr);
	return;
}
