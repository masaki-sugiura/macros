/*
	Error Hook for "TeX Module Control with HIDEMARU MACRO Ver.3.X.X".
	Copyright (C) 1997, M. Sugiura(PXF05775@niftyserve.or.jp)
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<mbstring.h>
#include	<process.h>
#include	<windows.h>

#define		__TMC_NORETURN__
#define		__TMC_NOWINAPP__
#include	"../tmctools.h"

#if	0
#define		_TMC_DEBUG	1
#endif

/*	���ϐ��̍ő啶����	*/
#ifndef	ENVMAX
#define	ENVMAX	MAX_PATH
#endif
/*	�R�}���h�I�v�V�����̍ő吔	*/
#define	CMDMAX	16

/*	�G���[�R�[�h	*/
#define	CODE_MAX				4
#define	ERROR_ARG_FEW			2
#define	ERROR_ARG_MANY			4
#define	ERROR_ENV_NOTFOUND		8
#define	ERROR_ENV_NODEF_NAME	16
#define	ERROR_ENV_NODEF_LINE	32

static unsigned char	szSecName[] = "Error_Hook",
						szValueName_ExecFlag[]	= "Exec_Flag",
						dyn_ini[MAX_LINE],
						*szAppName;

/*	prototype declarations	*/
void			eprintf( char * );
unsigned char*	getenv_from_file( unsigned char *, unsigned char *, unsigned char * );
void			normalize_filename( unsigned char * );
int				make_argv( unsigned char *, unsigned char ** );
int				setup_args( unsigned char ** );
int				WriteErrorInfo( int, unsigned char ** );
int				WriteReturnCode( int );
int				ExecEditor( char *, char ** );

int main( int argc, unsigned char **argv )
{
	szAppName = *(argv++);
	if( getinifilename(dyn_ini) == NULL ){
		/*	ini �t�@�C������T���Ȃ������炻�̂܂܌��̃R�}���h�����s	*/
		eprintf("[Error]: Cannot get the path of TeX-MC macro folder.");
		return ExecEditor(*argv,argv);
	}

	if( argc < 2 ){
		/*	�����������ꍇ�͎g������\�����ďI��	*/
		eprintf("[Usage]: err_hook editor_name [options]");
		return WriteReturnCode(ERROR_ARG_FEW);
	} else if( argc > CMDMAX - 1 ){
		/*	�����̐�����������ꍇ�͂��̂܂܌��̃R�}���h�����s	*/
		eprintf("[Error]: Too many arguments.");
		WriteReturnCode(ERROR_ARG_MANY);
		return ExecEditor(*argv,argv);
	}

	if( (!GetPrivateProfileInt(szSecName,szValueName_ExecFlag,0,dyn_ini))
		&& _mbscmp(*argv,"--") )
		return ExecEditor(*argv,argv);
#if	0
	WritePrivateProfileString(szSecName,szValueName_ExecFlag,"0",dyn_ini);
#endif
	/*	�{���̋@�\�����s����	*/
	return WriteReturnCode(WriteErrorInfo(argc-2,++argv));
}	/*	End of main()	*/


int WriteErrorInfo( int argc, unsigned char **argv )
/*
	TeX�̓f�����G���[�̌��ʂ� ini�t�@�C���ɏ�������
	argc, argv	:	TEXEDIT �̃R�}���h���C���I�v�V�����̕���
					(cf. TEXEDIT = err_hook editor %s %d
					     -> argv[0] = filename, argv[1] = errline)
	return		:	���^�[���R�[�h(ERROR_* �̃r�b�g�a)
*/
{
	int i, ac, ret = ERROR_ENV_NODEF_NAME|ERROR_ENV_NODEF_LINE;
	unsigned char	*s, *av[CMDMAX], av_buf[CMDMAX*ENVMAX],
					*pValueName,
					err_file[] = DYN_ERRORFILE,
					err_line[] = DYN_ERRORLINE;

	for( i=0; i < CMDMAX; i++ ) av[i] = &av_buf[ENVMAX*i];
	if( (ac = setup_args(av)) <= 2 ) return ERROR_ENV_NOTFOUND;
	for( i=0; (i < argc && i < (ac-2)); i++,argv++ ){
		if( (s = _mbschr(av[i+2],'%')) != NULL ){
			*argv += (s-av[i+2]);
			*(*argv + _mbslen(*argv) - _mbslen(s+2)) = '\0';
			if( *(s+1) == 's' ){
				ret ^= ERROR_ENV_NODEF_NAME;
				pValueName = err_file;
				normalize_filename(*argv);
			} else if( *(s+1) == 'd' ){
				ret ^= ERROR_ENV_NODEF_LINE;
				pValueName = err_line;
			} else continue;
#ifndef	_TMC_DEBUG
			WritePrivateProfileString(DYN_MODULECMD,pValueName,*argv,dyn_ini);
#else
			fputs(pValueName,stderr), fputs(" = ",stderr), fputs(*argv,stderr), fputs("\n",stderr);
#endif
		}
	}
	return ret;
}

int setup_args( unsigned char **av_buf )
/*
	���ϐ� TEXEDIT ����G���[���̈����̍\����ǂݎ��
	av_buf		:	���ϐ����瓾���I�v�V�����̕���
					(cf. TEXEDIT = err_hook editor %s %d
					     -> av_buf[0] = "err_hook", av_buf[1] = "editor", etc.)
*/
{
	unsigned char *szTeXEnv, szBufEnvStr[ENVMAX];

	if( (szTeXEnv = getenv("TEXEDIT")) != NULL ){
		_mbscpy(szBufEnvStr,szTeXEnv);
		szTeXEnv = _mbslwr(szBufEnvStr);
	} else if( (szTeXEnv = getenv("TEXENV")) != NULL ){
		/*	�ǎR�� pTeX2.1.4 �̏ꍇ(�{���Ȃ炱���ɂ͂���͂����Ȃ�)	*/
		szTeXEnv = getenv_from_file(szTeXEnv,"TEXEDIT",szBufEnvStr);
	}
	if( szTeXEnv == NULL ){
		eprintf("[Error]: Cannot find TEXEDIT environment variable.");
		return 0;
	}
	return make_argv( szTeXEnv, av_buf );
}

int make_argv( unsigned char *cmdstr, unsigned char **av )
/*
	���ϐ��̓��e���R�}���h���C���Ƃ��� **argv ���\�z�B
	�߂�l�� argc(< CMDMAX) �ɑΉ�����B
*/
{
	int i, ac;
	BOOL	flag_quote = FALSE,
			flag_space = FALSE;

	for( i=0,ac=0; (*cmdstr != '\0')&&(ac < CMDMAX-1) ; cmdstr++ ){
		switch( *cmdstr )
		{
		case '\"':
			flag_quote = !flag_quote;
			break;
		case ' ':
		case '\t':
			if( !flag_quote ){
				if( !flag_space ){
					*(*(av++) + i) = '\0';
					ac++, i = 0;
					flag_space = TRUE;
				}
				break;
			}
		default:
			if( i < ENVMAX - 1 ){
				*(*av + i) = *cmdstr;
				i++;
				flag_space = FALSE;
			} else {
				eprintf("[Error]: buffer full.");
				return 0;
			}
		}
	}
	if( i ) *(*(av++) + i) = '\0', ac++;
	*av = NULL;
	return ac;
}

unsigned char *getenv_from_file( unsigned char *filename, unsigned char *envname, unsigned char *env_buf )
/*
	�w�肳�ꂽ�t�@�C��������ϐ���ǂݎ��B
	�c���A���̊֐��͖{���Ȃ�Ăяo�����͂����Ȃ��񂾂��ǁc(^^;�B
	3rd. argument should be ... env_buf[ENVMAX]
*/
{
	FILE *stream;
	unsigned char *s, *r;

	if( filename == NULL ) return NULL;

	if( (stream = fopen(filename,"rt")) == NULL ) return NULL;
	while( fgets(env_buf,ENVMAX-1,stream) != NULL ){
		for( s=env_buf; (*s==' ')||(*s=='\t'); s++ ) ;
		if( _mbschr(s,'=') != NULL &&
			_mbsnicmp(s,envname,_mbslen(envname)) == 0
			) break;
		s = NULL;
	}
	fclose(stream);
	if( s == NULL ) return NULL;
	for(s=_mbschr(env_buf,'=')+1;(*s==' '||*s=='\t');s++) ;
	for(r=env_buf;*s!='\0';){
		if( (*s=='%')&&(*(s+1)=='%') ) s++;
		*(r++) = *(s++);
	}
	*r = '\0';
	return env_buf;
}

void normalize_filename( unsigned char *filename )
/*
	�t�@�C������ "/" -> "\" �ƑS�ď������ɕϊ�����
*/
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
#ifndef	_TMC_DEBUG
	WritePrivateProfileString(szSecName,szValueName_ExecFlag,ret_code,dyn_ini);
#else
	fputs("Return Code = ",stderr), fputs(ret_code,stderr), fputs("\n",stderr);
#endif
	return code;
}

void eprintf( char *str )
{
	fputs(szAppName,stderr);
	fputs(": ",stderr);
	fputs(str,stderr);
	fputs("\n",stderr);
	return;
}

int ExecEditor( char *cmd, char **argv )
{

#ifndef	_TMC_DEBUG
	return _spawnvp(P_WAIT,cmd,argv);
#else
	fputs("Spawning External Application:\n",stderr);
	fputs("  Command:   ",stderr), fputs(cmd,stderr), fputs("\n",stderr);
	fputs("  1st. argv: ",stderr), fputs(*argv,stderr), fputs("\n",stderr);
	return 0;
#endif
}
