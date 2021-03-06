#include "pch.h"
#include <iostream>
#include <fstream>
#pragma warning(disable:4996)

std::ifstream  failas;


typedef struct
{
	char code; // operacijos kodas
	char cop1; // registras su kuriuo bus atliekamas veiksmas

}VMCommand;


int main() {

	VMCommand* r_pc; //sukuriamas pc registru kintamasis kaip komandu strukturos pointeris
	char prog_mem[256]; 
	r_pc = (VMCommand*)prog_mem; // komandu registras
	unsigned char regs[16];  // resigstru(komandu) masyvas
	bool eoflag = false;  
	bool VM_work = true;
	unsigned short int iReg1, iReg2; // Kintamieji laikinam registrų numerių skaičiavimui:

	FILE* fp = fopen("decryptor.bin", "rb");

	if (fp == NULL) {
		printf("Neiseina atidaryti programos failo\n");
		exit(1);
	}

	for (int i = 0; i < sizeof(prog_mem); i++) {
		fread(prog_mem + i, 1, 1, fp);
		if (feof(fp)) break;
	}
	fclose(fp);

	failas.open("q1_encr.txt");

	while (1) {

		{
			iReg1 = r_pc->cop1 & 0x0F; // padarom reg1(cop1) = 0
			iReg2 = (r_pc->cop1 & 0xF0) >> 4; // padarom reg2(cop1)=40, pastumus gaunam 4
			switch (r_pc->code)
			{			

			case 0x04: //kopijuoja baito konstanta i registra R0. 2-tipo komanda
				regs[iReg1] = r_pc->cop1;
				break;

			case 0x05:  // Registro postumis i kaire per 1 bita
				regs[iReg1] = regs[iReg1] << 1;
				break;

			case 0x07: //suolis santykiniu adresu pridedant konstanta (su zenklu) <<addr>> prie komandu skaitiklio. Visi suoliai yra 2-tipo komandos
				r_pc = (VMCommand*)((char*)r_pc + r_pc->cop1);
				r_pc--;
				break;

			case 0x0A:  // kaip ir 07 (JPMT) tik jei yra failo pabaigos pozymis
				if (eoflag == true) {
					r_pc = (VMCommand*)((char*)r_pc + r_pc->cop1);
					r_pc--;
				}
				break;

			case 0x0B: // VM baigia darba
				failas.close();
				VM_work = false;
				break;

			case 0x0D: // atima registro Ry turini is registro Rx ir palieka rezultata Rx registre
				regs[iReg1] = regs[iReg1] - regs[iReg2];
				break;

			case 0x0E: //XOR operacija kiekvienam bitui atskirai, rezultatas paliekamas Rx registre
				regs[iReg1] = regs[iReg1] ^ regs[iReg2];
				break;

			case 0x0F: // OR operacija kiekvienam bitui atskirai, rezultatas paliekamas Rx registre
				regs[iReg1] = regs[iReg1] | regs[iReg2];
				break;

			case 0x10: //skaito 1 baita is duomenu failo ir nustato failo pabaigos pozymi, jei pasiekta failo pabaiga
				failas >> regs[iReg1];
				if (failas.eof()) {
					eoflag = true;
				}
				break;

			case 0x11:
				std::cout << (char)regs[iReg1];	//kadangi registrai yra hexai tai convertinam i chara kad coutas priimtu
				break;
			}
			r_pc++;	
		}
	}
	
	return 0;
}


