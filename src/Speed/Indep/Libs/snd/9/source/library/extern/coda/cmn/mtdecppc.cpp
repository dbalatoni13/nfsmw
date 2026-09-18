// Gestion del registro DEC (decrementer) para el profiler de SN.
// Cuerpo en asm: GCC no genera mfdec/mtdec ni la forma lis@h/ori@l de
// snProfHdr. Nota ngcas: los saltos condicionales con mnemonico (bge/beq)
// desde asm a nivel de fuente se codifican con offset absoluto; hay que usar
// la forma "bc BO,BI,label" que es la que emite el propio GCC.
extern "C" int snProfHdr[13];

extern "C" void PPCMtdec(int newticks) {
    asm volatile(
        "mfdec 7\n"
        "lis 6,snProfHdr@h\n"
        "ori 6,6,snProfHdr@l\n"
        "cmpwi 3,1\n"
        "bc 4,0,1f\n"
        "li 3,1\n"
        "1:\n"
        "stw 3,44(6)\n"
        "lwz 4,36(6)\n"
        "cmpwi 4,0\n"
        "bc 12,2,2f\n"
        "lwz 5,48(6)\n"
        "subf 5,7,5\n"
        "lwz 4,40(6)\n"
        "subf 4,5,4\n"
        "stw 4,40(6)\n"
        "cmpw 4,3\n"
        "bc 12,0,3f\n"
        "2:\n"
        "mtdec 3\n"
        "stw 3,48(6)\n"
        "blr\n"
        "3:\n"
        "cmpwi 4,0\n"
        "bc 4,0,4f\n"
        "li 4,0\n"
        "4:\n"
        "stw 4,40(6)\n"
        "mtdec 4\n"
        "stw 4,48(6)\n"
    );
}
