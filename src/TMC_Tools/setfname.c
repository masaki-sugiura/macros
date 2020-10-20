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
	TeX �t�@�C�������� FNAME �\���̂��쐬����

	�����F
		LPTSTR lpCmdLine	:	TeX �t�@�C�������܂ޕ�����ւ̃|�C���^
		FNAME *fname_buf	:	���ʂ��i�[���� FNAME �\���̂ւ̃|�C���^
	�߂�l�F
		FNAME*	:	�i�[�ɐ��������� = fname_buf, ���s������ = NULL
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

