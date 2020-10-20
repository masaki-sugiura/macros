#define		_MBCS
#include	<string.h>
#include	<mbstring.h>
#include	<windows.h>

#if	0
#define	_TMC_DEBUG	1
#endif

#define	MAX_SECTION	(UINT)0x7FFF

typedef struct	{
	UCHAR	id;
	UCHAR*	str;
} DETECT_DATA, *LPDETECT_DATA;

DETECT_DATA
TeXTypeDetectData_WinTeX[3] = {
	{1, "big"},
	{1, "ams"},
	{0, ""}
},
TeXTypeDetectData_pTeX2GUI[5] = {
	{3, "j"},
	{2, "pdf"},
	{1, "platex"},
	{1, "ptex"},
	{0, ""}
};
LPDETECT_DATA
TeXTypeDetectData[2] = {TeXTypeDetectData_WinTeX, TeXTypeDetectData_pTeX2GUI};

UCHAR	szKeyName[64] = "Software\\Sugi_Macro\\TeXModuleControl\\FormatMenu\\",
		szKeyName_pTeX2GUI[] = "Software\\OTOBE\\bwpvtex\\Format Files",
		szKeyName_WinTeX[] = "TeXMode",
		szIniFileName_WinTeX[] = "wintex.ini",
		*lpszPlatformName[2] = {"WinTeX","pTeX2GUI"};
HKEY	hKeyFormat;

UCHAR*	GetFormatInfo( void );

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
					  LPTSTR lpCmdLine, int nCmdShow )
{
	UCHAR	*buf;
	HKEY	hKeyFormat;
	DWORD	dwResult;
	int		type;

	if( hPrevInstance ) return 1;

	if( __argc < 2 ){
#ifdef	_TMC_DEBUG
		__argc = 2;
		__argv[1] = "ptex2gui";
#else
		MessageBox(NULL,"Invalid Command Line.",NULL,MB_OK);
		return 1;
#endif
	}
	for( type=0; type<2; type++ )
		if( !stricmp(__argv[1],lpszPlatformName[type]) ) break;

	switch( type ){
	case 2:
		return 1;

	case 1:
		if( !(buf = GetFormatInfo()) ) return 2;
		break;

	case 0:
		buf = (UCHAR*)malloc(MAX_SECTION);
		if( GetPrivateProfileSection(	szKeyName_WinTeX,
										buf,
										MAX_SECTION,
										szIniFileName_WinTeX	)
			== MAX_SECTION - 2 ){
			free(buf);
			return 2;
		}

	}

#ifndef	_TMC_DEBUG
	strcat(szKeyName,lpszPlatformName[type]);
	if( RegCreateKeyEx(	HKEY_CURRENT_USER,
						szKeyName,
						(DWORD)NULL,
						(LPTSTR)NULL,
						REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS,
						NULL,
						&hKeyFormat,
						&dwResult	) == ERROR_SUCCESS	){
#else
	{
#endif
		UCHAR	*p = buf, *w, *f, name[3];
		DWORD	dwLength, dwMaxLength = 0, dwFormatNum = 0,
				n, i, j;

		while( *p ){
			dwLength = strlen(p);
			if( !dwLength ) break;
			dwFormatNum++;
			dwMaxLength = max(dwMaxLength,dwLength);
			p += dwLength + 1;
		}
		w = (UCHAR*)malloc(dwMaxLength+10);

		for( n=0, p=buf; *p && n<dwFormatNum; n++ ){
			if( !(f = _mbschr(p,'=')) ) break;
			else *f++ = '\0';
			_mbslwr(f);
			for( i=1; *(w+i)=*f; i++, f++ ) ;
			*(w+i++) = '\0';
			strcpy(w+i,p);
			_mbslwr(p);
			for( j = 0; TeXTypeDetectData[type][j].id; j++ ){
				if( type ){
					if( _mbsstr(w+1,TeXTypeDetectData[type][j].str) ) break;
				} else {
					if( _mbsstr(p,TeXTypeDetectData[type][j].str) ) break;
				}
			}
			*w = '0' + TeXTypeDetectData[type][j].id;
			*(w+i-1) = ':';
			p = f + 1;
#ifndef	_TMC_DEBUG
			_itoa(n,name,10);
			RegSetValueEx(	hKeyFormat,
							(LPTSTR)name,
							(DWORD)NULL,
							REG_SZ,
							(LPTSTR)w,
							strlen(w) );
#else
			MessageBox(NULL,w,NULL,MB_OK);
#endif
		}
#ifndef	_TMC_DEBUG
		RegSetValueEx(	hKeyFormat,
						"Num",
						(DWORD)NULL,
						REG_DWORD,
						(LPBYTE)&dwFormatNum,
						sizeof(DWORD)	);
		RegCloseKey(hKeyFormat);
#endif
		free(w);
	}
	free(buf);
	return 0;
}

UCHAR* GetFormatInfo( void )
{
	UCHAR	*buf, *top, *value_buf, *data_buf, *s;
	DWORD	dwValueNum, dwMaxDataLength, dwMaxValueLength,
			dwValueLength, dwDataLength,
			dwIndex, dwType = REG_SZ;
	int		i = 0;
	HKEY	hKeyFormat;

	if( RegOpenKeyEx(	HKEY_CURRENT_USER,
						szKeyName_pTeX2GUI,
						0,
						KEY_ALL_ACCESS,
						&hKeyFormat ) != ERROR_SUCCESS ){
		return NULL;
	} else if(	RegQueryInfoKey(	hKeyFormat,
									NULL,NULL,
									NULL,
									NULL,NULL,NULL,
									&dwValueNum,
									&dwMaxValueLength,
									&dwMaxDataLength,
									NULL,NULL	) != ERROR_SUCCESS	||
		!(dwValueNum&&dwMaxValueLength&&dwMaxDataLength)	){
		RegCloseKey(hKeyFormat);
		return NULL;
	}

	top = buf = (UCHAR*)malloc(dwValueNum*(dwMaxValueLength+dwMaxDataLength+2)+2);
	value_buf = (UCHAR*)malloc(dwMaxValueLength+dwMaxDataLength+2);
	data_buf = value_buf + dwMaxValueLength + 1;
	dwIndex = 0;
	while( dwIndex < dwValueNum ){
		dwValueLength = dwMaxValueLength+1;
		dwDataLength = dwMaxDataLength+1;
		if( RegEnumValue(	hKeyFormat,
							dwIndex++,
							(LPTSTR)value_buf,
							&dwValueLength,
							NULL,
							&dwType,
							(LPTSTR)data_buf,
							&dwDataLength	) != ERROR_SUCCESS	||
			!(dwValueLength&&(dwDataLength-1))	) break;
		for(	s = data_buf+dwDataLength-1;
				*s != '(';
				s--	){
			if( *s == ')' || *s == '.' ) *s = '\0';
		}
		*s = '=';
		s = data_buf;
		while( *top++ = *s++ ) ;
	}
	*top = '\0';
	free(value_buf);
	RegCloseKey(hKeyFormat);
	return buf;
}

