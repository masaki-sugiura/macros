#
#	HEADER
#		name:		HIDEMARU Editor's Macro Subroutine Library
#						Library Data Correspondings Generating AWK Script
#		filename:	gen_csp.awk
#		version:	0.01
#		author:		M. Sugiura(sugiura@ceres.dti.ne.jp)
#		date:		1997/09/20
#
#	DOCUMENT
#
#		説明：
#			マクロライブラリファイル中のサブルーチン一覧テーブルを作成する
#			(出力は SRCDIR\corresp.txt に行う)
#
#		実行方法＆引数：
#
#			gawk -f common.awk -f gen_csp.awk inputfile(s)
#
#				inputfile(s)	:	マクロライブラリファイル名
#									(後述の SRCDIR からの相対パス or 絶対パス)
#
#

BEGIN	{
	#	デバッグフラグ
#	DEBUG = 1;

	#	AWK スクリプトファイル名
	SCRIPTNAME = "gen_csp.awk";

	#	エラーコード
	#		0	:	正常終了
	#		1	:	サブルーチン名取得エラー(スクリプトは続行)
	#		2	:	ファイルＩＯエラー(スクリプトは強制終了)
	#		255	:	引数/環境変数定義のエラー(　　　〃　　　)
	#
	ERROR = 0;

	#	引数の処理
	if( ARGC < 2 ){
		printf("Usage: %s -f common.awk -f %s inputfile(s)\n",ARGV[0],SCRIPTNAME);
		ERROR = 255;
		exit;
	}
	ERROR_MSG_HEADER = "BaseDir: [" SRCFILE "] In-files: ";
	for( i=1;i<ARGC;i++){
		ERROR_MSG_HEADER = ERROR_MSG_HEADER "[" ARGV[i] "] ";
		ARGV[i] = declare_filename(ARGV[i],SRCDIR);
	}

	if( DEBUG ){
		DB_ROUTINE_NUM = 0;
	}

	get_corresp(SRCDIR);
	CSPFILE = declare_filename(CSP_FILENAME,SRCDIR);

	CUR_INFILE = "";
	FS = "";
}

/^[A-Za-z0-9_]+:$/	{
	routine = substr($0,1,length($0)-1);
	if( DEBUG ){
		DB_ROUTINE_NUM++;
		eprintf(sprintf("Subroutine [%s] is found.\n",routine));
	}
	if( FILENAME != CUR_INFILE ){
		CUR_INFILE = FILENAME;
		CUR_LIBNAME = tolower(index(tolower(CUR_INFILE),tolower(SRCDIR))==1?substr(CUR_INFILE,length(SRCDIR)+2):CUR_INFILE);
		AR_ROUTINENUM[CUR_LIBNAME] = 0;
	}
	AR_ROUTINENUM[CUR_LIBNAME]++;
	AR_ROUTINENAME[CUR_LIBNAME,AR_ROUTINENUM[CUR_LIBNAME]] = routine;
}

END	{
	if( ERROR < 255 ){
		gen_cspfile_header(TMPFILE);
		for( libname in AR_ROUTINENUM ){
			printf("%s",libname) >> TMPFILE;
			for( i=1; i<=AR_ROUTINENUM[libname]; i++ )
				printf("\t%s",AR_ROUTINENAME[libname,i]) >> TMPFILE;
			printf("\n") >> TMPFILE;
		}
		printf("\n") >> TMPFILE;
		close(TMPFILE);
		if( DEBUG ){
			eprintf("Debug information:\n");
			eprintf(" GENERATED STRINGS:\n");
			eprintf(sprintf("  TEMP FILE  : %s\n",TMPFILE));
			eprintf(sprintf("  CSP FILE   : %s\n",CSPFILE));
			eprintf(" STATISTICS:\n");
			eprintf(sprintf("  INPUT LINE : %d\n",NR));
			eprintf(sprintf("  FOUND ROUTINE : %d\n",DB_ROUTINE_NUM));
			eprintf(sprintf("  RETURN CODE: %d\n",ERROR));
		} else if( !ERROR ) ERROR = movefile(TMPFILE,CSPFILE);
		if( (!DEBUG) && ERROR ){
			eprintf(sprintf("[%s] was not changed due to some errors.\n",CSPFILE));
			eprintf(sprintf("Output file is remaining as [%s].\n",TMPFILE));
		}
	}
	exit ERROR;
}

function	gen_cspfile_header( file )
{
	printf("# generated by %s at %s\n",SCRIPTNAME,strftime("%Y/%m/%d %H:%M:%S")) > file;
	printf("#\tマクロライブラリファイルとサブルーチン名の対応テーブル\n") >> file;
	printf("#\n") >> file;
	printf("#\t書式：\n") >> file;
	printf("#\t\tlibrary_macro_name\tSubroutine_name_1\tSubroutine_name_2\t...\n") >> file;
	printf("#\n") >> file;
	printf("#\t\t\tlibrary_macro_name\t:\tマクロライブラリファイル名\n") >> file;
	printf("#\t\t\tSubroutine_name_?\t:\tサブルーチン名\n") >> file;
	printf("#\n") >> file;
	printf("#\t\t※それぞれ空白(半角スペース、タブ)で区切る\n") >> file;
	printf("#\n") >> file;
	return;
}

