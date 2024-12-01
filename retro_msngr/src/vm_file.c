/** Виртуальная машина **/
#include "lib/e5vm.h"

/** Внешние функции для всех платформ **/
#include "lib/externs.h"

/** Обязательно должна быть реализация массива EXTERNS **/
CELL EXTERNS[][3] =
{
	{0xbfcadd1a, 	(CELL)ext_prints, 				1},
	{0x42a82460, 	(CELL)ext_printi, 				1},
	{0x9b94a3e2, 	(CELL)ext_bl_allc, 				2},
	{0x187cf1ec,	(CELL)ext_get_env,				1},
	{0x272d2162, 	(CELL)ext_atoi,					1},
	{0x98574167,	(CELL)ext_read,					2},
	{0x0e4d97e1,	(CELL)ext_mkdir,				1},
	{0x5018ceb8,	(CELL)ext_readfile,				1},
	{0x900f6a6e,	(CELL)ext_strcat,				2},
	{0xb868a14f,	(CELL)ext_securecat,			2},
	{0x52ff8a3f,	(CELL)ext_strstr,				2},
	{0x025d112d,	(CELL)ext_strlen,				1},
	{0x3bd7e17b,	(CELL)ext_strcmp,				2},
	{0xee2a7b8f, 	(CELL)ext_writefile,			2},
	{0x7ea97bee,	(CELL)ext_readcmd,				1},
	{0xec4281bf,	(CELL)ext_itoa,					1},
	{0x913417b1,	(CELL)ext_dircnt,				1},
	{0x8262cb25,	(CELL)ext_direxist,				1},
	{0xa3c8866e,	(CELL)ext_chdir,				1},
	{0x44c8f818,	(CELL)ext_list,					1},
	{0, 			0, 								0}
};

// Байткоды модулей, для исполнения на виртуальной машине
char* login = "[REMOVED]";

char* reg = "[REMOVED]";

char* sendmsg = "[REMOVED]";

char* getmsg = "[REMOVED]";

char* test  = "[REMOVED]";

char* list = "[REMOVED]";

char* admin = "[REMOVED]";


void main(int argc, char* argv[]){
	printf("Content-Type: text/html\r\n\r\n");

	if(			!strcmp("/var/www/html/login.m",argv[0])){
		runVM(login);
	}else if(	!strcmp("/var/www/html/register.m",argv[0])){
		runVM(reg);
	}else if(	!strcmp("/var/www/html/sendmsg.m",argv[0])){
		runVM(sendmsg);
	}else if(	!strcmp("/var/www/html/getmsg.m",argv[0])){
		runVM(getmsg);
	}else if(	!strcmp("/var/www/html/list.m",argv[0])){
		runVM(list);
	}else if(	!strcmp("/var/www/html/admin.m",argv[0])){
		runVM(admin);
	}
}