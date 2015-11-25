// Copyright (c) 2015, the Selfie Project authors. All rights reserved.
// Please see the AUTHORS file for details. Use of this source code is
// governed by a BSD license that can be found in the LICENSE file.
//
// Selfie is a project of the Computational Systems Group at the
// Department of Computer Sciences of the University of Salzburg
// in Austria. For further information and code please refer to:
//
// http://selfie.cs.uni-salzburg.at
//
// The Selfie Project provides an educational platform for teaching
// undergraduate and graduate students the design and implementation
// of programming languages and runtime systems. The focus is on the
// construction of compilers, libraries, operating systems, and even
// virtual machine monitors. The common theme is to identify and
// resolve self-reference in systems code which is seen as the key
// challenge when teaching systems engineering, hence the name.
//
// Selfie is a fully self-referential 4k-line C implementation of:
//
// 1. a self-compiling compiler called cstarc that compiles
//    a tiny but powerful subset of C called C Star (C*) to
//    a tiny but powerful subset of MIPS32 called MIPSter,
// 2. a self-executing emulator called mipster that executes
//    MIPSter code including itself when compiled with cstarc, and
// 3. a tiny C* library called libcstar utilized by cstarc and mipster.
//
// Selfie is kept minimal for simplicity and implemented in a single file.
// There is no linker, assembler, or debugger. However, there is minimal
// operating system support in the form of MIPS32 o32 system calls built
// into the emulator. Selfie is meant to be extended in numerous ways.
//
// C* is a tiny Turing-complete subset of C that includes dereferencing
// (the * operator) but excludes data structures, bitwise and Boolean
// operators, and many other features. There are only signed 32-bit
// integers and pointers as well as character and string constants.
// This choice turns out to be helpful for students to understand the
// true role of composite data structures such as arrays and records.
// Bitwise operations are implemented in libcstar using signed integer
// arithmetics helping students gain true understanding of two's complement.
// C* is supposed to be close to the minimum necessary for implementing
// a self-compiling, single-pass, recursive-descent compiler. C* can be
// taught in around two weeks of classes depending on student background.
//
// The compiler can readily be extended to compile features missing in C*
// and to improve performance of the generated code. The compiler generates
// MIPSter executables that can directly be executed by the emulator or
// written to a file in a simple, custom-defined format. Support of standard
// MIPS32 ELF binaries should be easy but remains future work.
//
// MIPSter is a tiny Turing-complete subset of the MIPS32 instruction set.
// It only features arithmetic, memory, and control-flow instructions but
// neither bitwise nor byte-level instructions. MIPSter can be properly
// explained in a single week of classes.
//
// The emulator implements minimal operating system support that is meant
// to be extended by students, first as part of the emulator, and then
// ported to run on top of it, similar to an actual operating system or
// virtual machine monitor. The fact that the emulator can execute itself
// helps exposing the self-referential nature of that challenge.
//
// Selfie is the result of many years of teaching systems engineering.
// The design of the compiler is inspired by the Oberon compiler of
// Professor Niklaus Wirth from ETH Zurich.

int *selfieName = (int*) 0;

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     L I B R A R Y     ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ----------------------- LIBRARY FUNCTIONS -----------------------
// -----------------------------------------------------------------

void initLibrary();

int twoToThePowerOf(int p);
int leftShift(int n, int b);
int rightShift(int n, int b);

int  stringLength(int *s);
void stringReverse(int *s);
int  stringCompare(int *s, int *t);

int  atoi(int *s);
int* itoa(int n, int *s, int b, int a);

void print(int *s);
void println();

void printCharacter(int character);
void printString(int *s);

void exit(int code);
int* malloc(int size);

// ------------------------ GLOBAL CONSTANTS -----------------------

int CHAR_EOF          = -1; // end of file
int CHAR_TAB          = 9;  // ASCII code 9  = tabulator
int CHAR_LF           = 10; // ASCII code 10 = line feed
int CHAR_CR           = 13; // ASCII code 13 = carriage return
int CHAR_SPACE        = ' ';
int CHAR_SEMICOLON    = ';';
int CHAR_PLUS         = '+';
int CHAR_DASH         = '-';
int CHAR_ASTERISK     = '*';
int CHAR_HASH         = '#';
int CHAR_SLASH        = '/';
int CHAR_UNDERSCORE   = '_';
int CHAR_EQUAL        = '=';
int CHAR_LPARENTHESIS = '(';
int CHAR_RPARENTHESIS = ')';
int CHAR_LBRACE       = '{';
int CHAR_RBRACE       = '}';
int CHAR_COMMA        = ',';
int CHAR_LT           = '<';
int CHAR_GT           = '>';
int CHAR_EXCLAMATION  = '!';
int CHAR_PERCENTAGE   = '%';
int CHAR_SINGLEQUOTE  = 39; // ASCII code 39 = '
int CHAR_DOUBLEQUOTE  = '"';

int *power_of_two_table;

int INT_MAX; // maximum numerical value of an integer
int INT_MIN; // minimum numerical value of an integer

int *string_buffer;

// ------------------------- INITIALIZATION ------------------------

void initLibrary() {
    int i;

    power_of_two_table = malloc(31*4);

    *power_of_two_table = 1; // 2^0

    i = 1;

    while (i < 31) {
        *(power_of_two_table + i) = *(power_of_two_table + (i - 1)) * 2;

        i = i + 1;
    }

    // computing INT_MAX and INT_MIN without overflows
    INT_MAX = (twoToThePowerOf(30) - 1) * 2 + 1;
    INT_MIN = -INT_MAX - 1;

    // accommodate 32-bit numbers for itoa
    string_buffer = malloc(33);
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     C O M P I L E R   ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- SCANNER ----------------------------
// -----------------------------------------------------------------

void initScanner();
void resetScanner();

void printSymbol(int symbol);
void printLineNumber(int* message);

void syntaxErrorMessage(int *message);
void syntaxErrorCharacter(int character);

void getCharacter();

int isCharacterWhitespace();
int findNextCharacter();
int isCharacterLetter();
int isCharacterDigit();
int isCharacterLetterOrDigitOrUnderscore();
int isNotDoubleQuoteOrEOF();
int identifierStringMatch(int stringIndex);
int identifierOrKeyword();

int getSymbol();

// ------------------------ GLOBAL CONSTANTS -----------------------

int SYM_EOF          = -1; // end of file
int SYM_IDENTIFIER   = 0;  // identifier
int SYM_INTEGER      = 1;  // integer
int SYM_VOID         = 2;  // VOID
int SYM_INT          = 3;  // INT
int SYM_SEMICOLON    = 4;  // ;
int SYM_IF           = 5;  // IF
int SYM_ELSE         = 6;  // ELSE
int SYM_PLUS         = 7;  // +
int SYM_MINUS        = 8;  // -
int SYM_ASTERISK     = 9;  // *
int SYM_DIV          = 10; // /
int SYM_EQUALITY     = 11; // ==
int SYM_ASSIGN       = 12; // =
int SYM_LPARENTHESIS = 13; // (
int SYM_RPARENTHESIS = 14; // )
int SYM_LBRACE       = 15; // {
int SYM_RBRACE       = 16; // }
int SYM_WHILE        = 17; // WHILE
int SYM_RETURN       = 18; // RETURN
int SYM_COMMA        = 19; // ,
int SYM_LT           = 20; // <
int SYM_LEQ          = 21; // <=
int SYM_GT           = 22; // >
int SYM_GEQ          = 23; // >=
int SYM_NOTEQ        = 24; // !=
int SYM_MOD          = 25; // %
int SYM_CHARACTER    = 26; // character
int SYM_STRING       = 27; // string

int *SYMBOLS; // array of strings representing symbols

int *character_buffer; // buffer for reading characters

int maxIdentifierLength = 64; // maximum number of characters in an identifier
int maxIntegerLength    = 10; // maximum number of characters in an integer
int maxStringLength     = 128; // maximum number of characters in a string

// ------------------------ GLOBAL VARIABLES -----------------------

int lineNumber = 1; // current Line Number for error reporting

int *identifier = (int*) 0; // stores scanned identifier as string
int *integer    = (int*) 0; // stores scanned integer as string
int *string     = (int*) 0; // stores scanned string

int constant = 0; // stores numerical value of scanned integer or character

int initialValue = 0; // stores initial value of variable definitions

int mayBeINTMINConstant = 0; // support INT_MIN constant
int isINTMINConstant    = 0;

int character; // most recently read character
int symbol;    // most recently recognized symbol

int *sourceName = (int*) 0; // name of source file
int sourceFD    = 0;        // file descriptor of source file

// ------------------------- INITIALIZATION ------------------------

void initScanner () {
    SYMBOLS = malloc(28*4);

    *(SYMBOLS + SYM_IDENTIFIER)   = (int) "identifier";
    *(SYMBOLS + SYM_INTEGER)      = (int) "integer";
    *(SYMBOLS + SYM_VOID)         = (int) "void";
    *(SYMBOLS + SYM_INT)          = (int) "int";
    *(SYMBOLS + SYM_SEMICOLON)    = (int) ";";
    *(SYMBOLS + SYM_IF)           = (int) "if";
    *(SYMBOLS + SYM_ELSE)         = (int) "else";
    *(SYMBOLS + SYM_PLUS)         = (int) "+";
    *(SYMBOLS + SYM_MINUS)        = (int) "-";
    *(SYMBOLS + SYM_ASTERISK)     = (int) "*";
    *(SYMBOLS + SYM_DIV)          = (int) "/";
    *(SYMBOLS + SYM_EQUALITY)     = (int) "==";
    *(SYMBOLS + SYM_ASSIGN)       = (int) "=";
    *(SYMBOLS + SYM_LPARENTHESIS) = (int) "(";
    *(SYMBOLS + SYM_RPARENTHESIS) = (int) ")";
    *(SYMBOLS + SYM_LBRACE)       = (int) "{";
    *(SYMBOLS + SYM_RBRACE)       = (int) "}";
    *(SYMBOLS + SYM_WHILE)        = (int) "while";
    *(SYMBOLS + SYM_RETURN)       = (int) "return";
    *(SYMBOLS + SYM_COMMA)        = (int) ",";
    *(SYMBOLS + SYM_LT)           = (int) "<";
    *(SYMBOLS + SYM_LEQ)          = (int) "<=";
    *(SYMBOLS + SYM_GT)           = (int) ">";
    *(SYMBOLS + SYM_GEQ)          = (int) ">=";
    *(SYMBOLS + SYM_NOTEQ)        = (int) "!=";
    *(SYMBOLS + SYM_MOD)          = (int) "%";
    *(SYMBOLS + SYM_CHARACTER)    = (int) "character";
    *(SYMBOLS + SYM_STRING)       = (int) "string";

    character_buffer = malloc(1);

    character = CHAR_EOF;
    symbol    = SYM_EOF;
}

void resetScanner() {
    lineNumber = 1;

    getCharacter();
    getSymbol();
}

// -----------------------------------------------------------------
// -------------------------- ASSIGNMENT2 --------------------------
// -----------------------------------------------------------------

void init_segmentation();
void process_get_segment(int *process, int segment_size, int argc, int *argv);

// -----------------------------------------------------------------
// -------------------------- ASSIGNMENT1 --------------------------
// -----------------------------------------------------------------

// NUM ... Number of bytes, returns destination
int  *memcpy(int *source, int num);
void printInt(int i);
void init_readyqueue(int argc, int *argv);
int  *process_schedule();
void process_switch(int *process);

// -----------------------------------------------------------------
// ----------------------------- DEBUG -----------------------------
// -----------------------------------------------------------------

int DEBUG_1; // Debug first assignment

// -----------------------------------------------------------------
// ----------------------------- STRUCT ----------------------------
// -----------------------------------------------------------------

int  *struct_init(int size);

int  *struct_get_element_at(int *array, int i);
void struct_set_element_at(int *array, int i, int *value);

void struct_test();

// -----------------------------------------------------------------
// ----------------------------- SEGMENT ---------------------------
// -----------------------------------------------------------------

int  *segment_init(int start, int size);

// Getters
int  segment_get_start(int *segment);
int  segment_get_size(int *segment);

// Setters
void segment_set_start(int *segment, int start);
void segment_set_size(int *segment, int size);

// -----------------------------------------------------------------
// ------------------------------ LIST -----------------------------
// -----------------------------------------------------------------

int  *list_init();

int  *list_get_head(int *list);
int  *list_get_tail(int *list);
int  list_get_size(int *list);

void list_set_head(int *list, int *head);
void list_set_tail(int *list, int *tail);
void list_set_size(int *list, int size);

int  list_is_in_bounds(int *list, int index);
int  list_is_empty(int *list);

void list_push_front(int *list, int *data);
void list_push_back(int *list, int *data);
int  *list_pop_front(int *list);
int  *list_pop_back(int *list);

void list_insert_at(int *list, int index, int *data);
int  *list_remove_at(int *list, int index);
int  *list_get_entry_at(int *list, int index);

int  *list_entry_get_next(int *entry);
int  *list_entry_get_prev(int *entry);
int  *list_entry_get_data(int *entry);

void list_entry_set_next(int *entry, int *next);
void list_entry_set_prev(int *entry, int *prev);
void list_entry_set_data(int *entry, int *data);

void list_swap(int *list, int index1, int index2);
// Sorts list by the field at FIELD_NR
void list_sort_by(int *list, int field_nr);

void list_test();

// -----------------------------------------------------------------
// ---------------------------- PROCESS ----------------------------
// -----------------------------------------------------------------

int  *process_allocate();
int  *process_init(int id, int *registers, int *memory, int reg_hi, int reg_lo);

int  process_get_id(int *process);
int  process_get_pc(int *process);
int  *process_get_registers(int *process);
int  *process_get_memory(int *process);
int  process_get_reg_hi(int *process);
int  process_get_reg_lo(int *process);
int  process_get_segment_id(int *process);


void process_set_id(int *process, int id);
void process_set_pc(int *process, int pc);
void process_set_registers(int *process, int *registers);
void process_set_memory(int *process, int *memory);
void process_set_reg_hi(int *process, int reg_hi);
void process_set_reg_lo(int *process, int reg_lo);
void process_set_segment_id(int *process, int segment_id);

void print_process_list(int *list);


// ------------------------ GLOBAL VARIABLES -----------------------

// A1
int  *g_readyqueue;
int  *g_running_process;
int  g_ticks;
// A2
int  *g_segment_table;
int  g_segment_counter;
int  g_segment_offset;
int  g_segmentation_active;
// A3
int  *g_process_table;
int  g_next_segment;

// A1
int  NUMBER_OF_INSTANCES;
int  TIME_SLICE;
int  MEMORY_SIZE;

// -----------------------------------------------------------------
// ------------------------- SYMBOL TABLE --------------------------
// -----------------------------------------------------------------

void resetGlobalSymbolTable();

void createSymbolTableEntry(int which, int *string, int data, int class, int type, int value);
int* getSymbolTableEntry(int *string, int class, int *symbol_table);

int* getNext(int *entry);
int* getString(int *entry);
int  getData(int *entry);
int  getClass(int *entry);
int  getType(int *entry);
int  getValue(int *entry);
int  getRegister(int *entry);

void setNext(int *entry, int *next);
void setString(int *entry, int *identifier);
void setData(int *entry, int data);
void setClass(int *entry, int class);
void setType(int *entry, int type);
void setValue(int *entry, int value);
void setRegister(int *entry, int reg);

// ------------------------ GLOBAL CONSTANTS -----------------------

// classes
int VARIABLE = 1;
int FUNCTION = 2;
int STRING   = 3;

// types
int INT_T     = 1;
int INTSTAR_T = 2;
int VOID_T    = 3;

// symbol tables
int GLOBAL_TABLE = 1;
int LOCAL_TABLE  = 2;

// ------------------------ GLOBAL VARIABLES -----------------------

// table pointers
int *global_symbol_table = (int*) 0;
int *local_symbol_table  = (int*) 0;

// ------------------------- INITIALIZATION ------------------------

void resetGlobalSymbolTable() {
    global_symbol_table = (int*) 0;
}

// -----------------------------------------------------------------
// ---------------------------- PARSER -----------------------------
// -----------------------------------------------------------------

int isNotRbraceOrEOF();
int isExpression();
int isStarOrDivOrModulo();
int isPlusOrMinus();
int isComparison();

int lookForFactor();
int lookForStatement();
int lookForType();

void save_temporaries();
void restore_temporaries(int numberOfTemporaries);

void syntaxErrorSymbol(int expected);
void syntaxErrorUnexpected();
int* putType(int type);
void typeWarning(int expected, int found);

int* getVariable(int *variable);
int  load_variable(int *variable);
void load_integer();
void load_string();

int  help_call_codegen(int *entry, int *procedure);
void help_procedure_prologue(int localVariables);
void help_procedure_epilogue(int parameters);

int  gr_call(int *procedure);
int  gr_factor();
int  gr_term();
int  gr_simpleExpression();
int  gr_expression();
void gr_while();
void gr_if();
void gr_return(int returnType);
void gr_statement();
int  gr_type();
void gr_variable(int offset);
void gr_initialization(int *name, int offset, int type);
void gr_procedure(int *procedure, int returnType);
void gr_cstar();

// ------------------------ GLOBAL VARIABLES -----------------------

int allocatedTemporaries = 0; // number of allocated temporaries

int allocatedMemory = 0; // number of bytes for global variables and strings

int mainJumpAddress = 0; // address of main function
int returnBranches  = 0; // fixup chain for return statements

int *currentProcedureName = (int*) 0; // name of currently parsed procedure

// -----------------------------------------------------------------
// ---------------------- MACHINE CODE LIBRARY ---------------------
// -----------------------------------------------------------------

void emitLeftShiftBy(int b);
void emitMainEntry();

// -----------------------------------------------------------------
// ----------------------------- MAIN ------------------------------
// -----------------------------------------------------------------

void compile();

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// -------------------     I N T E R F A C E     -------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- REGISTER ---------------------------
// -----------------------------------------------------------------

void initRegister();

void printRegister(int reg);

// ------------------------ GLOBAL CONSTANTS -----------------------

int REG_ZR = 0;
int REG_AT = 1;
int REG_V0 = 2;
int REG_V1 = 3;
int REG_A0 = 4;
int REG_A1 = 5;
int REG_A2 = 6;
int REG_A3 = 7;
int REG_T0 = 8;
int REG_T1 = 9;
int REG_T2 = 10;
int REG_T3 = 11;
int REG_T4 = 12;
int REG_T5 = 13;
int REG_T6 = 14;
int REG_T7 = 15;
int REG_S0 = 16;
int REG_S1 = 17;
int REG_S2 = 18;
int REG_S3 = 19;
int REG_S4 = 20;
int REG_S5 = 21;
int REG_S6 = 22;
int REG_S7 = 23;
int REG_T8 = 24;
int REG_T9 = 25;
int REG_K0 = 26;
int REG_K1 = 27;
int REG_GP = 28;
int REG_SP = 29;
int REG_FP = 30;
int REG_RA = 31;

int *REGISTERS; // array of strings representing registers

// ------------------------- INITIALIZATION ------------------------

void initRegister() {
    REGISTERS = malloc(32*4);

    *(REGISTERS + REG_ZR) = (int) "$zero";
    *(REGISTERS + REG_AT) = (int) "$at";
    *(REGISTERS + REG_V0) = (int) "$v0";
    *(REGISTERS + REG_V1) = (int) "$v1";
    *(REGISTERS + REG_A0) = (int) "$a0";
    *(REGISTERS + REG_A1) = (int) "$a1";
    *(REGISTERS + REG_A2) = (int) "$a2";
    *(REGISTERS + REG_A3) = (int) "$a3";
    *(REGISTERS + REG_T0) = (int) "$t0";
    *(REGISTERS + REG_T1) = (int) "$t1";
    *(REGISTERS + REG_T2) = (int) "$t2";
    *(REGISTERS + REG_T3) = (int) "$t3";
    *(REGISTERS + REG_T4) = (int) "$t4";
    *(REGISTERS + REG_T5) = (int) "$t5";
    *(REGISTERS + REG_T6) = (int) "$t6";
    *(REGISTERS + REG_T7) = (int) "$t7";
    *(REGISTERS + REG_S0) = (int) "$s0";
    *(REGISTERS + REG_S1) = (int) "$s1";
    *(REGISTERS + REG_S2) = (int) "$s2";
    *(REGISTERS + REG_S3) = (int) "$s3";
    *(REGISTERS + REG_S4) = (int) "$s4";
    *(REGISTERS + REG_S5) = (int) "$s5";
    *(REGISTERS + REG_S6) = (int) "$s6";
    *(REGISTERS + REG_S7) = (int) "$s7";
    *(REGISTERS + REG_T8) = (int) "$t8";
    *(REGISTERS + REG_T9) = (int) "$t9";
    *(REGISTERS + REG_K0) = (int) "$k0";
    *(REGISTERS + REG_K1) = (int) "$k1";
    *(REGISTERS + REG_GP) = (int) "$gp";
    *(REGISTERS + REG_SP) = (int) "$sp";
    *(REGISTERS + REG_FP) = (int) "$fp";
    *(REGISTERS + REG_RA) = (int) "$ra";
}

// -----------------------------------------------------------------
// ---------------------------- ENCODER ----------------------------
// -----------------------------------------------------------------

int encodeRFormat(int opcode, int rs, int rt, int rd, int function);
int encodeIFormat(int opcode, int rs, int rt, int immediate);
int encodeJFormat(int opcode, int instr_index);

int getOpcode(int instruction);
int getRS(int instruction);
int getRT(int instruction);
int getRD(int instruction);
int getFunction(int instruction);
int getImmediate(int instruction);
int getInstrIndex(int instruction);
int signExtend(int immediate);

// -----------------------------------------------------------------
// ---------------------------- DECODER ----------------------------
// -----------------------------------------------------------------

void initDecoder();

void printOpcode(int opcode);
void printFunction(int function);

void decode();
void decodeRFormat();
void decodeIFormat();
void decodeJFormat();

// ------------------------ GLOBAL CONSTANTS -----------------------

int OP_SPECIAL = 0;
int OP_J       = 2;
int OP_JAL     = 3;
int OP_BEQ     = 4;
int OP_BNE     = 5;
int OP_ADDIU   = 9;
int OP_LW      = 35;
int OP_SW      = 43;
    
int *OPCODES; // array of strings representing MIPS opcodes

int FCT_NOP     = 0;
int FCT_JR      = 8;
int FCT_SYSCALL = 12;
int FCT_MFHI    = 16;
int FCT_MFLO    = 18;
int FCT_MULTU   = 25;
int FCT_DIVU    = 27;
int FCT_ADDU    = 33;
int FCT_SUBU    = 35;
int FCT_SLT     = 42;
int FCT_TEQ     = 52;

int *FUNCTIONS; // array of strings representing MIPS functions

// ------------------------ GLOBAL VARIABLES -----------------------

int opcode      = 0;
int rs          = 0;
int rt          = 0;
int rd          = 0;
int immediate   = 0;
int function    = 0;
int instr_index = 0;

// ------------------------- INITIALIZATION ------------------------

void initDecoder() {
    OPCODES = malloc(44*4);

    *(OPCODES + OP_SPECIAL) = (int) "nop";
    *(OPCODES + OP_J)       = (int) "j";
    *(OPCODES + OP_JAL)     = (int) "jal";
    *(OPCODES + OP_BEQ)     = (int) "beq";
    *(OPCODES + OP_BNE)     = (int) "bne";
    *(OPCODES + OP_ADDIU)   = (int) "addiu";
    *(OPCODES + OP_LW)      = (int) "lw";
    *(OPCODES + OP_SW)      = (int) "sw";

    FUNCTIONS = malloc(53*4);

    *(FUNCTIONS + FCT_NOP)     = (int) "nop";
    *(FUNCTIONS + FCT_JR)      = (int) "jr";
    *(FUNCTIONS + FCT_SYSCALL) = (int) "syscall";
    *(FUNCTIONS + FCT_MFHI)    = (int) "mfhi";
    *(FUNCTIONS + FCT_MFLO)    = (int) "mflo";
    *(FUNCTIONS + FCT_MULTU)   = (int) "multu";
    *(FUNCTIONS + FCT_DIVU)    = (int) "divu";
    *(FUNCTIONS + FCT_ADDU)    = (int) "addu";
    *(FUNCTIONS + FCT_SUBU)    = (int) "subu";
    *(FUNCTIONS + FCT_SLT)     = (int) "slt";
    *(FUNCTIONS + FCT_TEQ)     = (int) "teq";
}

// -----------------------------------------------------------------
// ---------------------------- BINARY -----------------------------
// -----------------------------------------------------------------

int  loadBinary(int addr);
void storeBinary(int addr, int instruction);

void emitInstruction(int instruction);
void emitRFormat(int opcode, int rs, int rt, int rd, int function);
void emitIFormat(int opcode, int rs, int rt, int immediate);
void emitJFormat(int opcode, int instr_index);

void fixup_relative(int fromAddress);
void fixup_absolute(int fromAddress, int toAddress);
void fixlink_absolute(int fromAddress, int toAddress);

int copyStringToBinary(int *s, int a);

void emitGlobalsStrings();

void emit();
void load();

// ------------------------ GLOBAL CONSTANTS -----------------------

int maxBinaryLength = 131072; // 128KB

// ------------------------ GLOBAL VARIABLES -----------------------

int *binary       = (int*) 0;
int  binaryLength = 0;
int *binaryName   = (int*) 0;

// -----------------------------------------------------------------
// --------------------------- SYSCALLS ----------------------------
// -----------------------------------------------------------------

void emitExit();
void syscall_exit();

void emitRead();
void syscall_read();

void emitWrite();
void syscall_write();

void emitOpen();
void syscall_open();

void emitMalloc();
void syscall_malloc();

void emitPutchar();

// -----------------------------------------------------------------
// -------------------------- ASSIGNMENT2 --------------------------
// -----------------------------------------------------------------

void emitSchedYield();
void syscall_sched_yield();

// -----------------------------------------------------------------
// -------------------------- ASSIGNMENT3 --------------------------
// -----------------------------------------------------------------

void emitSchedSetParam();
void syscall_sched_setparam();

void emitSelect();
void syscall_select();

// ------------------------ GLOBAL CONSTANTS -----------------------

int SYSCALL_EXIT    = 4001;
int SYSCALL_READ    = 4003;
int SYSCALL_WRITE   = 4004;
int SYSCALL_OPEN    = 4005;
int SYSCALL_MALLOC  = 5001;
int SYSCALL_GETCHAR = 5002;
// A2
int SYSCALL_SCHED_YIELD = 5003;
int SYSCALL_SCHED_SETPARAM = 5004;
int SYSCALL_SELECT = 5005;

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     E M U L A T O R   ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- MEMORY -----------------------------
// -----------------------------------------------------------------

void initMemory(int megabytes);

int tlb(int vaddr);

int  loadMemory(int vaddr);
void storeMemory(int vaddr, int data);

// ------------------------ GLOBAL VARIABLES -----------------------

int  memorySize;
int *memory;

// ------------------------- INITIALIZATION ------------------------

void initMemory(int megabytes) {
    if (megabytes < 0)
        megabytes = 64;
    else if (megabytes > 1024)
        megabytes = 1024;

    memorySize = megabytes * 1024 * 1024;
    memory     = malloc(memorySize);
}

// -----------------------------------------------------------------
// ------------------------- INSTRUCTIONS --------------------------
// -----------------------------------------------------------------

void fct_syscall();
void fct_nop();
void op_jal();
void op_j();
void op_beq();
void op_bne();
void op_addiu();
void fct_jr();
void op_lui();
void fct_mfhi();
void fct_mflo();
void fct_multu();
void fct_divu();
void fct_addu();
void fct_subu();
void op_lw();
void fct_slt();
void op_sw();
void op_teq();

// -----------------------------------------------------------------
// -------------------------- INTERPRETER --------------------------
// -----------------------------------------------------------------

void initInterpreter();
void resetInterpreter();

void printException(int enumber);

void exception_handler(int enumber);

void pre_debug();
void post_debug();

void fetch();
void execute();
void run();

// -----------------------------------------------------------------
// ----------------------------- MAIN ------------------------------
// -----------------------------------------------------------------

void up_push(int value);
int  up_malloc(int size);
int  up_copyString(int *s);
void up_copyArguments(int argc, int *argv);

void copyBinaryToMemory();

void emulate(int argc, int *argv);

// ------------------------ GLOBAL CONSTANTS -----------------------

int debug_load = 0;

int debug_read    = 0;
int debug_write   = 0;
int debug_open    = 0;
int debug_malloc  = 0;

int debug_registers   = 0;
int debug_disassemble = 0;

int EXCEPTION_SIGNAL             = 1;
int EXCEPTION_ADDRESSERROR       = 2;
int EXCEPTION_UNKNOWNINSTRUCTION = 3;
int EXCEPTION_HEAPOVERFLOW       = 4;
int EXCEPTION_UNKNOWNSYSCALL     = 5;
int EXCEPTION_UNKNOWNFUNCTION    = 6;

int *EXCEPTIONS; // array of strings representing exceptions

// ------------------------ GLOBAL VARIABLES -----------------------

int *registers; // general purpose registers

int pc = 0; // program counter
int ir = 0; // instruction record

int reg_hi = 0; // hi register for multiplication/division
int reg_lo = 0; // lo register for multiplication/division

// ------------------------- INITIALIZATION ------------------------

void initInterpreter() {
    EXCEPTIONS = malloc(7*4);

    *(EXCEPTIONS + EXCEPTION_SIGNAL)             = (int) "signal";
    *(EXCEPTIONS + EXCEPTION_ADDRESSERROR)       = (int) "address error";
    *(EXCEPTIONS + EXCEPTION_UNKNOWNINSTRUCTION) = (int) "unknown instruction";
    *(EXCEPTIONS + EXCEPTION_HEAPOVERFLOW)       = (int) "heap overflow";
    *(EXCEPTIONS + EXCEPTION_UNKNOWNSYSCALL)     = (int) "unknown syscall";
    *(EXCEPTIONS + EXCEPTION_UNKNOWNFUNCTION)    = (int) "unknown function";

    registers = malloc(32*4);
}

void resetInterpreter() {
    pc = 0;

    reg_hi = 0;
    reg_lo = 0;
}
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     L I B R A R Y     ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ----------------------- LIBRARY FUNCTIONS -----------------------
// -----------------------------------------------------------------

int twoToThePowerOf(int p) {
    // assert: 0 <= p < 31
    return *(power_of_two_table + p);
}

int leftShift(int n, int b) {
    // assert: b >= 0;

    if (b > 30)
        return 0;
    else
        return n * twoToThePowerOf(b);
}

int rightShift(int n, int b) {
    // assert: b >= 0

    if (b > 30)
        return 0;
    else if (n >= 0)
        return n / twoToThePowerOf(b);
    else
        // works even if n == INT_MIN
        return ((n + 1) + INT_MAX) / twoToThePowerOf(b) +
            (INT_MAX / twoToThePowerOf(b) + 1);
}

int loadCharacter(int *s, int i) {
    // assert: i >= 0
    int a;

    a = i / 4;

    return rightShift(leftShift(*(s + a), 24 - (i % 4) * 8), 24);
}

int* storeCharacter(int *s, int i, int c) {
    // assert: i >= 0, all characters are 7-bit
    int a;

    a = i / 4;

    *(s + a) = (*(s + a) - leftShift(loadCharacter(s, i), (i % 4) * 8)) + leftShift(c, (i % 4) * 8);
    
    return s;
}

int stringLength(int *s) {
    int i;

    i = 0;

    while (loadCharacter(s, i) != 0)
        i = i + 1;

    return i;
}

void stringReverse(int *s) {
    int i;
    int j;
    int tmp;

    i = 0;
    j = stringLength(s) - 1;

    while (i < j) {
        tmp = loadCharacter(s, i);
        
        storeCharacter(s, i, loadCharacter(s, j));
        storeCharacter(s, j, tmp);

        i = i + 1;
        j = j - 1;
    }
}

int stringCompare(int *s, int *t) {
    int i;

    i = 0;

    while (1)
        if (loadCharacter(s, i) == 0)
            if (loadCharacter(t, i) == 0)
                return 1;
            else
                return 0;
        else if (loadCharacter(s, i) == loadCharacter(t, i))
            i = i + 1;
        else
            return 0;
}

int atoi(int *s) {
    int i;
    int n;
    int c;

    i = 0;

    n = 0;

    c = loadCharacter(s, i);

    while (c != 0) {
        c = c - '0';

        if (c < 0)
            return -1;
        else if (c > 9)
            return -1;

        n = n * 10 + c;
        
        i = i + 1;

        c = loadCharacter(s, i);

        if (n < 0) {
            if (n != INT_MIN)
                return -1;
            else if (c != 0)
                return -1;
        }
    }

    return n;
}

int* itoa(int n, int *s, int b, int a) {
    // assert: b in {2,4,8,10,16}

    int i;
    int sign;

    i = 0;

    sign = 0;

    if (n == 0) {
        storeCharacter(s, 0, '0');

        i = 1;
    } else if (n < 0) {
        sign = 1;

        if (b == 10) {
            if (n == INT_MIN) {
                // rightmost decimal digit of 32-bit INT_MIN
                storeCharacter(s, 0, '8');

                n = -(n / 10);
                i = i + 1;
            } else
                n = -n;
        } else {
            if (n == INT_MIN) {
                // rightmost non-decimal digit of INT_MIN
                storeCharacter(s, 0, '0');

                n = (rightShift(INT_MIN, 1) / b) * 2;
                i = i + 1;
            } else
                n = rightShift(leftShift(n, 1), 1);
        }
    }

    while (n != 0) {
        if (n % b > 9)
            storeCharacter(s, i, n % b - 10 + 'A');
        else
            storeCharacter(s, i, n % b + '0');

        n = n / b;
        i = i + 1;

        if (i == 1) {
            if (sign) {
                if (b != 10)
                    n = n + (rightShift(INT_MIN, 1) / b) * 2;
            }
        }
    }

    if (b != 10) {
        while (i < a) {
            storeCharacter(s, i, '0'); // align with zeros

            i = i + 1;
        }

        if (b == 8) {
            storeCharacter(s, i, '0');
            storeCharacter(s, i + 1, '0');

            i = i + 2;
        } else if (b == 16) {
            storeCharacter(s, i, 'x');
            storeCharacter(s, i + 1, '0');

            i = i + 2;
        }
    } else if (sign) {
        storeCharacter(s, i, '-');

        i = i + 1;
    }

    storeCharacter(s, i, 0); // null terminated string

    stringReverse(s);

    return s;
}

void print(int *s) {
    int i;

    i = 0;

    while (loadCharacter(s, i) != 0) {
        putchar(loadCharacter(s, i));

        i = i + 1;
    }
}

void println() {
    putchar(CHAR_LF);
}

void printCharacter(int character) {
    putchar(CHAR_SINGLEQUOTE);

    if (character == CHAR_EOF)
        print((int*) "end of file");
    else if (character == CHAR_TAB)
        print((int*) "tabulator");
    else if (character == CHAR_LF)
        print((int*) "line feed");
    else if (character == CHAR_CR)
        print((int*) "carriage return");
    else
        putchar(character);

    putchar(CHAR_SINGLEQUOTE);
}

void printString(int *s) {
    putchar(CHAR_DOUBLEQUOTE);

    print(s);
    
    putchar(CHAR_DOUBLEQUOTE);
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     C O M P I L E R   ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- SCANNER ----------------------------
// -----------------------------------------------------------------

void printSymbol(int symbol) {
    putchar(CHAR_DOUBLEQUOTE);

    if (symbol == SYM_EOF)
        print((int*) "end of file");
    else
        print((int*) *(SYMBOLS + symbol));

    putchar(CHAR_DOUBLEQUOTE);
}

void printLineNumber(int* message) {
    print(selfieName);
    print((int*) ": ");
    print(message);
    print((int*) " in ");
    print(sourceName);
    print((int*) " in line ");
    print(itoa(lineNumber, string_buffer, 10, 0));
    print((int*) ": ");
}

void syntaxErrorMessage(int *message) {
    printLineNumber((int*) "error");

    print(message);
    
    println();
}

void syntaxErrorCharacter(int expected) {
    printLineNumber((int*) "error");

    printCharacter(expected);
    print((int*) " expected but ");

    printCharacter(character);
    print((int*) " found");

    println();
}

void getCharacter() {
    int numberOfReadBytes;

    numberOfReadBytes = read(sourceFD, character_buffer, 1);

    if (numberOfReadBytes == 1)
        character = *character_buffer;
    else if (numberOfReadBytes == 0)
        character = CHAR_EOF;
    else {
        print(selfieName);
        print((int*) ": could not read character from input file ");
        print(sourceName);
        println();

        exit(-1);
    }
}

int isCharacterWhitespace() {
    if (character == CHAR_SPACE)
        return 1;
    else if (character == CHAR_TAB)
        return 1;
    else if (character == CHAR_LF)
        return 1;
    else if (character == CHAR_CR)
        return 1;
    else
        return 0;
}

int findNextCharacter() {
    int inComment;

    inComment = 0;

    while (1) {
        if (inComment) {
            getCharacter();

            if (character == CHAR_LF)
                inComment = 0;
            else if (character == CHAR_CR)
                inComment = 0;
            else if (character == CHAR_EOF)
                return character;

        } else if (isCharacterWhitespace()) {
            if (character == CHAR_LF)
                lineNumber = lineNumber + 1;
            else if (character == CHAR_CR)
                lineNumber = lineNumber + 1;

            getCharacter();

        } else if (character == CHAR_HASH) {
            getCharacter();

            inComment = 1;

        } else if (character == CHAR_SLASH) {
            getCharacter();

            if (character == CHAR_SLASH)
                inComment = 1;
            else {
                symbol = SYM_DIV;
                return character;
            }

        } else
            return character;
    }
}

int isCharacterLetter() {
    if (character >= 'a')
        if (character <= 'z')
            return 1;
        else
            return 0;
    else if (character >= 'A')
        if (character <= 'Z')
            return 1;
        else
            return 0;
    else
        return 0;
}

int isCharacterDigit() {
    if (character >= '0')
        if (character <= '9')
            return 1;
        else
            return 0;
    else
        return 0;
}

int isCharacterLetterOrDigitOrUnderscore() {
    if (isCharacterLetter())
        return 1;
    else if (isCharacterDigit())
        return 1;
    else if (character == CHAR_UNDERSCORE)
        return 1;
    else
        return 0;
}

int isNotDoubleQuoteOrEOF() {
    if (character == CHAR_DOUBLEQUOTE)
        return 0;
    else if (character == CHAR_EOF)
        return 0;
    else
        return 1;
}

int identifierStringMatch(int keyword) {
    return stringCompare(identifier, (int*) *(SYMBOLS + keyword));
}

int identifierOrKeyword() {
    if (identifierStringMatch(SYM_WHILE))
        return SYM_WHILE;
    if (identifierStringMatch(SYM_IF))
        return SYM_IF;
    if (identifierStringMatch(SYM_INT))
        return SYM_INT;
    if (identifierStringMatch(SYM_ELSE))
        return SYM_ELSE;
    if (identifierStringMatch(SYM_RETURN))
        return SYM_RETURN;
    if (identifierStringMatch(SYM_VOID))
        return SYM_VOID;
    else
        return SYM_IDENTIFIER;
}

int getSymbol() {
    int i;

    symbol = SYM_EOF;

    if (findNextCharacter() == CHAR_EOF)
        return SYM_EOF;
    else if (symbol == SYM_DIV)
        // check here because / was recognized instead of //
        return SYM_DIV;

    if (isCharacterLetter()) {
        identifier = malloc(maxIdentifierLength + 1);

        i = 0;

        while (isCharacterLetterOrDigitOrUnderscore()) {
            if (i >= maxIdentifierLength) {
                syntaxErrorMessage((int*) "identifier too long");
                exit(-1);
            }

            storeCharacter(identifier, i, character);

            i = i + 1;

            getCharacter();
        }

        storeCharacter(identifier, i, 0); // null terminated string

        symbol = identifierOrKeyword();

    } else if (isCharacterDigit()) {
        integer = malloc(maxIntegerLength + 1);

        i = 0;

        while (isCharacterDigit()) {
            if (i >= maxIntegerLength) {
                syntaxErrorMessage((int*) "integer out of bound");
                exit(-1);
            }

            storeCharacter(integer, i, character);

            i = i + 1;
            
            getCharacter();
        }

        storeCharacter(integer, i, 0); // null terminated string

        constant = atoi(integer);

        if (constant < 0) {
            if (constant == INT_MIN) {
                if (mayBeINTMINConstant)
                    isINTMINConstant = 1;
                else {
                    syntaxErrorMessage((int*) "integer out of bound");
                    exit(-1);
                }
            } else {
                syntaxErrorMessage((int*) "integer out of bound");
                exit(-1);
            }
        }

        symbol = SYM_INTEGER;

    } else if (character == CHAR_SINGLEQUOTE) {
        getCharacter();

        constant = 0;

        if (character == CHAR_EOF) {
            syntaxErrorMessage((int*) "reached end of file looking for a character constant");

            exit(-1);
        } else
            constant = character;

        getCharacter();

        if (character == CHAR_SINGLEQUOTE)
            getCharacter();
        else if (character == CHAR_EOF) {
            syntaxErrorCharacter(CHAR_SINGLEQUOTE);

            exit(-1);
        } else
            syntaxErrorCharacter(CHAR_SINGLEQUOTE);

        symbol = SYM_CHARACTER;

    } else if (character == CHAR_DOUBLEQUOTE) {
        getCharacter();

        string = malloc(maxStringLength + 1);

        i = 0;

        while (isNotDoubleQuoteOrEOF()) {
            if (i >= maxStringLength) {
                syntaxErrorMessage((int*) "string too long");
                exit(-1);
            }

            storeCharacter(string, i, character);

            i = i + 1;
            
            getCharacter();
        }

        if (character == CHAR_DOUBLEQUOTE)
            getCharacter();
        else {
            syntaxErrorCharacter(CHAR_DOUBLEQUOTE);

            exit(-1);
        }

        storeCharacter(string, i, 0); // null terminated string

        symbol = SYM_STRING;

    } else if (character == CHAR_SEMICOLON) {
        getCharacter();

        symbol = SYM_SEMICOLON;

    } else if (character == CHAR_PLUS) {
        getCharacter();

        symbol = SYM_PLUS;

    } else if (character == CHAR_DASH) {
        getCharacter();

        symbol = SYM_MINUS;

    } else if (character == CHAR_ASTERISK) {
        getCharacter();

        symbol = SYM_ASTERISK;

    } else if (character == CHAR_EQUAL) {
        getCharacter();

        if (character == CHAR_EQUAL) {
            getCharacter();

            symbol = SYM_EQUALITY;
        } else
            symbol = SYM_ASSIGN;

    } else if (character == CHAR_LPARENTHESIS) {
        getCharacter();

        symbol = SYM_LPARENTHESIS;

    } else if (character == CHAR_RPARENTHESIS) {
        getCharacter();

        symbol = SYM_RPARENTHESIS;

    } else if (character == CHAR_LBRACE) {
        getCharacter();

        symbol = SYM_LBRACE;

    } else if (character == CHAR_RBRACE) {
        getCharacter();

        symbol = SYM_RBRACE;

    } else if (character == CHAR_COMMA) {
        getCharacter();

        symbol = SYM_COMMA;

    } else if (character == CHAR_LT) {
        getCharacter();

        if (character == CHAR_EQUAL) {
            getCharacter();

            symbol = SYM_LEQ;
        } else
            symbol = SYM_LT;

    } else if (character == CHAR_GT) {
        getCharacter();

        if (character == CHAR_EQUAL) {
            getCharacter();

            symbol = SYM_GEQ;
        } else
            symbol = SYM_GT;

    } else if (character == CHAR_EXCLAMATION) {
        getCharacter();

        if (character == CHAR_EQUAL)
            getCharacter();
        else
            syntaxErrorCharacter(CHAR_EQUAL);

        symbol = SYM_NOTEQ;

    } else if (character == CHAR_PERCENTAGE) {
        getCharacter();

        symbol = SYM_MOD;

    } else {
        printLineNumber((int*) "error");
        print((int*) "found unknown character ");
        printCharacter(character);
        
        println();

        exit(-1);
    }

    return symbol;
}

// -----------------------------------------------------------------
// ------------------------- SYMBOL TABLE --------------------------
// -----------------------------------------------------------------

void createSymbolTableEntry(int whichTable, int *string, int data, int class, int type, int value) {
    int *newEntry;

    // symbol table entry:
    // +----+----------+
    // |  0 | next     | pointer to next entry
    // |  1 | string   | identifier string, string constant
    // |  2 | data     | VARIABLE: offset, FUNCTION: address, STRING: offset
    // |  3 | class    | VARIABLE, FUNCTION, STRING
    // |  4 | type     | INT_T, INTSTAR_T, VOID_T
    // |  5 | value    | VARIABLE: constant value
    // |  6 | register | REG_GP, REG_FP
    // +----+----------+

    newEntry = malloc(7 * 4);

    setString(newEntry, string);
    setData(newEntry, data);
    setClass(newEntry, class);
    setType(newEntry, type);
    setValue(newEntry, value);

    // create entry at head of symbol table
    if (whichTable == GLOBAL_TABLE) {
        setRegister(newEntry, REG_GP);
        setNext(newEntry, global_symbol_table);
        global_symbol_table = newEntry;
    } else {
        setRegister(newEntry, REG_FP);
        setNext(newEntry, local_symbol_table);
        local_symbol_table = newEntry;
    }
}

int* getSymbolTableEntry(int *string, int class, int *symbol_table) {
    while ((int) symbol_table != 0) {
        if (stringCompare(string, getString(symbol_table)))
            if (class == getClass(symbol_table))
                return symbol_table;
        
        // keep looking
        symbol_table = getNext(symbol_table);
    }

    return (int*) 0;
}

int* getNext(int *entry) {
    // cast only works if size of int and int* is equivalent
    return (int*) *entry;
}

int* getString(int *entry) {
    // cast only works if size of int and int* is equivalent
    return (int*) *(entry + 1);
}

int getData(int *entry) {
    return *(entry + 2);
}

int getClass(int *entry) {
    return *(entry + 3);
}

int getType(int *entry) {
    return *(entry + 4);
}

int getValue(int *entry) {
    return *(entry + 5);
}

int getRegister(int *entry) {
    return *(entry + 6);
}

void setNext(int *entry, int *next) {
    // cast only works if size of int and int* is equivalent
    *entry = (int) next;
}

void setString(int *entry, int *identifier) {
    // cast only works if size of int and int* is equivalent
    *(entry + 1) = (int) identifier;
}

void setData(int *entry, int data) {
    *(entry + 2) = data;
}

void setClass(int *entry, int class) {
    *(entry + 3) = class;
}

void setType(int *entry, int type) {
    *(entry + 4) = type;
}

void setValue(int *entry, int value) {
    *(entry + 5) = value;
}

void setRegister(int *entry, int reg) {
    *(entry + 6) = reg;
}

// -----------------------------------------------------------------
// ---------------------------- PARSER -----------------------------
// -----------------------------------------------------------------

int isNotRbraceOrEOF() {
    if (symbol == SYM_RBRACE)
        return 0;
    else if(symbol == SYM_EOF)
        return 0;
    else
        return 1;
}

int isExpression() {
    if (symbol == SYM_MINUS)
        return 1;
    else if (symbol == SYM_LPARENTHESIS)
        return 1;
    else if (symbol == SYM_IDENTIFIER)
        return 1;
    else if (symbol == SYM_INTEGER)
        return 1;
    else if (symbol == SYM_ASTERISK)
        return 1;
    else if (symbol == SYM_STRING)
        return 1;
    else if (symbol == SYM_CHARACTER)
        return 1;
    else
        return 0;
}

int isConstant() {
    if (symbol == SYM_INTEGER)
        return 1;
    else if (symbol == SYM_CHARACTER)
        return 1;
    else
        return 0;
}

int isStarOrDivOrModulo() {
    if (symbol == SYM_ASTERISK)
        return 1;
    else if (symbol == SYM_DIV)
        return 1;
    else if (symbol == SYM_MOD)
        return 1;
    else
        return 0;
}

int isPlusOrMinus() {
    if (symbol == SYM_MINUS)
        return 1;
    else if (symbol == SYM_PLUS)
        return 1;
    else
        return 0;
}

int isComparison() {
    if (symbol == SYM_EQUALITY)
        return 1;
    else if (symbol == SYM_NOTEQ)
        return 1;
    else if (symbol == SYM_LT)
        return 1;
    else if (symbol == SYM_GT)
        return 1;
    else if (symbol == SYM_LEQ)
        return 1;
    else if (symbol == SYM_GEQ)
        return 1;
    else
        return 0;
}

int lookForFactor() {
    if (symbol == SYM_LPARENTHESIS)
        return 0;
    else if (symbol == SYM_ASTERISK)
        return 0;
    else if (symbol == SYM_IDENTIFIER)
        return 0;
    else if (symbol == SYM_INTEGER)
        return 0;
    else if (symbol == SYM_CHARACTER)
        return 0;
    else if (symbol == SYM_STRING)
        return 0;
    else if (symbol == SYM_EOF)
        return 0;
    else
        return 1;
}

int lookForStatement() {
    if (symbol == SYM_ASTERISK)
        return 0;
    else if (symbol == SYM_IDENTIFIER)
        return 0;
    else if (symbol == SYM_WHILE)
        return 0;
    else if (symbol == SYM_IF)
        return 0;
    else if (symbol == SYM_RETURN)
        return 0;
    else if (symbol == SYM_EOF)
        return 0;
    else
        return 1;
}

int lookForType() {
    if (symbol == SYM_INT)
        return 0;
    else if (symbol == SYM_VOID)
        return 0;
    else if (symbol == SYM_EOF)
        return 0;
    else
        return 1;
}

void talloc() {
    // we use registers REG_T0-REG_T9 and REG_S0-REG_S7 for temporaries
    if (allocatedTemporaries < REG_T9 - REG_A3)
        allocatedTemporaries = allocatedTemporaries + 1;
    else {
        syntaxErrorMessage((int*) "out of registers");

        exit(-1);
    }
}

int currentTemporary() {
    if (allocatedTemporaries > 0)
        return allocatedTemporaries + REG_A3;
    else {
        syntaxErrorMessage((int*) "illegal register access");

        exit(-1);
    }
}

int previousTemporary() {
    if (allocatedTemporaries > 1)
        return currentTemporary() - 1;
    else {
        syntaxErrorMessage((int*) "illegal register access");

        exit(-1);
    }
}

int nextTemporary() {
    if (allocatedTemporaries < REG_T9 - REG_A3)
        return currentTemporary() + 1;
    else {
        syntaxErrorMessage((int*) "out of registers");

        exit(-1);
    }
}

void tfree(int numberOfTemporaries) {
    allocatedTemporaries = allocatedTemporaries - numberOfTemporaries;

    if (allocatedTemporaries < 0) {
        syntaxErrorMessage((int*) "illegal register deallocation");

        exit(-1);
    }
}

void save_temporaries() {
    while (allocatedTemporaries > 0) {
        emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);
        emitIFormat(OP_SW, REG_SP, currentTemporary(), 0);

        tfree(1);
    }
}

void restore_temporaries(int numberOfTemporaries) {
    while (allocatedTemporaries < numberOfTemporaries) {
        talloc();

        emitIFormat(OP_LW, REG_SP, currentTemporary(), 0);
        emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);
    }
}

void syntaxErrorSymbol(int expected) {
    printLineNumber((int*) "error");

    printSymbol(expected);
    print((int*) " expected but ");

    printSymbol(symbol);
    print((int*) " found");

    println();
}

void syntaxErrorUnexpected() {
    printLineNumber((int*) "error");

    print((int*) "unexpected symbol ");
    printSymbol(symbol);
    print((int*) " found");

    println();
}

int* putType(int type) {
    if (type == INT_T)
        return (int*) "int";
    else if (type == INTSTAR_T)
        return (int*) "int*";
    else if (type == VOID_T)
        return (int*) "void";
    else
        return (int*) "unknown";
}

void typeWarning(int expected, int found) {
    printLineNumber((int*) "warning");

    print((int*) "type mismatch, ");

    print(putType(expected));

    print((int*) " expected but ");

    print(putType(found));

    print((int*) " found");

    println();
}

int* getVariable(int *variable) {
    int *entry;

    entry = getSymbolTableEntry(variable, VARIABLE, local_symbol_table);

    if (entry == (int*) 0) {
        entry = getSymbolTableEntry(variable, VARIABLE, global_symbol_table);

        if (entry == (int*) 0) {
            printLineNumber((int*) "error");

            print(variable);

            print((int*) " undeclared");
            println();

            exit(-1);
        }
    }

    return entry;
}

int load_variable(int *variable) {
    int *entry;

    entry = getVariable(variable);

    talloc();

    emitIFormat(OP_LW, getRegister(entry), currentTemporary(), getData(entry));

    return getType(entry);
}

void load_integer() {
    // assert: constant >= 0 or constant == INT_MIN

    talloc();

    if (constant >= 0) {
        if (constant < twoToThePowerOf(15))
            // ADDIU can only load numbers < 2^15 without sign extension
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), constant);
        else if (constant < twoToThePowerOf(28)) {
            // load 14 msbs of a 28-bit number first
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), rightShift(constant, 14));

            // shift left by 14 bits
            emitLeftShiftBy(14);

            // and finally add 14 lsbs
            emitIFormat(OP_ADDIU, currentTemporary(), currentTemporary(), rightShift(leftShift(constant, 18), 18));
        } else {
            // load 14 msbs of a 31-bit number first
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), rightShift(constant, 17));

            emitLeftShiftBy(14);

            // then add the next 14 msbs
            emitIFormat(OP_ADDIU, currentTemporary(), currentTemporary(), rightShift(leftShift(constant, 15), 18));

            emitLeftShiftBy(3);

            // and finally add the remaining 3 lsbs
            emitIFormat(OP_ADDIU, currentTemporary(), currentTemporary(), rightShift(leftShift(constant, 29), 29));
        }
    } else {
        // load largest positive 16-bit number with a single bit set: 2^14
        emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), twoToThePowerOf(14));

        // and then multiply 2^14 by 2^14*2^3 to get to 2^31 == INT_MIN
        emitLeftShiftBy(14);
        emitLeftShiftBy(3);
    }
}

void load_string() {
    int l;

    l = stringLength(string) + 1;

    allocatedMemory = allocatedMemory + l;

    if (l % 4 != 0)
        allocatedMemory = allocatedMemory + 4 - l % 4;

    createSymbolTableEntry(GLOBAL_TABLE, string, -allocatedMemory, STRING, INTSTAR_T, 0);

    talloc();

    emitIFormat(OP_ADDIU, REG_GP, currentTemporary(), -allocatedMemory);
}

int help_call_codegen(int *entry, int *procedure) {
    int type;

    if (entry == (int*) 0) {
        // CASE 1: function call, no definition, no declaration.
        createSymbolTableEntry(GLOBAL_TABLE, procedure, binaryLength, FUNCTION, INT_T, 0);

        emitJFormat(OP_JAL, 0);

        type = INT_T; //assume default return type 'int'

    } else {
        type = getType(entry);

        if (getData(entry) == 0) {
            // CASE 2: function call, no definition, but declared.
            setData(entry, binaryLength);

            emitJFormat(OP_JAL, 0);
        } else if (getOpcode(loadBinary(getData(entry))) == OP_JAL) {
            // CASE 3: function call, no declaration
            emitJFormat(OP_JAL, getData(entry) / 4);

            setData(entry, binaryLength - 8);
        } else
            // CASE 4: function defined, use the address
            emitJFormat(OP_JAL, getData(entry) / 4);
    }

    return type;
}

void help_procedure_prologue(int localVariables) {
    // allocate space for return address
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);

    // save return address
    emitIFormat(OP_SW, REG_SP, REG_RA, 0);

    // allocate space for caller's frame pointer
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);

    // save caller's frame pointer
    emitIFormat(OP_SW, REG_SP, REG_FP, 0);

    // set callee's frame pointer
    emitIFormat(OP_ADDIU, REG_SP, REG_FP, 0);

    // allocate space for callee's local variables
    if (localVariables != 0)
        emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4 * localVariables);
}

void help_procedure_epilogue(int parameters) {
    // deallocate space for callee's frame pointer and local variables
    emitIFormat(OP_ADDIU, REG_FP, REG_SP, 0);

    // restore caller's frame pointer
    emitIFormat(OP_LW, REG_SP, REG_FP, 0);

    // deallocate space for caller's frame pointer
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    // restore return address
    emitIFormat(OP_LW, REG_SP, REG_RA, 0);

    // deallocate space for return address and parameters
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, (parameters + 1) * 4);

    // return
    emitRFormat(OP_SPECIAL, REG_RA, 0, 0, FCT_JR);
}

int gr_call(int *procedure) {
    int *entry;
    int numberOfTemporaries;
    int type;

    // assert: n = allocatedTemporaries

    entry = getSymbolTableEntry(procedure, FUNCTION, global_symbol_table);

    numberOfTemporaries = allocatedTemporaries;

    save_temporaries();

    // assert: allocatedTemporaries == 0

    if (isExpression()) {
        gr_expression();

        // TODO: check if types/number of parameters is correct
        // push first parameter onto stack
        emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);
        emitIFormat(OP_SW, REG_SP, currentTemporary(), 0);

        tfree(1);

        while (symbol == SYM_COMMA) {
            getSymbol();

            gr_expression();

            // push more parameters onto stack
            emitIFormat(OP_ADDIU, REG_SP, REG_SP, -4);
            emitIFormat(OP_SW, REG_SP, currentTemporary(), 0);

            tfree(1);
        }

        if (symbol == SYM_RPARENTHESIS) {
            getSymbol();
            
            type = help_call_codegen(entry, procedure);
        } else {
            syntaxErrorSymbol(SYM_RPARENTHESIS);

            type = INT_T;
        }
    } else if (symbol == SYM_RPARENTHESIS) {
        getSymbol();

        type = help_call_codegen(entry, procedure);
    } else {
        syntaxErrorSymbol(SYM_RPARENTHESIS);

        type = INT_T;
    }

    // assert: allocatedTemporaries == 0

    restore_temporaries(numberOfTemporaries);

    // assert: allocatedTemporaries == n
    return type;
}

int gr_factor() {
    int hasCast;
    int cast;
    int type;

    int *variableOrProcedureName;

    // assert: n = allocatedTemporaries

    hasCast = 0;

    type = INT_T;

    while (lookForFactor()) {
        syntaxErrorUnexpected();

        if (symbol == SYM_EOF)
            exit(-1);
        else
            getSymbol();
    }

    // optional cast: [ cast ]
    if (symbol == SYM_LPARENTHESIS) {
        getSymbol();

        // cast: "(" "int" [ "*" ] ")"
        if (symbol == SYM_INT) {
            hasCast = 1;

            cast = gr_type();

            if (symbol == SYM_RPARENTHESIS)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_RPARENTHESIS);

        // not a cast: "(" expression ")"
        } else {
            type = gr_expression();

            if (symbol == SYM_RPARENTHESIS)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_RPARENTHESIS);

            // assert: allocatedTemporaries == n + 1

            return type;
        }
    }

    // dereference?
    if (symbol == SYM_ASTERISK) {
        getSymbol();

        // ["*"] identifier
        if (symbol == SYM_IDENTIFIER) {
            type = load_variable(identifier);

            getSymbol();

        // * "(" expression ")"
        } else if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            type = gr_expression();

            if (symbol == SYM_RPARENTHESIS)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        } else
            syntaxErrorUnexpected();

        if (type != INTSTAR_T)
            typeWarning(INTSTAR_T, type);

        // dereference
        emitIFormat(OP_LW, currentTemporary(), currentTemporary(), 0);

        type = INT_T;

    // identifier?
    } else if (symbol == SYM_IDENTIFIER) {
        variableOrProcedureName = identifier;

        getSymbol();

        if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            // function call: identifier "(" ... ")"
            type = gr_call(variableOrProcedureName);

            talloc();

            emitIFormat(OP_ADDIU, REG_V0, currentTemporary(), 0);
        } else
            // variable access: identifier
            type = load_variable(variableOrProcedureName);

    // integer?
    } else if (symbol == SYM_INTEGER) {
        load_integer();

        getSymbol();

        type = INT_T;

    // character?
    } else if (symbol == SYM_CHARACTER) {
        talloc();

        emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), constant);

        getSymbol();
    
        type = INT_T;
        
    // string?
    } else if (symbol == SYM_STRING) {
        load_string();

        getSymbol();

        type = INTSTAR_T;

    //  "(" expression ")"
    } else if (symbol == SYM_LPARENTHESIS) {
        getSymbol();

        type = gr_expression();

        if (symbol == SYM_RPARENTHESIS)
            getSymbol();
        else
            syntaxErrorSymbol(SYM_RPARENTHESIS);
    } else
        syntaxErrorUnexpected();

    // assert: allocatedTemporaries == n + 1

    if (hasCast)
        return cast;
    else
        return type;
}

int gr_term() {
    int ltype;
    int operatorSymbol;
    int rtype;

    // assert: n = allocatedTemporaries

    ltype = gr_factor();

    // assert: allocatedTemporaries == n + 1

    // * / or % ?
    while (isStarOrDivOrModulo()) {
        operatorSymbol = symbol;

        getSymbol();

        rtype = gr_factor();

        // assert: allocatedTemporaries == n + 2
        
        if (ltype != rtype)
            typeWarning(ltype, rtype);

        if (operatorSymbol == SYM_ASTERISK) {
            emitRFormat(OP_SPECIAL, previousTemporary(), currentTemporary(), 0, FCT_MULTU);
            emitRFormat(OP_SPECIAL, 0, 0, previousTemporary(), FCT_MFLO);

        } else if (operatorSymbol == SYM_DIV) {
            emitRFormat(OP_SPECIAL, previousTemporary(), currentTemporary(), 0, FCT_DIVU);
            emitRFormat(OP_SPECIAL, 0, 0, previousTemporary(), FCT_MFLO);

        } else if (operatorSymbol == SYM_MOD) {
            emitRFormat(OP_SPECIAL, previousTemporary(), currentTemporary(), 0, FCT_DIVU);
            emitRFormat(OP_SPECIAL, 0, 0, previousTemporary(), FCT_MFHI);
        }

        tfree(1);
    }

    // assert: allocatedTemporaries == n + 1

    return ltype;
}

int gr_simpleExpression() {
    int sign;
    int ltype;
    int operatorSymbol;
    int rtype;

    // assert: n = allocatedTemporaries

    // optional: -
    if (symbol == SYM_MINUS) {
        sign = 1;

        mayBeINTMINConstant = 1;
        isINTMINConstant    = 0;

        getSymbol();

        mayBeINTMINConstant = 0;

        if (isINTMINConstant) {
            isINTMINConstant = 0;
            
            // avoids 0-INT_MIN overflow when bootstrapping
            // even though 0-INT_MIN == INT_MIN
            sign = 0;
        }
    } else
        sign = 0;

    ltype = gr_term();

    // assert: allocatedTemporaries == n + 1

    if (sign == 1) {
        if (ltype != INT_T) {
            typeWarning(INT_T, ltype);

            ltype = INT_T;
        }

        emitRFormat(OP_SPECIAL, REG_ZR, currentTemporary(), currentTemporary(), FCT_SUBU);
    }

    // + or -?
    while (isPlusOrMinus()) {
        operatorSymbol = symbol;

        getSymbol();

        rtype = gr_term();

        // assert: allocatedTemporaries == n + 2

        if (operatorSymbol == SYM_PLUS) {
            if (ltype == INTSTAR_T) {
                if (rtype == INT_T)
                    // pointer arithmetic: factor of 2^2 of integer operand
                    emitLeftShiftBy(2);
            } else if (rtype == INTSTAR_T)
                typeWarning(ltype, rtype);

            emitRFormat(OP_SPECIAL, previousTemporary(), currentTemporary(), previousTemporary(), FCT_ADDU);

        } else if (operatorSymbol == SYM_MINUS) {
            if (ltype != rtype)
                typeWarning(ltype, rtype);

            emitRFormat(OP_SPECIAL, previousTemporary(), currentTemporary(), previousTemporary(), FCT_SUBU);
        }

        tfree(1);
    }

    // assert: allocatedTemporaries == n + 1

    return ltype;
}

int gr_expression() {
    int ltype;
    int operatorSymbol;
    int rtype;

    // assert: n = allocatedTemporaries

    ltype = gr_simpleExpression();

    // assert: allocatedTemporaries == n + 1

    //optional: ==, !=, <, >, <=, >= simpleExpression
    if (isComparison()) {
        operatorSymbol = symbol;

        getSymbol();

        rtype = gr_simpleExpression();

        // assert: allocatedTemporaries == n + 2

        if (ltype != rtype)
            typeWarning(ltype, rtype);

        if (operatorSymbol == SYM_EQUALITY) {
            // subtract, if result = 0 then 1, else 0
            emitRFormat(OP_SPECIAL, previousTemporary(), currentTemporary(), previousTemporary(), FCT_SUBU);

            tfree(1);

            emitIFormat(OP_BEQ, REG_ZR, currentTemporary(), 4);
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), 0);
            emitIFormat(OP_BEQ, REG_ZR, currentTemporary(), 2);
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), 1);

        } else if (operatorSymbol == SYM_NOTEQ) {
            // subtract, if result = 0 then 0, else 1
            emitRFormat(OP_SPECIAL, previousTemporary(), currentTemporary(), previousTemporary(), FCT_SUBU);

            tfree(1);

            emitIFormat(OP_BNE, REG_ZR, currentTemporary(), 4);
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), 0);
            emitIFormat(OP_BEQ, REG_ZR, currentTemporary(), 2);
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), 1);

        } else if (operatorSymbol == SYM_LT) {
            // set to 1 if a < b, else 0
            emitRFormat(OP_SPECIAL, previousTemporary(), currentTemporary(), previousTemporary(), FCT_SLT);

            tfree(1);

        } else if (operatorSymbol == SYM_GT) {
            // set to 1 if b < a, else 0
            emitRFormat(OP_SPECIAL, currentTemporary(), previousTemporary(), previousTemporary(), FCT_SLT);

            tfree(1);

        } else if (operatorSymbol == SYM_LEQ) {
            // if b < a set 0, else 1
            emitRFormat(OP_SPECIAL, currentTemporary(), previousTemporary(), previousTemporary(), FCT_SLT);

            tfree(1);

            emitIFormat(OP_BNE, REG_ZR, currentTemporary(), 4);
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), 1);
            emitIFormat(OP_BEQ, REG_ZR, REG_ZR, 2);
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), 0);

        } else if (operatorSymbol == SYM_GEQ) {
            // if a < b set 0, else 1
            emitRFormat(OP_SPECIAL, previousTemporary(), currentTemporary(), previousTemporary(), FCT_SLT);

            tfree(1);

            emitIFormat(OP_BNE, REG_ZR, currentTemporary(), 4);
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), 1);
            emitIFormat(OP_BEQ, REG_ZR, REG_ZR, 2);
            emitIFormat(OP_ADDIU, REG_ZR, currentTemporary(), 0);
        }
    }
    
    // assert: allocatedTemporaries == n + 1

    return ltype;
}

void gr_while() {
    int brBackToWhile;
    int brForwardToEnd;

    // assert: allocatedTemporaries == 0

    brBackToWhile = binaryLength;

    brForwardToEnd = 0;

    // while ( expression )
    if (symbol == SYM_WHILE) {
        getSymbol();

        if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            gr_expression();

            // do not know where to branch, fixup later
            brForwardToEnd = binaryLength;

            emitIFormat(OP_BEQ, REG_ZR, currentTemporary(), 0);

            tfree(1);

            if (symbol == SYM_RPARENTHESIS) {
                getSymbol();

                // zero or more statements: { statement }
                if (symbol == SYM_LBRACE) {
                    getSymbol();

                    while (isNotRbraceOrEOF())
                        gr_statement();

                    if (symbol == SYM_RBRACE)
                        getSymbol();
                    else {
                        syntaxErrorSymbol(SYM_RBRACE);

                        exit(-1);
                    }
                }
                // only one statement without {}
                else
                    gr_statement();
            } else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        } else
            syntaxErrorSymbol(SYM_LPARENTHESIS);
    } else
        syntaxErrorSymbol(SYM_WHILE);

    // unconditional branch to beginning of while
    emitIFormat(OP_BEQ, 0, 0, (brBackToWhile - binaryLength - 4) / 4);

    if (brForwardToEnd != 0)
        // first instruction after loop comes here
        // now we have our address for the conditional jump from above
        fixup_relative(brForwardToEnd);

    // assert: allocatedTemporaries == 0
}

void gr_if() {
    int brForwardToElseOrEnd;
    int brForwardToEnd;

    // assert: allocatedTemporaries == 0

    // if ( expression )
    if (symbol == SYM_IF) {
        getSymbol();

        if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            gr_expression();

            // if the "if" case is not true, we jump to "else" (if provided)
            brForwardToElseOrEnd = binaryLength;

            emitIFormat(OP_BEQ, REG_ZR, currentTemporary(), 0);

            tfree(1);

            if (symbol == SYM_RPARENTHESIS) {
                getSymbol();

                // zero or more statements: { statement }
                if (symbol == SYM_LBRACE) {
                    getSymbol();

                    while (isNotRbraceOrEOF())
                        gr_statement();

                    if (symbol == SYM_RBRACE)
                        getSymbol();
                    else {
                        syntaxErrorSymbol(SYM_RBRACE);

                        exit(-1);
                    }
                }
                // only one statement without {}
                else
                    gr_statement();

                //optional: else
                if (symbol == SYM_ELSE) {
                    getSymbol();

                    // if the "if" case was true, we jump to the end
                    brForwardToEnd = binaryLength;
                    emitIFormat(OP_BEQ, 0, 0, 0);

                    // if the "if" case was not true, we jump here
                    fixup_relative(brForwardToElseOrEnd);

                    // zero or more statements: { statement }
                    if (symbol == SYM_LBRACE) {
                        getSymbol();

                        while (isNotRbraceOrEOF())
                            gr_statement();

                        if (symbol == SYM_RBRACE)
                            getSymbol();
                        else {
                            syntaxErrorSymbol(SYM_RBRACE);

                            exit(-1);
                        }

                    // only one statement without {}
                    } else
                        gr_statement();

                    // if the "if" case was true, we jump here
                    fixup_relative(brForwardToEnd);
                } else
                    // if the "if" case was not true, we jump here
                    fixup_relative(brForwardToElseOrEnd);
            } else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        } else
            syntaxErrorSymbol(SYM_LPARENTHESIS);
    } else
        syntaxErrorSymbol(SYM_IF);

    // assert: allocatedTemporaries == 0
}

void gr_return(int returnType) {
    int type;

    // assert: allocatedTemporaries == 0

    if (symbol == SYM_RETURN)
        getSymbol();
    else
        syntaxErrorSymbol(SYM_RETURN);

    // optional: expression
    if (symbol != SYM_SEMICOLON) {
        type = gr_expression();

        if (returnType == VOID_T)
            typeWarning(type, returnType);
        else if (type != returnType)
            typeWarning(returnType, type);

        // save value of expression in return register
        emitRFormat(OP_SPECIAL, REG_ZR, currentTemporary(), REG_V0, FCT_ADDU);
        
        tfree(1);
    }

    // unconditional branch to procedure epilogue
    // maintain fixup chain for later fixup
    emitJFormat(OP_J, returnBranches / 4);

    // new head of fixup chain
    // offest is 8 rather than 4 bytes because of delay slot NOP
    returnBranches = binaryLength - 8;

    // assert: allocatedTemporaries == 0
}

void gr_statement() {
    int ltype;
    int rtype;
    int *variableOrProcedureName;
    int *entry;

    // assert: allocatedTemporaries == 0;

    while (lookForStatement()) {
        syntaxErrorUnexpected();

        if (symbol == SYM_EOF)
            exit(-1);
        else
            getSymbol();
    }

    // ["*"]
    if (symbol == SYM_ASTERISK) {
        getSymbol();

        // "*" identifier
        if (symbol == SYM_IDENTIFIER) {
            ltype = load_variable(identifier);

            if (ltype != INTSTAR_T)
                typeWarning(INTSTAR_T, ltype);

            getSymbol();

            // "*" identifier "="
            if (symbol == SYM_ASSIGN) {
                getSymbol();

                rtype = gr_expression();

                if (rtype != INT_T)
                    typeWarning(INT_T, rtype);

                emitIFormat(OP_SW, previousTemporary(), currentTemporary(), 0);

                tfree(2);
            } else
                syntaxErrorSymbol(SYM_ASSIGN);

            if (symbol == SYM_SEMICOLON)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_SEMICOLON);

        // "*" "(" expression ")"
        } else if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            ltype = gr_expression();

            if (ltype != INTSTAR_T)
                typeWarning(INTSTAR_T, ltype);

            if (symbol == SYM_RPARENTHESIS) {
                getSymbol();

                // "*" "(" expression ")" "="
                if (symbol == SYM_ASSIGN) {
                    getSymbol();

                    rtype = gr_expression();

                    if (rtype != INT_T)
                        typeWarning(INT_T, rtype);

                    emitIFormat(OP_SW, previousTemporary(), currentTemporary(), 0);

                    tfree(2);
                } else
                    syntaxErrorSymbol(SYM_ASSIGN);

                if (symbol == SYM_SEMICOLON)
                    getSymbol();
                else
                    syntaxErrorSymbol(SYM_SEMICOLON);
            } else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        } else
            syntaxErrorSymbol(SYM_LPARENTHESIS);
    }
    // identifier "=" expression | call
    else if (symbol == SYM_IDENTIFIER) {
        variableOrProcedureName = identifier;

        getSymbol();

        // call
        if (symbol == SYM_LPARENTHESIS) {
            getSymbol();

            gr_call(variableOrProcedureName);

            if (symbol == SYM_SEMICOLON)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_SEMICOLON);

        // identifier = expression
        } else if (symbol == SYM_ASSIGN) {
            entry = getVariable(variableOrProcedureName);

            ltype = getType(entry);

            getSymbol();

            rtype = gr_expression();

            if (ltype != rtype)
                typeWarning(ltype, rtype);

            emitIFormat(OP_SW, getRegister(entry), currentTemporary(), getData(entry));

            tfree(1);

            if (symbol == SYM_SEMICOLON)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_SEMICOLON);
        } else
            syntaxErrorUnexpected();
    }
    // while statement?
    else if (symbol == SYM_WHILE) {
        gr_while();
    }
    // if statement?
    else if (symbol == SYM_IF) {
        gr_if();
    }
    // return statement?
    else if (symbol == SYM_RETURN) {
        entry = getSymbolTableEntry(currentProcedureName, FUNCTION, global_symbol_table);

        gr_return(getType(entry));

        if (symbol == SYM_SEMICOLON)
            getSymbol();
        else
            syntaxErrorSymbol(SYM_SEMICOLON);
    }
}

int gr_type() {
    int type;

    type = INT_T;
    
    if (symbol == SYM_INT) {
        getSymbol();

        if (symbol == SYM_ASTERISK) {
            type = INTSTAR_T;

            getSymbol();
        }
    } else
        syntaxErrorSymbol(SYM_INT);

    return type;
}

void gr_variable(int offset) {
    int type;

    type = gr_type();

    if (symbol == SYM_IDENTIFIER) {
        createSymbolTableEntry(LOCAL_TABLE, identifier, offset, VARIABLE, type, 0);

        getSymbol();
    } else {
        syntaxErrorSymbol(SYM_IDENTIFIER);

        createSymbolTableEntry(LOCAL_TABLE, (int*) "missing variable name", offset, VARIABLE, type, 0);
    }
}

void gr_initialization(int *name, int offset, int type) {
    int hasCast;
    int cast;
    int sign;

    initialValue = 0;

    hasCast = 0;

    if (symbol == SYM_SEMICOLON)
        getSymbol();
    else if (symbol == SYM_ASSIGN) {
        getSymbol();

        // optional cast: [ cast ]
        if (symbol == SYM_LPARENTHESIS) {
            hasCast = 1;

            getSymbol();

            cast = gr_type();

            if (symbol == SYM_RPARENTHESIS)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        }

        // optional: -
        if (symbol == SYM_MINUS) {
            sign = 1;

            mayBeINTMINConstant = 1;
            isINTMINConstant    = 0;

            getSymbol();

            mayBeINTMINConstant = 0;

            if (isINTMINConstant) {
                isINTMINConstant = 0;
            
                // avoids 0-INT_MIN overflow when bootstrapping
                // even though 0-INT_MIN == INT_MIN
                sign = 0;
            }
        } else
            sign = 0;

        if (isConstant()) {
            initialValue = constant;

            getSymbol();

            if (sign == 1)
                initialValue = -initialValue;
        } else
            syntaxErrorUnexpected();

        if (symbol == SYM_SEMICOLON)
            getSymbol();
        else
            syntaxErrorSymbol(SYM_SEMICOLON);
    } else
        syntaxErrorUnexpected();

    if (hasCast) {
        if (type != cast)
            typeWarning(type, cast);
    } else if (type != INT_T)
        typeWarning(type, INT_T);

    createSymbolTableEntry(GLOBAL_TABLE, name, offset, VARIABLE, type, initialValue);
}

void gr_procedure(int *procedure, int returnType) {
    int numberOfParameters;
    int parameters;
    int localVariables;
    int functionStart;
    int *entry;

    currentProcedureName = procedure;

    numberOfParameters = 0;

    // ( variable , variable ) ;
    if (symbol == SYM_LPARENTHESIS) {
        getSymbol();

        if (symbol != SYM_RPARENTHESIS) {
            gr_variable(0);

            numberOfParameters = 1;

            while (symbol == SYM_COMMA) {
                getSymbol();

                gr_variable(0);

                numberOfParameters = numberOfParameters + 1;
            }

            entry = local_symbol_table;

            parameters = 0;

            while (parameters < numberOfParameters) {
                // 8 bytes offset to skip frame pointer and link
                setData(entry, parameters * 4 + 8);

                parameters = parameters + 1;
                entry      = getNext(entry);
            }

            if (symbol == SYM_RPARENTHESIS)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_RPARENTHESIS);
        } else
            getSymbol();
    } else
        syntaxErrorSymbol(SYM_LPARENTHESIS);

    if (symbol == SYM_SEMICOLON) {
        entry = getSymbolTableEntry(currentProcedureName, FUNCTION, global_symbol_table);

        if (entry == (int*) 0)
            createSymbolTableEntry(GLOBAL_TABLE, currentProcedureName, 0, FUNCTION, returnType, 0);

        getSymbol();

    // ( variable, variable ) { variable; variable; statement }
    } else if (symbol == SYM_LBRACE) {
        functionStart = binaryLength;
        
        getSymbol();

        entry = getSymbolTableEntry(currentProcedureName, FUNCTION, global_symbol_table);

        if (entry == (int*) 0)
            createSymbolTableEntry(GLOBAL_TABLE, currentProcedureName, binaryLength, FUNCTION, returnType, 0);
        else {
            if (getData(entry) != 0) {
                if (getOpcode(loadBinary(getData(entry))) == OP_JAL)
                    fixlink_absolute(getData(entry), functionStart);
                else {
                    printLineNumber((int*) "error");

                    print((int*) "multiple definitions of ");

                    print(currentProcedureName);

                    println();
                }
            }

            setData(entry, functionStart);

            if (getType(entry) != returnType)
                typeWarning(getType(entry), returnType);

            setType(entry, returnType);
        }

        localVariables = 0;

        while (symbol == SYM_INT) {
            localVariables = localVariables + 1;

            gr_variable(-4 * localVariables);

            if (symbol == SYM_SEMICOLON)
                getSymbol();
            else
                syntaxErrorSymbol(SYM_SEMICOLON);
        }

        help_procedure_prologue(localVariables);

        // create a fixup chain for return statements
        returnBranches = 0;

        while (isNotRbraceOrEOF())
            gr_statement();

        if (symbol == SYM_RBRACE)
            getSymbol();
        else {
            syntaxErrorSymbol(SYM_RBRACE);

            exit(-1);
        }

        fixlink_absolute(returnBranches, binaryLength);

        returnBranches = 0;

        help_procedure_epilogue(numberOfParameters);

    } else
        syntaxErrorUnexpected();

    local_symbol_table = (int*) 0;

    // assert: allocatedTemporaries == 0
}

void gr_cstar() {
    int type;
    int *variableOrProcedureName;

    while (symbol != SYM_EOF) {
        while (lookForType()) {
            syntaxErrorUnexpected();

            if (symbol == SYM_EOF)
                exit(-1);
            else
                getSymbol();
        }

        // void identifier procedure
        if (symbol == SYM_VOID) {
            type = VOID_T;

            getSymbol();

            if (symbol == SYM_IDENTIFIER) {
                variableOrProcedureName = identifier;

                getSymbol();

                gr_procedure(variableOrProcedureName, type);
            } else
                syntaxErrorSymbol(SYM_IDENTIFIER);
        } else {
            type = gr_type();

            if (symbol == SYM_IDENTIFIER) {
                variableOrProcedureName = identifier;

                getSymbol();

                // type identifier "(" procedure declaration or definition
                if (symbol == SYM_LPARENTHESIS)
                    gr_procedure(variableOrProcedureName, type);
                else {
                    allocatedMemory = allocatedMemory + 4;

                    // type identifier ";" global variable declaration
                    if (symbol == SYM_SEMICOLON) {
                        getSymbol();

                        createSymbolTableEntry(GLOBAL_TABLE, variableOrProcedureName, -allocatedMemory, VARIABLE, type, 0);

                    // type identifier "=" global variable definition
                    } else
                        gr_initialization(variableOrProcedureName, -allocatedMemory, type);
                }
            } else
                syntaxErrorSymbol(SYM_IDENTIFIER);
        }
    }
}

// -----------------------------------------------------------------
// ------------------------ MACHINE CODE LIBRARY -------------------
// -----------------------------------------------------------------

void emitLeftShiftBy(int b) {
    // assert: 0 <= b < 15

    // load multiplication factor less than 2^15 to avoid sign extension
    emitIFormat(OP_ADDIU, REG_ZR, nextTemporary(), twoToThePowerOf(b));
    emitRFormat(OP_SPECIAL, currentTemporary(), nextTemporary(), 0, FCT_MULTU);
    emitRFormat(OP_SPECIAL, 0, 0, currentTemporary(), FCT_MFLO);
}

void emitMainEntry() {
    // instruction at address zero cannot be fixed up
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP);

    createSymbolTableEntry(GLOBAL_TABLE, (int*) "main", binaryLength, FUNCTION, INT_T, 0);

    mainJumpAddress = binaryLength;

    emitJFormat(OP_JAL, 0);
}

// -----------------------------------------------------------------
// ----------------------------- MAIN ------------------------------
// -----------------------------------------------------------------

void compile() {
    print(selfieName);
    print((int*) ": this is selfie's cstarc compiling ");
    print(sourceName);
    println();

    sourceFD = open(sourceName, 0, 0); // 0 = O_RDONLY

    if (sourceFD < 0) {
        print(selfieName);
        print((int*) ": could not open input file ");
        print(sourceName);
        println();

        exit(-1);
    }

    // reset scanner
    resetScanner();

    // reset global symbol table
    resetGlobalSymbolTable();

    // allocate space for storing binary
    binary       = malloc(maxBinaryLength);
    binaryLength = 0;

    // jump to main
    emitMainEntry();

    // library:
    // exit must be first to exit main
    // if exit call in main is missing
    emitExit();
    emitRead();
    emitWrite();
    emitOpen();
    emitMalloc();
    emitPutchar();
    // A2
    emitSchedYield();
    // A3
    emitSchedSetParam();
    emitSelect();

    // parser
    gr_cstar();

    // emit global variables and strings
    emitGlobalsStrings();

    if (getInstrIndex(loadBinary(mainJumpAddress)) == 0) {
        print(selfieName);
        print((int*) ": main function missing in ");
        print(sourceName);
        println();

        exit(-1);
    }
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// -------------------     I N T E R F A C E     -------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- REGISTER ---------------------------
// -----------------------------------------------------------------

void printRegister(int reg) {
    print((int*) *(REGISTERS + reg));
}

// -----------------------------------------------------------------
// ---------------------------- ENCODER ----------------------------
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// 32 bit
//
// +------+-----+-----+-----+-----+------+
// |opcode|  rs |  rt |  rd |00000|fction|
// +------+-----+-----+-----+-----+------+
//    6      5     5     5     5     6
int encodeRFormat(int opcode, int rs, int rt, int rd, int function) {
    // assert: 0 <= opcode < 2^6
    // assert: 0 <= rs < 2^5
    // assert: 0 <= rt < 2^5
    // assert: 0 <= rd < 2^5
    // assert: 0 <= function < 2^6
    return leftShift(leftShift(leftShift(leftShift(opcode, 5) + rs, 5) + rt, 5) + rd, 11) + function;
}

// -----------------------------------------------------------------
// 32 bit
//
// +------+-----+-----+----------------+
// |opcode|  rs |  rt |    immediate   |
// +------+-----+-----+----------------+
//    6      5     5          16
int encodeIFormat(int opcode, int rs, int rt, int immediate) {
    // assert: 0 <= opcode < 2^6
    // assert: 0 <= rs < 2^5
    // assert: 0 <= rt < 2^5
    // assert: -2^15 <= immediate < 2^15
    if (immediate < 0)
        // convert from 32-bit to 16-bit two's complement
        immediate = immediate + twoToThePowerOf(16);

    return leftShift(leftShift(leftShift(opcode, 5) + rs, 5) + rt, 16) + immediate;
}

// --------------------------------------------------------------
// 32 bit
//
// +------+--------------------------+
// |opcode|       instr_index        |
// +------+--------------------------+
//    6                26
int encodeJFormat(int opcode, int instr_index) {
    // assert: 0 <= opcode < 2^6
    // assert: 0 <= immediate < 2^26
    return leftShift(opcode, 26) + instr_index;
}

int getOpcode(int instruction) {
    return rightShift(instruction, 26);
}

int getRS(int instruction) {
    return rightShift(leftShift(instruction, 6), 27);
}

int getRT(int instruction) {
    return rightShift(leftShift(instruction, 11), 27);
}

int getRD(int instruction) {
    return rightShift(leftShift(instruction, 16), 27);
}

int getFunction(int instruction) {
    return rightShift(leftShift(instruction, 26), 26);
}

int getImmediate(int instruction) {
    return rightShift(leftShift(instruction, 16), 16);
}

int getInstrIndex(int instruction) {
    return rightShift(leftShift(instruction, 6), 6);
}

int signExtend(int immediate) {
    // sign-extend from 16-bit to 32-bit two's complement
    if (immediate < twoToThePowerOf(15))
        return immediate;
    else
        return immediate - twoToThePowerOf(16);
}

// -----------------------------------------------------------------
// ---------------------------- DECODER ----------------------------
// -----------------------------------------------------------------

void printOpcode(int opcode) {
    print((int*) *(OPCODES + opcode));
}

void printFunction(int function) {
    print((int*) *(FUNCTIONS + function));
}

void decode() {
    opcode = getOpcode(ir);

    if (opcode == 0)
        decodeRFormat();
    else if (opcode == OP_JAL)
        decodeJFormat();
    else if (opcode == OP_J)
        decodeJFormat();
    else
        decodeIFormat();
}

// --------------------------------------------------------------
// 32 bit
//
// +------+-----+-----+-----+-----+------+
// |opcode|  rs |  rt |  rd |00000|fction|
// +------+-----+-----+-----+-----+------+
//    6      5     5     5     5     6
void decodeRFormat() {
    rs          = getRS(ir);
    rt          = getRT(ir);
    rd          = getRD(ir);
    immediate   = 0;
    function    = getFunction(ir);
    instr_index = 0;
}

// --------------------------------------------------------------
// 32 bit
//
// +------+-----+-----+----------------+
// |opcode|  rs |  rt |    immediate   |
// +------+-----+-----+----------------+
//    6      5     5          16
void decodeIFormat() {
    rs          = getRS(ir);
    rt          = getRT(ir);
    rd          = 0;
    immediate   = getImmediate(ir);
    function    = 0;
    instr_index = 0;
}

// --------------------------------------------------------------
// 32 bit
//
// +------+--------------------------+
// |opcode|       instr_index        |
// +------+--------------------------+
//    6                26
void decodeJFormat() {
    rs          = 0;
    rt          = 0;
    rd          = 0;
    immediate   = 0;
    function    = 0;
    instr_index = getInstrIndex(ir);
}

// -----------------------------------------------------------------
// ---------------------------- BINARY -----------------------------
// -----------------------------------------------------------------

int loadBinary(int addr) {
    return *(binary + addr / 4);
}

void storeBinary(int addr, int instruction) {
    *(binary + addr / 4) = instruction;
}

void emitInstruction(int instruction) {
    if (binaryLength >= maxBinaryLength) {
        syntaxErrorMessage((int*) "exceeded maximum binary length");
        exit(-1);
    } else {
        storeBinary(binaryLength, instruction);
        
        binaryLength = binaryLength + 4;
    }
}

void emitRFormat(int opcode, int rs, int rt, int rd, int function) {
    emitInstruction(encodeRFormat(opcode, rs, rt, rd, function));

    if (opcode == OP_SPECIAL) {
        if (function == FCT_JR)
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // delay slot
        else if (function == FCT_MFLO) {
            // In MIPS I-III two instructions after MFLO/MFHI
            // must not modify the LO/HI registers
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // pipeline delay
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // pipeline delay
        } else if (function == FCT_MFHI) {
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // pipeline delay
            emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // pipeline delay
        }
    }
}

void emitIFormat(int opcode, int rs, int rt, int immediate) {
    emitInstruction(encodeIFormat(opcode, rs, rt, immediate));

    if (opcode == OP_BEQ)
        emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // delay slot
    else if (opcode == OP_BNE)
        emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // delay slot
}

void emitJFormat(int opcode, int instr_index) {
    emitInstruction(encodeJFormat(opcode, instr_index));

    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_NOP); // delay slot
}

void fixup_relative(int fromAddress) {
    int instruction;

    instruction = loadBinary(fromAddress);

    storeBinary(fromAddress,
        encodeIFormat(getOpcode(instruction),
            getRS(instruction),
            getRT(instruction),
            (binaryLength - fromAddress - 4) / 4));
}

void fixup_absolute(int fromAddress, int toAddress) {
    storeBinary(fromAddress,
        encodeJFormat(getOpcode(loadBinary(fromAddress)), toAddress / 4));
}

void fixlink_absolute(int fromAddress, int toAddress) {
    int previousAddress;

    while (fromAddress != 0) {
        previousAddress = getInstrIndex(loadBinary(fromAddress)) * 4;
        fixup_absolute(fromAddress, toAddress);
        fromAddress = previousAddress;
    }
}

int copyStringToBinary(int *s, int a) {
    int l;
    int w;

    l = stringLength(s) + 1;

    w = a + l;

    if (l % 4 != 0)
        w = w + 4 - l % 4;

    while (a < w) {
        storeBinary(a, *s);

        s = s + 1;
        a = a + 4;
    }

    return w;
}

void emitGlobalsStrings() {
    int *entry;

    entry = global_symbol_table;

    // assert: n = binaryLength

    // allocate space for global variables and copy strings
    while ((int) entry != 0) {
        if (getClass(entry) == VARIABLE) {
            storeBinary(binaryLength, getValue(entry));

            binaryLength = binaryLength + 4;
        } else if (getClass(entry) == STRING)
            binaryLength = copyStringToBinary(getString(entry), binaryLength);

        entry = getNext(entry);
    }

    // assert: binaryLength == n + allocatedMemory

    allocatedMemory = 0;
}

void emit() {
    int fd;

    // 1537 = 0x0601 = O_CREAT (0x0200) | O_WRONLY (0x0001) | O_TRUNC (0x0400)
    // 420 = 00644 = S_IRUSR (00400) | S_IWUSR (00200) | S_IRGRP (00040) | S_IROTH (00004)
    fd = open(binaryName, 1537, 420);

    if (fd < 0) {
        print(selfieName);
        print((int*) ": could not create output file ");
        print(binaryName);
        println();

        exit(-1);
    }

    print(selfieName);
    print((int*) ": writing code into output file ");
    print(binaryName);
    println();

    write(fd, binary, binaryLength);
}

void load() {
    int fd;
    int numberOfReadBytes;

    fd = open(binaryName, 0, 0); // 0 = O_RDONLY

    if (fd < 0) {
        print(selfieName);
        print((int*) ": could not open input file ");
        print(binaryName);
        println();

        exit(-1);
    }

    binary       = malloc(maxBinaryLength);
    binaryLength = 0;

    numberOfReadBytes = 4;

    print(selfieName);
    print((int*) ": loading code from input file ");
    print(binaryName);
    println();

    while (numberOfReadBytes == 4) {
        numberOfReadBytes = read(fd, binary + binaryLength / 4, 4);

        if (debug_load) {
            print(binaryName);
            print((int*) ": ");
            print(itoa(binaryLength, string_buffer, 16, 8));
            print((int*) ": ");
            print(itoa(loadBinary(binaryLength), string_buffer, 16, 8));
            println();
        }

        if (numberOfReadBytes == 4)
            binaryLength = binaryLength + 4;
    }
}

// -----------------------------------------------------------------
// --------------------------- SYSCALLS ----------------------------
// -----------------------------------------------------------------

void emitExit() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "exit", binaryLength, FUNCTION, INT_T, 0);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A1, 0);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0); // exit code
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_EXIT);
    emitRFormat(0, 0, 0, 0, FCT_SYSCALL);
}

void syscall_exit() {
    int exitCode;

    exitCode = *(registers+REG_A0);

    *(registers+REG_V0) = exitCode;

    print(binaryName);
    print((int*) ": exiting with error code ");
    print(itoa(exitCode, string_buffer, 10, 0));
    println();

    exit(0);
}

void emitRead() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "read", binaryLength, FUNCTION, INT_T, 0);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);

    emitIFormat(OP_LW, REG_SP, REG_A2, 0); // count
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A1, 0); // *buffer
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0); // fd
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_READ);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitRFormat(OP_SPECIAL, REG_RA, 0, 0, FCT_JR);
}

void syscall_read() {
    int count;
    int vaddr;
    int fd;
    int *buffer;
    int size;

    count = *(registers+REG_A2);
    vaddr = *(registers+REG_A1);
    fd    = *(registers+REG_A0);

    buffer = memory + tlb(vaddr);

    size = read(fd, buffer, count);

    *(registers+REG_V0) = size;

    if (debug_read) {
        print(binaryName);
        print((int*) ": read ");
        print(itoa(size, string_buffer, 10, 0));
        print((int*) " bytes from file with descriptor ");
        print(itoa(fd, string_buffer, 10, 0));
        print((int*) " into buffer at address ");
        print(itoa((int) buffer, string_buffer, 16, 8));
        println();
    }
}

void emitWrite() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "write", binaryLength, FUNCTION, INT_T, 0);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);

    emitIFormat(OP_LW, REG_SP, REG_A2, 0); // size
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A1, 0); // *buffer
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0); // fd
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_WRITE);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitRFormat(OP_SPECIAL, REG_RA, 0, 0, FCT_JR);
}

void syscall_write() {
    int size;
    int vaddr;
    int fd;
    int *buffer;

    size  = *(registers+REG_A2);
    vaddr = *(registers+REG_A1);
    fd    = *(registers+REG_A0);

    buffer = memory + tlb(vaddr);

    size = write(fd, buffer, size);

    *(registers+REG_V0) = size;

    if (debug_write) {
        print(binaryName);
        print((int*) ": wrote ");
        print(itoa(size, string_buffer, 10, 0));
        print((int*) " bytes from buffer at address ");
        print(itoa((int) buffer, string_buffer, 16, 8));
        print((int*) " into file with descriptor ");
        print(itoa(fd, string_buffer, 10, 0));
        println();
    }
}

void emitOpen() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "open", binaryLength, FUNCTION, INT_T, 0);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);

    emitIFormat(OP_ADDIU, REG_SP, REG_A2, 0); // mode
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A1, 0); // flags
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0); // filename
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_OPEN);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitRFormat(OP_SPECIAL, REG_RA, 0, 0, FCT_JR);
}

void syscall_open() {
    int mode;
    int flags;
    int vaddr;
    int *filename;
    int fd;

    mode  = *(registers+REG_A2);
    flags = *(registers+REG_A1);
    vaddr = *(registers+REG_A0);

    filename = memory + tlb(vaddr);

    fd = open(filename, flags, mode);

    *(registers+REG_V0) = fd;

    if (debug_open) {
        print(binaryName);
        print((int*) ": opened file ");
        printString(filename);
        print((int*) " with flags ");
        print(itoa(flags, string_buffer, 16, 0));
        print((int*) " and mode ");
        print(itoa(mode, string_buffer, 8, 0));
        print((int*) " returning file descriptor ");
        print(itoa(fd, string_buffer, 10, 0));
        println();
    }
}

void emitMalloc() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "malloc", binaryLength, FUNCTION, INTSTAR_T, 0);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 0);
    emitIFormat(OP_ADDIU, REG_ZR, REG_A1, 0);

    // load argument for malloc (size)
    emitIFormat(OP_LW, REG_SP, REG_A0, 0);

    // remove the argument from the stack
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    // load the correct syscall number and invoke syscall
    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_MALLOC);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    // jump back to caller, return value is in REG_V0
    emitRFormat(OP_SPECIAL, REG_RA, 0, 0, FCT_JR);
}

void syscall_malloc() {
    int size;
    int bump;

    size = *(registers+REG_A0);

    if (size % 4 != 0)
        size = size + 4 - size % 4;

    bump = *(registers+REG_K1);

    if (bump + size >= *(registers+REG_SP))
        exception_handler(EXCEPTION_HEAPOVERFLOW);

    *(registers+REG_K1) = bump + size;
    *(registers+REG_V0) = bump;

    if (debug_malloc) {
        print(binaryName);
        print((int*) ": malloc ");
        print(itoa(size, string_buffer, 10, 0));
        print((int*) " bytes returning address ");
        print(itoa(bump, string_buffer, 16, 8));
        println();
    }
}

void emitPutchar() {
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "putchar", binaryLength, FUNCTION, INT_T, 0);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 4); // write one integer, 4 bytes

    emitIFormat(OP_ADDIU, REG_SP, REG_A1, 0); // pointer to character
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_A0, 1); // stdout file descriptor

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_WRITE);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitRFormat(OP_SPECIAL, REG_RA, 0, 0, FCT_JR);
}

// -----------------------------------------------------------------
// -------------------------- ASSIGNMENT2 --------------------------
// -----------------------------------------------------------------

void emitSchedYield() {
    // Create the symbol table entry fpr sched_yield
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "sched_yield", binaryLength, FUNCTION, INT_T, 0);

    // sched_yield doesn't have any arguments
    // emitIFormat(OP_ADDIU, REG_ZR, REG_A3, 0);
    // emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 0);
    // emitIFormat(OP_ADDIU, REG_ZR, REG_A1, 0);

    // emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_SCHED_YIELD);
    // emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    emitIFormat(OP_LW, REG_SP, REG_A3, 0);
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A2, 0);
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A1, 0);
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0);
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_SCHED_YIELD);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    // We don't have a return value so we just jump back
    emitRFormat(OP_SPECIAL, REG_RA, 0, 0, FCT_JR); // REG_RA instead of REG_LINK
}

void syscall_sched_yield() {
    //int *process;
    int vaddr;
    int *filename;
    int arg2;
    int arg3;
    int arg4;

    // We need to increase the pc before the actual switch
    pc = pc + 4;
    //process = process_schedule();
    //process_switch(process);
    // Reset ticks
    // TODO: Next process gets one tick too few
    //g_ticks = 0;

    // vaddr = *(registers+REG_A0);
    // filename = memory + tlb(vaddr);
    // print((int*)filename);
    // println();

    // arg2 = *(registers + REG_A1);
    // printInt(arg2);
    // println();

    // arg3 = *(registers + REG_A2);
    // printInt(arg3);
    // println();

    // arg4 = *(registers + REG_A3);
    // printInt(arg4);
    // println();

    //if (DEBUG_1) {
        print((int*)"call sched_yield");
        println();
    //}
}

// -----------------------------------------------------------------
// -------------------------- ASSIGNMENT3 --------------------------
// -----------------------------------------------------------------

// PID 
void emitSchedSetParam() {
    // Create the symbol table entry fpr sched_yield
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "sched_setparam", binaryLength, FUNCTION, INT_T, 0);

    emitIFormat(OP_LW, REG_SP, REG_A2, 0);
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4); // File name

    emitIFormat(OP_LW, REG_SP, REG_A1, 0);
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4); // Segment Size

    emitIFormat(OP_LW, REG_SP, REG_A0, 0);
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4); // Process ID

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_SCHED_SETPARAM);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    // We don't have a return value so we just jump back
    emitRFormat(OP_SPECIAL, REG_RA, 0, 0, FCT_JR); // REG_RA instead of REG_LINK
}

void syscall_sched_setparam() {
    int pid;
    int segment_size;
    int vaddr;

    int *process;
    int *new_registers;

    print((int*)"--------------------------");
    println();
    print((int*)"System Call sched_setparam");
    println();

    pid = *(registers + REG_A0);
    segment_size = *(registers + REG_A1);
    vaddr = *(registers + REG_A2);
    binaryName = memory + tlb(vaddr);

    print((int*)"PID: ");
    printInt(pid);
    println();

    print((int*)"Segment Size: ");
    printInt(segment_size);
    println();

    print((int*)"Filename: ");
    print((int*)binaryName);
    println();

    load(); // Load the new file
    g_segment_offset = 2 * 1024 * 1024;
    copyBinaryToMemory();

    resetInterpreter();

    new_registers = memcpy(registers, 32); // 32 registers

    *(new_registers + REG_SP) = segment_size + g_segment_offset;
    *(new_registers + REG_GP) = binaryLength + g_segment_offset;
    *(new_registers + REG_K1) = *(new_registers + REG_GP);

    print((int*)"Stack pointer: ");
    printInt(*(new_registers+REG_SP));
    println();

    print((int*)"Global pointer: ");
    printInt(*(new_registers+REG_GP));
    println();

    print((int*)"Binary Length: ");
    printInt(binaryLength);
    println();

    process = process_allocate();
    process_set_id(process, pid);
    process_set_pc(process, 0);
    process_set_registers(process, new_registers);
    process_set_reg_hi(process, 0);
    process_set_segment_id(process, g_segment_counter);

    g_segment_counter = g_segment_counter + 1;

    print((int*)"--------------------------");
    println();

    up_copyArguments(0, (int*)0); // We can't handle arguments for now

    print((int*)"Starting process");
    println();

    registers = process_get_registers(process);
    pc = process_get_pc(process);
    reg_hi = process_get_reg_hi(process);
    reg_lo = process_get_reg_lo(process);

    g_segment_offset = 0;
}

void emitSelect() {
    // Create the symbol table entry fpr sched_yield
    createSymbolTableEntry(GLOBAL_TABLE, (int*) "select", binaryLength, FUNCTION, INT_T, 0);

    // sched_yield doesn't have any arguments
    emitIFormat(OP_ADDIU, REG_ZR, REG_A2, 0);
    
    emitIFormat(OP_LW, REG_SP, REG_A1, 0);
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_LW, REG_SP, REG_A0, 0);
    emitIFormat(OP_ADDIU, REG_SP, REG_SP, 4);

    emitIFormat(OP_ADDIU, REG_ZR, REG_V0, SYSCALL_SELECT);
    emitRFormat(OP_SPECIAL, 0, 0, 0, FCT_SYSCALL);

    // We don't have a return value so we just jump back
    emitRFormat(OP_SPECIAL, REG_RA, 0, 0, FCT_JR); // REG_RA instead of REG_LINK
}

void syscall_select() {
    int prev_pid;
    int next_pid;

    prev_pid = *(registers + REG_A0);
    next_pid = *(registers + REG_A1);

    print((int*)"--------------------------");
    println();
    print((int*)"System Call select");
    println();

    print((int*)"Previos process id: ");
    printInt(prev_pid);
    println();

    print((int*)"Next process id: ");
    printInt(next_pid);
    println();
    print((int*)"--------------------------");
    println();
}


// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------------     kOS   ---------------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

void kernel(int argc, int* argv) {

    binaryName = (int*) *argv;
    sched_setparam(1, 4*1024 * 1024, binaryName);
    select(12, 32);

    argc = argc - 1;
    argv = argv + 1;

    // pass binaryName as first argument replacing size
    *argv = (int) binaryName;
    
    print((int*)"kernel done");
    println();

    //emulate(argc, argv);
}

// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~
// -----------------------------------------------------------------
// ---------------------     E M U L A T O R   ---------------------
// -----------------------------------------------------------------
// *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~ *~*~

// -----------------------------------------------------------------
// ---------------------------- MEMORY -----------------------------
// -----------------------------------------------------------------

int tlb(int vaddr) {
    // int segment_id;
    // int offset;

    if (vaddr % 4 != 0)
        exception_handler(EXCEPTION_ADDRESSERROR);

    vaddr = vaddr + g_segment_offset;
    // Get the right offset
    // if(g_segmentation_active) {
    //     segment_id = process_get_segment_id(g_running_process);
    //     offset = segment_get_start(struct_get_element_at(g_segment_table, segment_id));
    //     vaddr = vaddr + offset;
    // }

    // physical memory is word-addressed for lack of byte-sized data type
    return vaddr / 4;
}

int loadMemory(int vaddr) {
    return *(memory + tlb(vaddr));
}

void storeMemory(int vaddr, int data) {
    *(memory + tlb(vaddr)) = data;
}

// -----------------------------------------------------------------
// ------------------------- INSTRUCTIONS --------------------------
// -----------------------------------------------------------------

void fct_syscall() {
    if (debug_disassemble) {
        printFunction(function);
        println();
    }

    if (*(registers+REG_V0) == SYSCALL_EXIT) {
        syscall_exit();
        pc = pc + 4;
    } else if (*(registers+REG_V0) == SYSCALL_READ) {
        syscall_read();
        pc = pc + 4;
    } else if (*(registers+REG_V0) == SYSCALL_WRITE) {
        syscall_write();
        pc = pc + 4;
    } else if (*(registers+REG_V0) == SYSCALL_OPEN) {
        syscall_open();
        pc = pc + 4;
    } else if (*(registers+REG_V0) == SYSCALL_MALLOC) {
        syscall_malloc();
        pc = pc + 4;
        // A2
    } else if (*(registers+REG_V0) == SYSCALL_SCHED_YIELD) {
        // SYSCALL_SCHED_YIELDs pc gets increased before it's execution
        syscall_sched_yield();
    } else if (*(registers+REG_V0) == SYSCALL_SCHED_SETPARAM) {
        // SYSCALL_SCHED_YIELDs pc gets increased before it's execution
        syscall_sched_setparam();
        pc = pc + 4;
    } else if (*(registers+REG_V0) == SYSCALL_SELECT) {
        // SYSCALL_SCHED_YIELDs pc gets increased before it's execution
        syscall_select();
        pc = pc + 4;
    } else {
        exception_handler(EXCEPTION_UNKNOWNSYSCALL);
    }
}

void fct_nop() {
    pc = pc + 4;

    if (debug_disassemble) {
        printFunction(function);
        println();
    }
}

void op_jal() {
    *(registers+REG_RA) = pc + 8;

    pc = instr_index * 4;

    // TODO: execute delay slot

    if (debug_disassemble) {
        printOpcode(opcode);
        print((int*) " ");
        print(itoa(instr_index, string_buffer, 16, 8));
        println();
    }
}

void op_j() {
    pc = instr_index * 4;

    // TODO: execute delay slot

    if (debug_disassemble) {
        printOpcode(opcode);
        print((int*) " ");
        print(itoa(instr_index, string_buffer, 16, 8));
        println();
    }
}

void op_beq() {
    pc = pc + 4;

    if (*(registers+rs) == *(registers+rt)) {
        pc = pc + signExtend(immediate) * 4;

        // TODO: execute delay slot
    }

    if (debug_disassemble) {
        printOpcode(opcode);
        print((int*) " ");
        printRegister(rs);
        putchar(',');
        printRegister(rt);
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));
        println();
    }
}

void op_bne() {
    pc = pc + 4;

    if (*(registers+rs) != *(registers+rt)) {
        pc = pc + signExtend(immediate) * 4;

        // TODO: execute delay slot
    }

    if (debug_disassemble) {
        printOpcode(opcode);
        print((int*) " ");
        printRegister(rs);
        putchar(',');
        printRegister(rt);
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));
        println();
    }
}

void op_addiu() {
    *(registers+rt) = *(registers+rs) + signExtend(immediate);

    // TODO: check for overflow

    pc = pc + 4;

    if (debug_disassemble) {
        printOpcode(opcode);
        print((int*) " ");
        printRegister(rt);
        putchar(',');
        printRegister(rs);
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));
        println();
    }
}

void fct_jr() {
    pc = *(registers+rs);

    if (debug_disassemble) {
        printFunction(function);
        print((int*) " ");
        printRegister(rs);
        println();
    }
}

void op_lui() {
    *(registers+rt) = leftShift(immediate, 16);

    pc = pc + 4;

    if (debug_disassemble) {
        printOpcode(opcode);
        print((int*) " ");
        printRegister(rt);
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));
        println();
    }
}

void fct_mfhi() {
    *(registers+rd) = reg_hi;

    pc = pc + 4;

    if (debug_disassemble) {
        printFunction(function);
        print((int*) " ");
        printRegister(rd);
        println();
    }
}

void fct_mflo() {
    *(registers+rd) = reg_lo;

    pc = pc + 4;

    if (debug_disassemble) {
        printFunction(function);
        print((int*) " ");
        printRegister(rd);
        println();
    }
}

void fct_multu() {
    // TODO: 64-bit resolution currently not supported
    reg_lo = *(registers+rs) * *(registers+rt);

    pc = pc + 4;

    if (debug_disassemble) {
        printFunction(function);
        print((int*) " ");
        printRegister(rs);
        putchar(',');
        printRegister(rt);
        println();
    }
}

void fct_divu() {
    reg_lo = *(registers+rs) / *(registers+rt);
    reg_hi = *(registers+rs) % *(registers+rt);

    pc = pc + 4;

    if (debug_disassemble) {
        printFunction(function);
        print((int*) " ");
        printRegister(rs);
        putchar(',');
        printRegister(rt);
        println();
    }
}

void fct_addu() {
    *(registers+rd) = *(registers+rs) + *(registers+rt);

    pc = pc + 4;

    if (debug_disassemble) {
        printFunction(function);
        print((int*) " ");
        printRegister(rd);
        putchar(',');
        printRegister(rs);
        putchar(',');
        printRegister(rt);
        println();
    }
}

void fct_subu() {
    *(registers+rd) = *(registers+rs) - *(registers+rt);

    pc = pc + 4;

    if (debug_disassemble) {
        printFunction(function);
        print((int*) " ");
        printRegister(rd);
        putchar(',');
        printRegister(rs);
        putchar(',');
        printRegister(rt);
        println();
    }
}

void op_lw() {
    int vaddr;

    vaddr = *(registers+rs) + signExtend(immediate);

    *(registers+rt) = loadMemory(vaddr);

    pc = pc + 4;

    if (debug_disassemble) {
        printOpcode(opcode);
        print((int*) " ");
        printRegister(rt);
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));
        putchar('(');
        printRegister(rs);
        putchar(')');
        println();
    }
}

void fct_slt() {
    if (*(registers+rs) < *(registers+rt))
        *(registers+rd) = 1;
    else
        *(registers+rd) = 0;

    pc = pc + 4;

    if (debug_disassemble) {
        printFunction(function);
        print((int*) " ");
        printRegister(rd);
        putchar(',');
        printRegister(rs);
        putchar(',');
        printRegister(rt);
        println();
    }
}

void op_sw() {
    int vaddr;

    vaddr = *(registers+rs) + signExtend(immediate);

    storeMemory(vaddr, *(registers+rt));

    pc = pc + 4;

    if (debug_disassemble) {
        printOpcode(opcode);
        print((int*) " ");
        printRegister(rt);
        putchar(',');
        print(itoa(signExtend(immediate), string_buffer, 10, 0));
        putchar('(');
        printRegister(rs);
        putchar(')');
        println();
    }
}

void fct_teq() {
    if (*(registers+rs) == *(registers+rt))
        exception_handler(EXCEPTION_SIGNAL);

    pc = pc + 4;

    if (debug_disassemble) {
        printFunction(function);
        print((int*) " ");
        printRegister(rs);
        putchar(',');
        printRegister(rt);
        println();
    }
}

// -----------------------------------------------------------------
// -------------------------- INTERPRETER --------------------------
// -----------------------------------------------------------------

void printException(int enumber) {
    print((int*) *(EXCEPTIONS + enumber));
}

void exception_handler(int enumber) {
    print(binaryName);
    print((int*) ": exception: ");
    printException(enumber);
    println();

    exit(enumber);
}

void pre_debug() {
    if (debug_disassemble) {
        print(binaryName);
        print((int*) ": $pc=");
        print(itoa(pc, string_buffer, 16, 8));
        print((int*) ": ");
    }
}

void post_debug() {
    int i;
    if (debug_registers) {
        i = 0;

        while (i < 32) {
            if (*(registers+i) != 0) {
                print(binaryName);
                print((int*) ": ");
                printRegister(i);
                putchar(CHAR_EQUAL);
                print(itoa(*(registers+i), string_buffer, 16, 8));
                println();
            }
            i = i + 1;
        }
        println();
    }
}

void fetch() {
    ir = loadMemory(pc);
}

void execute() {

    if(DEBUG_1) {
        printInt(g_ticks);
        print((int*)" || ");
        printInt(pc);
        print((int*)" || ");
        printInt((int)registers);
        print((int*)" || ");
        printInt((int)ir);
        println();
    }

    if (opcode == OP_SPECIAL) {
        if (function == FCT_NOP) {
            fct_nop();
        } else if (function == FCT_ADDU) {
            fct_addu();
        } else if (function == FCT_SUBU) {
            fct_subu();
        } else if (function == FCT_MULTU) {
            fct_multu();
        } else if (function == FCT_DIVU) {
            fct_divu();
        } else if (function == FCT_MFHI) {
            fct_mfhi();
        } else if (function == FCT_MFLO) {
            fct_mflo();
        } else if (function == FCT_SLT) {
            fct_slt();
        } else if (function == FCT_JR) {
            fct_jr();
        } else if (function == FCT_SYSCALL) {
            fct_syscall();
        } else if (function == FCT_TEQ) {
            fct_teq();
        } else {
            exception_handler(EXCEPTION_UNKNOWNINSTRUCTION);
        }
    } else if (opcode == OP_ADDIU) {
        op_addiu();
    } else if (opcode == OP_LW) {
        op_lw();
    } else if (opcode == OP_SW) {
        op_sw();
    } else if (opcode == OP_BEQ) {
        op_beq();
    } else if (opcode == OP_BNE) {
        op_bne();
    } else if (opcode == OP_JAL) {
        op_jal();
    } else if (opcode == OP_J) {
        op_j();
    } else {
        exception_handler(EXCEPTION_UNKNOWNINSTRUCTION);
    }
}

void run() {
    int *process;

    while (1) {
        fetch();
        decode();
        pre_debug();
        execute();
        post_debug();

        // A1: Every process is allowed to run TIME_SLICE many ticks
        g_ticks = g_ticks + 1;
        if(g_ticks == TIME_SLICE) {
            process = process_schedule();
            process_switch(process);
            g_ticks = 0;
        } 
    }
}

void up_push(int value) {
    int vaddr;

    // allocate space for one value on the stack
    *(registers+REG_SP) = *(registers+REG_SP) - 4;

    // compute address
    vaddr = *(registers+REG_SP);

    // store value
    storeMemory(vaddr, value);
}

int up_malloc(int size) {
    *(registers+REG_A0) = size;

    syscall_malloc();

    return *(registers+REG_V0);
}

int up_copyString(int *s) {
    int l;
    int a;
    int w;
    int t;

    l = stringLength(s) + 1;

    a = up_malloc(l);

    w = a + l;

    if (l % 4 != 0)
        w = w + 4 - l % 4;

    t = a;

    while (a < w) {
        storeMemory(a, *s);

        s = s + 1;
        a = a + 4;
    }

    return t;
}

void up_copyArguments(int argc, int *argv) {
    int vaddr;

    up_push(argc);

    vaddr = up_malloc(argc * 4);

    up_push(vaddr);

    while (argc > 0) {
        storeMemory(vaddr, up_copyString((int*) *argv));

        vaddr = vaddr + 4;

        argv = argv + 1;
        argc = argc - 1;
    }
}

void copyBinaryToMemory() {
    int a;

    a = 0;

    while (a < binaryLength) {
        storeMemory(a, loadBinary(a));

        a = a + 4;
    }
}

void emulate(int argc, int *argv) {
    print(selfieName);
    print((int*) ": this is selfie's mipster executing ");
    print(binaryName);
    print((int*) " with ");
    print(itoa(memorySize / 1024 / 1024, string_buffer, 10, 0));
    print((int*) "MB of memory");
    println();

    copyBinaryToMemory();

    resetInterpreter();

    print((int*)"--------------------------");
    println();

    print((int*)"Loading first process into: ");
    printInt((int)memory);
    println();

    //*(registers+REG_SP) = memorySize - 4;
    *(registers+REG_SP) = 2 * 1024 * 1024; // The kernel gets 2 MB of memory
    *(registers+REG_GP) = binaryLength;
    *(registers+REG_K1) = *(registers+REG_GP);

    g_next_segment = 2 * 1024 * 1024;

    print((int*)"Initial stack pointer: ");
    printInt(*(registers+REG_SP));
    println();

    print((int*)"Initial global pointer: ");
    printInt(*(registers+REG_GP));
    println();

    g_segment_counter = g_segment_counter + 1;

    print((int*)"Segment counter: ");
    printInt(g_segment_counter);
    println();

    print((int*)"--------------------------");
    println();

    up_copyArguments(argc, argv);
    // A2
    // Call INIT_SEGMENTATION _before_ calling INIT_READYQUEUE!
    //init_segmentation();
    //init_readyqueue(argc, argv);

    run();
}

// -----------------------------------------------------------------
// -------------------------- ASSIGNMENT2 --------------------------
// -----------------------------------------------------------------

void process_get_segment(int *process, int segment_size, int argc, int *argv) {
    int *segment;
    int *new_registers;

    // Insert the newly allocated segment into the segment table
    segment = segment_init(g_segment_offset, segment_size);
    struct_set_element_at(g_segment_table, g_segment_counter, segment);
    // The process only remembers his segment id
    process_set_segment_id(process, g_segment_counter);
    // We just added a segment -> increase the segment counter
    g_segment_counter = g_segment_counter + 1;
    g_segment_offset = g_segment_offset + segment_size;

    g_running_process = process;

    copyBinaryToMemory();

    resetInterpreter();

    *(registers+REG_SP) = segment_size;
    *(registers+REG_GP) = binaryLength;
    *(registers+REG_K1) = *(registers+REG_GP);

    // Each process has it's own registers
    new_registers = memcpy(registers, 32);
    // Required otherwise UP_COPYARGUMENTS wouldn't work
    registers = new_registers;
    process_set_registers(process, new_registers);
    // Initialize the rest of the fields
    process_set_pc(process, 0);
    process_set_reg_hi(process, reg_hi);
    process_set_reg_lo(process, reg_lo);

    up_copyArguments(argc, argv);
}

void init_segmentation() {
    g_segment_table = struct_init(100); // Maximum of 100 Segments
    g_segment_counter = 0;
    g_segment_offset = 0;
    g_segmentation_active = 1;
}

// -----------------------------------------------------------------
// -------------------------- ASSIGNMENT1 --------------------------
// -----------------------------------------------------------------

void init_readyqueue(int argc, int *argv) {
    int i;
    int *process;

    g_ticks = 0;
    g_readyqueue = list_init();
    
    TIME_SLICE = 10;
    NUMBER_OF_INSTANCES = 32;

    i = 0;
    while(i < NUMBER_OF_INSTANCES) {
        if(i == NUMBER_OF_INSTANCES - 1) {
            g_running_process = process_allocate();
            process_set_id(g_running_process, i);
            process_get_segment(g_running_process, 1024 * 1024, argc, argv);
        } else {
            process = process_allocate();
            process_set_id(process, i);
            process_get_segment(process, 1024 * 1024, argc, argv);
            list_push_back(g_readyqueue, process);
        }
        i = i + 1;
    }
}

// Returns the next process to run
int *process_schedule() {
    int *process;
    
    if(list_is_empty(g_readyqueue))
        return g_running_process;

    process = list_entry_get_data(list_pop_front(g_readyqueue));

    return process;
}

// Switches to PROCESS
void process_switch(int *process) {

    if(process == g_running_process)
        return;

    // Save the state of the current process
    process_set_pc(g_running_process, pc);
    process_set_registers(g_running_process, registers);
    //process_set_memory(g_running_process, memory);
    process_set_reg_hi(g_running_process, reg_hi);
    process_set_reg_lo(g_running_process, reg_lo);

    list_push_back(g_readyqueue, g_running_process);

    // Switch to PROCESS
    pc = process_get_pc(process);
    registers = process_get_registers(process);
    //memory = process_get_memory(process);
    reg_hi = process_get_reg_hi(process);
    reg_lo = process_get_reg_lo(process);

    // We get a new running process
    g_running_process = process;

    if(DEBUG_1) {
        print((int*)"switch to process ");
        printInt(process_get_id(process));
        println();
    }
}

// NUM ... Number of bytes
int *memcpy(int *source, int num) {
    int i;
    int *dest;

    i = 0;
    dest = (int*)malloc(num * 4);
    while(i < num) {
        *(dest + i) = *(source + i);
        i = i + 1;
    }

    return dest;
}

void printInt(int i) {
    print(itoa(i, string_buffer, 10, 0));
}

// -----------------------------------------------------------------
// ----------------------------- STRUCT ----------------------------
// -----------------------------------------------------------------

int *struct_init(int size) {
    int *array;

    array = (int*)malloc(size * 4);

    return array;
}

int *struct_get_element_at(int *array, int i) {
    return (int*)*(array + i);
}

void struct_set_element_at(int *array, int i, int *value) {
    *(array + i) = (int)value;
}

// -----------------------------------------------------------------
// ----------------------------- SEGMENT ---------------------------
// -----------------------------------------------------------------

int *segment_init(int start, int size) {
    int *segment;

    // process:
    // +----+-------+
    // |  0 | start |
    // |  1 | size  |
    // +----+-------+

    segment = (int*)malloc(2 * 4);
    segment_set_start(segment, start);
    segment_set_size(segment, size);

    return segment;
}

// Getters
int segment_get_start(int *segment) {
    return *segment;
}

int segment_get_size(int *segment) {
    return *(segment + 1);
}

// Setters
void segment_set_start(int *segment, int start) {
    *segment = start;
}

void segment_set_size(int *segment, int size) {
    *(segment + 1) = size;
}

// -----------------------------------------------------------------
// ------------------------------ LIST -----------------------------
// -----------------------------------------------------------------

int *list_init() {
    int *list;
    int size;
    int *head;
    int *tail;
    // list:
    // +----+-------------+
    // |  0 | ptr to head |
    // |  1 | ptr to tail |
    // |  2 | size        |
    // +----+-------------+

    list = (int*)malloc(3 * 4);
    list_set_head(list, 0);
    list_set_tail(list, 0);
    list_set_size(list, 0);

    return list;
}

// Getters
int *list_get_head(int *list) {
    return (int*)*list;
}

int *list_get_tail(int *list) {
    return (int*)*(list + 1);
}

int list_get_size(int *list) {
    return *(list + 2);
}

// Setters
void list_set_head(int *list, int *head) {
    *list = (int)head;
}

void list_set_tail(int *list, int *tail) {
    *(list + 1) = (int)tail;
}

void list_set_size(int *list, int size) {
    *(list + 2) = size;
}

// Inserts DATA at HEAD
void list_push_front(int *list, int *data) {
    int *newEntry;
    int *head;
    int size;

    // list entry:
    // +----+-------------+
    // |  0 | ptr to next |
    // |  1 | ptr to prev |
    // |  2 | ptr to data |
    // +----+-------------+

    head = list_get_head(list);
    size = list_get_size(list);
    newEntry = (int*)malloc(3 * 4);

    // Init NEWENTRY
    list_entry_set_next(newEntry, head);
    // A new entry at HEAD doesn't have a previous element
    list_entry_set_prev(newEntry, 0);
    list_entry_set_data(newEntry, data);
    // Set head to the newest entry
    list_set_head(list, newEntry);
    // If list was empty then HEAD = TAIL
    if(list_is_empty(list))
        list_set_tail(list, newEntry);
    else
        list_entry_set_prev(head, newEntry);

    list_set_size(list, size + 1);
}

void list_push_back(int *list, int *data) {
    int *newEntry;
    int *tail;
    int size;

    // list entry:
    // +----+-------------+
    // |  0 | ptr to next |
    // |  1 | ptr to prev |
    // |  2 | ptr to data |
    // +----+-------------+

    tail = list_get_tail(list);
    size = list_get_size(list);
    newEntry = (int*)malloc(3 * 4);

    // Init NEWENTRY
    list_entry_set_next(newEntry, 0);
    list_entry_set_prev(newEntry, tail);
    list_entry_set_data(newEntry, data);
    // Set tail to the newest entry
    list_set_tail(list, newEntry);
    // If list was empty then HEAD = TAIL
    if(list_is_empty(list))
        list_set_head(list, newEntry);
    else
        list_entry_set_next(tail, newEntry);

    list_set_size(list, size + 1);
}

int *list_pop_front(int *list) {
    int *head;
    int *next;
    int size;
    
    if(list_is_empty(list))
        return (int*)0;

    head = list_get_head(list);
    next = list_entry_get_next(head);
    // Set new head to NEXT
    list_set_head(list, next);
    // Reduce size by one
    size = list_get_size(list);
    list_set_size(list, size - 1);
    
    if(list_is_empty(list) == 0)
        // The new head doesn't have a previous element
        list_entry_set_prev(next, 0);
    
    return head;
}

int *list_pop_back(int *list) {
    int *tail;
    int *prev;
    int size;

    if(list_is_empty(list))
        return (int*)0;
    
    tail = list_get_tail(list);
    prev = list_entry_get_prev(tail);
    // Set new tail to PREV
    list_set_tail(list, prev);
    size = list_get_size(list);
    // Reduce size by one
    list_set_size(list, size - 1);
    
    if(list_is_empty(list) == 0)
        // The new tail doesn't have a next element
        list_entry_set_next(prev, 0);
    
    return tail;
}

void list_insert_at(int *list, int index, int *data) {
    int *newEntry;
    int size;
    int *prev;
    int *curr;

    // Check bounds
    if(list_is_in_bounds(list, index) == 0)
        return;

    size = list_get_size(list);

    if(index == 0) {
        list_push_front(list, data);
        return;
    }

    newEntry = (int*)malloc(3 * 4);
    prev = list_get_entry_at(list, index - 1);
    curr = list_entry_get_next(prev);

    list_entry_set_next(prev, newEntry);
    list_entry_set_next(newEntry, curr);
    list_entry_set_data(newEntry, data);

    list_set_size(list, size + 1);
}

int *list_remove_at(int *list, int index) {
    int *prev;
    int *entry;
    int *next;
    int size;

    // Check bounds
    if(list_is_in_bounds(list, index) == 0)
        return (int*)0;

    // If it's the first index just change the head
    size = list_get_size(list);

    if(index == 0)
        return list_pop_front(list);
    else if(index == size - 1)
        return list_pop_back(list);
    
    prev = list_get_entry_at(list, index - 1);
    entry = list_entry_get_next(prev);
    next = list_entry_get_next(entry);
    list_entry_set_next(prev, next);
    list_set_size(list, size - 1);

    return entry;
}

int *list_get_entry_at(int *list, int index) {
    int i;
    int size;
    int *head;
    int *tail;
 
    size = list_get_size(list);
    // If it's the last element just return TAIL
    if(index == size - 1) {
        tail = list_get_tail(list);
        return tail;
    }

    head = list_get_head(list);
    i = 0;
    while(i < size) {
        if(i == index)
            return head;
        head = list_entry_get_next(head);
        i = i + 1;
    }

    return (int*)0;
}


// Return the entry with VALUE at FIELD_NR
int *list_find_entry_by(int *list, int value, int field_nr) {
    int *head;
    int current_value;

    // In all but the first recursion steps this basically calls list_entry_get_next(...)!
    // This works because the NEXT element in a list entry and the HEAD element in the 
    // actual list are at the same spot (FIELD_NR 0)!
    head = list_get_head(list);

    if((int)head == 0)
        return (int*)0;

    current_value = *(list_entry_get_data(head + field_nr));

    if(current_value == value)
        return head;

    list_find_entry_by(head, value, field_nr);
}

int list_is_in_bounds(int *list, int index) {
    int size;

    size = list_get_size(list);
    if(index < 0)
        return 0;
    else if(size <= index)
        return 0;
    return 1;
}

int list_is_empty(int *list) {
    int size;

    size = list_get_size(list);

    return size == 0;
}

void list_swap(int *list, int index1, int index2) {
    int *entry1;
    int *entry2;
    int *data1;
    int *data2;

    if(list_is_in_bounds(list, index1) == 0)
        return;
    if(list_is_in_bounds(list, index2) == 0)
        return;

    entry1 = list_get_entry_at(list, index1);
    entry2 = list_get_entry_at(list, index2);
    data1 = list_entry_get_data(entry1);
    data2 = list_entry_get_data(entry2);

    list_entry_set_data(entry1, data2);
    list_entry_set_data(entry2, data1);
}

// Sorts list by the field at FIELD_NR
void list_sort_by(int *list, int field_nr) {
    int i;
    int j;
    int size;
    int l;
    int r;

    size = list_get_size(list);
    i = size;
    while(i > 1) {
        j = 0;
        while(j < size - 1) {
            l = *(list_entry_get_data(list_get_entry_at(list, j)) + field_nr);
            r = *(list_entry_get_data(list_get_entry_at(list, j + 1)) + field_nr);
            if(l > r)
                list_swap(list, j, j + 1);
            j = j + 1;
        }
        i = i - 1;
    }
}

// Entry
// Getters
int *list_entry_get_next(int *entry) {
    return (int*)*entry;
}

int *list_entry_get_prev(int *entry) {
    return (int*)*(entry + 1);
}

int *list_entry_get_data(int *entry) {
    return (int*)*(entry + 2);
}

// Setters
void list_entry_set_next(int *entry, int *next) {
    *entry = (int)next;
}

void list_entry_set_prev(int *entry, int *prev) {
    *(entry + 1) = (int)prev;
}

void list_entry_set_data(int *entry, int *data) {
    *(entry + 2) = (int)data;
}

// -----------------------------------------------------------------
// ---------------------------- PROCESS ----------------------------
// -----------------------------------------------------------------

int  *process_allocate() {
    int *process;

    process = (int*)malloc(7 * 4);

    return process;
}

// TODO: Change memory to segment
int *process_init(int id, int *registers, int *memory, int reg_hi, int reg_lo) {
    int *process;

    // process:
    // +----+------------------+
    // |  0 | id               |
    // |  1 | pc               |
    // |  2 | ptr to registers |
    // |  3 | ptr to memory    |
    // |  4 | reg_hi           |
    // |  5 | reg_lo           |
    // |  6 | segment_id       |
    // +----+------------------+

    process = (int*)malloc(7 * 4);
    process_set_id(process, id);
    process_set_pc(process, 0); // Initially always 0
    process_set_registers(process, registers);
    process_set_memory(process, memory);
    process_set_reg_hi(process, reg_hi);
    process_set_reg_lo(process, reg_lo);

    return process;
}

// Getters
int process_get_id(int *process) {
    return *process;
}

int process_get_pc(int *process) {
    return *(process + 1);
}

int *process_get_registers(int *process) {
    return (int*)*(process + 2);
}

int *process_get_memory(int *process) {
    return (int*)*(process + 3);
}

int process_get_reg_hi(int *process) {
    return *(process + 4);
}

int process_get_reg_lo(int *process) {
    return *(process + 5);
}

int process_get_segment_id(int *process) {
    return *(process + 6);
}

// Setters
void process_set_id(int *process, int id) {
    *process = id;
}

void process_set_pc(int *process, int pc) {
    *(process + 1) = pc;
}

void process_set_registers(int *process, int *registers) {
    *(process + 2) = (int)registers;
}

void process_set_memory(int *process, int *memory) {
    *(process + 3) = (int)memory;
}

void process_set_reg_hi(int *process, int reg_hi) {
    *(process + 4) = reg_hi;
}

void process_set_reg_lo(int *process, int reg_lo) {
    *(process + 5) = reg_lo;
}

void process_set_segment_id(int *process, int segment_id) {
    *(process + 6) = segment_id;
}

void print_process_list(int *list) {
    int i;
    int size;
    int *process;
    int id;

    i = 0;
    size = list_get_size(list);

    putchar('[');
    while(i < size) {
        process = list_entry_get_data(list_get_entry_at(list, i));
        id = process_get_id(process);
        printInt(id);
        if(i != size - 1) {
            putchar(',');
            putchar(' ');
        }
        i = i + 1;
    }
    putchar(']');
    putchar(10); // Linefeed
}

// -----------------------------------------------------------------
// ---------------------------- TESTS ------------------------------
// -----------------------------------------------------------------

void list_test() {
    int *list;
    int *process;
    int *data;
    int id;

    println();
    print((int*) "Ready List Test");
    println();

    list = list_init();
    process = process_init(1, registers, memory, 0, 0);
    
    print((int*) "push front 1");
    println();
    list_push_front(list, process);
    print_process_list(list);
    
    print((int*) "pop front ");
    data = list_entry_get_data(list_pop_front(list));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);
    
    list = list_init();
    
    print((int*) "push back 1");
    println();
    list_push_front(list, process);
    print_process_list(list);
    
    print((int*) "pop front ");
    data = list_entry_get_data(list_pop_front(list));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);
    
    sched_yield();

    list = list_init();
    
    print((int*) "push front 1");
    println();
    list_push_front(list, process);
    print_process_list(list);
    
    print((int*) "pop back ");
    data = list_entry_get_data(list_pop_back(list));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);

    process = process_init(2, registers, memory, 0, 0);
    
    print((int*) "push front 2");
    println();
    list_push_front(list, process);
    print_process_list(list);

    process = process_init(3, registers, memory, 0, 0);
    
    print((int*) "push back 3");
    println();
    list_push_back(list, process);
    print_process_list(list);

    process = process_init(9, registers, memory, 0, 0);

    print((int*) "push back 9");
    println();
    list_push_back(list, process);
    print_process_list(list);

    process = process_init(4, registers, memory, 0, 0);
    
    print((int*) "push front 4");
    println();
    list_push_front(list, process);
    print_process_list(list);

    process = process_init(5, registers, memory, 0, 0);
    
    print((int*) "push front 5");
    println();
    list_push_front(list, process);
    print_process_list(list);
    
    print((int*) "pop back ");
    data = list_entry_get_data(list_pop_back(list));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);

    process = process_init(6, registers, memory, 0, 0);
    
    print((int*) "insert 6 at 2");
    println();
    list_insert_at(list, 2, process);
    print_process_list(list);

    process = process_init(7, registers, memory, 0, 0);
    
    print((int*) "insert 7 at 0");
    println();
    list_insert_at(list, 0, process);
    print_process_list(list);
    
    print((int*) "sort list");
    println();
    list_sort_by(list, 0);
    print_process_list(list);

    process = process_init(8, registers, memory, 0, 0);
    
    print((int*) "insert 8 at -1");
    println();
    list_insert_at(list, -1, process);
    print_process_list(list);
    
    print((int*) "swap 0 and 4");
    println();
    list_swap(list, 0, 4);
    print_process_list(list);

    print((int*) "remove at 3 -> ");
    data = list_entry_get_data(list_remove_at(list, 3));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);

    print((int*) "remove at 0 -> ");
    data = list_entry_get_data(list_remove_at(list, 0));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);

    print((int*) "pop front ");
    data = list_entry_get_data(list_pop_front(list));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);

    print((int*) "remove at 1 -> ");
    data = list_entry_get_data(list_remove_at(list, 1));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);

    print((int*) "pop front ");
    data = list_entry_get_data(list_pop_front(list));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);

    print((int*) "pop front ");
    data = list_entry_get_data(list_pop_front(list));
    id = process_get_id(data);
    printInt(id);
    println();
    print_process_list(list);
}

// -----------------------------------------------------------------
// ----------------------------- MAIN ------------------------------
// -----------------------------------------------------------------

int selfie(int argc, int* argv) {
    if (argc < 2)
        return -1;
    else {
        while (argc >= 2) {
            if (stringCompare((int*) *argv, (int*) "-c")) {
                sourceName = (int*) *(argv+1);
                binaryName = sourceName;

                argc = argc - 2;
                argv = argv + 2;

                compile();
            } else if (stringCompare((int*) *argv, (int*) "-o")) {
                binaryName = (int*) *(argv+1);

                argc = argc - 2;
                argv = argv + 2;

                if (binaryLength > 0)
                    emit();
                else {                    
                    print(selfieName);
                    print((int*) ": nothing to emit to output file ");
                    print(binaryName);
                    println();
                }
            } else if (stringCompare((int*) *argv, (int*) "-l")) {
                binaryName = (int*) *(argv+1);

                argc = argc - 2;
                argv = argv + 2;

                load();
            } else if (stringCompare((int*) *argv, (int*) "-m")) {
                initMemory(atoi((int*) *(argv+1)));

                argc = argc - 1;
                argv = argv + 1;

                // pass binaryName as first argument replacing size
                *argv = (int) binaryName;

                if (binaryLength > 0)
                    emulate(argc, argv);
                else {
                    print(selfieName);
                    print((int*) ": nothing to emulate");
                    println();

                    exit(-1);
                }

                return 0;
            } else if (stringCompare((int*) *argv, (int*) "-k")) {
                print(selfieName);
                print((int*) ": selfie -k size ... not yet implemented");
                println();

                argc = argc - 1;
                argv = argv + 1;

                kernel(argc, argv);

                return 0;
            } else
                return -1;
        }
    }

    return 0;
}

int main(int argc, int *argv) {
    // A2
    g_segment_offset = 0;
    g_next_segment = 0;
    g_segmentation_active = 0;
    DEBUG_1 = 0;

    initLibrary();
    initScanner();
    initRegister();
    initDecoder();
    
    initInterpreter();

    list_test();

    exit(0);
}