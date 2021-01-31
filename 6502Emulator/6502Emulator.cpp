#include <iostream>
#include <stdint.h>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;

#define DEBUG_6502

typedef uint8_t BYTE;
typedef uint16_t WORD;

class MEMSPACE {
public:
    MEMSPACE(WORD sta, WORD len) {
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
    WORD length;
};

class ROM : public MEMSPACE {
public:
    ROM(WORD sta, WORD len, BYTE data[])
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
    RAM(WORD sta, WORD len)
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

int default_fn(WORD address, BYTE value, bool write) {
    return 0;
}

class IO : public MEMSPACE {
public:
    IO(WORD sta, WORD len, BYTE(*iofunc)(WORD address, BYTE value, bool write))
    : MEMSPACE(sta,len) {
        if (iofunc != nullptr) {
            iofunction = iofunc;
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
    RAM *create_memaddr(WORD start, WORD length) {
        RAM* ram = new RAM(start, length);
        space.push_back(ram);
        return ram;
    }

    ROM *create_romaddr(WORD start, WORD length, BYTE data[]) {
        ROM* rom = new ROM(start, length, data);
        space.push_back(rom);
        return rom;
    }

    IO *create_ioaddr(WORD start, WORD length, BYTE(*iofunc)(WORD address, BYTE value, bool write)) {
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
                cout << "WRITE " << address << ":" << hex << (WORD)value << endl;
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
    BYTE U : 1;
    BYTE B : 1;
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
                resetsequence = false;
                break;
            }
        }
        else {
            if (cycle == 0) {
#ifdef DEBUG_6502
                cout << "FETCH" << endl;
#endif
                opcode = space.read(PC);
                dump_regs();
                cycle++;
                return;
            }
            // Check for standalone instructions
            // These are instructions the last 5 bits are 0b00000, 0b01000, or 0b11010
            if (opcode_bbbcc(opcode) == 0b00000 || opcode_bbbcc(opcode) == 0b01000 || opcode_bbbcc(opcode) == 0b11010) {
                instruction_standalone();
            }
            else if (opcode_cc(opcode) == 1) {
                decode_01();
            }
            else if (opcode_cc(opcode) == 0) {
                decode_00();
            }
            dump_regs();
        }
    }

    void dump_regs() {
#ifdef DEBUG_6502
        cout << hex << "A:" << (WORD)A << " X:" << (WORD)X << " Y:" << (WORD)Y << " PC:" << PC << " SP:" << (WORD)SP << " CY:" << (WORD)cycle << " OP:" << (WORD)opcode << dec << endl;
#endif
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

    // Set non-arithmetic flags (N, Z)
    void set_nflags(BYTE test) {
        P.Z = (test == 0);
        P.N = (test > 127);
    }

    BYTE BCD_add(BYTE a, BYTE b) {
        BYTE convA = (a & 0xF) + ((a >> 4) * 10);
        BYTE convB = (b & 0xF) + ((b >> 4) * 10);

        BYTE result = convA + convB;
        if (result > 100) {
            P.C = 1;
            result -= 100;
        }

        BYTE ret = result%10;
        ret |= (result / 10) << 4;
        return ret;
    }

    BYTE BCD_sub(BYTE a, BYTE b) {
        BYTE convA = (a & 0xF) + ((a >> 4) * 10);
        BYTE convB = (b & 0xF) + ((b >> 4) * 10);

        BYTE result = convA - convB;
        if (result > 100) {
            P.C = 1;
            result += 100;
        }

        BYTE ret = result % 10;
        ret |= (result / 10) << 4;
        return ret;
    }

    // Set arithmetic flags (C, V) {
    BYTE set_aflags(BYTE testA, BYTE testB, bool sub) {
        BYTE result;
        if (P.D) {
            result = sub ? BCD_sub(testA, testB) : BCD_add(testA, testB);
        }
        else {
            result = sub ? (testA - testB) : (testA + testB);
            if (P.C == 1) {
                result += sub ? -1 : 1;
            }
            if (!sub && !((testA ^ testB) & 0x80) && ((testA ^ result) & 0x80)) {
                P.V = 1;
            }
            if ((sub && (testB > testA) || testB + testA < testA)) {
                P.C = 1;
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
                PTR += X;
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
        switch((OPCODE_01)aaa) {
        case OPCODE_01::LDA:
            done = readaddr_01(bbb, false, 0);
            if (done) {
                A = IMM;
                set_nflags(IMM);
#ifdef DEBUG_6502
                cout << "LDA" << endl;
#endif
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_01::STA:
            done = readaddr_01(bbb, true, A);
            if (done) {
#ifdef DEBUG_6502
                cout << "STA" << endl;
#endif
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_01::ADC:
            done = readaddr_01(bbb, false, 0);
            if (done) {
                A = set_aflags(A, IMM, false);
#ifdef DEBUG_6502
                cout << "ADC" << endl;
#endif
                next_instr();
            }
            break;
        case OPCODE_01::SBC:
            done = readaddr_01(bbb, false, 0);
            if (done) {
                A = set_aflags(A, IMM, true);
#ifdef DEBUG_6502
                cout << "SBC" << endl;
#endif
                next_instr();
            }
        }
    }

    bool readaddr_00(BYTE bbb) {
        switch ((ADDRMODE_00)bbb) {
        case ADDRMODE_00::IMM:
            switch (cycle) {
            case 1:
                IMM = space.read(++PC);
                true;
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
                return true;
            }
        }
        return false;
    }

    void decode_00() {
        BYTE aaa = opcode_aaa(opcode);
        BYTE bbb = opcode_bbb(opcode);
        bool done = false;
        switch((OPCODE_00)aaa) {
        case OPCODE_00::JMP:
            done = readaddr_00(bbb);
            if (cycle == 2) {
                PC = PTR-1;
#ifdef DEBUG_6502
                cout << "JMP" << endl;
#endif
                next_instr();
            }
            else {
                cycle++;
            }
            break;
        case OPCODE_00::JAB:
            done = readaddr_00(bbb);
            if (done) {
                switch (cycle) {
                case 3:
                    PTR2 = PTR;
                    set_PTR_lower(space.read(PTR2++));
                case 4:
                    set_PTR_upper(space.read(PTR2));
                    PC = PTR - 1;
#ifdef DEBUG_6502
                    cout << "JMP (abs)" << endl;
#endif
                    next_instr();
                }
            }
            else {
                cycle++;
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
            next_instr();
            break;
        case INSTRS::SEC:
            P.C = 1;
            next_instr();
            break;
        case INSTRS::CLD:
            P.D = 0;
            next_instr();
            break;
        case INSTRS::SED:
            P.D = 1;
            next_instr();
            break;
        case INSTRS::CLV:
            P.V = 1;
            next_instr();
            break;
        case INSTRS::CLI:
            P.I = 0;
            next_instr();
            break;
        case INSTRS::SEI:
            P.I = 1;
            next_instr();
            break;
        }
    }
};

BYTE text_output(WORD address, BYTE value, bool write) {
    if (address == 0) {
        if (write) {
            cout << (char)value;
            return 0;
        }
        else {
            return 0x42;
        }
    }
    return 0;
}

int main()
{
    auto romcontents = new BYTE[0x8000];
    ifstream romfile;
    char* userprofile;
    errno_t err = _dupenv_s(&userprofile, nullptr, "USERPROFILE");
    if (err) return 2;
    romfile.open(string(userprofile) + string("\\Documents\\ROM.BIN"), ios::in|ios::binary|ios::ate);
    if (!romfile.is_open() || romfile.tellg()!=0x8000) {
        cerr << "Invalid file " << userprofile << "\\Documents\\ROM.BIN" << endl;
        return 1;
    }
    romfile.seekg(0, ios::beg);
    romfile.read((char *)romcontents, 0x8000);
    romfile.close();

    ADDR space;
    ROM* rom = space.create_romaddr(0x8000, 0x8000, romcontents);
    delete[] romcontents;
    RAM* ram = space.create_memaddr(0x0000, 0x4000);
    IO* io = space.create_ioaddr(0x6000, 1, &text_output);
    CPU cpu(space);
    cpu.reset();
    //while (true)
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
        cpu.tick();
}