#include	<windows.h>

static LPCTSTR	lpszKeyName = "SOFTWARE\\Reg_Access_Test",
				lpszValueName = "value";

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int nCmdShow )
{
	HKEY hKeyTest;
	REGSAM	OpenOpt[3] = {KEY_ALL_ACCESS,KEY_READ|KEY_WRITE,KEY_READ};
	LPTSTR	OptStr[3] = {"KEY_ALL_ACCESS","KEY_READ|KEY_WRITE","KEY_READ"},
			szMsg[2] = {"Succeed to open Reg. for ","Failed to open Reg. for "};
	char	msgbuf[64];
	int i, msg;

	for( i=0; i < 3; i++ ){
		if( RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
							lpszKeyName,
							0,
							OpenOpt[i],
							&hKeyTest ) == ERROR_SUCCESS ) msg = 0;
		else msg = 1;
		lstrcpy(msgbuf,szMsg[msg]), lstrcat(msgbuf,OptStr[i]);
		MessageBox( NULL, msgbuf, NULL, MB_OK );
		RegCloseKey(hKeyTest);
	}
	return 0;
}
