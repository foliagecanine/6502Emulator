#include <iostream>
#include <stdint.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <Windows.h>
using namespace std;

//#define DEBUG_6502

typedef uint8_t BYTE;
typedef uint16_t WORD;

string ops[] = {
    "BRK",
    "BPL r",
    "JSR a",
    "BMI r",
    "RTI",
    "BVC r",
    "RTS",
    "BVS r",
    "ERR",
    "BCC r",
    "LDY #",
    "BCS r",
    "CPY #",
    "BNE r",
    "CPX #",
    "BEQ r",

    "ORA (d,X)",
    "ORA (d),Y",
    "AND (d,X)",
    "AND (d),Y",
    "EOR (d,X)",
    "EOR (d),Y",
    "ADC (d,X)",
    "ADC (d),Y",
    "STA (d,X)",
    "STA (d),Y",
    "LDA (d,X)",
    "LDA (d),Y",
    "CMP (d,X)",
    "CMP (d),Y",
    "SBC (d,X)",
    "SBC (d),Y",

    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "LDX #",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",

    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",

    "ERR",
    "ERR",
    "BIT d",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "STY d",
    "STY d,X",
    "LDY d",
    "LDY d,X",
    "CPY d",
    "ERR",
    "CPX d",
    "ERR",

    "ORA d",
    "ORA d,X",
    "AND d",
    "AND d,X",
    "EOR d",
    "EOR d,X",
    "ADC d",
    "ADC d,X",
    "STA d",
    "STA d,X",
    "LDA d",
    "LDA d,X",
    "CMP d",
    "CMP d,X",
    "SBC d",
    "SBC d,X",

    "ASL d",
    "ASL d,X",
    "ROL d",
    "ROL d,X",
    "LSR d",
    "LSR d,X",
    "ROR d",
    "ROR d,X",
    "STX d",
    "STX d,Y",
    "LDX d",
    "LDX d,Y",
    "DEC d",
    "DEC d,X",
    "INC d",
    "INC d,X",

    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",

    "PHP",
    "CLC",
    "PLP",
    "SEC",
    "PHA",
    "CLI",
    "PLA",
    "SEI",
    "DEY",
    "TYA",
    "TAY",
    "CLV",
    "INY",
    "CLD",
    "INX",
    "SED",

    "ORA #",
    "ORA a,Y",
    "AND #",
    "AND a,Y",
    "EOR #",
    "EOR a,Y",
    "ADC #",
    "ADC a,Y",
    "ERR",
    "STA a,Y",
    "LDA #",
    "LDA a,Y",
    "CMP #",
    "CMP a,Y",
    "SBC #",
    "SBC a,Y",
    
    "ASL A",
    "ERR",
    "ROL A",
    "ERR",
    "LSR A",
    "ERR",
    "ROR A",
    "ERR",
    "TXA",
    "TXS",
    "TAX",
    "TSX",
    "DEX",
    "ERR",
    "NOP",
    "ERR",

    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",

    "ERR",
    "ERR",
    "BIT a",
    "ERR",
    "JMP a",
    "ERR",
    "JMP (a)",
    "ERR",
    "STY a",
    "ERR",
    "LDY a",
    "LDY a,X",
    "CPY a",
    "ERR",
    "CPX a",
    "ERR",

    "ORA a",
    "ORA a,X",
    "AND a",
    "AND a,X",
    "EOR a",
    "EOR a,X",
    "ADC a",
    "ADC a,X",
    "STA a",
    "STA a,X",
    "LDA a",
    "LDA a,X",
    "CMP a",
    "CMP a,X",
    "SBC a",
    "SBC a,X",

    "ASL a",
    "ASL a,X",
    "ROL a",
    "ROL a,X",
    "LSR a",
    "LSR a,X",
    "ROR a",
    "ROR a,X",
    "STX a",
    "ERR",
    "LDX a",
    "LDX a,Y",
    "DEC a",
    "DEC a,X",
    "INC a",
    "INC a,X",
    
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
    "ERR",
};

class MEMSPACE {
public:
    MEMSPACE(WORD sta, int len) {
        start = sta;
        length = len;
    }

    virtual BYTE read(WORD address) {
        return 0;
    }

    virtual void write(WORD address, BYTE value) {
        
    }

    bool contains(WORD address) {
        return (address >= start && address < start + length);
    }
protected:
    WORD start;
    int length;
};

class ROM : public MEMSPACE {
public:
    ROM(WORD sta, int len, BYTE data[])
    : MEMSPACE(sta,len), rom(new BYTE[len]) {
        for (int i = 0; i < len; i++)
            rom[i] = data[i];
    }

    BYTE read(WORD address) {
        return rom[address-start];
    }

    void write(WORD address, BYTE value) {

    }
private:
    unique_ptr<BYTE[]> rom;
};

class RAM : public MEMSPACE {
public:
    RAM(WORD sta, int len)
    : MEMSPACE(sta,len), ram(new BYTE[len]) {
        for (int i = 0; i < len; i++)
            ram[i] = 0;
    }

    BYTE read(WORD address) {
        return ram[address-start];
    }

    void write(WORD address, BYTE value) {
        ram[address - start] = value;
    }
private:
    unique_ptr<BYTE[]> ram;
};

BYTE default_fn(WORD address, BYTE value, bool write) {
    return 0;
}

class IO : public MEMSPACE {
public:
    IO(WORD sta, int len, BYTE(*iofunc)(WORD address, BYTE value, bool write))
    : MEMSPACE(sta,len), iofunction(iofunc) {
        if (iofunction == nullptr) {
            cerr << "IO has no function!" << endl;
            iofunction = &default_fn;
        }
    }

    BYTE read(WORD address) {
        return iofunction(address-start, 0, false);
    }

    void write(WORD address, BYTE value) {
        iofunction(address-start, value, true);
    }
    
private:
    BYTE(*iofunction)(WORD address, BYTE value, bool write);
};

class ADDR {
public:
    RAM *create_memaddr(WORD start, int length) {
        RAM* ram = new RAM(start, length);
        space.push_back(ram);
        return ram;
    }

    ROM *create_romaddr(WORD start, int length, BYTE data[]) {
        ROM* rom = new ROM(start, length, data);
        space.push_back(rom);
        return rom;
    }

    IO *create_ioaddr(WORD start, int length, BYTE(*iofunc)(WORD address, BYTE value, bool write)) {
        IO* io = new IO(start, length, iofunc);
        space.push_back(io);
        return io;
    }

    void remove_memaddr(MEMSPACE* addr) {
        auto remove = find(space.begin(), space.end(), addr);
        if (remove != space.end())
            space.erase(remove);
    }

    BYTE read(WORD address) {
        for (size_t i = 0; i < space.size(); i++) {
            if (space[i]->contains(address)) {
                BYTE r = space[i]->read(address);
#ifdef DEBUG_6502
                cout << "READ " << hex << address << ":"  << (WORD)r << endl;
#endif
                return r;
            }
        }
#ifdef DEBUG_6502
        cout << "Address " << hex << address << " not found" << endl;
#endif
        return 0;
    }

    void write(WORD address, BYTE value) {
        for (size_t i = 0; i < space.size(); i++) {
            if (space[i]->contains(address)) {
#ifdef DEBUG_6502
                cout << "WRITE " << hex << address << ":" << hex << (WORD)value << endl;
#endif
                space[i]->write(address, value);
            }
        }
    }
private:
    vector<MEMSPACE*> space;
};

struct FLAGS {
    BYTE C : 1;
    BYTE Z : 1;
    BYTE I : 1;
    BYTE D : 1;
    BYTE B : 1;
    BYTE U : 1;
    BYTE V : 1;
    BYTE N : 1;
};

class CPU {
public:
    CPU(ADDR& addrspace) : space(addrspace) {
        
    }

    void reset() {
        resetsequence = true;
    }

    void reset(WORD pc) {
        resetsequence = true;
        customstart = true;
        customstart_pc = pc;
    }

    void tick() {
        if (resetsequence) {
            switch (cycle) {
            case 0:
                SP = 0;
                opcode = 0;
                cycle++;
                break;
            case 1:
            case 2:
                space.read(0x00FF);
                cycle++;
                break;
            case 3:
            case 4:
            case 5:
                space.read(0x0100 + SP);
                SP--;
                cycle++;
                break;
            case 6:
                IMM = space.read(0xFFFC);
                set_PC_lower(IMM);
                cycle++;
                break;
            case 7:
                BYTE IMM = space.read(0xFFFD);
                set_PC_upper(IMM);
                cycle = 0;
                if (customstart)
                    PC = customstart_pc;
                resetsequence = false;
                break;
            }
        }
        else if (nmisequence) {
            switch (cycle) {
            case 1:
                nmi_req = false;
                space.read(++PC);
                PC--;
                cycle++;
                break;
            case 2:
                push((BYTE)(PC >> 8));
                cycle++;
                break;
            case 3:
                push((BYTE)PC);
                cycle++;
                break;
            case 4:
                push(get_pflags());
                cycle++;
                break;
            case 5:
                set_PC_lower(space.read(0xFFFA));
                cycle++;
                break;
            case 6:
                set_PC_upper(space.read(0xFFFB));
                nmisequence = false;
                P.I = 1;
                cycle = 0;
                break;
            }
        }
        else if (irqsequence) {
            switch (cycle) {
            case 1:
                irq_req = false;
                space.read(++PC);
                if (!P.B)
                    PC--;
                cycle++;
                break;
            case 2:
                push((BYTE)(PC >> 8));
                cycle++;
                break;
            case 3:
                push((BYTE)PC);
                cycle++;
                break;
            case 4:
                push(get_pflags());
                cycle++;
                break;
            case 5:
                set_PC_lower(space.read(0xFFFE));
                cycle++;
                break;
            case 6:
                set_PC_upper(space.read(0xFFFF));
                irqsequence = false;
                P.I = 1;
                cycle = 0;
                break;
            }
        }
        else {
            if (cycle == 0) {
                if (!in_interrupt) {
                    if (nmi_req) {
#ifdef DEBUG_6502
                        cout << "NMI" << endl;
#endif
                        nmisequence = true;
                    }
                    else if (irq_req) {
#ifdef DEBUG_6502
                        cout << "IRQ" << endl;
#endif
                        irqsequence = true;
                    }
                }
                opcode = space.read(PC);
#ifdef DEBUG_6502
                cout << "FETCH" << endl;
                BYTE t = (opcode << 4) | (opcode >> 4);
                cout << hex << PC << " " << (WORD)opcode << ":" << ops[t] << endl;
                dump_regs();
#endif
                cycle++;
                return;
            }
            BYTE t = (opcode << 4) | (opcode >> 4);

            // Check for standalone instructions
            // These are instructions the last 4 bits are 0b1000, bits 7 and 4-0 are 0, or its first 4 bits are >= 8 and its last four bits are A
            if ((opcode_bbbcc(opcode) | 0b10000) == 0b11000 || ((opcode&0b10011111)==0) || (((opcode&0xF0)>=0x80) && ((opcode&0x0F)==0x0A))) {
                instruction_standalone();
            }
            else if (opcode_cc(opcode) == 1) {
                decode_01();
            }
            else if (opcode_cc(opcode) == 0 && opcode_bbb(opcode) == 4) {
                branch_00();
            }
            else if (opcode_cc(opcode) == 2) {
                decode_10();
            }
            else if (opcode_cc(opcode) == 0) {
                decode_00();
            }
            dump_regs();
        }
    }

    void dump_regs() {
#ifdef DEBUG_6502
            BYTE t = (opcode << 4) | (opcode >> 4);
            cout << hex << "A:" << (WORD)A << " X:" << (WORD)X << " Y:" << (WORD)Y << " PC:" << PC << " P:" << (P.N ? 'N' : 'n') << (P.V ? 'V' : 'v') << (P.B ? 'B' : 'b') << (P.D ? 'D' : 'd') << (P.I ? 'I' : 'i') << (P.Z ? 'Z' : 'z') << (P.C ? 'C' : 'c') << " SP:" << (WORD)SP << " CY:" << (WORD)cycle << " OP:" << (WORD)opcode << dec << ":" << ops[t] << endl;
#endif
    }

    void nmi() {
        nmi_req = true;
        P.B = 0;
    }

    void irq() {
        if (!P.I) {
            irq_req = true;
            P.B = 0;
        }
    }

private:
    BYTE A = 0;
    BYTE X = 0;
    BYTE Y = 0;
    BYTE SP = 0;
    FLAGS P = {};

    WORD PC = 0;

    BYTE opcode = 0;
    BYTE immediate = 0;
    BYTE cycle = 0;

    BYTE IMM = 0;
    WORD PTR = 0;
    WORD PTR2 = 0;

    bool resetsequence = false;
    bool customstart = false;
    WORD customstart_pc = 0;
    bool nmisequence = false;
    bool irqsequence = false;
    bool in_interrupt = false;
    bool nmi_req = false;
    bool irq_req = false;

    ADDR& space;

    enum class OPCODE_01 {
        ORA = 0,
        AND = 1,
        EOR = 2,
        ADC = 3,
        STA = 4,
        LDA = 5,
        CMP = 6,
        SBC = 7
    };

    enum class ADDRMODE_01 {
        ZXA = 0,
        ZPI = 1,
        IMM = 2,
        ABS = 3,
        ZAY = 4,
        ZPX = 5,
        ABY = 6,
        ABX = 7
    };

    enum class OPCODE_10 {
        ASL = 0,
        ROL = 1,
        LSR = 2,
        ROR = 3,
        STX = 4,
        LDX = 5,
        DEC = 6,
        INC = 7
    };

    enum class ADDRMODE_10 {
        IMM = 0,
        ZPI = 1,
        ACC = 2,
        ABS = 3,
        ZPX = 5,
        ABX = 7
    };

    enum class OPCODE_00 {
        BRA = 0,
        BIT = 1,
        JMP = 2,
        JAB = 3,
        STY = 4,
        LDY = 5,
        CPY = 6,
        CPX = 7
    };

    enum class ADDRMODE_00 {
        IMM = 0,
        ZPI = 1,
        ABS = 3,
        ZPX = 5,
        ABX = 7
    };

    enum class BRANCH_00 {
        BPL = 0,
        BMI = 1,
        BVC = 2,
        BVS = 3,
        BCC = 4,
        BCS = 5,
        BNE = 6,
        BEQ = 7
    };

    enum class INSTRS {
        BRK = 0x00,
        JSR = 0x20,
        RTI = 0x40,
        RTS = 0x60,

        PHP = 0x08,
        PLP = 0x28,
        PHA = 0x48,
        PLA = 0x68,
        DEY = 0x88,
        TAY = 0xA8,
        INY = 0xC8,
        INX = 0xE8,

        CLC = 0x18,
        SEC = 0x38,
        CLI = 0x58,
        SEI = 0x78,
        TYA = 0x98,
        CLV = 0xB8,
        CLD = 0xD8,
        SED = 0xF8,

        TXA = 0x8A,
        TXS = 0x9A,
        TAX = 0xAA,
        TSX = 0xBA,
        DEX = 0xCA,
        NOP = 0xEA
    };

    void set_PC_lower(BYTE value) {
        PC = (PC & 0xFF00) | value;
    }

    void set_PC_upper(BYTE value) {
        PC = (PC & 0x00FF) | (value << 8);
    }

    void set_PTR_lower(BYTE value) {
        PTR = (PTR & 0xFF00) | value;
    }

    void set_PTR_upper(BYTE value) {
        PTR = (PTR & 0x00FF) | (value << 8);
    }

    BYTE opcode_aaa(BYTE op) {
        return op >> 5;
    }

    BYTE opcode_bbb(BYTE op) {
        return (op >> 2) & 0b00000111;
    }

    BYTE opcode_cc(BYTE op) {
        return op & 0b00000011;
    }

    BYTE opcode_bbbcc(BYTE op) {
        return op & 0b00011111;
    }

    void push(BYTE val) {
#ifdef DEBUG_6502
        cout << "PUSH ";
#endif
        space.write(0x100 + (WORD)SP, val);
        SP--;
    }

    BYTE pop() {
        SP++;
        BYTE t = space.read(0x100 + (WORD)SP);
#ifdef DEBUG_6502
        cout << "POP ";
        cout << hex << (WORD)t << endl;
#endif
        return t;
    }

    BYTE get_pflags() {
        BYTE r = 0;
        r = (r | P.N) << 1;
        r = (r | P.V) << 1;
        r = (r | 1) << 1; // Always return 1 for P.U
        r = (r | P.B) << 1;
        r = (r | P.D) << 1;
        r = (r | P.I) << 1;
        r = (r | P.Z) << 1;
        r = (r | P.C);
        return r;
    }

    void set_pflags(BYTE val) {
        BYTE r = val;
        P.C = r & 1;
        r >>= 1;
        P.Z = r & 1;
        r >>= 1;
        P.I = r & 1;
        r >>= 1;
        P.D = r & 1;
        r >>= 1;
        P.B = r & 1;
        r >>= 1;
        P.U = r & 1;
        r >>= 1;
        P.V = r & 1;
        r >>= 1;
        P.N = r;
    }

    // Set non-arithmetic flags (N, Z)
    void set_nflags(BYTE test) {
        P.Z = (test == 0);
        P.N = (test > 127);
    }

    BYTE BCD_add(BYTE a, BYTE b) {
        BYTE convA = (a & 0xF) + ((a >> 4) * 10);
        BYTE convB = (b & 0xF) + ((b >> 4) * 10);

        BYTE result = convA + convB + P.C;
        if (result >= 100) {
            P.C = 1;
            result -= 100;
        }
        else {
            P.C = 0;
        }

        P.V = 0;
        P.N = 0;

        BYTE ret = result%10;
        ret |= (result / 10) << 4;
        return ret;
    }

    BYTE BCD_sub(BYTE a, BYTE b) {
        BYTE convA = (a & 0xF) + ((a >> 4) * 10);
        BYTE convB = (b & 0xF) + ((b >> 4) * 10);

        BYTE result = (convA - convB) - !P.C;
        if (result >= 100) {
            P.C = 0;
            result += 100;
        }
        else {
            P.C = 1;
        }

        P.V = 0;
        P.N = 0;

        BYTE ret = result % 10;
        ret |= (result / 10) << 4;
        return ret;
    }

    // Set arithmetic flags (C, V) {
    BYTE set_aflags(BYTE testA, BYTE testB, bool sub, bool v, bool carry) {
        BYTE result;
        if (P.D && carry) {
            result = sub ? BCD_sub(testA, testB) : BCD_add(testA, testB);
        }
        else {
            result = sub ? (testA - testB) : (testA + testB);
            if (carry) {
                if (P.C == 1 && !sub) {
                    result++;
                }
                else if (P.C == 0 && sub) {
                    result--;
                }
            }
            if (v) {
                BYTE ntestB = sub ? testB ^ 0x80 : testB;
                P.V = !!((testA ^ result) & (ntestB ^ result) & 0x80);
            }
            if ((sub && (testB + (carry ? !P.C : 0) <= testA)) || (!sub && (WORD)testB + (WORD)testA + (WORD)(carry && P.C) > 255)) {
                P.C = 1;
            }
            else {
                P.C = 0;
            }
        }
        return result;
    }

    void next_instr() {
        cycle = 0;
        PC++;
    }

    bool readaddr_01(BYTE bbb, bool out, BYTE outval) {
        switch ((ADDRMODE_01)bbb) {
        case ADDRMODE_01::IMM:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                return true;
            }
        case ADDRMODE_01::ABS:
            switch (cycle) {
            case 1:
                set_PTR_lower(space.read(++PC));
                return false;
            case 2:
                set_PTR_upper(space.read(++PC));
                return false;
            case 3:
                if (out)
                    space.write(PTR,outval);
                else
                    IMM = space.read(PTR);
                return true;
            }
        case ADDRMODE_01::ZPI:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                return false;
            case 2:
                if (out)
                    space.write((WORD)IMM, outval);
                else
                    IMM = space.read((WORD)IMM);
                return true;
            }
        case ADDRMODE_01::ZPX:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                return false;
            case 2:
                IMM += X;
                return false;
            case 3:
                if (out)
                    space.write((WORD)IMM, outval);
                else
                    IMM = space.read((WORD)IMM);
                return true;
            }
        case ADDRMODE_01::ABX:
            switch (cycle) {
            case 1:
                set_PTR_lower(space.read(++PC));
                return false;
            case 2:
                set_PTR_upper(space.read(++PC));
                return false;
            case 3:
                if ((PTR & 0xFF) == 0xFF || out)
                    return false;
            case 4:
                if (out)
                    space.write(PTR + X, outval);
                else
                    IMM = space.read(PTR + X);
                return true;
            }
        case ADDRMODE_01::ABY:
            switch (cycle) {
            case 1:
                set_PTR_lower(space.read(++PC));
                return false;
            case 2:
                set_PTR_upper(space.read(++PC));
                return false;
            case 3:
                if ((PTR & 0xFF) == 0xFF || out)
                    return false;
            case 4:
                if (out)
                    space.write(PTR + Y, outval);
                else
                    IMM = space.read(PTR + Y);
                return true;
            }
        case ADDRMODE_01::ZXA:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                return false;
            case 2:
                IMM += X;
                return false;
            case 3:
                set_PTR_lower(space.read(IMM++));
                return false;
            case 4:
                set_PTR_upper(space.read(IMM));
                return false;
            case 5:
                if (out)
                    space.write(PTR, outval);
                else
                    IMM = space.read(PTR);
                return true;
            }
        case ADDRMODE_01::ZAY:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                return false;
            case 2:
                set_PTR_lower(space.read(IMM++));
                return false;
            case 3:
                set_PTR_upper(space.read(IMM));
                return false;
            case 4:
                if ((PTR & 0xFF) == 0xFF || out)
                    return false;
            case 5:
                if (out)
                    space.write(PTR + Y, outval);
                else
                    IMM = space.read(PTR + Y);
                return true;
            }
        }
        return false;
    }

    void decode_01() {
        BYTE aaa = opcode_aaa(opcode);
        BYTE bbb = opcode_bbb(opcode);
        bool done = false;
        switch ((OPCODE_01)aaa) {
        case OPCODE_01::LDA:
            done = readaddr_01(bbb, false, 0);
            if (done) {
                A = IMM;
                set_nflags(IMM);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_01::STA:
            done = readaddr_01(bbb, true, A);
            if (done) {
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_01::ADC:
            done = readaddr_01(bbb, false, 0);
            if (done) {
                A = set_aflags(A, IMM, false, true, true);
                set_nflags(A);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_01::SBC:
            done = readaddr_01(bbb, false, 0);
            if (done) {
                A = set_aflags(A, IMM, true, true, true);
                set_nflags(A);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_01::ORA:
            done = readaddr_01(bbb, false, 0);
            if (done) {
                A |= IMM;
                set_nflags(A);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_01::AND:
            done = readaddr_01(bbb, false, 0);
            if (done) {
                A &= IMM;
                set_nflags(A);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_01::EOR:
            done = readaddr_01(bbb, false, 0);
            if (done) {
                A ^= IMM;
                set_nflags(A);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_01::CMP:
            done = readaddr_01(bbb, false, 0);
            if (done) {
#ifdef DEBUG_6502
                cout << "CMP " << (WORD)A << " TO " << (WORD)IMM << endl;
#endif
                IMM = set_aflags(A, IMM, true, false, false);
#ifdef DEBUG_6502
                cout << " = " << (WORD)IMM << endl;
#endif
                set_nflags(IMM);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        }
    }

    bool readaddr_00(BYTE bbb, bool out, BYTE outval) {
        switch ((ADDRMODE_00)bbb) {
        case ADDRMODE_00::IMM:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                return true;
            }
        case ADDRMODE_00::ABS:
            switch (cycle) {
            case 1:
                set_PTR_lower(space.read(++PC));
                return false;
            case 2:
                set_PTR_upper(space.read(++PC));
                return false;
            case 3:
                if (out)
                    space.write(PTR, outval);
                else
                    IMM = space.read(PTR);
                return true;
            }
        case ADDRMODE_00::ZPI:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                return false;
            case 2:
                if (out)
                    space.write((WORD)IMM, outval);
                else
                    IMM = space.read((WORD)IMM);
                return true;
            }
        case ADDRMODE_00::ZPX:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                return false;
            case 2:
                IMM += X;
                return false;
            case 3:
                if (out)
                    space.write((WORD)IMM, outval);
                else
                    IMM = space.read((WORD)IMM);
                return true;
            }
        case ADDRMODE_00::ABX:
            switch (cycle) {
            case 1:
                set_PTR_lower(space.read(++PC));
                return false;
            case 2:
                set_PTR_upper(space.read(++PC));
                return false;
            case 3:
                if ((PTR & 0xFF) == 0xFF || out)
                    return false;
            case 4:
                if (out)
                    space.write(PTR + X, outval);
                else
                    IMM = space.read(PTR + X);
                return true;
            }
        }
        return false;
    }

    void decode_00() {
        BYTE aaa = opcode_aaa(opcode);
        BYTE bbb = opcode_bbb(opcode);
        bool done = false;
        switch ((OPCODE_00)aaa) {
        case OPCODE_00::JMP:
            done = readaddr_00(bbb, false, 0);
            if (cycle == 2) {
                PC = PTR - 1;
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_00::JAB:
            done = readaddr_00(bbb, false, 0);
            if (done) {
                switch (cycle) {
                case 3:
                    PTR2 = PTR;
                    set_PTR_lower(space.read(PTR2++));
                    cycle++;
                    break;
                case 4:
                    set_PTR_upper(space.read(PTR2));
                    PC = PTR - 1;
                    next_instr();
                    break;
                }
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_00::BIT:
            done = readaddr_00(bbb, false, 0);
            if (done) {
                P.N = (IMM >> 7) & 1;
                P.V = (IMM >> 6) & 1;
                IMM = A & IMM;
                P.Z = (IMM == 0);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_00::CPX:
            done = readaddr_00(bbb, false, 0);
            if (done) {
#ifdef DEBUG_6502
                cout << "CPX " << (WORD)X << " TO " << (WORD)IMM << endl;
#endif
                IMM = set_aflags(X, IMM, true, false, false);
#ifdef DEBUG_6502
                cout << " = " << (WORD)IMM << endl;
#endif
                set_nflags(IMM);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_00::CPY:
            done = readaddr_00(bbb, false, 0);
            if (done) {
#ifdef DEBUG_6502
                cout << "CPY " << (WORD)Y << " TO " << (WORD)IMM << endl;
#endif
                IMM = set_aflags(Y, IMM, true, false, false);
#ifdef DEBUG_6502
                cout << " = " << (WORD)IMM << endl;
#endif
                set_nflags(IMM);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_00::LDY:
            done = readaddr_00(bbb, false, 0);
            if (done) {
                Y = IMM;
                set_nflags(IMM);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_00::STY:
            done = readaddr_00(bbb, true, Y);
            if (done) {
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        }
    }

    void branch_00() {
        BYTE aaa = opcode_aaa(opcode);
        if (cycle == 2) {
            next_instr();
            return;
        }
        switch ((BRANCH_00)aaa) {
        case BRANCH_00::BCC:
            if (P.C) {
                space.read(++PC);
                next_instr();
                return;
            }
            break;
        case BRANCH_00::BCS:
            if (!P.C) {
                space.read(++PC);
                next_instr();
                return;
            }
            break;
        case BRANCH_00::BVC:
            if (P.V) {
                space.read(++PC);
                next_instr();
                return;
            }
            break;
        case BRANCH_00::BVS:
            if (!P.V) {
                space.read(++PC);
                next_instr();
                return;
            }
            break;
        case BRANCH_00::BPL:
            if (P.N) {
                space.read(++PC);
                next_instr();
                return;
            }
            break;
        case BRANCH_00::BMI:
            if (!P.N) {
                space.read(++PC);
                next_instr();
                return;
            }
            break;
        case BRANCH_00::BEQ:
            if (!P.Z) {
                space.read(++PC);
                next_instr();
                return;
            }
            break;
        case BRANCH_00::BNE:
            if (P.Z) {
                space.read(++PC);
                next_instr();
                return;
            }
            break;
        }
        int8_t t = (int8_t)space.read(++PC);
#ifdef DEBUG_6502
        cout << "BRANCH" << endl;        
        cout << "RELATIVE " << dec << static_cast<int>(t) << endl;
#endif
        PTR = PC + t;
        if ((PTR & 0xFF00) == (PC & 0xFF00)) {
#ifdef DEBUG_6502
            cout << "SAMEPAGE" << endl;
#endif
            PC = PTR;
            next_instr();
            return;
        }
        PC = PTR;
        cycle++;
    }

    bool readaddr_10(OPCODE_10 aaa, BYTE bbb, bool out, BYTE outval) {
        switch ((ADDRMODE_10)bbb) {
        case ADDRMODE_10::IMM:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                return true;
            }
        case ADDRMODE_10::ACC:
            switch (cycle) {
            case 1:
            case 2:
                space.read(PC + 1);
                if (out) {
                    A = IMM;
                }
                else {
                    IMM = A;
                }
                return true;
            }
        case ADDRMODE_10::ZPI:
            switch (cycle) {
            case 1:
                PTR = (WORD)space.read(++PC);
                return false;
            case 2:
                IMM = space.read(PTR);
                return (aaa == OPCODE_10::STX || aaa == OPCODE_10::LDX);
            case 3:
                return true;
            case 4:
                // Assume out
                space.write(PTR, outval);
                return true;
            }
        case ADDRMODE_10::ZPX:
            switch (cycle) {
            case 1:
                IMM = (WORD)space.read(++PC);
                return false;
            case 2:
                if (aaa == OPCODE_10::STX || aaa == OPCODE_10::LDX)
                    IMM += Y;
                else
                    IMM += X;
                return false;
            case 3:
                PTR = IMM;
                IMM = space.read((WORD)IMM);
                return (aaa == OPCODE_10::STX || aaa == OPCODE_10::LDX);
            case 4:
                return true;
            case 5:
                // Assume out
                space.write(PTR, outval);
                return true;
            }
        case ADDRMODE_10::ABS:
            switch (cycle) {
            case 1:
                set_PTR_lower(space.read(++PC));
                return false;
            case 2:
                set_PTR_upper(space.read(++PC));
                return false;
            case 3:
                if (!out) 
                    IMM = space.read(PTR);
                return (aaa == OPCODE_10::STX || aaa == OPCODE_10::LDX);
            case 4:
                return true;
            case 5:
                // Assume out
                space.write(PTR, outval);
                return true;
            }
        case ADDRMODE_10::ABX:
            switch (cycle) {
            case 1:
                set_PTR_lower(space.read(++PC));
                return false;
            case 2:
                set_PTR_upper(space.read(++PC));
                return false;
            case 3:
                if (aaa == OPCODE_10::LDX)
                    PTR += Y;
                else
                    PTR += X;
                return false;
            case 4:
                IMM = space.read(PTR);
                return (aaa == OPCODE_10::LDX);
            case 5:
                return true;
            case 6:
                // Assume out
                space.write(PTR, outval);
                return true;
            }
        }
        return false;
    }

    bool datready = false;
    void decode_10() {
        OPCODE_10 aaa = (OPCODE_10)opcode_aaa(opcode);
        BYTE bbb = opcode_bbb(opcode);
        bool done = false;
        switch (aaa) {
        case OPCODE_10::ASL:
            if (!datready) {
                done = readaddr_10(aaa, bbb, false, 0);
                if (done) {
                    P.C = IMM >> 7;
                    IMM <<= 1;
                    datready = true;
                }
                cycle++;
            }
            else {
                readaddr_10(aaa, bbb, true, IMM);
                set_nflags(IMM);
                datready = false;
                next_instr();
            }
            break;
        case OPCODE_10::LSR:
            if (!datready) {
                done = readaddr_10(aaa, bbb, false, 0);
                if (done) {
                    P.C = IMM & 1;
                    IMM >>= 1;
                    datready = true;
                }
                cycle++;
            }
            else {
                readaddr_10(aaa, bbb, true, IMM);
                set_nflags(IMM);
                datready = false;
                next_instr();
            }
            break;
        case OPCODE_10::ROL:
            if (!datready) {
                done = readaddr_10(aaa, bbb, false, 0);
                if (done) {
                    BYTE t = P.C;
                    P.C = IMM >> 7;
                    IMM <<= 1;
                    IMM |= t;
                    datready = true;
                }
                cycle++;
            }
            else {
                readaddr_10(aaa, bbb, true, IMM);
                set_nflags(IMM);
                datready = false;
                next_instr();
            }
            break;
        case OPCODE_10::ROR:
            if (!datready) {
                done = readaddr_10(aaa, bbb, false, 0);
                if (done) {
                    BYTE t = P.C;
                    P.C = IMM & 1;
                    IMM >>= 1;
                    IMM |= (t << 7);
                    datready = true;
                }
                cycle++;
            }
            else {
                readaddr_10(aaa, bbb, true, IMM);
                set_nflags(IMM);
                datready = false;
                next_instr();
            }
            break;
        case OPCODE_10::STX:
            done = readaddr_10(aaa, bbb, true, X);
            if (done) {
                cycle += 2;
                done = readaddr_10(aaa, bbb, true, X);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_10::LDX:
            done = readaddr_10(aaa, bbb, false, 0);
            if (done) {
                X = IMM;
                set_nflags(X);
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_10::DEC:
            if (!datready) {
                done = readaddr_10(aaa, bbb, false, 0);
                if (done) {
                    IMM--;
                    datready = true;
                }
                cycle++;
            }
            else {
                readaddr_10(aaa, bbb, true, IMM);
                set_nflags(IMM);
                datready = false;
                next_instr();
            }
            break;
        case OPCODE_10::INC:
            if (!datready) {
                done = readaddr_10(aaa, bbb, false, 0);
                if (done) {
                    IMM++;
                    datready = true;
                }
                cycle++;
            }
            else {
                readaddr_10(aaa, bbb, true, IMM);
                set_nflags(IMM);
                datready = false;
                next_instr();
            }
            break;
        }
    }

    void instruction_standalone() {
        switch ((INSTRS)opcode) {
        case INSTRS::NOP:
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::CLC: 
            P.C = 0;
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::SEC:
            P.C = 1;
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::CLD:
            P.D = 0;
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::SED:
            P.D = 1;
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::CLV:
            P.V = 0;
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::CLI:
            P.I = 0;
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::SEI:
            P.I = 1;
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::INX:
            X++;
            set_nflags(X);
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::DEX:
            X--;
            set_nflags(X);
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::INY:
            Y++;
            set_nflags(Y);
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::DEY:
            Y--;
            set_nflags(Y);
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::TAX:
            X = A;
            set_nflags(X);
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::TXA:
            A = X;
            set_nflags(A);
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::TAY:
            Y = A;
            set_nflags(Y);
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::TYA:
            A = Y;
            set_nflags(A);
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::TXS:
            SP = X;
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::TSX:
            X = SP;
            set_nflags(X);
            space.read(PC + 1);
            next_instr();
            break;
        case INSTRS::PHP:
            switch (cycle) {
            case 1:
                space.read(PC + 1);
                cycle++;
                break;
            case 2:
                P.B = 1;
                push(get_pflags());
                next_instr();
                break;
            }
            break;
        case INSTRS::PLP:
            switch (cycle) {
            case 1:
                space.read(PC + 1);
                cycle++;
                break;
            case 2:
                set_pflags(pop());
                next_instr();
                break;
            }
            break;
        case INSTRS::PHA:
            switch (cycle) {
            case 1:
                space.read(PC + 1);
                cycle++;
                break;
            case 2:
                push(A);
                next_instr();
                break;
            }
            break;
        case INSTRS::PLA:
            switch (cycle) {
            case 1:
                space.read(PC + 1);
                cycle++;
                break;
            case 2:
                A = pop();
                set_nflags(A);
                next_instr();
                break;
            }
            break;
        case INSTRS::BRK:
            P.B = 1;
            irq_req = true;
            next_instr();
            break;
        case INSTRS::RTI:
            switch (cycle) {
            case 1:
                set_pflags(pop());
                cycle++;
                break;
            case 2:
                set_PC_lower(pop());
                cycle++;
                break;
            case 3:
                set_PC_upper(pop());
                cycle++;
                break;
            case 4:
                cycle++;
                break;
            case 5:
                cycle = 0;
                break;
            }
            break;
        case INSTRS::JSR:
            switch (cycle) {
            case 1:
                set_PTR_lower(space.read(++PC));
                cycle++;
                break;
            case 2:
                set_PTR_upper(space.read(++PC));
                cycle++;
                break;
            case 3:
                push((BYTE)(PC>>8));
                cycle++;
                break;
            case 4:
                push((BYTE)PC);
                cycle++;
                break;
            case 5:
                PC = PTR-1;
                next_instr();
                break;
            }
            break;
        case INSTRS::RTS:
            switch (cycle) {
            case 1:
                space.read(++PC);
                cycle++;
                break;
            case 2:
                set_PC_lower(pop());
                cycle++;
                break;
            case 3:
                set_PC_upper(pop());
                cycle++;
                break;
            case 4:
                cycle++;
                break;
            case 5:
                next_instr();
                break;
            }
            break;
        }
    }
};

BYTE text_output(WORD address, BYTE value, bool write) {
    if (address == 0) {
        if (write) {
            std::cout << (char)value;
            return 0;
        }
        else {
            return 0x42;
        }
    }
    return 0;
}

#define ROMSIZE 0x8000

int main(int argc, char** argv)
{
    if (argc < 2) {
        cout << "Please specify ROM file." << endl;
        cout << "Use --help for help." << endl;
        cerr << "Failed to open file: No file specified" << endl;
        return 1;
    }

    if (string(argv[1]) == "-h" || string(argv[1]) == "--help") {
        cout << "6502Emulator Help:" << endl;
        cout << "Usage: " << argv[0] << " ROMFILE" << endl;
        cout << endl;
        cout << "ROMFILE: Binary file of size " << dec << ROMSIZE << "." << endl;
        cout << "  Probably should contain 6502 code, but that's up to you." << endl;
        return 0;
    }

    auto romcontents = new BYTE[ROMSIZE];
    ifstream romfile;
    romfile.open(argv[1], ios::in|ios::binary|ios::ate);
    if (!romfile.is_open()) {
        cerr << "Failed to open file: Invalid file: " << argv[1] << endl;
        return 1;
    }
    if (romfile.tellg() != ROMSIZE) {
        cout << "File is wrong size. It must be " << dec << ROMSIZE << " bytes." << endl;
        cerr << "Failed to open file: Invalid filesize: " << dec << romfile.tellg() << " bytes" << endl;
        return 2;
    }
    romfile.seekg(0, ios::beg);
    romfile.read((char *)romcontents, ROMSIZE);
    romfile.close();

    ADDR space;
    RAM* ram = space.create_memaddr(0x0000, 0x4000);
    ROM* rom = space.create_romaddr(0x10000-ROMSIZE, ROMSIZE, romcontents);

    delete[] romcontents;
    IO* io = space.create_ioaddr(0x6000, 1, &text_output);
    CPU cpu(space);
    cpu.reset();
    while (true) {
        cpu.tick();
    }
}