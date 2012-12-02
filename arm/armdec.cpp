#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <sstream>

using namespace std;

string getCC(uint32_t instr)
{
	const char *cc[] = {
		"EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC", "HI", "LS", "GE", "LT", "GT", "LE", "", "NV"
	};

	return cc[instr >> 28];
}

string decode(uint32_t instr, uint32_t pc)
{
	stringstream st;
	const char *opcodes[] = {
		"AND", "EOR", "SUB", "RSB", "ADD", "ADC", "SBC", "RSC", "TST", "TEQ", "CMP", "CMN", "ORR", "MOV", "BIC", "MVN"
	};

	// address and hex instruction code
	st << hex << setfill('0') << setw(8) << instr << "\t";

	// fook, need to rewrite... lots of commonality we're not taking advantage of.

	if (((instr >> 24) & 0x0f) == 0xf) {
		// SWI
		st << "SWI" << getCC(instr) << "\t&" << hex << (instr & 0xFFFFFF);
	} else if (((instr >> 25) & 0x7) == 0x5) {
		// BRANCH or BRANCH WITH LINK
		int32_t ofs = instr & 0xFFFFFF;
		if (ofs & 0x800000) ofs |= 0xFF000000;
		ofs = (ofs << 2);
		ofs = (ofs < 0) ? ofs + 8 : ofs + 4;
		if ((instr >> 24) & 1)
			st << "BL";
		else
			st << "B";
		st << getCC(instr) << "\t&" << hex << (ofs + pc);
	} else if ((((instr >> 22) & 63) == 0) && (((instr >> 4) & 0xf) == 0x9)) {
		// MULTIPLY INSTRUCTION
		int rd = (instr >> 12) & 0xf;
		int rn = (instr >> 16) & 0xf;
		int rs = (instr >> 16) & 0xf;
		int rm = (instr >> 16) & 0xf;
		int s = (instr >> 20) & 1;

		if (instr & (1<<21)) {
			st << "MLA" << s ? "S" : "";
		} else {
			st << "MUL" << s ? "S" : "";
		}

		st << "\tr" << dec << rd << ",\tr" << rm << ",\tr" << rs << endl;
	} else if (((instr >> 26) & 3) == 1) {
		// SINGLE REGISTER TRANSFER
		int rd = (instr >> 12) & 0xf;
		int rn = (instr >> 16) & 0xf;
		uint16_t offset = instr & 0xFFF;

		if ((instr >> 20) & 1) {
			st << "LDR";
		} else {
			st << "STR";
		}

		st << "\tr" << dec << rd << ",\tr" << dec << rn;

		// writeback
		if ((instr >> 21) & 1) {
			st << "!";
		}
	} else if (((instr >> 25) & 7) == 4) {
		// MULTIPLE REGISTER DATA TRANSFER
		st << "MRT-----";
	} else if (((instr >> 26) & 3) == 0) {
		// DATA PROCESSING INSTRUCTION
		int rd = (instr >> 12) & 0xf;
		int rn = (instr >> 16) & 0xf;
		uint16_t oper2 = instr & 0xFFF;
		int is_mov = ((instr >> 21) & 0xd) == 0xd;
		int s = (instr >> 20) & 1;

		if (instr & (1<<25)) {
			// immediate value
			if (is_mov)
				st << opcodes[(instr >> 21) & 0xf] << getCC(instr) << (s ? "S" : "") << "\tr" << dec << rd << ",\t#&" << hex << oper2;
			else
				st << opcodes[(instr >> 21) & 0xf] << getCC(instr) << (s ? "S" : "") << "\tr" << dec << rd << ",\tr" << dec << rn << ",\t#&" << hex << oper2;
		} else {
			// register
			if (is_mov)
				st << opcodes[(instr >> 21) & 0xf] << getCC(instr) << (s ? "S" : "") << "\tr" << dec << rd << ",\tr" << dec << (oper2 & 0x0f);
			else
				st << opcodes[(instr >> 21) & 0xf] << getCC(instr) << (s ? "S" : "") << "\tr" << dec << rd << ",\tr" << dec << rn << ",\tr" << dec << (oper2 & 0x0f);
			oper2 >>= 4;
			// don't waste time indicating a shift of zero
			if (!((!(oper2 & 1)) && ((oper2 >> 3) == 0))) {
				switch ((oper2 >> 1) & 0x03) {
					case 0x0:	st << " LSL "; break;
					case 0x1:	st << " LSR "; break;
					case 0x2:	st << " ASR "; break;
					case 0x3:	st << " ROR "; break;
				}

				if (oper2 & 1) {
					// type 1: shift by register
					st << "r" << dec << (oper2 >> 4);
				} else {
					// type 0: shift by immediate value
					st << "#" << dec << (oper2 >> 3);
				}
			}
		}
	}

	return st.str();
}

uint32_t getword(ifstream &f)
{
	unsigned char blk[4];
	f.read((char *)&blk, sizeof(blk));
	return (blk[0]) | (blk[1] << 8) | (blk[2] << 16) | (blk[3] << 24);
}

int main(void)
{
	uint32_t instr;
	ifstream f("Decompress", ios::binary);

	instr = getword(f);
	cout << "patchtable: .DW $" << hex << setfill('0') << setw(8) << instr << endl;
	instr = getword(f);
	cout << "initialise: " << hex << decode(instr, f.tellg()) << endl;
	instr = getword(f);
	cout << "decompress: " << hex << decode(instr, f.tellg()) << endl;

	while (f.good()) {
		uint32_t pc = f.tellg();
		instr = getword(f);
		cout << "l" << hex << setfill('0') << setw(8) << pc << ": " << decode(instr, pc) << endl;
	}
}
