#include <stdlib.h>
#include <inttypes.h>

typedef uint64_t 	CELL;
typedef uint32_t	HCEL;
typedef uint16_t	HHCL;
typedef uint8_t		BYTE;
typedef CELL 		(*FUNC)();

/** Работа с получением значений **/
// Получение 8 байтового значения
CELL getCELL(BYTE* ptr){
	return (	(((CELL)ptr[0] & 0xff) << 56) | (((CELL)ptr[1] & 0xff) << 48) 
			| 	(((CELL)ptr[2] & 0xff) << 40) | (((CELL)ptr[3] & 0xff) << 32) 
			| 	(((CELL)ptr[4] & 0xff) << 24) | (((CELL)ptr[5] & 0xff) << 16) 
			| 	(((CELL)ptr[6] & 0xff) <<  8) | (((CELL)ptr[7] & 0xff)));
}
// Получение 4 байтового значения
HCEL getHCEL(BYTE* ptr){ 
	return (	(((HCEL)ptr[0] & 0xff) << 24) | (((HCEL)ptr[1] & 0xff) << 16) 
			| 	(((HCEL)ptr[2] & 0xff) <<  8) | (((HCEL)ptr[3] & 0xff)));
}
// Получение 2 байтового значения
HHCL getHHCL(BYTE* ptr){ 
	return (	(((HHCL)ptr[0] & 0xff) <<  8) | (((HHCL)ptr[1] & 0xff)));
}

void setCELL(BYTE* ptr, HCEL val){
	ptr[0] = val >> 56;
	ptr[1] = val >> 48;
	ptr[2] = val >> 40;
	ptr[3] = val >> 32;
	ptr[4] = val >> 24;
	ptr[5] = val >> 16;
	ptr[6] = val >> 8;
	ptr[7] = val;
}

void setHCEL(BYTE* ptr, HCEL val){
	ptr[0] = val >> 24;
	ptr[1] = val >> 16;
	ptr[2] = val >> 8;
	ptr[3] = val;
}

struct ELEM_s {
	struct ELEM_s* prev;
	CELL  val;
} typedef ELEM;

struct STCK_s {
	ELEM* frst;
	ELEM* last;
	HCEL  leng;
	HCEL  reta;
	BYTE  isrt;
} typedef STCK;

STCK* crtSTCK(HCEL reta, BYTE isrt){
	STCK* stc = (STCK*)malloc(sizeof(STCK));
	stc->last = 0;
	stc->frst = 0;
	stc->leng = 0;
	
	stc->reta = reta;
	stc->isrt = isrt;
	return stc;
}

void pshSTCK(STCK* stc, CELL val){
	ELEM* elm = (ELEM*)malloc(sizeof(ELEM));
	elm->val  = val;
	
	if(stc->leng == 0){
		elm->prev = 0;
		stc->frst = elm;
	}else{
		elm->prev = stc->last;
	}
	
	stc->last = elm;
	stc->leng++;
}

void insSTCK(STCK* stc, CELL val){
	ELEM* elm = (ELEM*)malloc(sizeof(ELEM));
	elm->val  = val;
	elm->prev = 0;

	if(stc->leng == 0){
		stc->last = elm;
	}else{
		stc->frst->prev = elm;  
	}

	stc->frst = elm;
	stc->leng++;
}

CELL popSTCK(STCK* stc){
	if(stc->leng == 0) return 0;

	CELL ret  = ((ELEM*)stc->last)->val;
	ELEM* elm = stc->last;

	stc->last = elm->prev;
	free(elm);

	stc->leng--;
	return ret;
}

CELL getELEM(STCK* stc, HCEL id){
	if(id >= stc->leng) return -1;
	ELEM* elm = stc->last;

	for(HCEL i = 0;i<stc->leng-id-1;i++){
		elm = elm->prev;
	}
	return (CELL)(&(elm->val));
}

void freeSTCK(STCK* stc){
	while(stc->last){
		popSTCK(stc);
	}
	free(stc);
}

HCEL crc32(BYTE* s, HCEL n) {
	HCEL crc=0xFFFFFFFF;
	for(HCEL i=0;i<n;i++) {
		BYTE ch=s[i];
		for(HCEL j=0;j<8;j++) {
			HCEL b=(ch^crc)&1;
			crc>>=1;
			if(b) crc=crc^0xEDB88320;
			ch>>=1;
		}
	}
	return ~crc;
}

void decryptBTLT(BYTE* s, HCEL n, CELL keyc){
	for(HCEL i = 0;i<n;i++){
		s[i] ^= keyc >> (56-(i%8)*8);
	}
}

void prtSTCK(STCK* stc){
	ELEM* elm = stc->last;
	for(int i = 0;i<stc->leng;i++){
		printf("---> %p <\n", elm->val);
		elm = elm->prev;
	}
}