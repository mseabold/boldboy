#include "cpu.h"

Cpu::Opcode::Opcode() {
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, const char *_mnemonic) {
    init(_handler, _cycles, _mnemonic, 0, 0);
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, uint8_t _branchCycles, const char *_mnemonic) {
    init(_handler, _cycles, _branchCycles, _mnemonic, 0, 0);
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, const char *_mnemonic, uint16_t _p1) {
    init(_handler, _cycles, _mnemonic, _p1, 0);
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, const char *_mnemonic, uint16_t _p1, uint16_t _p2) {
    init(_handler, _cycles, 0, _mnemonic, _p1, _p2);
}

void Cpu::Opcode::init(OpHandler _handler, uint8_t _cycles, uint8_t _branchCycles, const char *_mnemonic, uint16_t _p1, uint16_t _p2) {
    handler = _handler;
    cycles = _cycles;
    branchCycles = _branchCycles;
    mnemonic = _mnemonic;
    p1 = _p1;
    p2 = _p2;
}

void Cpu::initOps(void) {
    mOpTable[0x00].init(&Cpu::oph_Nop, 4, "NOP");
    mOpTable[0x01].init(&Cpu::oph_Nop, 12, "LD BC,d16");
    mOpTable[0x02].init(&Cpu::oph_Nop, 8, "LD (BC),A");
    mOpTable[0x03].init(&Cpu::oph_Nop, 8, "INC BC");
    mOpTable[0x04].init(&Cpu::oph_Nop, 4, "INC B");
    mOpTable[0x05].init(&Cpu::oph_Nop, 4, "DEC B");
    mOpTable[0x06].init(&Cpu::oph_Nop, 8, "LD B,d8");
    mOpTable[0x07].init(&Cpu::oph_Nop, 4, "RLCA");
    mOpTable[0x08].init(&Cpu::oph_Nop, 20, "LD (a16),SP");
    mOpTable[0x09].init(&Cpu::oph_Nop, 8, "ADD HL,BC");
    mOpTable[0x0a].init(&Cpu::oph_Nop, 8, "LD A,(BC)");
    mOpTable[0x0b].init(&Cpu::oph_Nop, 8, "DEC BC");
    mOpTable[0x0c].init(&Cpu::oph_Nop, 4, "INC C");
    mOpTable[0x0d].init(&Cpu::oph_Nop, 4, "DEC C");
    mOpTable[0x0e].init(&Cpu::oph_Nop, 8, "LD C,d8");
    mOpTable[0x0f].init(&Cpu::oph_Nop, 4, "RRCA");
    mOpTable[0x10].init(&Cpu::oph_Nop, 4, "STOP 0");
    mOpTable[0x11].init(&Cpu::oph_Nop, 12, "LD DE,d16");
    mOpTable[0x12].init(&Cpu::oph_Nop, 8, "LD (DE),A");
    mOpTable[0x13].init(&Cpu::oph_Nop, 8, "INC DE");
    mOpTable[0x14].init(&Cpu::oph_Nop, 4, "INC D");
    mOpTable[0x15].init(&Cpu::oph_Nop, 4, "DEC D");
    mOpTable[0x16].init(&Cpu::oph_Nop, 8, "LD D,d8");
    mOpTable[0x17].init(&Cpu::oph_Nop, 4, "RLA");
    mOpTable[0x18].init(&Cpu::oph_Nop, 12, "JR r8");
    mOpTable[0x19].init(&Cpu::oph_Nop, 8, "ADD HL,DE");
    mOpTable[0x1a].init(&Cpu::oph_Nop, 8, "LD A,(DE)");
    mOpTable[0x1b].init(&Cpu::oph_Nop, 8, "DEC DE");
    mOpTable[0x1c].init(&Cpu::oph_Nop, 4, "INC E");
    mOpTable[0x1d].init(&Cpu::oph_Nop, 4, "DEC E");
    mOpTable[0x1e].init(&Cpu::oph_Nop, 8, "LD E,d8");
    mOpTable[0x1f].init(&Cpu::oph_Nop, 4, "RRA");
    mOpTable[0x20].init(&Cpu::oph_Nop, 8, 12, "JR NZ,r8");
    mOpTable[0x21].init(&Cpu::oph_Nop, 12, "LD HL,d16");
    mOpTable[0x22].init(&Cpu::oph_Nop, 8, "LD (HL+),A");
    mOpTable[0x23].init(&Cpu::oph_Nop, 8, "INC HL");
    mOpTable[0x24].init(&Cpu::oph_Nop, 4, "INC H");
    mOpTable[0x25].init(&Cpu::oph_Nop, 4, "DEC H");
    mOpTable[0x26].init(&Cpu::oph_Nop, 8, "LD H,d8");
    mOpTable[0x27].init(&Cpu::oph_Nop, 4, "DAA");
    mOpTable[0x28].init(&Cpu::oph_Nop, 8, 12, "JR Z,r8");
    mOpTable[0x29].init(&Cpu::oph_Nop, 8, "ADD HL,HL");
    mOpTable[0x2a].init(&Cpu::oph_Nop, 8, "LD A,(HL+)");
    mOpTable[0x2b].init(&Cpu::oph_Nop, 8, "DEC HL");
    mOpTable[0x2c].init(&Cpu::oph_Nop, 4, "INC L");
    mOpTable[0x2d].init(&Cpu::oph_Nop, 4, "DEC L");
    mOpTable[0x2e].init(&Cpu::oph_Nop, 8, "LD L,d8");
    mOpTable[0x2f].init(&Cpu::oph_Nop, 4, "CPL");
    mOpTable[0x30].init(&Cpu::oph_Nop, 8, 12, "JR NC,r8");
    mOpTable[0x31].init(&Cpu::oph_Nop, 12, "LD SP,d16");
    mOpTable[0x32].init(&Cpu::oph_Nop, 8, "LD (HL-),A");
    mOpTable[0x33].init(&Cpu::oph_Nop, 8, "INC SP");
    mOpTable[0x34].init(&Cpu::oph_Nop, 12, "INC (HL)");
    mOpTable[0x35].init(&Cpu::oph_Nop, 12, "DEC (HL)");
    mOpTable[0x36].init(&Cpu::oph_Nop, 12, "LD (HL),d8");
    mOpTable[0x37].init(&Cpu::oph_Nop, 4, "SCF");
    mOpTable[0x38].init(&Cpu::oph_Nop, 8, 12, "JR C,r8");
    mOpTable[0x39].init(&Cpu::oph_Nop, 8, "ADD HL,SP");
    mOpTable[0x3a].init(&Cpu::oph_Nop, 8, "LD A,(HL-)");
    mOpTable[0x3b].init(&Cpu::oph_Nop, 8, "DEC SP");
    mOpTable[0x3c].init(&Cpu::oph_Nop, 4, "INC A");
    mOpTable[0x3d].init(&Cpu::oph_Nop, 4, "DEC A");
    mOpTable[0x3e].init(&Cpu::oph_Nop, 8, "LD A,d8");
    mOpTable[0x3f].init(&Cpu::oph_Nop, 4, "CCF");
    mOpTable[0x40].init(&Cpu::oph_Nop, 4, "LD B,B");
    mOpTable[0x41].init(&Cpu::oph_Nop, 4, "LD B,C");
    mOpTable[0x42].init(&Cpu::oph_Nop, 4, "LD B,D");
    mOpTable[0x43].init(&Cpu::oph_Nop, 4, "LD B,E");
    mOpTable[0x44].init(&Cpu::oph_Nop, 4, "LD B,H");
    mOpTable[0x45].init(&Cpu::oph_Nop, 4, "LD B,L");
    mOpTable[0x46].init(&Cpu::oph_Nop, 8, "LD B,(HL)");
    mOpTable[0x47].init(&Cpu::oph_Nop, 4, "LD B,A");
    mOpTable[0x48].init(&Cpu::oph_Nop, 4, "LD C,B");
    mOpTable[0x49].init(&Cpu::oph_Nop, 4, "LD C,C");
    mOpTable[0x4a].init(&Cpu::oph_Nop, 4, "LD C,D");
    mOpTable[0x4b].init(&Cpu::oph_Nop, 4, "LD C,E");
    mOpTable[0x4c].init(&Cpu::oph_Nop, 4, "LD C,H");
    mOpTable[0x4d].init(&Cpu::oph_Nop, 4, "LD C,L");
    mOpTable[0x4e].init(&Cpu::oph_Nop, 8, "LD C,(HL)");
    mOpTable[0x4f].init(&Cpu::oph_Nop, 4, "LD C,A");
    mOpTable[0x50].init(&Cpu::oph_Nop, 4, "LD D,B");
    mOpTable[0x51].init(&Cpu::oph_Nop, 4, "LD D,C");
    mOpTable[0x52].init(&Cpu::oph_Nop, 4, "LD D,D");
    mOpTable[0x53].init(&Cpu::oph_Nop, 4, "LD D,E");
    mOpTable[0x54].init(&Cpu::oph_Nop, 4, "LD D,H");
    mOpTable[0x55].init(&Cpu::oph_Nop, 4, "LD D,L");
    mOpTable[0x56].init(&Cpu::oph_Nop, 8, "LD D,(HL)");
    mOpTable[0x57].init(&Cpu::oph_Nop, 4, "LD D,A");
    mOpTable[0x58].init(&Cpu::oph_Nop, 4, "LD E,B");
    mOpTable[0x59].init(&Cpu::oph_Nop, 4, "LD E,C");
    mOpTable[0x5a].init(&Cpu::oph_Nop, 4, "LD E,D");
    mOpTable[0x5b].init(&Cpu::oph_Nop, 4, "LD E,E");
    mOpTable[0x5c].init(&Cpu::oph_Nop, 4, "LD E,H");
    mOpTable[0x5d].init(&Cpu::oph_Nop, 4, "LD E,L");
    mOpTable[0x5e].init(&Cpu::oph_Nop, 8, "LD E,(HL)");
    mOpTable[0x5f].init(&Cpu::oph_Nop, 4, "LD E,A");
    mOpTable[0x60].init(&Cpu::oph_Nop, 4, "LD H,B");
    mOpTable[0x61].init(&Cpu::oph_Nop, 4, "LD H,C");
    mOpTable[0x62].init(&Cpu::oph_Nop, 4, "LD H,D");
    mOpTable[0x63].init(&Cpu::oph_Nop, 4, "LD H,E");
    mOpTable[0x64].init(&Cpu::oph_Nop, 4, "LD H,H");
    mOpTable[0x65].init(&Cpu::oph_Nop, 4, "LD H,L");
    mOpTable[0x66].init(&Cpu::oph_Nop, 8, "LD H,(HL)");
    mOpTable[0x67].init(&Cpu::oph_Nop, 4, "LD H,A");
    mOpTable[0x68].init(&Cpu::oph_Nop, 4, "LD L,B");
    mOpTable[0x69].init(&Cpu::oph_Nop, 4, "LD L,C");
    mOpTable[0x6a].init(&Cpu::oph_Nop, 4, "LD L,D");
    mOpTable[0x6b].init(&Cpu::oph_Nop, 4, "LD L,E");
    mOpTable[0x6c].init(&Cpu::oph_Nop, 4, "LD L,H");
    mOpTable[0x6d].init(&Cpu::oph_Nop, 4, "LD L,L");
    mOpTable[0x6e].init(&Cpu::oph_Nop, 8, "LD L,(HL)");
    mOpTable[0x6f].init(&Cpu::oph_Nop, 4, "LD L,A");
    mOpTable[0x70].init(&Cpu::oph_Nop, 8, "LD (HL),B");
    mOpTable[0x71].init(&Cpu::oph_Nop, 8, "LD (HL),C");
    mOpTable[0x72].init(&Cpu::oph_Nop, 8, "LD (HL),D");
    mOpTable[0x73].init(&Cpu::oph_Nop, 8, "LD (HL),E");
    mOpTable[0x74].init(&Cpu::oph_Nop, 8, "LD (HL),H");
    mOpTable[0x75].init(&Cpu::oph_Nop, 8, "LD (HL),L");
    mOpTable[0x76].init(&Cpu::oph_Nop, 4, "HALT");
    mOpTable[0x77].init(&Cpu::oph_Nop, 8, "LD (HL),A");
    mOpTable[0x78].init(&Cpu::oph_Nop, 4, "LD A,B");
    mOpTable[0x79].init(&Cpu::oph_Nop, 4, "LD A,C");
    mOpTable[0x7a].init(&Cpu::oph_Nop, 4, "LD A,D");
    mOpTable[0x7b].init(&Cpu::oph_Nop, 4, "LD A,E");
    mOpTable[0x7c].init(&Cpu::oph_Nop, 4, "LD A,H");
    mOpTable[0x7d].init(&Cpu::oph_Nop, 4, "LD A,L");
    mOpTable[0x7e].init(&Cpu::oph_Nop, 8, "LD A,(HL)");
    mOpTable[0x7f].init(&Cpu::oph_Nop, 4, "LD A,A");
    mOpTable[0x80].init(&Cpu::oph_Nop, 4, "ADD A,B");
    mOpTable[0x81].init(&Cpu::oph_Nop, 4, "ADD A,C");
    mOpTable[0x82].init(&Cpu::oph_Nop, 4, "ADD A,D");
    mOpTable[0x83].init(&Cpu::oph_Nop, 4, "ADD A,E");
    mOpTable[0x84].init(&Cpu::oph_Nop, 4, "ADD A,H");
    mOpTable[0x85].init(&Cpu::oph_Nop, 4, "ADD A,L");
    mOpTable[0x86].init(&Cpu::oph_Nop, 8, "ADD A,(HL)");
    mOpTable[0x87].init(&Cpu::oph_Nop, 4, "ADD A,A");
    mOpTable[0x88].init(&Cpu::oph_Nop, 4, "ADC A,B");
    mOpTable[0x89].init(&Cpu::oph_Nop, 4, "ADC A,C");
    mOpTable[0x8a].init(&Cpu::oph_Nop, 4, "ADC A,D");
    mOpTable[0x8b].init(&Cpu::oph_Nop, 4, "ADC A,E");
    mOpTable[0x8c].init(&Cpu::oph_Nop, 4, "ADC A,H");
    mOpTable[0x8d].init(&Cpu::oph_Nop, 4, "ADC A,L");
    mOpTable[0x8e].init(&Cpu::oph_Nop, 8, "ADC A,(HL)");
    mOpTable[0x8f].init(&Cpu::oph_Nop, 4, "ADC A,A");
    mOpTable[0x90].init(&Cpu::oph_Nop, 4, "SUB B");
    mOpTable[0x91].init(&Cpu::oph_Nop, 4, "SUB C");
    mOpTable[0x92].init(&Cpu::oph_Nop, 4, "SUB D");
    mOpTable[0x93].init(&Cpu::oph_Nop, 4, "SUB E");
    mOpTable[0x94].init(&Cpu::oph_Nop, 4, "SUB H");
    mOpTable[0x95].init(&Cpu::oph_Nop, 4, "SUB L");
    mOpTable[0x96].init(&Cpu::oph_Nop, 8, "SUB (HL)");
    mOpTable[0x97].init(&Cpu::oph_Nop, 4, "SUB A");
    mOpTable[0x98].init(&Cpu::oph_Nop, 4, "SBC A,B");
    mOpTable[0x99].init(&Cpu::oph_Nop, 4, "SBC A,C");
    mOpTable[0x9a].init(&Cpu::oph_Nop, 4, "SBC A,D");
    mOpTable[0x9b].init(&Cpu::oph_Nop, 4, "SBC A,E");
    mOpTable[0x9c].init(&Cpu::oph_Nop, 4, "SBC A,H");
    mOpTable[0x9d].init(&Cpu::oph_Nop, 4, "SBC A,L");
    mOpTable[0x9e].init(&Cpu::oph_Nop, 8, "SBC A,(HL)");
    mOpTable[0x9f].init(&Cpu::oph_Nop, 4, "SBC A,A");
    mOpTable[0xa0].init(&Cpu::oph_Nop, 4, "AND B");
    mOpTable[0xa1].init(&Cpu::oph_Nop, 4, "AND C");
    mOpTable[0xa2].init(&Cpu::oph_Nop, 4, "AND D");
    mOpTable[0xa3].init(&Cpu::oph_Nop, 4, "AND E");
    mOpTable[0xa4].init(&Cpu::oph_Nop, 4, "AND H");
    mOpTable[0xa5].init(&Cpu::oph_Nop, 4, "AND L");
    mOpTable[0xa6].init(&Cpu::oph_Nop, 8, "AND (HL)");
    mOpTable[0xa7].init(&Cpu::oph_Nop, 4, "AND A");
    mOpTable[0xa8].init(&Cpu::oph_Nop, 4, "XOR B");
    mOpTable[0xa9].init(&Cpu::oph_Nop, 4, "XOR C");
    mOpTable[0xaa].init(&Cpu::oph_Nop, 4, "XOR D");
    mOpTable[0xab].init(&Cpu::oph_Nop, 4, "XOR E");
    mOpTable[0xac].init(&Cpu::oph_Nop, 4, "XOR H");
    mOpTable[0xad].init(&Cpu::oph_Nop, 4, "XOR L");
    mOpTable[0xae].init(&Cpu::oph_Nop, 8, "XOR (HL)");
    mOpTable[0xaf].init(&Cpu::oph_Nop, 4, "XOR A");
    mOpTable[0xb0].init(&Cpu::oph_Nop, 4, "OR B");
    mOpTable[0xb1].init(&Cpu::oph_Nop, 4, "OR C");
    mOpTable[0xb2].init(&Cpu::oph_Nop, 4, "OR D");
    mOpTable[0xb3].init(&Cpu::oph_Nop, 4, "OR E");
    mOpTable[0xb4].init(&Cpu::oph_Nop, 4, "OR H");
    mOpTable[0xb5].init(&Cpu::oph_Nop, 4, "OR L");
    mOpTable[0xb6].init(&Cpu::oph_Nop, 8, "OR (HL)");
    mOpTable[0xb7].init(&Cpu::oph_Nop, 4, "OR A");
    mOpTable[0xb8].init(&Cpu::oph_Nop, 4, "CP B");
    mOpTable[0xb9].init(&Cpu::oph_Nop, 4, "CP C");
    mOpTable[0xba].init(&Cpu::oph_Nop, 4, "CP D");
    mOpTable[0xbb].init(&Cpu::oph_Nop, 4, "CP E");
    mOpTable[0xbc].init(&Cpu::oph_Nop, 4, "CP H");
    mOpTable[0xbd].init(&Cpu::oph_Nop, 4, "CP L");
    mOpTable[0xbe].init(&Cpu::oph_Nop, 8, "CP (HL)");
    mOpTable[0xbf].init(&Cpu::oph_Nop, 4, "CP A");
    mOpTable[0xc0].init(&Cpu::oph_Nop, 8, 20, "RET NZ");
    mOpTable[0xc1].init(&Cpu::oph_Nop, 12, "POP BC");
    mOpTable[0xc2].init(&Cpu::oph_Nop, 12, 16, "JP NZ,a16");
    mOpTable[0xc3].init(&Cpu::oph_Nop, 16, "JP a16");
    mOpTable[0xc4].init(&Cpu::oph_Nop, 12, 24, "CALL NZ,a16");
    mOpTable[0xc5].init(&Cpu::oph_Nop, 16, "PUSH BC");
    mOpTable[0xc6].init(&Cpu::oph_Nop, 8, "ADD A,d8");
    mOpTable[0xc7].init(&Cpu::oph_Nop, 16, "RST 00H");
    mOpTable[0xc8].init(&Cpu::oph_Nop, 8, 20, "RET Z");
    mOpTable[0xc9].init(&Cpu::oph_Nop, 16, "RET");
    mOpTable[0xca].init(&Cpu::oph_Nop, 12, 16, "JP Z,a16");
    mOpTable[0xcb].init(&Cpu::oph_Nop, 4, "PREFIX CB");
    mOpTable[0xcc].init(&Cpu::oph_Nop, 12, 24, "CALL Z,a16");
    mOpTable[0xcd].init(&Cpu::oph_Nop, 24, "CALL a16");
    mOpTable[0xce].init(&Cpu::oph_Nop, 8, "ADC A,d8");
    mOpTable[0xcf].init(&Cpu::oph_Nop, 16, "RST 08H");
    mOpTable[0xd0].init(&Cpu::oph_Nop, 8, 20, "RET NC");
    mOpTable[0xd1].init(&Cpu::oph_Nop, 12, "POP DE");
    mOpTable[0xd2].init(&Cpu::oph_Nop, 12, 16, "JP NC,a16");
    mOpTable[0xd3].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xd4].init(&Cpu::oph_Nop, 12, 24, "CALL NC,a16");
    mOpTable[0xd5].init(&Cpu::oph_Nop, 16, "PUSH DE");
    mOpTable[0xd6].init(&Cpu::oph_Nop, 8, "SUB d8");
    mOpTable[0xd7].init(&Cpu::oph_Nop, 16, "RST 10H");
    mOpTable[0xd8].init(&Cpu::oph_Nop, 8, 20, "RET C");
    mOpTable[0xd9].init(&Cpu::oph_Nop, 16, "RETI");
    mOpTable[0xda].init(&Cpu::oph_Nop, 12, 16, "JP C,a16");
    mOpTable[0xdb].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xdc].init(&Cpu::oph_Nop, 12, 24, "CALL C,a16");
    mOpTable[0xdd].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xde].init(&Cpu::oph_Nop, 8, "SBC A,d8");
    mOpTable[0xdf].init(&Cpu::oph_Nop, 16, "RST 18H");
    mOpTable[0xe0].init(&Cpu::oph_Nop, 12, "LDH (a8),A");
    mOpTable[0xe1].init(&Cpu::oph_Nop, 12, "POP HL");
    mOpTable[0xe2].init(&Cpu::oph_Nop, 8, "LD (C),A");
    mOpTable[0xe3].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xe4].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xe5].init(&Cpu::oph_Nop, 16, "PUSH HL");
    mOpTable[0xe6].init(&Cpu::oph_Nop, 8, "AND d8");
    mOpTable[0xe7].init(&Cpu::oph_Nop, 16, "RST 20H");
    mOpTable[0xe8].init(&Cpu::oph_Nop, 16, "ADD SP,r8");
    mOpTable[0xe9].init(&Cpu::oph_Nop, 4, "JP (HL)");
    mOpTable[0xea].init(&Cpu::oph_Nop, 16, "LD (a16),A");
    mOpTable[0xeb].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xec].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xed].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xee].init(&Cpu::oph_Nop, 8, "XOR d8");
    mOpTable[0xef].init(&Cpu::oph_Nop, 16, "RST 28H");
    mOpTable[0xf0].init(&Cpu::oph_Nop, 12, "LDH A,(a8)");
    mOpTable[0xf1].init(&Cpu::oph_Nop, 12, "POP AF");
    mOpTable[0xf2].init(&Cpu::oph_Nop, 8, "LD A,(C)");
    mOpTable[0xf3].init(&Cpu::oph_Nop, 4, "DI");
    mOpTable[0xf4].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xf5].init(&Cpu::oph_Nop, 16, "PUSH AF");
    mOpTable[0xf6].init(&Cpu::oph_Nop, 8, "OR d8");
    mOpTable[0xf7].init(&Cpu::oph_Nop, 16, "RST 30H");
    mOpTable[0xf8].init(&Cpu::oph_Nop, 12, "LD HL,SP+r8");
    mOpTable[0xf9].init(&Cpu::oph_Nop, 8, "LD SP,HL");
    mOpTable[0xfa].init(&Cpu::oph_Nop, 16, "LD A,(a16)");
    mOpTable[0xfb].init(&Cpu::oph_Nop, 4, "EI");
    mOpTable[0xfc].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xfd].init(&Cpu::oph_Invalid, 0, "Invalid");
    mOpTable[0xfe].init(&Cpu::oph_Nop, 8, "CP d8");
    mOpTable[0xff].init(&Cpu::oph_Nop, 16, "RST 38H");

    mCbOpTable[0x00].init(&Cpu::oph_Nop, 8, "RLC B");
    mCbOpTable[0x01].init(&Cpu::oph_Nop, 8, "RLC C");
    mCbOpTable[0x02].init(&Cpu::oph_Nop, 8, "RLC D");
    mCbOpTable[0x03].init(&Cpu::oph_Nop, 8, "RLC E");
    mCbOpTable[0x04].init(&Cpu::oph_Nop, 8, "RLC H");
    mCbOpTable[0x05].init(&Cpu::oph_Nop, 8, "RLC L");
    mCbOpTable[0x06].init(&Cpu::oph_Nop, 16, "RLC (HL)");
    mCbOpTable[0x07].init(&Cpu::oph_Nop, 8, "RLC A");
    mCbOpTable[0x08].init(&Cpu::oph_Nop, 8, "RRC B");
    mCbOpTable[0x09].init(&Cpu::oph_Nop, 8, "RRC C");
    mCbOpTable[0x0a].init(&Cpu::oph_Nop, 8, "RRC D");
    mCbOpTable[0x0b].init(&Cpu::oph_Nop, 8, "RRC E");
    mCbOpTable[0x0c].init(&Cpu::oph_Nop, 8, "RRC H");
    mCbOpTable[0x0d].init(&Cpu::oph_Nop, 8, "RRC L");
    mCbOpTable[0x0e].init(&Cpu::oph_Nop, 16, "RRC (HL)");
    mCbOpTable[0x0f].init(&Cpu::oph_Nop, 8, "RRC A");
    mCbOpTable[0x10].init(&Cpu::oph_Nop, 8, "RL B");
    mCbOpTable[0x11].init(&Cpu::oph_Nop, 8, "RL C");
    mCbOpTable[0x12].init(&Cpu::oph_Nop, 8, "RL D");
    mCbOpTable[0x13].init(&Cpu::oph_Nop, 8, "RL E");
    mCbOpTable[0x14].init(&Cpu::oph_Nop, 8, "RL H");
    mCbOpTable[0x15].init(&Cpu::oph_Nop, 8, "RL L");
    mCbOpTable[0x16].init(&Cpu::oph_Nop, 16, "RL (HL)");
    mCbOpTable[0x17].init(&Cpu::oph_Nop, 8, "RL A");
    mCbOpTable[0x18].init(&Cpu::oph_Nop, 8, "RR B");
    mCbOpTable[0x19].init(&Cpu::oph_Nop, 8, "RR C");
    mCbOpTable[0x1a].init(&Cpu::oph_Nop, 8, "RR D");
    mCbOpTable[0x1b].init(&Cpu::oph_Nop, 8, "RR E");
    mCbOpTable[0x1c].init(&Cpu::oph_Nop, 8, "RR H");
    mCbOpTable[0x1d].init(&Cpu::oph_Nop, 8, "RR L");
    mCbOpTable[0x1e].init(&Cpu::oph_Nop, 16, "RR (HL)");
    mCbOpTable[0x1f].init(&Cpu::oph_Nop, 8, "RR A");
    mCbOpTable[0x20].init(&Cpu::oph_Nop, 8, "SLA B");
    mCbOpTable[0x21].init(&Cpu::oph_Nop, 8, "SLA C");
    mCbOpTable[0x22].init(&Cpu::oph_Nop, 8, "SLA D");
    mCbOpTable[0x23].init(&Cpu::oph_Nop, 8, "SLA E");
    mCbOpTable[0x24].init(&Cpu::oph_Nop, 8, "SLA H");
    mCbOpTable[0x25].init(&Cpu::oph_Nop, 8, "SLA L");
    mCbOpTable[0x26].init(&Cpu::oph_Nop, 16, "SLA (HL)");
    mCbOpTable[0x27].init(&Cpu::oph_Nop, 8, "SLA A");
    mCbOpTable[0x28].init(&Cpu::oph_Nop, 8, "SRA B");
    mCbOpTable[0x29].init(&Cpu::oph_Nop, 8, "SRA C");
    mCbOpTable[0x2a].init(&Cpu::oph_Nop, 8, "SRA D");
    mCbOpTable[0x2b].init(&Cpu::oph_Nop, 8, "SRA E");
    mCbOpTable[0x2c].init(&Cpu::oph_Nop, 8, "SRA H");
    mCbOpTable[0x2d].init(&Cpu::oph_Nop, 8, "SRA L");
    mCbOpTable[0x2e].init(&Cpu::oph_Nop, 16, "SRA (HL)");
    mCbOpTable[0x2f].init(&Cpu::oph_Nop, 8, "SRA A");
    mCbOpTable[0x30].init(&Cpu::oph_Nop, 8, "SWAP B");
    mCbOpTable[0x31].init(&Cpu::oph_Nop, 8, "SWAP C");
    mCbOpTable[0x32].init(&Cpu::oph_Nop, 8, "SWAP D");
    mCbOpTable[0x33].init(&Cpu::oph_Nop, 8, "SWAP E");
    mCbOpTable[0x34].init(&Cpu::oph_Nop, 8, "SWAP H");
    mCbOpTable[0x35].init(&Cpu::oph_Nop, 8, "SWAP L");
    mCbOpTable[0x36].init(&Cpu::oph_Nop, 16, "SWAP (HL)");
    mCbOpTable[0x37].init(&Cpu::oph_Nop, 8, "SWAP A");
    mCbOpTable[0x38].init(&Cpu::oph_Nop, 8, "SRL B");
    mCbOpTable[0x39].init(&Cpu::oph_Nop, 8, "SRL C");
    mCbOpTable[0x3a].init(&Cpu::oph_Nop, 8, "SRL D");
    mCbOpTable[0x3b].init(&Cpu::oph_Nop, 8, "SRL E");
    mCbOpTable[0x3c].init(&Cpu::oph_Nop, 8, "SRL H");
    mCbOpTable[0x3d].init(&Cpu::oph_Nop, 8, "SRL L");
    mCbOpTable[0x3e].init(&Cpu::oph_Nop, 16, "SRL (HL)");
    mCbOpTable[0x3f].init(&Cpu::oph_Nop, 8, "SRL A");
    mCbOpTable[0x40].init(&Cpu::oph_Nop, 8, "BIT 0,B");
    mCbOpTable[0x41].init(&Cpu::oph_Nop, 8, "BIT 0,C");
    mCbOpTable[0x42].init(&Cpu::oph_Nop, 8, "BIT 0,D");
    mCbOpTable[0x43].init(&Cpu::oph_Nop, 8, "BIT 0,E");
    mCbOpTable[0x44].init(&Cpu::oph_Nop, 8, "BIT 0,H");
    mCbOpTable[0x45].init(&Cpu::oph_Nop, 8, "BIT 0,L");
    mCbOpTable[0x46].init(&Cpu::oph_Nop, 16, "BIT 0,(HL)");
    mCbOpTable[0x47].init(&Cpu::oph_Nop, 8, "BIT 0,A");
    mCbOpTable[0x48].init(&Cpu::oph_Nop, 8, "BIT 1,B");
    mCbOpTable[0x49].init(&Cpu::oph_Nop, 8, "BIT 1,C");
    mCbOpTable[0x4a].init(&Cpu::oph_Nop, 8, "BIT 1,D");
    mCbOpTable[0x4b].init(&Cpu::oph_Nop, 8, "BIT 1,E");
    mCbOpTable[0x4c].init(&Cpu::oph_Nop, 8, "BIT 1,H");
    mCbOpTable[0x4d].init(&Cpu::oph_Nop, 8, "BIT 1,L");
    mCbOpTable[0x4e].init(&Cpu::oph_Nop, 16, "BIT 1,(HL)");
    mCbOpTable[0x4f].init(&Cpu::oph_Nop, 8, "BIT 1,A");
    mCbOpTable[0x50].init(&Cpu::oph_Nop, 8, "BIT 2,B");
    mCbOpTable[0x51].init(&Cpu::oph_Nop, 8, "BIT 2,C");
    mCbOpTable[0x52].init(&Cpu::oph_Nop, 8, "BIT 2,D");
    mCbOpTable[0x53].init(&Cpu::oph_Nop, 8, "BIT 2,E");
    mCbOpTable[0x54].init(&Cpu::oph_Nop, 8, "BIT 2,H");
    mCbOpTable[0x55].init(&Cpu::oph_Nop, 8, "BIT 2,L");
    mCbOpTable[0x56].init(&Cpu::oph_Nop, 16, "BIT 2,(HL)");
    mCbOpTable[0x57].init(&Cpu::oph_Nop, 8, "BIT 2,A");
    mCbOpTable[0x58].init(&Cpu::oph_Nop, 8, "BIT 3,B");
    mCbOpTable[0x59].init(&Cpu::oph_Nop, 8, "BIT 3,C");
    mCbOpTable[0x5a].init(&Cpu::oph_Nop, 8, "BIT 3,D");
    mCbOpTable[0x5b].init(&Cpu::oph_Nop, 8, "BIT 3,E");
    mCbOpTable[0x5c].init(&Cpu::oph_Nop, 8, "BIT 3,H");
    mCbOpTable[0x5d].init(&Cpu::oph_Nop, 8, "BIT 3,L");
    mCbOpTable[0x5e].init(&Cpu::oph_Nop, 16, "BIT 3,(HL)");
    mCbOpTable[0x5f].init(&Cpu::oph_Nop, 8, "BIT 3,A");
    mCbOpTable[0x60].init(&Cpu::oph_Nop, 8, "BIT 4,B");
    mCbOpTable[0x61].init(&Cpu::oph_Nop, 8, "BIT 4,C");
    mCbOpTable[0x62].init(&Cpu::oph_Nop, 8, "BIT 4,D");
    mCbOpTable[0x63].init(&Cpu::oph_Nop, 8, "BIT 4,E");
    mCbOpTable[0x64].init(&Cpu::oph_Nop, 8, "BIT 4,H");
    mCbOpTable[0x65].init(&Cpu::oph_Nop, 8, "BIT 4,L");
    mCbOpTable[0x66].init(&Cpu::oph_Nop, 16, "BIT 4,(HL)");
    mCbOpTable[0x67].init(&Cpu::oph_Nop, 8, "BIT 4,A");
    mCbOpTable[0x68].init(&Cpu::oph_Nop, 8, "BIT 5,B");
    mCbOpTable[0x69].init(&Cpu::oph_Nop, 8, "BIT 5,C");
    mCbOpTable[0x6a].init(&Cpu::oph_Nop, 8, "BIT 5,D");
    mCbOpTable[0x6b].init(&Cpu::oph_Nop, 8, "BIT 5,E");
    mCbOpTable[0x6c].init(&Cpu::oph_Nop, 8, "BIT 5,H");
    mCbOpTable[0x6d].init(&Cpu::oph_Nop, 8, "BIT 5,L");
    mCbOpTable[0x6e].init(&Cpu::oph_Nop, 16, "BIT 5,(HL)");
    mCbOpTable[0x6f].init(&Cpu::oph_Nop, 8, "BIT 5,A");
    mCbOpTable[0x70].init(&Cpu::oph_Nop, 8, "BIT 6,B");
    mCbOpTable[0x71].init(&Cpu::oph_Nop, 8, "BIT 6,C");
    mCbOpTable[0x72].init(&Cpu::oph_Nop, 8, "BIT 6,D");
    mCbOpTable[0x73].init(&Cpu::oph_Nop, 8, "BIT 6,E");
    mCbOpTable[0x74].init(&Cpu::oph_Nop, 8, "BIT 6,H");
    mCbOpTable[0x75].init(&Cpu::oph_Nop, 8, "BIT 6,L");
    mCbOpTable[0x76].init(&Cpu::oph_Nop, 16, "BIT 6,(HL)");
    mCbOpTable[0x77].init(&Cpu::oph_Nop, 8, "BIT 6,A");
    mCbOpTable[0x78].init(&Cpu::oph_Nop, 8, "BIT 7,B");
    mCbOpTable[0x79].init(&Cpu::oph_Nop, 8, "BIT 7,C");
    mCbOpTable[0x7a].init(&Cpu::oph_Nop, 8, "BIT 7,D");
    mCbOpTable[0x7b].init(&Cpu::oph_Nop, 8, "BIT 7,E");
    mCbOpTable[0x7c].init(&Cpu::oph_Nop, 8, "BIT 7,H");
    mCbOpTable[0x7d].init(&Cpu::oph_Nop, 8, "BIT 7,L");
    mCbOpTable[0x7e].init(&Cpu::oph_Nop, 16, "BIT 7,(HL)");
    mCbOpTable[0x7f].init(&Cpu::oph_Nop, 8, "BIT 7,A");
    mCbOpTable[0x80].init(&Cpu::oph_Nop, 8, "RES 0,B");
    mCbOpTable[0x81].init(&Cpu::oph_Nop, 8, "RES 0,C");
    mCbOpTable[0x82].init(&Cpu::oph_Nop, 8, "RES 0,D");
    mCbOpTable[0x83].init(&Cpu::oph_Nop, 8, "RES 0,E");
    mCbOpTable[0x84].init(&Cpu::oph_Nop, 8, "RES 0,H");
    mCbOpTable[0x85].init(&Cpu::oph_Nop, 8, "RES 0,L");
    mCbOpTable[0x86].init(&Cpu::oph_Nop, 16, "RES 0,(HL)");
    mCbOpTable[0x87].init(&Cpu::oph_Nop, 8, "RES 0,A");
    mCbOpTable[0x88].init(&Cpu::oph_Nop, 8, "RES 1,B");
    mCbOpTable[0x89].init(&Cpu::oph_Nop, 8, "RES 1,C");
    mCbOpTable[0x8a].init(&Cpu::oph_Nop, 8, "RES 1,D");
    mCbOpTable[0x8b].init(&Cpu::oph_Nop, 8, "RES 1,E");
    mCbOpTable[0x8c].init(&Cpu::oph_Nop, 8, "RES 1,H");
    mCbOpTable[0x8d].init(&Cpu::oph_Nop, 8, "RES 1,L");
    mCbOpTable[0x8e].init(&Cpu::oph_Nop, 16, "RES 1,(HL)");
    mCbOpTable[0x8f].init(&Cpu::oph_Nop, 8, "RES 1,A");
    mCbOpTable[0x90].init(&Cpu::oph_Nop, 8, "RES 2,B");
    mCbOpTable[0x91].init(&Cpu::oph_Nop, 8, "RES 2,C");
    mCbOpTable[0x92].init(&Cpu::oph_Nop, 8, "RES 2,D");
    mCbOpTable[0x93].init(&Cpu::oph_Nop, 8, "RES 2,E");
    mCbOpTable[0x94].init(&Cpu::oph_Nop, 8, "RES 2,H");
    mCbOpTable[0x95].init(&Cpu::oph_Nop, 8, "RES 2,L");
    mCbOpTable[0x96].init(&Cpu::oph_Nop, 16, "RES 2,(HL)");
    mCbOpTable[0x97].init(&Cpu::oph_Nop, 8, "RES 2,A");
    mCbOpTable[0x98].init(&Cpu::oph_Nop, 8, "RES 3,B");
    mCbOpTable[0x99].init(&Cpu::oph_Nop, 8, "RES 3,C");
    mCbOpTable[0x9a].init(&Cpu::oph_Nop, 8, "RES 3,D");
    mCbOpTable[0x9b].init(&Cpu::oph_Nop, 8, "RES 3,E");
    mCbOpTable[0x9c].init(&Cpu::oph_Nop, 8, "RES 3,H");
    mCbOpTable[0x9d].init(&Cpu::oph_Nop, 8, "RES 3,L");
    mCbOpTable[0x9e].init(&Cpu::oph_Nop, 16, "RES 3,(HL)");
    mCbOpTable[0x9f].init(&Cpu::oph_Nop, 8, "RES 3,A");
    mCbOpTable[0xa0].init(&Cpu::oph_Nop, 8, "RES 4,B");
    mCbOpTable[0xa1].init(&Cpu::oph_Nop, 8, "RES 4,C");
    mCbOpTable[0xa2].init(&Cpu::oph_Nop, 8, "RES 4,D");
    mCbOpTable[0xa3].init(&Cpu::oph_Nop, 8, "RES 4,E");
    mCbOpTable[0xa4].init(&Cpu::oph_Nop, 8, "RES 4,H");
    mCbOpTable[0xa5].init(&Cpu::oph_Nop, 8, "RES 4,L");
    mCbOpTable[0xa6].init(&Cpu::oph_Nop, 16, "RES 4,(HL)");
    mCbOpTable[0xa7].init(&Cpu::oph_Nop, 8, "RES 4,A");
    mCbOpTable[0xa8].init(&Cpu::oph_Nop, 8, "RES 5,B");
    mCbOpTable[0xa9].init(&Cpu::oph_Nop, 8, "RES 5,C");
    mCbOpTable[0xaa].init(&Cpu::oph_Nop, 8, "RES 5,D");
    mCbOpTable[0xab].init(&Cpu::oph_Nop, 8, "RES 5,E");
    mCbOpTable[0xac].init(&Cpu::oph_Nop, 8, "RES 5,H");
    mCbOpTable[0xad].init(&Cpu::oph_Nop, 8, "RES 5,L");
    mCbOpTable[0xae].init(&Cpu::oph_Nop, 16, "RES 5,(HL)");
    mCbOpTable[0xaf].init(&Cpu::oph_Nop, 8, "RES 5,A");
    mCbOpTable[0xb0].init(&Cpu::oph_Nop, 8, "RES 6,B");
    mCbOpTable[0xb1].init(&Cpu::oph_Nop, 8, "RES 6,C");
    mCbOpTable[0xb2].init(&Cpu::oph_Nop, 8, "RES 6,D");
    mCbOpTable[0xb3].init(&Cpu::oph_Nop, 8, "RES 6,E");
    mCbOpTable[0xb4].init(&Cpu::oph_Nop, 8, "RES 6,H");
    mCbOpTable[0xb5].init(&Cpu::oph_Nop, 8, "RES 6,L");
    mCbOpTable[0xb6].init(&Cpu::oph_Nop, 16, "RES 6,(HL)");
    mCbOpTable[0xb7].init(&Cpu::oph_Nop, 8, "RES 6,A");
    mCbOpTable[0xb8].init(&Cpu::oph_Nop, 8, "RES 7,B");
    mCbOpTable[0xb9].init(&Cpu::oph_Nop, 8, "RES 7,C");
    mCbOpTable[0xba].init(&Cpu::oph_Nop, 8, "RES 7,D");
    mCbOpTable[0xbb].init(&Cpu::oph_Nop, 8, "RES 7,E");
    mCbOpTable[0xbc].init(&Cpu::oph_Nop, 8, "RES 7,H");
    mCbOpTable[0xbd].init(&Cpu::oph_Nop, 8, "RES 7,L");
    mCbOpTable[0xbe].init(&Cpu::oph_Nop, 16, "RES 7,(HL)");
    mCbOpTable[0xbf].init(&Cpu::oph_Nop, 8, "RES 7,A");
    mCbOpTable[0xc0].init(&Cpu::oph_Nop, 8, "SET 0,B");
    mCbOpTable[0xc1].init(&Cpu::oph_Nop, 8, "SET 0,C");
    mCbOpTable[0xc2].init(&Cpu::oph_Nop, 8, "SET 0,D");
    mCbOpTable[0xc3].init(&Cpu::oph_Nop, 8, "SET 0,E");
    mCbOpTable[0xc4].init(&Cpu::oph_Nop, 8, "SET 0,H");
    mCbOpTable[0xc5].init(&Cpu::oph_Nop, 8, "SET 0,L");
    mCbOpTable[0xc6].init(&Cpu::oph_Nop, 16, "SET 0,(HL)");
    mCbOpTable[0xc7].init(&Cpu::oph_Nop, 8, "SET 0,A");
    mCbOpTable[0xc8].init(&Cpu::oph_Nop, 8, "SET 1,B");
    mCbOpTable[0xc9].init(&Cpu::oph_Nop, 8, "SET 1,C");
    mCbOpTable[0xca].init(&Cpu::oph_Nop, 8, "SET 1,D");
    mCbOpTable[0xcb].init(&Cpu::oph_Nop, 8, "SET 1,E");
    mCbOpTable[0xcc].init(&Cpu::oph_Nop, 8, "SET 1,H");
    mCbOpTable[0xcd].init(&Cpu::oph_Nop, 8, "SET 1,L");
    mCbOpTable[0xce].init(&Cpu::oph_Nop, 16, "SET 1,(HL)");
    mCbOpTable[0xcf].init(&Cpu::oph_Nop, 8, "SET 1,A");
    mCbOpTable[0xd0].init(&Cpu::oph_Nop, 8, "SET 2,B");
    mCbOpTable[0xd1].init(&Cpu::oph_Nop, 8, "SET 2,C");
    mCbOpTable[0xd2].init(&Cpu::oph_Nop, 8, "SET 2,D");
    mCbOpTable[0xd3].init(&Cpu::oph_Nop, 8, "SET 2,E");
    mCbOpTable[0xd4].init(&Cpu::oph_Nop, 8, "SET 2,H");
    mCbOpTable[0xd5].init(&Cpu::oph_Nop, 8, "SET 2,L");
    mCbOpTable[0xd6].init(&Cpu::oph_Nop, 16, "SET 2,(HL)");
    mCbOpTable[0xd7].init(&Cpu::oph_Nop, 8, "SET 2,A");
    mCbOpTable[0xd8].init(&Cpu::oph_Nop, 8, "SET 3,B");
    mCbOpTable[0xd9].init(&Cpu::oph_Nop, 8, "SET 3,C");
    mCbOpTable[0xda].init(&Cpu::oph_Nop, 8, "SET 3,D");
    mCbOpTable[0xdb].init(&Cpu::oph_Nop, 8, "SET 3,E");
    mCbOpTable[0xdc].init(&Cpu::oph_Nop, 8, "SET 3,H");
    mCbOpTable[0xdd].init(&Cpu::oph_Nop, 8, "SET 3,L");
    mCbOpTable[0xde].init(&Cpu::oph_Nop, 16, "SET 3,(HL)");
    mCbOpTable[0xdf].init(&Cpu::oph_Nop, 8, "SET 3,A");
    mCbOpTable[0xe0].init(&Cpu::oph_Nop, 8, "SET 4,B");
    mCbOpTable[0xe1].init(&Cpu::oph_Nop, 8, "SET 4,C");
    mCbOpTable[0xe2].init(&Cpu::oph_Nop, 8, "SET 4,D");
    mCbOpTable[0xe3].init(&Cpu::oph_Nop, 8, "SET 4,E");
    mCbOpTable[0xe4].init(&Cpu::oph_Nop, 8, "SET 4,H");
    mCbOpTable[0xe5].init(&Cpu::oph_Nop, 8, "SET 4,L");
    mCbOpTable[0xe6].init(&Cpu::oph_Nop, 16, "SET 4,(HL)");
    mCbOpTable[0xe7].init(&Cpu::oph_Nop, 8, "SET 4,A");
    mCbOpTable[0xe8].init(&Cpu::oph_Nop, 8, "SET 5,B");
    mCbOpTable[0xe9].init(&Cpu::oph_Nop, 8, "SET 5,C");
    mCbOpTable[0xea].init(&Cpu::oph_Nop, 8, "SET 5,D");
    mCbOpTable[0xeb].init(&Cpu::oph_Nop, 8, "SET 5,E");
    mCbOpTable[0xec].init(&Cpu::oph_Nop, 8, "SET 5,H");
    mCbOpTable[0xed].init(&Cpu::oph_Nop, 8, "SET 5,L");
    mCbOpTable[0xee].init(&Cpu::oph_Nop, 16, "SET 5,(HL)");
    mCbOpTable[0xef].init(&Cpu::oph_Nop, 8, "SET 5,A");
    mCbOpTable[0xf0].init(&Cpu::oph_Nop, 8, "SET 6,B");
    mCbOpTable[0xf1].init(&Cpu::oph_Nop, 8, "SET 6,C");
    mCbOpTable[0xf2].init(&Cpu::oph_Nop, 8, "SET 6,D");
    mCbOpTable[0xf3].init(&Cpu::oph_Nop, 8, "SET 6,E");
    mCbOpTable[0xf4].init(&Cpu::oph_Nop, 8, "SET 6,H");
    mCbOpTable[0xf5].init(&Cpu::oph_Nop, 8, "SET 6,L");
    mCbOpTable[0xf6].init(&Cpu::oph_Nop, 16, "SET 6,(HL)");
    mCbOpTable[0xf7].init(&Cpu::oph_Nop, 8, "SET 6,A");
    mCbOpTable[0xf8].init(&Cpu::oph_Nop, 8, "SET 7,B");
    mCbOpTable[0xf9].init(&Cpu::oph_Nop, 8, "SET 7,C");
    mCbOpTable[0xfa].init(&Cpu::oph_Nop, 8, "SET 7,D");
    mCbOpTable[0xfb].init(&Cpu::oph_Nop, 8, "SET 7,E");
    mCbOpTable[0xfc].init(&Cpu::oph_Nop, 8, "SET 7,H");
    mCbOpTable[0xfd].init(&Cpu::oph_Nop, 8, "SET 7,L");
    mCbOpTable[0xfe].init(&Cpu::oph_Nop, 16, "SET 7,(HL)");
    mCbOpTable[0xff].init(&Cpu::oph_Nop, 8, "SET 7,A");
}

void Cpu::oph_Nop(uint16_t p1, uint16_t p2) {
}
void Cpu::oph_Invalid(uint16_t p1, uint16_t p2) {
}
