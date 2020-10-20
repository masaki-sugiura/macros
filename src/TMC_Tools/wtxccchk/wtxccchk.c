/*
	TeX Compile Complete Checker (for 32bit Windows)
		for "TeX Module Control with HIDEMARU MACRO Ver.3.X.X".
	Copyright (C) 1997, M. Sugiura(PXF05775@niftyserve.or.jp)
*/

#include	<windows.h>

#define		__TMC_NOCONSOLE__
#define		__TMC_LOGREADER__
#include	"../tmctools.h"

FNAME	fname;

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
			LPTSTR lpCmdLine, int nCmdShow )
{
	if( !set_FNAME(__argv[1],&fname) ) return Return_VirTeX(RETURN_ERROR);
	return Return_VirTeX(isCompileComplete(&fname));
	/* Not Reached. */
}

