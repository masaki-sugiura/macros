/*
	Common Includefile for TeX Module Control with HIDEMARU Macro Support Tools
*/

#ifndef		__TMC_INC__
#define		__TMC_INC__

//	string buffer size
#define		MAX_LINE	MAX_PATH

//	returned value from virtex module
#ifndef		__TMC_NORETURN__
#define		RETURN_COMPLETE		  0
#define		RETURN_CONTINUE		  1
#define		RETURN_EDIT			  2
#define		RETURN_STOP			  4
#define		RETURN_ERROR		255
#endif	/* __TMC_NORETURN__ */

//	registry key & value names
#ifndef		__TMC_NOREG__
#define		RK_LM_TMC_ROOT		"Software\\Sugi_Macro\\TeXModuleControl"
#define		RK_CU_TMC_ROOT		"Software\\Sugi_Macro\\TeXModuleControl"
#define		RK_LM_TMC_MACROCFG	RK_LM_TMC_ROOT "\\MacroConfig"
#define		RK_CU_TMC_MACROCFG	RK_CU_TMC_ROOT "\\MacroConfig"
#define		RV_TMC_MACRODIR		"MacroDir"
#define		DYN_INI				"\\dyndata.ini"
#define		DYN_MODULECMD		"ModuleCmd"
#define		DYN_RETURN_VTEX		"return_virtex"
#define		DYN_ERRORFILE		"Error_FileName"
#define		DYN_ERRORLINE		"Error_Line"
#endif	/* __TMC_NOREG__ */

typedef struct FNAME_tag {
	short			ishortname[2];
	unsigned char	filename[MAX_LINE];
} FNAME;

#ifndef		__TMC_NODECLARE_FUNC__
extern unsigned char*	getinifilename( unsigned char * );
extern LPTSTR	getshortname( LPTSTR, int );
#if		(!defined(__TMC_NOWINAPP__)) || defined(__TMC_NOCONSOLE__)
extern FNAME*	set_FNAME( LPTSTR, FNAME * );
#endif	/* (!defined(__TMC_NOWINAPP__)) || defined(__TMC_NOCONSOLE__) */
#ifdef		__TMC_LOGREADER__
extern int	isCompileComplete( FNAME * );
extern int	Return_VirTeX( int );
#endif	/* __TMC_LOGREADER__ */
#endif	/* __TMC_NODECLARE_FUNC__ */

#endif	/* __TMC_INC__ */

