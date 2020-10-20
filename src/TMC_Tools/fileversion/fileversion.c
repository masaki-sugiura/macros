#include	<windows.h>
#include	<winver.h>
#include	<stdio.h>

int main( int argc, char **argv )
{
	VS_FIXEDFILEINFO	*fileinfo;
	DWORD	hUnknown = 1, infosize;

	if( argc <= 1 ) return -1;
	++argv;

	printf("FILE INFORMATION FOR %s\n",*argv);
	if( !(infosize = GetFileVersionInfoSize(*argv,&hUnknown)) ){
		printf("Get no file information, code is %ld.\n",GetLastError());
		return 1;
	}
	if( (fileinfo = (VS_FIXEDFILEINFO *)calloc(infosize,sizeof(char))) == NULL ){
		printf("No memory was available.\n");
		return 2;
	}
	if( !GetFileVersionInfo(*argv,hUnknown,infosize,fileinfo) ){
		printf("Version Information cannot be retrieved.\n");
		return 4;
	}
	printf("OS TYPE: ");
	switch( (fileinfo->dwFileOS)&(VOS_DOS|VOS_OS216|VOS_OS232|VOS_NT) )
	{
	case	VOS_DOS:
		printf("MS-DOS");
		break;
	case	VOS_OS216:
		printf("IBM OS2(16bit)");
		break;
	case	VOS_OS232:
		printf("IBM OS2(32bit)");
		break;
	case	VOS_NT:
		printf("Windows NT");
		break;
	default:
		printf("Unknown");
		break;
	}
	printf("\nWINDOW MANAGER TYPE: ");
	switch( (fileinfo->dwFileOS)&(VOS__WINDOWS16|VOS__PM16|VOS__PM32|VOS__WINDOWS32) )
	{
	case	VOS__WINDOWS16:
		printf("Windows 3.X");
		break;
	case	VOS__PM16:
		printf("Presentation Manager (16bit)");
		break;
	case	VOS__PM32:
		printf("Presentation Manager (32bit)");
		break;
	case	VOS__WINDOWS32:
		printf("Windows 95/NT");
		break;
	default:
		printf("Unknown");
		break;
	}
	printf("\n");
	free(fileinfo);
	return 0;
}
