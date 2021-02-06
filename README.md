# 6502Emulator
 A project to teach myself C++ with
  
Opcodes done:
 - [x] NOP
 - [x] LDA
 - [x] STA
 - [x] JMP
 - [x] ADC
 - [x] SBC
 - [x] All flag opcodes (CLC, SEC, etc.)
 - [x] INX
 - [x] DEX
 - [x] INY
 - [x] DEY
 - [x] TAX
 - [x] TXA
 - [x] TAY
 - [x] TYA
 - [x] ORA
 - [x] AND
 - [x] BIT
 - [x] CPX
 - [x] CPY
 - [x] LDY
 - [x] STY
 - [x] All branch instructions
 - [x] PHP
 - [x] PLP
 - [x] PHA
 - [x] PLA
 - [x] ASL
 - [x] LSR
 - [x] ROL
 - [x] ROR
 - [x] STX
 - [x] LDX
 - [x] INC
 - [x] DEC
 - [x] JSR
 - [x] RTS
 - [x] BRK
 - [x] RTI
 
Internal stuff done:
 - [x] Non-arithmetic flags (N, Z)
 - [x] Flow control flags (B, I)
 - [x] Arithmetic flags (C, V, D)
 - [x] Interrupts
 - [x] NMIs
 
This emulator has passed the 6502 testing suite at https://github.com/Klaus2m5/6502_65C02_functional_tests with the following options:
 - ROM_vectors = 1
 - load_data_direct = 1
 - I_flag = 3
 - disable_selfmod = 1 (due to being off by 2 in the branching code)
 - disable_decimal = 0