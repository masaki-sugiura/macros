#include	<windows.h>
#include	<string.h>
#include	<mbstring.h>

#if	0
#define	_TMC_DEBUG	1
#endif

UCHAR	KeyName[5*MAX_PATH] = "Software\\Sugi_Macro\\TeXModuleControl";

void	DeleteKeys( HKEY, UCHAR * );

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpszCmdLine, int CmdShow )
{
	HKEY	hKey;

#ifdef	_TMC_DEBUG
	__argc = 2;
	*(__argv+1) = "-c:";
#endif

	if( __argc != 2 ){
		MessageBox(NULL,"Invalid Command Line",NULL,MB_OK);
		return 1;
	}
	__argv++;
	if( !strnicmp(*__argv,"-c:",3) ) hKey = HKEY_CURRENT_USER;
	else if( !strnicmp(*__argv,"-l:",3) ) hKey = HKEY_LOCAL_MACHINE;
	else {
		MessageBox(NULL,"Invalid Command Line",NULL,MB_OK);
		return 1;
	}
	strcat(KeyName,*__argv+3);
	DeleteKeys(hKey,KeyName);
	return 0;
}

void DeleteKeys( HKEY hKeyRoot, UCHAR *szDeleteKeyName )
{
	HKEY	hKeyDelete;
	DWORD	dwCounter = 0L, dwLength = strlen(szDeleteKeyName),
			dwKeyNum, dwMaxKeyNameLength;
	UCHAR	*subkey = szDeleteKeyName + dwLength, *buf;

	if( RegOpenKeyEx(	hKeyRoot,
						szDeleteKeyName,
						0,
						KEY_ALL_ACCESS,
						&hKeyDelete ) != ERROR_SUCCESS	||
		RegQueryInfoKey(	hKeyDelete,
							NULL, 0,
							NULL,
							&dwKeyNum,
							&dwMaxKeyNameLength,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL	) != ERROR_SUCCESS	){
		MessageBox(NULL,"Failed to Open Registry.",NULL,MB_OK);
		return;
	}
	if( dwLength + dwMaxKeyNameLength >= MAX_PATH ) return;
	*subkey++ = '\\';
	buf = (UCHAR*)malloc(dwKeyNum*(dwMaxKeyNameLength+1)+1);
	for( dwCounter=0; dwCounter<dwKeyNum; dwCounter++ ){
		dwLength = dwMaxKeyNameLength + 1;
		RegEnumKeyEx(	hKeyDelete,
						dwCounter,
						(LPTSTR)(buf+dwCounter*(dwMaxKeyNameLength+1)),
						&dwLength,
						NULL,
						NULL,
						NULL,
						NULL	);
	}
	RegCloseKey(hKeyDelete);
	for( dwCounter=0; dwCounter<dwKeyNum; dwCounter++ ){
		strcpy(subkey,buf+dwCounter*(dwMaxKeyNameLength+1));
		DeleteKeys(hKeyRoot,szDeleteKeyName);
	}
	free(buf);
	*--subkey = '\0';
#ifndef	_TMC_DEBUG
	if( RegDeleteKey(hKeyRoot,szDeleteKeyName) != ERROR_SUCCESS ){
#else
	{
#endif
		buf = (UCHAR*)malloc(strlen(szDeleteKeyName)+24);
#ifndef	_TMC_DEBUG
		strcpy(buf,"Failed to Delete :\n ");
#else
		*buf = '\0';
#endif
		strcat(buf,szDeleteKeyName);
		MessageBox(NULL,buf,NULL,MB_OK);
		free(buf);
	}
	return;
}
