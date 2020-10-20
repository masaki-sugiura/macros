/*
	VirTeX Loader for "TeX Module Control with HIDEMARU MACRO Ver.3.X.X".
	Copyright (C) 1997-1999, M. Sugiura(PXF05775@niftyserve.or.jp)
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<process.h>
#include	<errno.h>
#include	<mbstring.h>
#include	<windows.h>

#define		__TMC_LOGREADER__
#include	"../tmctools.h"

#define		MAX_CMDLINE		1024

/*	Execute File Name	*/
static TCHAR	szAppName[MAX_PATH];
/*	Command line giving to the child process.	*/
static TCHAR	szChildCmdLine[MAX_CMDLINE];
/*	Environment Variable of "TEXEDIT".	*/
static TCHAR	szEnvTeXEdit[MAX_PATH] = "TEXEDIT=\"";
#ifdef	SIGNAL_INTERRUPT
/*	If Win95, ctrl-c/ctrl-break event should be removed?	*/
static	BOOL	bIgnoreSignal;
#endif

BOOL	ParseCmdLine(FNAME*);
BOOL	SetEnvTeXEdit(void);
BOOL	GetTeXModuleName(LPCSTR, LPCSTR, LPSTR);
void	usage(void);
void	help(void);
LPTSTR	getlongfilename(LPCSTR, LPSTR);
void	CopyAndStepPtr(LPSTR*, LPCSTR);
void	ChangeFileSep(LPSTR, BOOL);
void	eprintf(LPCSTR);

#define	BStoS(str)	ChangeFileSep(str,TRUE);
#define	StoBS(str)	ChangeFileSep(str,FALSE);

int WINAPI WinMain(
	HINSTANCE	hInstance,
	HINSTANCE	hPrevInstance,
	LPTSTR		lpszCmdLine,
	int			nCmdShow	)
/*
	Command line:
		execvtex virtex_name [&|-fmt=|-progname=]format_name [other options]
		virtex_name を format_name.exe に変えた物が存在すれば
		そのモジュールを実行する。
		なければ argv[1] 以降をそのまま実行する。
*/
{
	static	FNAME	fname;
	static	TCHAR	org_dir[MAX_PATH];
	static	STARTUPINFO			si;
	static	PROCESS_INFORMATION	pi;
	LPSTR	s;
	HANDLE	hStdHandle;

	if (!ParseCmdLine(&fname) || !SetEnvTeXEdit())
		/*	コマンドラインパースと環境変数の変更中にエラー発生	*/
		return Return_VirTeX(RETURN_ERROR);

	/*	カレントディレクトリの変更	*/
	s = NULL;
	if (*fname.filename && (s = _mbsrchr(fname.filename,'\\')) != NULL) {
		GetCurrentDirectory(MAX_PATH,org_dir);
		*s = '\0';
		if (!SetCurrentDirectory(fname.filename))
			return Return_VirTeX(RETURN_ERROR);
		*s = '\\';
	}

	//	親プロセスからの標準入出力ハンドルを閉じておく。
	if (hStdHandle = GetStdHandle(STD_INPUT_HANDLE)) {
		CloseHandle(hStdHandle);
		SetStdHandle(STD_INPUT_HANDLE,NULL);
	}
	if (hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE)) {
		CloseHandle(hStdHandle);
		SetStdHandle(STD_OUTPUT_HANDLE,NULL);
	}
	if (hStdHandle = GetStdHandle(STD_ERROR_HANDLE)) {
		CloseHandle(hStdHandle);
		SetStdHandle(STD_ERROR_HANDLE,NULL);
	}

	/*	新しいコンソールを獲得	*/
	FreeConsole();
	AllocConsole();
	SetConsoleTitle(szChildCmdLine);

	/*	STARTUPINFO の設定	*/
	si.cb		=	sizeof(STARTUPINFO);
	si.dwFlags	=	STARTF_USESTDHANDLES;
	si.hStdInput	=	GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput	=	GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError	=	GetStdHandle(STD_ERROR_HANDLE);
	FlushConsoleInputBuffer(si.hStdInput);

	/*	プロセスの開始	*/
	if (!CreateProcess(	NULL,
						szChildCmdLine,
						NULL,NULL,TRUE,
						0,
						NULL,NULL,&si,&pi	)
		) {
		eprintf("Failed to open process.");
		return Return_VirTeX(RETURN_ERROR);
	}

	/*	プロセスの終了を待つ	*/
	WaitForSingleObject(pi.hProcess,INFINITE);
	CloseHandle(pi.hProcess);

	/*	コンソールの破棄	*/
	FreeConsole();

	/*	カレントディレクトリの復元	*/
	if (s != NULL) SetCurrentDirectory(org_dir);

	if (*fname.filename) {
		/*	コンパイルの結果を調べて dyndata.ini に書き込む	*/
		StoBS(fname.filename);
		return Return_VirTeX(isCompileComplete(&fname));
	} else {
		return Return_VirTeX(RETURN_COMPLETE);
	}
	/* NOT REACHED.*/
}

/*
	コマンドラインを解釈して ChildProcess に渡す引数を構築する
*/
BOOL ParseCmdLine(FNAME *pfname)
{
#define	argc	__argc
#define	argv	__argv

	int		len, fmt_offset;
	LPSTR	module, fmt_opt, cmd_head;

	_mbscpy(szAppName,*(argv++));	/*	Copy exe-file name to szAppName[].	*/

	/*	Currently argv points __argv[1].	*/

	/*	The number of parameters is invalid, show help/usage.	*/
	if (argc < 4) {
		if (*argv != NULL && _mbsicmp(*argv,"-help") == 0) {
			help();	/*	ヘルプの表示	*/
		} else {
			/*	簡易ヘルプの表示	*/
			if (argc > 1) eprintf("Too few arguments.");
			usage();
		}
		return FALSE;
	}

	module	= *argv++;	/*	TeX module name	*/
	fmt_opt	= *argv++;	/*	Format specification option	*/
	if (*fmt_opt == '&') fmt_offset = 1;
	else if (_mbsnicmp(fmt_opt,"-fmt=",5) == 0) fmt_offset = 5;
	else if (_mbsnicmp(fmt_opt,"-progname=",10) == 0) fmt_offset = 10;
	else {
		eprintf("Invalid format specification.");
		usage();
		return FALSE;
	}

	/*	Merge compiler's name and the specified format name	*/
	StoBS(module);
	if (GetTeXModuleName(module,fmt_opt+fmt_offset,szChildCmdLine)) {
		/*	"virtex_dir\format_name.exe" exists.	*/
		fmt_opt = NULL;
	} else if (szChildCmdLine[0] == '\0') {
		eprintf("Failed to get a module file name.");
		return FALSE;
	}

	len = lstrlen(szChildCmdLine);

	/*	Analyze other options	*/
	for (	cmd_head = szChildCmdLine+len;
			*argv;
			++argv	) {
		*cmd_head++	= ' ';
		*cmd_head++	= '"';
		if (**argv != '-' && fmt_opt != NULL) {
			/* Move format specification option
			   to the head of other options of "-???". */
			CopyAndStepPtr(&cmd_head,fmt_opt);
			*cmd_head++ = '"';
			*cmd_head++ = ' ';
			*cmd_head++ = '"';
			fmt_opt = NULL;	/*	次のループで下の分岐へ	*/
			--argv;	/*	もう一度同じ文字列を見る	*/
		} else {
			LPCSTR	ptr;
			if (**argv != '\\') {
				/*	convert filename which is given to (vir)(p)tex.exe.	*/
				if (getlongfilename(*argv,pfname->filename) == NULL) {
					eprintf("Failed to get long filename.");
					return FALSE;
				}
				set_FNAME(pfname->filename,pfname);
#if 0
				BStoS(pfname->filename);	/*	Convert "\" to "/"	*/
#endif
				/*	ファイル名からディレクトリ部分を除いて引数にコピー	*/
				if ((ptr = _mbsrchr(pfname->filename,'\\')) == NULL)
					ptr = pfname->filename;	/*	ディレクトリ名無し	*/
				else if (*++ptr == '\0') {
					eprintf("Failed to get a source file name.");
					return FALSE;
				}
			} else {
				/*	ファイル名でないオプション	*/
				ptr = *argv;
			}
			/*	引数をコピー	*/
			CopyAndStepPtr(&cmd_head,ptr);
		}
		*cmd_head++	= '"';
	}

#ifdef	_DEBUG
	/*	Debug message	*/
	MessageBox(NULL,szChildCmdLine,NULL,MB_OK);
#endif
	return TRUE;
}

/*
	環境変数 TEXEDIT を err_hook を呼び出すように変更
*/
BOOL SetEnvTeXEdit(void)
{
	LPSTR	s;

	/*	環境変数 TEXEDIT を err_hook を呼び出すように設定	*/
	getinifilename(szEnvTeXEdit+9);	/*	get directory name of dyndata.ini.	*/
	if (!szEnvTeXEdit[9] || (s = _mbsrchr(szEnvTeXEdit,'\\')) == NULL) {
		eprintf("Failed to find dyndata.ini.");
		return FALSE;
	}

	_mbscpy(++s,"tools\\err_hook\" %s %d");

	/*	Win95(for NEC PC98) だと "\" を受け付けない(らしい)ので
		セパレータを "/" に設定	*/
	if (getenv("KSH_NEC_WIN95") != NULL) BStoS(szEnvTeXEdit+9);
	_putenv(szEnvTeXEdit);

	return TRUE;
}

/*
	If there exists a "format_name.exe" in the same directory of virtexname,
	replace virtexname[] into "virtex_dir\format_name.exe".
	NOTICE: size of buf[] should be equal to or greater than MAX_PATH.
*/
BOOL GetTeXModuleName(LPCSTR virtexname, LPCSTR av_formatname, LPSTR buf)
{
	BOOL			bRet;
	LPSTR			buf_head = buf, basename_ptr_buf, ext_ptr;
	LPCSTR			basename_ptr_org;

	*buf++ = '"';

	/*	Copy virtex_dir to virtexname[].	*/
	if ((basename_ptr_org = _mbsrchr(virtexname,'\\')) != NULL) {
		/*	Here it is assumed that virtexname is a full path name
			when it has a path separating character.	*/
		LPCSTR	ptr = virtexname;
		basename_ptr_org++;
		while (ptr < basename_ptr_org) *buf++ = *ptr++;
		basename_ptr_buf = buf;
	} else {
		basename_ptr_org = virtexname;
		if (!SearchPath(NULL,virtexname,".exe",MAX_PATH-1,buf,&basename_ptr_buf))
			/*	Could not find such an executable.	*/
			return FALSE;
	}

	/*	Merge av_formatname to virtex_dir.	*/
	_mbscpy(basename_ptr_buf,av_formatname);
	if ((ext_ptr = _mbsrchr(basename_ptr_buf,'.')) != NULL	&&
		_mbsicmp(ext_ptr,".fmt") == 0) *ext_ptr = '\0';
	else {
		for (ext_ptr = basename_ptr_buf; *ext_ptr; ext_ptr++)
			;
	}
	_mbscpy(ext_ptr,".exe");

	if (!(bRet = (GetFileAttributes(buf_head+1) != 0xFFFFFFFF))) {
		/*	Could not find "virtex_dir\format_name.exe".	*/
		_mbscpy(basename_ptr_buf,basename_ptr_org);
	}

	/*	引用符を閉じる	*/
	while (*ext_ptr) ext_ptr++;
	*ext_ptr++ = '\"';
	*ext_ptr = '\0';

	return bRet;
}

/*
	簡単なヘルプの表示
*/
void usage(void)
{
	eprintf("\nUsage: execvtex virtex format_opt [options] texfile .\n"
			"       Type \"execvtex -help\" for more information.");
	return;
}

void help(void)
{
	MessageBox(NULL,
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
		"\n",
		"ExecVTeX Help Dialog",MB_OK);
}

/*
	strcpy のポインタインクリメントバージョン
*/
void CopyAndStepPtr(LPSTR *buf_ptr, LPCSTR str_ptr)
{
	while (**buf_ptr = *str_ptr++) (*buf_ptr)++;
}

/*
	ディレクトリセパレータを変更する
*/
void ChangeFileSep(LPTSTR str, BOOL bsep)
{
	TCHAR	oldsep = bsep?'\\':'/',
			newsep = bsep?'/':'\\';

	while (str = _mbschr(str,oldsep)) *str = newsep;
}

LPTSTR getlongfilename(LPCTSTR dosname, LPTSTR lfn_buf)
/*
	DOS名からロングファイル名を得ます。
*/
{
	if (*(dosname+1) == ':'			&&
		_mbschr(dosname,'~') == NULL) _mbscpy(lfn_buf,dosname);
	else {
		static TCHAR	buf[MAX_PATH];
		WIN32_FIND_DATA fdata;
		HANDLE	hdata;
		LPSTR	buf_ptr = buf, lfn_ptr = lfn_buf, p;

		/*	この関数呼び出しでフルパス名に変換される(でもDOS名だよ…(--;)。	*/
		GetFullPathName(dosname,MAX_PATH,buf,&p);

		*lfn_buf = '\0';
		while (buf_ptr != NULL) {
			p = buf_ptr;
			if ((buf_ptr = _mbschr(p,'\\')) != NULL) *buf_ptr = '\0';
			if (_mbschr(p,'~') != NULL) {
				if ((hdata = FindFirstFile(buf,&fdata)) == INVALID_HANDLE_VALUE)
					return NULL;
				FindClose(hdata);
				if (buf_ptr == NULL || (fdata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
					p = fdata.cFileName;
				else return NULL;
			}
			if (*lfn_buf != '\0') *lfn_ptr++ = '\\';
			while (*lfn_ptr = *p++) lfn_ptr++;
			if (buf_ptr != NULL) *buf_ptr++ = '\\';
		}
	}
	return _mbslwr(lfn_buf);
}

void eprintf( LPCSTR msg )
{
	static	TCHAR	msgbuf[MAX_CMDLINE];
	wsprintf(msgbuf,"Module: %s\nError: %s",szAppName,msg);
	MessageBox(NULL,msgbuf,NULL,MB_OK);
}

