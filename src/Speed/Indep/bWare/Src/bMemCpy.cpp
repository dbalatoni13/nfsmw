#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern "C" {

#ifndef EA_PLATFORM_WIN32
#ifdef EA_PLATFORM_PLAYSTATION2
// EE retail quadword, doubleword, word, then byte copy paths.
asm(".set noreorder\n\t"
    ".set nomacro\n\t"
    ".globl bMemCpy\n\t"
    ".ent bMemCpy\n\t"
    "bMemCpy:\n\t"
    "daddu $8, $4, $0\n\t"
    "daddu $7, $5, $0\n\t"
    "or $3, $8, $7\n\t"
    "andi $2, $3, 0xf\n\t"
    "bnez $2, bMemCpy_64\n\t"
    "daddu $9, $6, $0\n\t"
    "slti $2, $9, 0x40\n\t"
    "bnez $2, bMemCpy_68\n\t"
    "andi $11, $3, 0x3\n\t"
    "andi $10, $3, 0x7\n\t"
    "bMemCpy_28:\n\t"
    "lq $4, 0x0($7)\n\t"
    "addiu $9, $9, -0x40\n\t"
    "lq $5, 0x10($7)\n\t"
    "slti $6, $9, 0x40\n\t"
    "lq $2, 0x20($7)\n\t"
    "lq $3, 0x30($7)\n\t"
    "sq $4, 0x0($8)\n\t"
    "addiu $7, $7, 0x40\n\t"
    "sq $5, 0x10($8)\n\t"
    "sq $2, 0x20($8)\n\t"
    "sq $3, 0x30($8)\n\t"
    "beqz $6, bMemCpy_28\n\t"
    "addiu $8, $8, 0x40\n\t"
    "b bMemCpy_6c\n\t"
    "nop\n\t"
    "bMemCpy_64:\n\t"
    "andi $11, $3, 0x3\n\t"
    "bMemCpy_68:\n\t"
    "andi $10, $3, 0x7\n\t"
    "bMemCpy_6c:\n\t"
    "bnez $10, bMemCpy_a4\n\t"
    "slti $2, $9, 0x10\n\t"
    "bnez $2, bMemCpy_a4\n\t"
    "nop\n\t"
    "nop\n\t"
    "bMemCpy_80:\n\t"
    "ld $2, 0x0($7)\n\t"
    "addiu $9, $9, -0x10\n\t"
    "ld $3, 0x8($7)\n\t"
    "slti $4, $9, 0x10\n\t"
    "sd $2, 0x0($8)\n\t"
    "addiu $7, $7, 0x10\n\t"
    "sd $3, 0x8($8)\n\t"
    "beqz $4, bMemCpy_80\n\t"
    "addiu $8, $8, 0x10\n\t"
    "bMemCpy_a4:\n\t"
    "bnez $11, bMemCpy_dc\n\t"
    "slti $2, $9, 0x8\n\t"
    "bnez $2, bMemCpy_dc\n\t"
    "nop\n\t"
    "nop\n\t"
    "bMemCpy_b8:\n\t"
    "lw $2, 0x0($7)\n\t"
    "addiu $9, $9, -0x8\n\t"
    "lw $3, 0x4($7)\n\t"
    "slti $4, $9, 0x8\n\t"
    "sw $2, 0x0($8)\n\t"
    "addiu $7, $7, 0x8\n\t"
    "sw $3, 0x4($8)\n\t"
    "beqz $4, bMemCpy_b8\n\t"
    "addiu $8, $8, 0x8\n\t"
    "bMemCpy_dc:\n\t"
    "beqz $9, bMemCpy_108\n\t"
    "nop\n\t"
    "nop\n\t"
    "bMemCpy_e8:\n\t"
    "lbu $2, 0x0($7)\n\t"
    "addiu $9, $9, -0x1\n\t"
    "addiu $7, $7, 0x1\n\t"
    "sb $2, 0x0($8)\n\t"
    "addiu $8, $8, 0x1\n\t"
    "bnez $9, bMemCpy_e8\n\t"
    "nop\n\t"
    "nop\n\t"
    "bMemCpy_108:\n\t"
    "jr $31\n\t"
    "nop\n\t"
    ".end bMemCpy\n\t"
    ".set macro\n\t"
    ".set reorder\n\t");
#else
void bMemCpy(void *dest, const void *src, unsigned int numbytes) {
    char *pdest = (char *)dest;
    char *psrc = (char *)src;
    int size = numbytes;
    int alignment = (int)pdest | (int)psrc;

    if (!(alignment & 7)) {
        for (; size > 15;) {
            size -= 16;
            unsigned long long t0 = ((unsigned long long *)psrc)[0];
            unsigned long long t1 = ((unsigned long long *)psrc)[1];
            ((unsigned long long *)pdest)[0] = t0;
            ((unsigned long long *)pdest)[1] = t1;
            psrc += 16;
            pdest += 16;
        }
    }
    if (!(alignment & 3)) {
        for (; size > 7;) {
            size -= 8;
            unsigned int t0 = ((unsigned int *)psrc)[0];
            unsigned int t1 = ((unsigned int *)psrc)[1];
            ((unsigned int *)pdest)[0] = t0;
            ((unsigned int *)pdest)[1] = t1;
            psrc += 8;
            pdest += 8;
        }
    }
    for (; size;) {
        --size;
        unsigned char t0 = *psrc;
        *pdest = t0;
        ++psrc;
        ++pdest;
    }
}

#endif

#ifdef EA_PLATFORM_PLAYSTATION2
// Retail MMI pattern expansion and aligned quadword stores.
asm(".set noreorder\n\t"
    ".set nomacro\n\t"
    ".globl bMemSet\n\t"
    ".ent bMemSet\n\t"
    "bMemSet:\n\t"
    "andi $5, $5, 0xff\n\t"
    "daddu $7, $4, $0\n\t"
    "sll $2, $5, 16\n\t"
    "sll $4, $5, 24\n\t"
    "sll $3, $5, 8\n\t"
    "addu $4, $4, $5\n\t"
    "addu $3, $3, $2\n\t"
    "addiu $29, $29, -0x10\n\t"
    "andi $2, $7, 0x3\n\t"
    "bnez $2, bMemSet_6c\n\t"
    "addu $4, $4, $3\n\t"
    "andi $3, $7, 0xf\n\t"
    "beqz $3, bMemSet_78\n\t"
    "sltiu $2, $6, 0x4\n\t"
    "bnez $2, bMemSet_70\n\t"
    "nop\n\t"
    "sw $4, 0x0($7)\n\t"
    "nop\n\t"
    "bMemSet_48:\n\t"
    "addiu $7, $7, 0x4\n\t"
    "andi $3, $7, 0xf\n\t"
    "beqz $3, bMemSet_78\n\t"
    "addiu $6, $6, -0x4\n\t"
    "sltiu $2, $6, 0x4\n\t"
    "beqzl $2, bMemSet_48\n\t"
    "sw $4, 0x0($7)\n\t"
    "b bMemSet_70\n\t"
    "nop\n\t"
    "bMemSet_6c:\n\t"
    "andi $3, $7, 0xf\n\t"
    "bMemSet_70:\n\t"
    "bnez $3, bMemSet_b0\n\t"
    "andi $2, $7, 0x7\n\t"
    "bMemSet_78:\n\t"
    "pcpyh $3, $4\n\t"
    "sltiu $2, $6, 0x20\n\t"
    "pcpyld $3, $3, $3\n\t"
    "bnez $2, bMemSet_b0\n\t"
    "andi $2, $7, 0x7\n\t"
    "nop\n\t"
    "bMemSet_90:\n\t"
    "sq $3, 0x0($7)\n\t"
    "addiu $6, $6, -0x20\n\t"
    "sq $3, 0x10($7)\n\t"
    "sltiu $2, $6, 0x20\n\t"
    "addiu $7, $7, 0x20\n\t"
    "beqz $2, bMemSet_90\n\t"
    "nop\n\t"
    "andi $2, $7, 0x7\n\t"
    "bMemSet_b0:\n\t"
    "bnez $2, bMemSet_f0\n\t"
    "andi $2, $7, 0x3\n\t"
    "sw $4, 0x0($29)\n\t"
    "sltiu $2, $6, 0x10\n\t"
    "sw $4, 0x4($29)\n\t"
    "ld $3, 0x0($29)\n\t"
    "bnez $2, bMemSet_f0\n\t"
    "andi $2, $7, 0x3\n\t"
    "bMemSet_d0:\n\t"
    "sd $3, 0x0($7)\n\t"
    "addiu $6, $6, -0x10\n\t"
    "sd $3, 0x8($7)\n\t"
    "sltiu $2, $6, 0x10\n\t"
    "addiu $7, $7, 0x10\n\t"
    "beqz $2, bMemSet_d0\n\t"
    "nop\n\t"
    "andi $2, $7, 0x3\n\t"
    "bMemSet_f0:\n\t"
    "bnez $2, bMemSet_11c\n\t"
    "sltiu $2, $6, 0x8\n\t"
    "bnez $2, bMemSet_11c\n\t"
    "nop\n\t"
    "bMemSet_100:\n\t"
    "sw $4, 0x0($7)\n\t"
    "addiu $6, $6, -0x8\n\t"
    "sw $4, 0x4($7)\n\t"
    "sltiu $2, $6, 0x8\n\t"
    "addiu $7, $7, 0x8\n\t"
    "beqz $2, bMemSet_100\n\t"
    "nop\n\t"
    "bMemSet_11c:\n\t"
    "beqz $6, bMemSet_148\n\t"
    "nop\n\t"
    "nop\n\t"
    "bMemSet_128:\n\t"
    "sb $5, 0x0($7)\n\t"
    "addiu $6, $6, -0x1\n\t"
    "addiu $7, $7, 0x1\n\t"
    "nop\n\t"
    "nop\n\t"
    "bnez $6, bMemSet_128\n\t"
    "nop\n\t"
    "nop\n\t"
    "bMemSet_148:\n\t"
    "jr $31\n\t"
    "addiu $29, $29, 0x10\n\t"
    ".end bMemSet\n\t"
    ".set macro\n\t"
    ".set reorder\n\t");
#elif !defined(EA_PLATFORM_XENON)
void bMemSet(void *dest, unsigned char pattern, unsigned int size) {
    int original_size = size;
    char *pdest = reinterpret_cast<char *>(dest);
    int32 pattern32 = (pattern << 24) + (pattern << 16) + (pattern << 8) + pattern;

    if ((reinterpret_cast<uintptr_t>(pdest) & 3) == 0) {
        while (((reinterpret_cast<uintptr_t>(pdest) & 0xf) != 0) && (size > 3)) {
            *reinterpret_cast<uint32 *>(&pdest[0]) = pattern32;
            pdest += 4;
            size -= 4;
        }
    }

    if ((reinterpret_cast<uintptr_t>(pdest) & 7) == 0) {
        volatile uint64 convert64;
        reinterpret_cast<volatile uint32 *>(&convert64)[0] = pattern32;
        reinterpret_cast<volatile uint32 *>(&convert64)[1] = pattern32;
        uint64 pattern64 = convert64;

        while (size > 15) {
            *reinterpret_cast<uint64 *>(&pdest[0]) = pattern64;
            size -= 16;
            *reinterpret_cast<uint64 *>(&pdest[8]) = pattern64;
            pdest += 16;
        }
    }

    if ((reinterpret_cast<uintptr_t>(pdest) & 3) == 0) {
        while (size > 7) {
            reinterpret_cast<uint32 *>(pdest)[0] = pattern32;
            reinterpret_cast<uint32 *>(pdest)[1] = pattern32;
            pdest += 8;
            size -= 8;
        }
    }

    while (size != 0) {
        int n;
        *pdest = pattern;
        pdest++;
        size--;
    }
}

#endif

#ifdef EA_PLATFORM_PLAYSTATION2
asm(".set noreorder\n\t"
    ".set nomacro\n\t"
    ".globl bMemCmp\n\t"
    ".ent bMemCmp\n\t"
    "bMemCmp:\n\t"
    "bne $6, $0, bMemCmp_check\n\t"
    "addiu $6, $6, -1\n\t"
    "jr $31\n\t"
    "daddu $2, $0, $0\n\t"
    "bMemCmp_loop:\n\t"
    "addiu $5, $5, 1\n\t"
    "addiu $6, $6, -1\n\t"
    "bMemCmp_check:\n\t"
    "beql $6, $0, bMemCmp_last\n\t"
    "lbu $3, 0($4)\n\t"
    "lb $3, 0($4)\n\t"
    "lb $2, 0($5)\n\t"
    "beql $3, $2, bMemCmp_loop\n\t"
    "addiu $4, $4, 1\n\t"
    "lbu $3, 0($4)\n\t"
    "bMemCmp_last:\n\t"
    "lbu $2, 0($5)\n\t"
    "jr $31\n\t"
    "subu $2, $3, $2\n\t"
    ".end bMemCmp\n\t"
    ".set macro\n\t"
    ".set reorder\n\t");
#else
int bMemCmp(const void *s1, const void *s2, unsigned int numbytes) {
    if (numbytes == 0) {
        return 0;
    }
    while (numbytes--, numbytes != 0 && *(unsigned char *)s1 == *(unsigned char *)s2) {
        s1 = reinterpret_cast<void *>(reinterpret_cast<unsigned int>(s1) + 1);
        s2 = reinterpret_cast<void *>(reinterpret_cast<unsigned int>(s2) + 1);
    }
    return *reinterpret_cast<const unsigned char *>(s1) - *reinterpret_cast<const unsigned char *>(s2);
}
#endif
#endif

void bOverlappedMemCpy(void *dest, const void *src, unsigned int numbytes) {
    char *cdest = reinterpret_cast<char *>(dest);
    const char *csrc = reinterpret_cast<const char *>(src);
    int overlap_amount = cdest - csrc;

    if ((overlap_amount <= 0) || (overlap_amount >= (int)numbytes)) {
        bMemCpy(dest, src, numbytes);
    } else {
        int pos = numbytes;
        if (pos >= overlap_amount) {
            do {
                pos -= overlap_amount;
                bMemCpy(cdest + pos, csrc + pos, overlap_amount);
            } while (pos >= overlap_amount);
        }
        bMemCpy(dest, src, pos);
    }
}
}
