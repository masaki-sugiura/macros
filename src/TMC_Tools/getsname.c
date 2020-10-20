#include	<mbstring.h>
#include	<windows.h>
#define	__TMC_NODECLARE_FUNC__
#include	"tmctools.h"

/*	Declarations	*/
LPTSTR	getshortname( LPTSTR, int );

/*	Definitions	*/
LPTSTR	getshortname( LPTSTR filename, int ptexversion )
/*
	TeX �t�@�C�������� LOG, AUX, DVI �t�@�C��etc. �̃t�@�C���m�[�h����
	���邽�߂̏I�[�|�C���^���擾

	�����F
		LPTSTR filename	:	TeX �t�@�C�����ւ̃|�C���^
		int ptexversion	:	pTeX �̃}�C�i�[�o�[�W���� - 4
							(ex. 2.1.4 -> = 0, 2.1.5 -> = 1)
	�߂�l�F
		LPTSTR	:	�t�@�C���x�[�X����
						ptexversion = 0	:	�Ō��
						ptexversion = 1	:	(�t�@�C���x�[�X���擪������)�ŏ���
					�s���I�h�̃A�h���X
*/
{
	LPTSTR	s;

	if( (s = _mbsrchr(filename,'\\')) == NULL ) s = filename;
	else s++;
	if( *s == '.' ) s++;
	return ptexversion?_mbsrchr(s,'.'):_mbschr(s,'.');
}

