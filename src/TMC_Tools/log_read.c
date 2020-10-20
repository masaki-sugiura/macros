#include	<stdio.h>
#include	<stdlib.h>
#include	<mbstring.h>
#include	<windows.h>
#define	__TMC_NODECLARE_FUNC__
#include	"tmctools.h"

/*	Declarations	*/
int	isCompileComplete( FNAME * );
int	Return_VirTeX( int );
extern	unsigned char*	getinifilename( unsigned char * );

typedef struct {
	int	err_code;
	unsigned char	err_msg[64];
} ERRMSG;

/*		*/
ERRMSG	err[] = {
					{ RETURN_EDIT, "? e" },
					{ RETURN_STOP, "? x" },
					{ RETURN_STOP, "! Emergency stop." },
					{ RETURN_STOP, "No pages of output." },
					{ RETURN_CONTINUE, "LaTeX Warning: Label(s) may have changed." },
					{ RETURN_CONTINUE, "LaTeX Warning: There were undefined references." },
					{ RETURN_ERROR, "" }
				};

/*
	�ȉ��͊֐���`
*/

int isCompileComplete( FNAME *fname )
/*
	�w�肳�ꂽ TeX �t�@�C���� LOG �t�@�C������R���p�C���̏I����Ԃ��擾

	�����F
		FNAME *fname	:	TeX �t�@�C����etc. ���i�[����\���̂ւ̃|�C���^
	�߂�l�F
		int	:	RETURN_*** (�Ӗ��ɂ��Ă� tmctools.h ���Q��)
*/
{
	static unsigned char buf[MAX_LINE];
	int i, ret;
	FILE *stream;

	_mbscpy(buf,fname->filename);
	for(i=1;i>=0;i--){
		_mbscpy(buf+fname->ishortname[i],".log");
		if( (stream = fopen(buf,"rt")) != NULL ) break;
	}
	if( i < 0 ) return RETURN_ERROR;

	ret = RETURN_COMPLETE;
	while( fgets(buf,MAX_LINE-1,stream) != NULL ){
		for( i=0; err[i].err_code != RETURN_ERROR; i++ ){
			if( _mbsstr(buf,err[i].err_msg) != NULL ){
				ret = err[i].err_code;
				goto _end;
			}
		}
	}
_end:
	fclose(stream);
	return ret;
}

int Return_VirTeX( int ret )
/*
	�w�肳�ꂽ�l�� ini file �ɏ������݂܂��B

	ret		:	�������ޒl
	return	:	ret �����̂܂ܕԂ��B
*/
{
	static unsigned char	buf[8], szIniFileName[MAX_LINE];

	_itoa(ret,buf,10);
	if( getinifilename(szIniFileName) != NULL )
		WritePrivateProfileString(DYN_MODULECMD,DYN_RETURN_VTEX,buf,szIniFileName);
	return ret;
}

