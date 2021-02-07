# 6502Emulator
 A project to teach myself C++ with
  
All 6502 opcodes implemented.
 
Internal stuff done:
 - [x] Non-arithmetic flags (N, Z)
 - [x] Flow control flags (B, I)
 - [x] Arithmetic flags (C, V, D)
 - [x] Interrupts
 - [x] NMIs
 
This emulator has passed the 6502 testing program at https://github.com/Klaus2m5/6502_65C02_functional_tests with the following options:
 - ROM_vectors = 1
 - load_data_direct = 1
 - I_flag = 3
 - disable_selfmod = 1 (due to being off by 2 in the branching code)
 - disable_decimal = 0