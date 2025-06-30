#pragma once

// Relocation Types

/// None
#define R_MIPS_NONE 0
/// `S + sign_extend(A)`
#define R_MIPS_16 1
/// `S + A`
#define R_MIPS_32 2
/// Alias of `R_MIPS_32`
#define R_MIPS_ADD R_MIPS_32
/// `S + A - EA`jA
#define R_MIPS_REL32 3
/// Alias of `MIPS_REL32`
#define R_MIPS_REL R_MIPS_REL32
/// local - `(((A << 2) | (P&0xf0000000)) + S) >> 2`
/// external - `(sign_extend(A<<2) + S) >> 2`
#define R_MIPS_26 4
/// `%high (AHL + S)`
#define R_MIPS_HI16 5
/// `%high (AHL + S)`
#define R_MIPS_LO16 6
/// external `sign_extend(A) + S - GP`
#define R_MIPS_GPREL16 7
/// local `sign_extend(A) + S + GP0 - GP`
#define R_MIPS_GPREL R_MIPS_GPREL16
/// `sign_extend(A) + L`
#define R_MIPS_LITERAL 8
/// external - `G`
#define R_MIPS_GOT16 9
/// local - `f`
#define R_MIPS_GOT R_MIPS_GOT16
/// `sign_extend(A) + S - P`
#define R_MIPS_PC16 10
/// `G`
#define R_MIPS_CALL16 11
/// Alias for `R_MIPS_CALL16`
#define R_MIPS_CALL R_MIPS_CALL16
/// `A + S + GP0 - GP`
#define R_MIPS_GPREL32 12
/// `S`
#define R_MIPS_SHIFT5 16
/// `S`
#define R_MIPS_SHIFT6 17
/// `S + A`
#define R_MIPS_64 18
/// `G`
#define R_MIPS_GOT_DISP 19
/// `h`
#define R_MIPS_GOT_PAGE 20
/// `h`
#define R_MIPS_GOT_OFST 21
/// `%high(G)`
#define R_MIPS_GOT_HI16 22
/// `G`
#define R_MIPS_GOT_LO16 23
/// `S - A`
#define R_MIPS_SUB 24
/// Insert addend as instruction immediately prior to addressed location
#define R_MIPS_INSERT_A 25
/// Insert addend as instruction immediately prior to addressed location
#define R_MIPS_INSERT_B 26
/// Remove the addressed 32-bit object (normally an instruction).
#define R_MIPS_DELETE 27
/// `%higher(A+S)`
#define R_MIPS_HIGHER 28
/// `%highest(A+S)`
#define R_MIPS_HIGHEST 29
/// `%high(G)`
#define R_MIPS_CALL_HI16 30
/// `G`
#define R_MIPS_CALL_LO16 31
/// `S+A-scn_addr` (section displacement)
#define R_MIPS_SCN_DISP 32
/// `S + A`
#define R_MIPS_REL16 33
/// V-half16 any oS + sign_extend(A)
#define R_MIPS_ADD_IMMEDIATE 34
/// T-word32 any Deprecated (protected jump)
#define R_MIPS_PJUMP 35
/// T-word32 any qS + A - EA
#define R_MIPS_RELGOT 36
/// T-word32 any pProtected jump conversion
#define R_MIPS_JALR 37

/// Module number 32 bit
#define R_MIPS_TLS_DTPMOD32 38
/// Module-relative offset 32-bit
#define R_MIPS_TLS_DTPREL3 39
/// Module number 64 bit
#define R_MIPS_TLS_DTPMOD6 40
/// Module-relative offset 64-bit
#define R_MIPS_TLS_DTPREL64 41
/// 16-bit GOT offset for GD
#define R_MIPS_TLS_GD 42
/// 16 bit GOT offset for LDM
#define R_MIPS_TLS_LDM 43
/// Module-relative offset, high 16-bits
#define R_MIPS_TLS_DTPREL_HI16 44
/// Module-relative offset, low 16-bits
#define R_MIPS_TLS_DTPREL_LO16 45
/// 16-bit GOT offset for IE
#define R_MIPS_TLS_GOTTPREL 46
/// TP-relative offset, 32-bit
#define R_MIPS_TLS_TPREL32 47
///  TP-relative offset, 64-bit
#define R_MIPS_TLS_TPREL64 48
/// TP-relative offset, high 16-bits
#define R_MIPS_TLS_TPREL_HI16 49
/// TP-relative offset, low 16-bits
#define R_MIPS_TLS_TPREL_LO16 50
#define R_MIPS_GLOB_DAT 51
#define R_MIPS_COPY 126
#define R_MIPS_JUMP_SLOT 127