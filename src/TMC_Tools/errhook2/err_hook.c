/*
	Error Hook for "TeX Module Control with HIDEMARU MACRO Ver.3.X.X".
	Copyright (C) 1997, M. Sugiura(PXF05775@niftyserve.or.jp)
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<mbstring.h>
#include	<windows.h>

#define		__TMC_NORETURN__
#include	"../tmctools.h"

//	エラーコード
#define	CODE_MAX				4
#define	ERROR_ARG_FEW			2
#define	ERROR_ARG_MANY			4

static unsigned char	dyn_ini[MAX_LINE],
						szSecName[] = "Error_Hook",
						szValueName_ExecFlag[] = "Exec_Flag",
						*szAppName;

//	prototype declarations
extern unsigned char*	getinifilename( unsigned char * );
void			eprintf( char * );
void			normalize_filename( unsigned char * );
int				WriteErrorInfo( unsigned char ** );
int				WriteReturnCode( int );

int main( int argc, unsigned char **argv )
{
	szAppName = *(argv++);
	getinifilename(dyn_ini);

	if( argc != 3 ){
		//	引数の数が正しくない場合は使い方を表示して終了
//		eprintf(sprintf("[Error]: Too %s arguments.",argc>3?"many":"few"));
		eprintf("[Usage]: err_hook error_filename error_line");
		return WriteReturnCode(argc>3?ERROR_ARG_MANY:ERROR_ARG_FEW);
	}

	return WriteReturnCode(WriteErrorInfo(argv));
}	//	End of main()


int WriteErrorInfo( unsigned char **argv )
{
	normalize_filename(*argv);
	WritePrivateProfileString(DYN_MODULECMD,DYN_ERRORFILE,*(argv++),dyn_ini);
	WritePrivateProfileString(DYN_MODULECMD,DYN_ERRORLINE,*argv,dyn_ini);
	return 0;
}


void normalize_filename( unsigned char *filename )
{
	unsigned char *s;

	_mbslwr(filename);
	while( (s = _mbschr(filename,'/')) != NULL ) *s = '\\';
	return;
}


int WriteReturnCode( int code )
{
	char	ret_code[CODE_MAX];

	_itoa(code,ret_code,10);
	WritePrivateProfileString(szSecName,szValueName_ExecFlag,ret_code,dyn_ini);
	return code;
}

void eprintf( char *str )
{
	fprintf(stderr,"%s: %s\n",szAppName,str);
	return;
}

