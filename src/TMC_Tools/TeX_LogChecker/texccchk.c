/*
	TeX Compile Complete Checker (for Win32 Console)
		for "TeX Module Control with HIDEMARU MACRO Ver.3.X.X".
	Copyright (C) 1997, M. Sugiura(PXF05775@niftyserve.or.jp)
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<mbstring.h>
#include	<windows.h>

#include	"../tmctools.h"

static unsigned char	szSecName[] = DYN_MODULECMD,
						szKeyName[] = DYN_RETURN_VTEX,
						*szIniFileName,
						*szAppName;

extern unsigned char*	getinifilename( unsigned char * );
void			eprintf( char * );
int				isCompileComplete( unsigned char * );
unsigned char*	getlogfilename( unsigned char *, unsigned char * );
int				Return_VirTeX( int );

int main( int argc, unsigned char **argv )
{
	unsigned char inifile[MAX_LINE];

	szIniFileName = getinifilename( inifile );
	szAppName = *argv;
	if( argc < 2 ){
		eprintf("Usage: texccchk.exe [-rR] tex_filename");
		return RETURN_ERROR;
	} else if( **(++argv) == '-' ){
		switch( *(*argv+1) )
		{
		case 'r':
		case 'R':
			if( *(++argv) == NULL ){
				eprintf("Usage: texccchk.exe [-rR] tex_filename");
				return RETURN_ERROR;
			} else return Return_VirTeX(isCompileComplete(*argv));
			/* NOT REACHED. */
		}
	}
	return isCompileComplete(*argv);
	/* NOT REACHED. */
}

int isCompileComplete( unsigned char *filename )
{
	FILE *stream;
	unsigned char logname[256], buf[MAX_LINE];
	int ret;

	if( (stream = fopen(getlogfilename(filename,logname),"rt")) == NULL )
		return RETURN_ERROR;
	ret = RETURN_COMPLETE;
	while( fgets(buf,MAX_LINE-1,stream) != NULL ){
		if( !buf[MAX_LINE-1] ){
			if( strstr(buf,"? e") != NULL ) ret = RETURN_EDIT;
			else if( strstr(buf,"? x") != NULL ) ret = RETURN_STOP;
			else if( strstr(buf,"! Emergency stop.") != NULL ) ret = RETURN_STOP;
			else if( strstr(buf,"LaTeX Warning: Label(s) may have changed.") != NULL )
				ret = RETURN_CONTINUE;
			else if( strstr(buf,"No pages of output.") != NULL ) ret = RETURN_STOP;
			if( ret != RETURN_COMPLETE ) break;
		} else buf[MAX_LINE-1] = '\0';	//	行がバッファに収まらなかった場合の処理
	}
	fclose(stream);
	return ret;
}

unsigned char *getlogfilename( unsigned char *texfilename, unsigned char *logfilename )
{
	unsigned char *s;

	lstrcpy(logfilename,texfilename);
	if( (s = _mbsrchr(logfilename,'\\')) == NULL ) s = logfilename;
	else s++;
	if( *s == '.' ) s++;
	if( (s = _mbschr(s,'.')) == NULL ){
		eprintf("Failed to get log file name.");
		exit( RETURN_ERROR );
	} else *s = '\0';
	lstrcat(logfilename,".log");
	return logfilename;
}

int Return_VirTeX( int ret )
{
	unsigned char buf[8];

	_itoa(ret,buf,10);
	if( szIniFileName != NULL )
		WritePrivateProfileString(szSecName,szKeyName,buf,szIniFileName);
	return ret;
}

void eprintf( char *msg )
{
	fprintf(stderr,"%s: %s\n",szAppName,msg);
	return;
}
