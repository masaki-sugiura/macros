#
#	HEADER
#		name:		HIDEMARU Editor's Macro Subroutine Library
#						Common Definitions and Functions AWK Script
#		filename:	common.awk
#		version:	0.03
#		author:		M. Sugiura(sugiura@ceres.dti.ne.jp)
#		date:		1999/06/01
#
#	DOCUMENT
#
#		説明：
#			マクロライブラリファイル管理 AWK SCRIPT 共通変数と関数の定義
#
#
BEGIN	{
	#	文字列定数の定義

	#	環境変数名(内容は下記参照)
	ENV_SRCDIR = "HM_SRCDIR";
	ENV_LIBDIR = "HM_LIBDIR";
	ENV_MACDIR = "HM_MACDIR";
	ENV_TMPDIR = "TMPDIR";

	#	サブルーチン名とライブラリファイル名を対応づけるファイルのファイル名
	CSP_FILENAME = "corresp.txt";

	#	環境変数を定義するファイルのファイル名
	ENV_FILENAME = "environ.txt";

	#	出力先(テンポラリ)ファイル名
	TMP_FILENAME = "hmmaclib.$$$";

	#	エラー情報出力ファイル名
	ERR_FILENAME = "err_info.txt";

	#	カレントディレクトリ取得コマンド名
	CMDSTR_CD = "cd";

	#	ファイル削除コマンド名
	CMDSTR_RM = "del";

	#	ファイル移動コマンド名
	CMDSTR_MV = "move";

	#	カレントディレクトリ名の取得
	CMDSTR_CD | getline CURDIR;
	close(CMDSTR_CD);
	if( substr(CURDIR,length(CURDIR),1) == "\\" )
		CURDIR = tolower(substr(CURDIR,1,length(CURDIR)-1));
	if( DEBUG ) printf("CURDIR is defined as [%s].\n",CURDIR);

	#	ディレクトリの定義(環境変数より)
	#
	#	SRCDIR	:	#include 文を含んだマクロファイルのディレクトリ名
	#	LIBDIR	:	サブルーチンライブラリマクロのディレクトリ名
	#	MACDIR	:	マクロ出力先ディレクトリ名
	#	TMPDIR	:	テンポラリファイル出力ディレクトリ名
	#
	if( (SRCDIR = tolower(get_env(ENV_SRCDIR))) == "" ) SRCDIR = CURDIR;
	if( (LIBDIR = tolower(get_env(ENV_LIBDIR))) == "" ) LIBDIR = CURDIR;
	if( (MACDIR = tolower(get_env(ENV_MACDIR))) == "" ) MACDIR = CURDIR;
	if( (TMPDIR = tolower(get_env(ENV_TMPDIR))) == "" ) TMPDIR = CURDIR;

	#	対応づけファイルとテンポラリファイルのフルパス名の生成
	TMPFILE = declare_filename(TMP_FILENAME,TMPDIR);

	#	エラー情報出力ファイル名の生成
	ERRFILE = declare_filename(ERR_FILENAME,SRCDIR);
}


function	get_corresp( dir	,file,i,ret,line,csp,libdir,n,a )
#	指定されたディレクトリにある CSP_FILENAME(corresp.txt) から
#	サブルーチン名とファイル名の対応を調べる関数
#
#	引数：
#		char*	dir	:	";" で区切られたディレクトリ名
#	戻り値：
#		none
#
{
	n = split(dir,libdir,";");
	for( a=1; a<=n; a++ ){
		if( libdir[a] == "" ) continue;
		file = declare_filename(CSP_FILENAME,libdir[a]);
		for(i=0;;){
			ret = (getline line < file);
			if( ret == -1 ) return;
			else if( !ret ) break;
			else if( substr(line,1,1) != "#" ){
				i = split(line,csp," ");
				AR_ROUTINEDIR[csp[1]] = libdir[a];
				AR_ROUTINENUM[csp[1]] = i - 1;
				for(;i>1;i--) AR_ROUTINENAME[csp[1],(i-1)] = csp[i];
			}
		}
		close(file);
	}
	return;
}

function	movefile( orgfile, newfile	,err )
#	指定ファイルを移動する関数
#
#	引数：
#		char*	orgfile	:	移動元ファイル名
#		char*	newfile	:	移動先ファイル名
#	戻り値：
#		int		err	:	失敗した場合は = 2, 成功なら = 0.
#
{
	err = 0;
	oldfile = newfile;
	sub(/\.[^\.]+$/,".old",oldfile);
	system(sprintf("if exist %s %s %s",oldfile,CMDSTR_RM,oldfile));
	system(sprintf("if exist %s %s %s %s",newfile,CMDSTR_MV,newfile,oldfile));
	if( !system(sprintf("%s %s %s",CMDSTR_MV,orgfile,newfile)) ){
		printf("%s was changed.\n",newfile);
		err = 0;
	}else {
		eprintf("Fatal Error: Move file is failed.\n");
		err = 2;
	}
	if( DEBUG )
		eprintf(sprintf("[%s]=>[%s] [%s!!]\n",orgfile,newfile,err?"NG":"OK"));
	return err;
}

function	declare_filename( basename, dir )
#	ファイルベース名とディレクトリ名からフルパス名を生成する関数
#
#	引数：
#		char*	basename	:	ファイルベース名
#								もし basename が絶対パス名だった場合は
#								これをそのまま返す。
#		char*	dir			:	ディレクトリ名
#	戻り値：
#		char*	:	生成されたフルパス名
#
{
	if( substr(dir,length(dir),1)!="\\" ) dir = dir "\\";
	if( substr(basename,1,1)=="\\" || index(basename,":") ) return basename;
	else return sprintf("%s%s",tolower(dir),basename);
	#	not reached.
}

function	get_env( envname	, env )
#	環境変数から値を得る関数
#
#	引数：
#		char*	envname	:	環境変数名
#	戻り値：
#		char*	:	得られた環境変数の値
#
{
	env = "";
	if( envname in ENVIRON ) env = ENVIRON[envname];
	if( env == "" && CURDIR != "" ) env = get_env_from_file(envname,CURDIR);
	if( env == "" && SRCDIR != "" ) env = get_env_from_file(envname,SRCDIR);
	if( env == "" && LIBDIR != "" ) env = get_env_from_file(envname,LIBDIR);
	return env;
}

function	get_env_from_file( envname, dir		, file, line, env, temp )
#	環境変数をファイルから得る関数
#
#	引数：
#		char*	envname	:	環境変数名
#		char*	dir		:	ENV_FILENAME のあるフォルダ名
#	戻り値：
#		char*	:	得られた環境変数の値
#
{
	env = "";
	file = declare_filename(ENV_FILENAME,dir);
	if( !(file in ENVFILE_LIST) ){
		if( DEBUG ) eprintf(sprintf("Getting Env. from [%s]\n",file));
		while( (getline line < file) > 0 ){
			if( (substr(line,1,1) == "#") || (split(line,temp,/[ \t]*=[ \t]*/) != 2) ) continue;
			if( DEBUG ) eprintf(sprintf("Got Env. Def. for %s = %s\n",temp[1],temp[2]));
			sub(/^[ \t]*/,"",temp[1]);
			temp[1] = toupper(temp[1]);
			if( !(temp[1] in ENVIRON) ) ENVIRON[temp[1]] = temp[2];
		}
		ENVFILE_LIST[file] = "";
	}
	close(file);
	if( envname in ENVIRON ) return ENVIRON[envname];
	else return "";
}

function	eprintf( msg )
#	エラーメッセージ出力関数
#
#	引数：
#		char*	msg	:	出力するメッセージ
#	戻り値：
#		none
#
{
	if( !DECLARE_ERRFILE_HEADER ){
		printf("##### Error (& Debug) Output from %s.\n",SCRIPTNAME) > ERRFILE;
		printf("##### Date: %s\n",strftime("%Y/%m/%d %H:%M:%S")) >> ERRFILE;
		printf("##### Other Info: %s\n",ERROR_MSG_HEADER) >> ERRFILE;
		DECLARE_ERRFILE_HEADER = 1;
	}
	printf("%s",msg) > "/dev/stdout";
	printf("%s",msg) >> ERRFILE;
	return;
}

