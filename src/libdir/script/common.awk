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
#		�����F
#			�}�N�����C�u�����t�@�C���Ǘ� AWK SCRIPT ���ʕϐ��Ɗ֐��̒�`
#
#
BEGIN	{
	#	������萔�̒�`

	#	���ϐ���(���e�͉��L�Q��)
	ENV_SRCDIR = "HM_SRCDIR";
	ENV_LIBDIR = "HM_LIBDIR";
	ENV_MACDIR = "HM_MACDIR";
	ENV_TMPDIR = "TMPDIR";

	#	�T�u���[�`�����ƃ��C�u�����t�@�C������Ή��Â���t�@�C���̃t�@�C����
	CSP_FILENAME = "corresp.txt";

	#	���ϐ����`����t�@�C���̃t�@�C����
	ENV_FILENAME = "environ.txt";

	#	�o�͐�(�e���|����)�t�@�C����
	TMP_FILENAME = "hmmaclib.$$$";

	#	�G���[���o�̓t�@�C����
	ERR_FILENAME = "err_info.txt";

	#	�J�����g�f�B���N�g���擾�R�}���h��
	CMDSTR_CD = "cd";

	#	�t�@�C���폜�R�}���h��
	CMDSTR_RM = "del";

	#	�t�@�C���ړ��R�}���h��
	CMDSTR_MV = "move";

	#	�J�����g�f�B���N�g�����̎擾
	CMDSTR_CD | getline CURDIR;
	close(CMDSTR_CD);
	if( substr(CURDIR,length(CURDIR),1) == "\\" )
		CURDIR = tolower(substr(CURDIR,1,length(CURDIR)-1));
	if( DEBUG ) printf("CURDIR is defined as [%s].\n",CURDIR);

	#	�f�B���N�g���̒�`(���ϐ����)
	#
	#	SRCDIR	:	#include �����܂񂾃}�N���t�@�C���̃f�B���N�g����
	#	LIBDIR	:	�T�u���[�`�����C�u�����}�N���̃f�B���N�g����
	#	MACDIR	:	�}�N���o�͐�f�B���N�g����
	#	TMPDIR	:	�e���|�����t�@�C���o�̓f�B���N�g����
	#
	if( (SRCDIR = tolower(get_env(ENV_SRCDIR))) == "" ) SRCDIR = CURDIR;
	if( (LIBDIR = tolower(get_env(ENV_LIBDIR))) == "" ) LIBDIR = CURDIR;
	if( (MACDIR = tolower(get_env(ENV_MACDIR))) == "" ) MACDIR = CURDIR;
	if( (TMPDIR = tolower(get_env(ENV_TMPDIR))) == "" ) TMPDIR = CURDIR;

	#	�Ή��Â��t�@�C���ƃe���|�����t�@�C���̃t���p�X���̐���
	TMPFILE = declare_filename(TMP_FILENAME,TMPDIR);

	#	�G���[���o�̓t�@�C�����̐���
	ERRFILE = declare_filename(ERR_FILENAME,SRCDIR);
}


function	get_corresp( dir	,file,i,ret,line,csp,libdir,n,a )
#	�w�肳�ꂽ�f�B���N�g���ɂ��� CSP_FILENAME(corresp.txt) ����
#	�T�u���[�`�����ƃt�@�C�����̑Ή��𒲂ׂ�֐�
#
#	�����F
#		char*	dir	:	";" �ŋ�؂�ꂽ�f�B���N�g����
#	�߂�l�F
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
#	�w��t�@�C�����ړ�����֐�
#
#	�����F
#		char*	orgfile	:	�ړ����t�@�C����
#		char*	newfile	:	�ړ���t�@�C����
#	�߂�l�F
#		int		err	:	���s�����ꍇ�� = 2, �����Ȃ� = 0.
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
#	�t�@�C���x�[�X���ƃf�B���N�g��������t���p�X���𐶐�����֐�
#
#	�����F
#		char*	basename	:	�t�@�C���x�[�X��
#								���� basename ����΃p�X���������ꍇ��
#								��������̂܂ܕԂ��B
#		char*	dir			:	�f�B���N�g����
#	�߂�l�F
#		char*	:	�������ꂽ�t���p�X��
#
{
	if( substr(dir,length(dir),1)!="\\" ) dir = dir "\\";
	if( substr(basename,1,1)=="\\" || index(basename,":") ) return basename;
	else return sprintf("%s%s",tolower(dir),basename);
	#	not reached.
}

function	get_env( envname	, env )
#	���ϐ�����l�𓾂�֐�
#
#	�����F
#		char*	envname	:	���ϐ���
#	�߂�l�F
#		char*	:	����ꂽ���ϐ��̒l
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
#	���ϐ����t�@�C�����瓾��֐�
#
#	�����F
#		char*	envname	:	���ϐ���
#		char*	dir		:	ENV_FILENAME �̂���t�H���_��
#	�߂�l�F
#		char*	:	����ꂽ���ϐ��̒l
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
#	�G���[���b�Z�[�W�o�͊֐�
#
#	�����F
#		char*	msg	:	�o�͂��郁�b�Z�[�W
#	�߂�l�F
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

