#include <stdio.h>
#include <string.h>

#include "lib.h"

#define POPCURRLVS 			popSTCK((STCK*)st_lvar->last->val) 
#define PSHCURRLVS(Xarg) 	pshSTCK((STCK*)st_lvar->last->val,(CELL)Xarg)
#define PSHARGLVST(Xarg) 	insSTCK((STCK*)st_lvar->last->val,(CELL)Xarg)

#define POPCURBTLT 			popSTCK((STCK*)st_btlt->last->val)
#define PSHCURBTLT(Xarg) 	pshSTCK((STCK*)st_btlt->last->val,(CELL)Xarg)

#define GETLVSADR(Xarg)		getELEM((STCK*)st_lvar->last->val,(HCEL)Xarg)

#define MAXARGS 			32

CELL EXTERNS[][3];

CELL runVM(BYTE* program){
	enum CMDS {
		NONCMD,
		PSHBYT,
		PSHWRD,
		PSHDWD,
		PSHQWD,
		JMPADR,
		EXTEND,
		DECRPT,
		PSHARG,
		POPARG,
		CALFNC,
		RETURN,
		VARVAL,
		VARADR,
		VALADD,
		VALSUB,
		VALMUL,
		VALDIV,
		VALMOD,
		VALAND,
		VALORR,
		VALNOT,
		VALXOR,
		VALSHR,
		VALSHL,
		VALGET,
		VALSET,
		LOGNOT,
		LLOWER,
		LGREAT,
		LLOWEQ,
		LGRTEQ,
		LEQUAL,
		LNOTEQ,
		LANDVL,
		LORVAL
	};
	// Указатель на начало кода
	BYTE* prog_pointer;
	// Ключ
	CELL  keyc;
	// Контрольная сумма кода и длина кода
	HCEL  hash;
	HCEL  plen;
	// Стэк карты байт-листов, стэк аргументов
	STCK* btlt_stack	= crtSTCK(0,0);
	STCK* args_stack	= crtSTCK(0,0);
	// Лист стэков расшифрованных байт-листов и локальных переменных
	STCK* st_btlt	= crtSTCK(0,0);
	STCK* st_lvar	= crtSTCK(0,0);
	
	// Получение данных о ключе, КС, длине кода
	keyc = getCELL(program);
	program += 8;
	hash = getHCEL(program);
	program += 4;
	plen = getHCEL(program);
	program += 4;
	
	// Сохранение указателя на код
	prog_pointer = program;
	// Выходим, если не сходятся КС-а расчитанная и полученная 
	if(hash != crc32(program, plen)) return -2;
	program += plen;
		
	// Создание карты байт-листов
	while(1){
		HCEL bl_size = getHCEL(program);
		if(bl_size == 0xffffffff) break;

		pshSTCK(btlt_stack, (CELL)program);
		program += bl_size+4;
	}
	// Добавление аргументов в стэк аргументов
	HCEL arg_cnt = *(program+4);
	while(arg_cnt--){
		program += 8;
		pshSTCK(args_stack, getCELL(program));
	}
	
	// Работа ВМ
	CELL args[MAXARGS];
	
	CELL pp_curr 	= 0;
	CELL qwd		= 0;
	CELL qwd1		= 0;
	HCEL dwd		= 0;
	HHCL wrd		= 0;
	BYTE byt		= 0;
	
	STCK* stck;
	
	stck = crtSTCK(0,0);
	pshSTCK(st_lvar, (CELL)stck);
	stck = crtSTCK(0,0);
	pshSTCK(st_btlt, (CELL)stck);
	
	while(pp_curr<plen){
		BYTE opcode = prog_pointer[pp_curr] ^ keyc >> 56;

		pp_curr++;
		switch(opcode){
			case PSHBYT:
				// Расшифровка значения
				byt = prog_pointer[pp_curr] ^ keyc >> 56;
				// Кладем значение в самый верхний СЛП
				PSHCURRLVS(byt);
				// Скипаем байт значения(Далее тоже самое, но скипается 2,4,8 байт значения)
				pp_curr++;
			break;
			case PSHWRD:
				wrd = getHHCL(prog_pointer+pp_curr) ^ keyc >> 48;
				PSHCURRLVS(wrd);
				pp_curr+=2;
			break;
			case PSHDWD:
				dwd = getHCEL(prog_pointer+pp_curr) ^ keyc >> 32;	
				PSHCURRLVS(dwd);
				pp_curr+=4;
			break;
			case PSHQWD:
				qwd = getCELL(prog_pointer+pp_curr) ^ keyc;
				PSHCURRLVS(qwd);
				pp_curr+=8;
			break;
			case JMPADR:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				if(!qwd1) pp_curr = qwd;
			break;
			case EXTEND:
				qwd  = POPCURRLVS;
				while(qwd--) PSHCURRLVS(0);
			break;
			case DECRPT:
				qwd  = POPCURRLVS;
				dwd  = getHCEL((BYTE*)*(CELL*)getELEM(btlt_stack,qwd));
				
				qwd1 = (CELL)malloc(dwd+5);
				memcpy((void*)qwd1, (BYTE*)*(CELL*)getELEM(btlt_stack,qwd), dwd+4);
				decryptBTLT((BYTE*)(qwd1+4), dwd, keyc);
				((BYTE*)qwd1)[dwd+4] = 0;
				
				PSHCURRLVS(qwd1+4);
				PSHCURBTLT(qwd1);
			break;
			case PSHARG:
				qwd  = POPCURRLVS;
				pshSTCK(args_stack, qwd);
			break;
			case POPARG:
				qwd = popSTCK(args_stack);
				PSHARGLVST(qwd);
			break;
			case CALFNC:
				qwd  = POPCURRLVS; // Вернет ли значение
				qwd1 = POPCURRLVS; // Внешняя или внутренняя
				dwd  = POPCURRLVS; // Адрес или код функции
				
				
				// Вызов внутренней
				if(!qwd1){
					stck = crtSTCK(pp_curr, qwd);
					pshSTCK(st_lvar, (CELL)stck);
					stck = crtSTCK(0,0);
					pshSTCK(st_btlt, (CELL)stck);
					
					pp_curr = dwd;
				// Вызов внешней
				}else{
					qwd1 = 0;
					while(1){
						if(!EXTERNS[qwd1][0]) return -2;
						if( EXTERNS[qwd1][0] == dwd) break;
						qwd1++;
					}
					dwd = 0;
					memset(args, 0, 8*MAXARGS);
					
					if(EXTERNS[qwd1][2] == -1)	byt = popSTCK(args_stack);
					else 						byt = EXTERNS[qwd1][2];
					
					while(dwd<byt){
						args[byt-dwd-1] = popSTCK(args_stack);
						dwd++;
					}
					
					qwd1 = ((FUNC)EXTERNS[qwd1][1])(args);
					if(qwd) PSHCURRLVS(qwd1);
				}
			break;
			case RETURN:
				dwd = ((STCK*)st_lvar->last->val)->reta;
				byt = ((STCK*)st_lvar->last->val)->isrt;
												
				// Очистка байтлистов
				while(1){
					qwd = POPCURBTLT;
					if(!qwd) break;
					free((void*)qwd);
				}
				
				freeSTCK((STCK*)popSTCK(st_btlt));
				
				qwd = POPCURRLVS;				
				
				// Очистка текущего СЛП				
				freeSTCK((STCK*)popSTCK(st_lvar));

				if(byt) PSHCURRLVS(qwd);
				if(dwd) pp_curr = dwd;
			break;
			case VARVAL:
				//prtSTCK((STCK*)st_lvar->last->val);
				qwd  = POPCURRLVS;
				PSHCURRLVS(*(CELL*)GETLVSADR(qwd));
			break;
			case VARADR:
				qwd  = POPCURRLVS;
				PSHCURRLVS(GETLVSADR(qwd));
			break;
			case VALADD:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1+qwd);
			break;
			case VALSUB:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1-qwd);
			break;
			case VALMUL:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1*qwd);
			break;
			case VALDIV:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1/qwd);
			break;
			case VALMOD:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1%qwd);
			break;
			case VALAND:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1&qwd);
			break;
			case VALORR:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1|qwd);
			break;
			case VALNOT:
				qwd  = POPCURRLVS;
				PSHCURRLVS(~qwd);
			break;
			case VALXOR:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1^qwd);
			break;
			case VALSHR:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1>>qwd);
			break;
			case VALSHL:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1<<qwd);
			break;
			case VALGET:
				qwd  = POPCURRLVS;
				PSHCURRLVS(*(CELL*)qwd);
			break;
			case VALSET:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				if(!qwd1) break;
				*(CELL*)qwd1 = qwd;
			break;
			case LOGNOT:
				qwd  = POPCURRLVS;
				PSHCURRLVS(!qwd);
			break;
			case LLOWER:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1<qwd);
			break;
			case LGREAT:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1>qwd);
			break;
			case LLOWEQ:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1<=qwd);
			break;
			case LGRTEQ:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1>=qwd);
			break;
			case LEQUAL:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1==qwd);
			break;
			case LNOTEQ:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1!=qwd);
			break;
			case LANDVL:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1&&qwd);
			break;
			case LORVAL:
				qwd  = POPCURRLVS;
				qwd1 = POPCURRLVS;
				PSHCURRLVS(qwd1||qwd);
			break;
		}
	}
	freeSTCK(btlt_stack);
	freeSTCK(args_stack);

	return qwd;
}


