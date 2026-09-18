#include "eagl4supportdlopen.h"
#include "eagl4supportconspool.h"
#include "eagl4supportdef.h"

#include <cstring>
#include <types.h>

static void *dlsym(void *handle, const char *name);

extern const char gRuntimeAllocType[] = "RUNTIME_ALLOC::";

namespace EAGL4 {

SymbolPool DynamicLoader::gSymbolPool;
ConstructorPool DynamicLoader::gConsPool;
RuntimeAllocConstructorPool DynamicLoader::gRuntimeAllocConsPool;

// const char DynamicLoader::ModelType[];
// const char DynamicLoader::BBoxType[];
// const char DynamicLoader::TARType[];
// const char DynamicLoader::ShapeType[];
// r64-eagl: estas dos son una SEGUNDA copia de los bytes 0x2A4/0x2BC del prefijo
// de .rodata (0x803D428C y 0x803D42A4).  Quitarlas y poner el simbolo en el
// prefijo es NEGATIVO MEDIDO: ver la nota del bloque asm() en zEagl4Anim.cpp.
const char DynamicLoader::AnimBankType[] = "AnimationBank";
// const char DynamicLoader::BoneType[];
const char DynamicLoader::SkeletonType[] = "Skeleton";
// const char DynamicLoader::MorphType[];
// const char DynamicLoader::VersionType[];
// const char DynamicLoader::AnimClipSetType[];
// const char DynamicLoader::AnimIdType[];
// const char DynamicLoader::PosePaletteBankType[];

static long unsigned int elfhash(const char *name) {
    long unsigned int h = 0; // r3
    long unsigned int g;     // r9

    while (*name) {
        h = h * 0x10 + *name;
        name++;
        g = h & 0xf0000000;
        if (g != 0) {
            h ^= g >> 0x18;
        }
        h &= ~g;
    }
    return h & 0xFF;
}

DynamicLoader::DynamicLoader(void *d, unsigned int len, DynamicUserCallback pSearchFunction)
    : mpData((char *)d),                //
      mDataLen(len),                    //
      mIsResolved(false),               //
      handle(nullptr),                  //
      nDestructors(0),                  //
      destructors(nullptr),             //
      RuntimeAllocDestructors(nullptr), //
      mpReloc(nullptr),                 //
      mSearchCallback(pSearchFunction), //
      mNumPatchAddresses(0),            //
      mMaxPatchAddresses(0),            //
      mpPatchAddresses32(nullptr) {
    Initialize(pSearchFunction);
    Resolve();
    DoVersionCheck();
}

bool DynamicLoader::DoVersionCheck() {
    return true;
}

DynamicLoader::~DynamicLoader() {
    Release();
    RunDestructors();
    if (mpPatchAddresses32) {
        EAGL4Internal::EAGL4Free(mpPatchAddresses32, mMaxPatchAddresses);
    }
    mpPatchAddresses32 = nullptr;
}

// TODO where does this go?
static HashPointer *hashhead = 0;

void DynamicLoader::Release() {
    if (handle) {
        HashPointer *h = reinterpret_cast<HashPointer *>(handle);

        if (h->prev) {
            h->prev->next = h->next;
        } else {
            hashhead = h->next;
        }

        if (h->next) {
            h->next->prev = h->prev;
        }

        h->~HashPointer();
        HashPointer::operator delete(h, sizeof(HashPointer));

        handle = nullptr;
    }
    mIsResolved = false;
}

void DynamicLoader::RunConstructors() {
    int n = GetCount();
    int N = 0;

    for (int i = 0; i < n; i++) {
        Symbol s = GetSymbol(i);
        if (s.isInternalRef) {
            Constructor c = gConsPool.FindConstructor(s.type);
            if (c) {
                N++;
            }
        }
    }
    if (N > 0) {
        destructors = reinterpret_cast<DestructorEntry *>(EAGL4Internal::EAGL4Malloc(N * sizeof(*destructors), "EAGL4::dynamic destructor list"));
        N = 0;
        for (int i = 0; i < n; i++) {
            Symbol s = GetSymbol(i);
            if (s.isInternalRef) {
                Constructor c = gConsPool.FindConstructor(s.type);
                if (!c) {
                    continue;
                }
                Destructor d = gConsPool.FindDestructor(s.type);
                c(s.data, this, s.name);
                destructors[N].d = d;
                destructors[N].data = s.data;
                N++;
            }
        }
    }
    nDestructors = N;
}

void DynamicLoader::RunDestructors() {
    if (destructors) {
        for (int i = nDestructors - 1; i >= 0; i--) {
            destructors[i].d(destructors[i].data);
        }

        EAGL4Internal::EAGL4Free(destructors, nDestructors * sizeof(*destructors));
        destructors = nullptr;
    }
    RuntimeAllocDestructorEntry *de = RuntimeAllocDestructors;
    while (de) {
        RuntimeAllocDestructorEntry *tempde = de->next;

        de->d(de->data, de->auxData);
        delete de;

        de = tempde;
    }

    RuntimeAllocDestructors = nullptr;
}

void DynamicLoader::Resolve() {
    HashPointer *h;
    ELFSectionHeader *sheader;
    ELFHeader *e;
    const int MAX_UNRESOLVED_ERRORS = 32;
    int i;
    int j;
    int unresolvedSymbolError;
    const char *unresolvedList[MAX_UNRESOLVED_ERRORS];
    int numUnresolved;

    h = reinterpret_cast<HashPointer *>(handle);

    if (h) {

        if (!h->resolved) {

            h->resolved = true;

            sheader = h->sections;
            e = h->e;

            unresolvedSymbolError = 0;

            numUnresolved = 0;

            for (i = 0; i < e->e_shnum; i++) {

                if (sheader[i].sh_type == SHT_REL) {

                    int relocations = static_cast<int>(sheader[i].sh_size) / static_cast<int>(sizeof(ELF32_Rel));
                    char *patchbase = reinterpret_cast<char *>(sheader[i].sh_vinfo);

                    ELF32_Rel *r = reinterpret_cast<ELF32_Rel *>(sheader[i].sh_voffset);

                    ELF32_Sym *symtab = reinterpret_cast<ELF32_Sym *>(sheader[i].sh_vlink);

                    while (relocations--) {

                        unsigned int baseaddr = 0;
                        int iIndex;
                        ELF32_Sym *sym;
                        unsigned int *patchaddr;
                        unsigned short *patchaddr16;

                        r->r_offset = htotul(r->r_offset);
                        r->r_info = htotul(r->r_info);

                        sym = &symtab[r->r_info >> 8];

                    retry:
                        switch (sym->st_info & 0xF) {

                        case STT_NOTYPE:
                        case STT_OBJECT:
                        case STT_FUNC:
                        case STT_SECTION:

                            iIndex = sym->st_shndx;

                            if (iIndex > 0 && iIndex < e->e_shnum) {

                                baseaddr = reinterpret_cast<unsigned int>(sheader[iIndex].sh_voffset);

                            } else {

                                HashPointer *hp = hashhead;

                                if (hp) {

                                    do {

                                        if (hp != h) {

                                            void *addr = dlsym(hp, &h->strtab[sym->st_name]);
                                            if (addr) {

                                                sym->st_shndx = 1;
                                                sym->st_value = reinterpret_cast<unsigned int>(addr) -
                                                                reinterpret_cast<unsigned int>(sheader[1].sh_voffset);
                                                sym->st_other = 2;
                                                goto retry;
                                            }
                                        }

                                        hp = hp->next;

                                    } while (hp);
                                }

                                if (h->pSearchFunction) {

                                    bool valid;
                                    void *addr = h->pSearchFunction(&h->strtab[sym->st_name], valid);

                                    if (valid) {

                                        sym->st_shndx = 1;
                                        sym->st_value = reinterpret_cast<unsigned int>(addr) -
                                                        reinterpret_cast<unsigned int>(sheader[1].sh_voffset);
                                        sym->st_other = 3;
                                        goto retry;
                                    }
                                }

                                {
                                    bool valid;
                                    void *addr = gSymbolPool.Search(&h->strtab[sym->st_name], valid);

                                    if (valid) {

                                        sym->st_shndx = 1;
                                        sym->st_value = reinterpret_cast<unsigned int>(addr) -
                                                        reinterpret_cast<unsigned int>(sheader[1].sh_voffset);
                                        sym->st_other = 4;
                                        goto retry;
                                    }
                                }

                                {
                                    Symbol s;

                                    s.name = &h->strtab[sym->st_name];
                                    s.type = &h->strtab[sym->st_name] + strlen(s.name);
                                    s.type++;
                                    if (s.type[0] == 0x7F) {
                                        s.type++;
                                    } else {
                                        s.type--;
                                    }

                                    if (strncmp(gRuntimeAllocType, s.name, strlen(gRuntimeAllocType)) == 0) {

                                        const char *stripped_name = s.name + strlen(gRuntimeAllocType);

                                        RuntimeAllocConstructor c = gRuntimeAllocConsPool.FindConstructor(s.type);

                                        if (c) {

                                            RuntimeAllocDestructor d = gRuntimeAllocConsPool.FindDestructor(s.type);

                                            int auxData;
                                            bool bCallDestructor = false;

                                            s.data = c(stripped_name, this, auxData, bCallDestructor, s.name);

                                            if (bCallDestructor && s.data) {

                                                RuntimeAllocDestructorEntry *de =
                                                    new RuntimeAllocDestructorEntry(d, s.data, auxData);

                                                de->next = RuntimeAllocDestructors;
                                                RuntimeAllocDestructors = de;
                                            }

                                            sym->st_shndx = 1;
                                            sym->st_value = reinterpret_cast<unsigned int>(s.data) -
                                                            reinterpret_cast<unsigned int>(sheader[1].sh_voffset);
                                            sym->st_other = 5;
                                            goto retry;
                                        }
                                    }
                                }

                                if (!unresolvedSymbolError) {

                                    unresolvedSymbolError = 1;
                                }

                                if (numUnresolved < MAX_UNRESOLVED_ERRORS) {

                                    bool found = false;

                                    for (j = 0; j < numUnresolved; j++) {

                                        if (&h->strtab[sym->st_name] == unresolvedList[j]) {
                                            found = true;
                                            break;
                                        }
                                    }

                                    if (!found) {
                                        unresolvedList[numUnresolved] = &h->strtab[sym->st_name];
                                        numUnresolved++;
                                    }
                                }
                            }

                            baseaddr += sym->st_value;
                            break;
                        }

                        patchaddr = reinterpret_cast<unsigned int *>(&patchbase[r->r_offset]);
                        patchaddr16 = reinterpret_cast<unsigned short *>(patchaddr);

                        *patchaddr = htotul(*patchaddr);

                        switch (static_cast<int>(r->r_info & 0xFF)) {

                        case R_MIPS_32:
                            *patchaddr += baseaddr;
                            break;

                        case R_MIPS_16:
                        case R_MIPS_REL32:
                        case R_MIPS_GPREL16:
                        case R_MIPS_LITERAL:
                        case R_MIPS_GOT16:
                        case R_MIPS_PC16:
                        case R_MIPS_CALL16:
                        case R_MIPS_GPREL32:
                            break;

                        case R_MIPS_26:
                            baseaddr += (*patchaddr & 0x03FFFFFF) << 2;
                            *patchaddr = (*patchaddr & 0xFC000000) | ((baseaddr >> 2) & 0x03FFFFFF);
                            break;

                        case R_MIPS_LO16:
                            *patchaddr16 += baseaddr;
                            break;

                        case R_MIPS_HI16:
                            *patchaddr16 += baseaddr >> 16;
                            break;

                        case R_MIPS_UNUSED2:
                        case R_MIPS_UNUSED3:
                        case R_MIPS_SHIFT5:
                        case R_MIPS_SHIFT6:
                        case R_MIPS_64:
                        case R_MIPS_GOT_DISP:
                        case R_MIPS_GOT_PAGE:
                        case R_MIPS_GOT_OFST:
                            break;
                        }

                        r++;
                    }
                }
            }

            RunConstructors();

            mIsResolved = true;
        }
    }
}

void DynamicLoader::Initialize(DynamicUserCallback pSearchFunction) {
    int i;
    HashPointer *pHP = new HashPointer(this);
    HashPointer &h = *pHP;
    ELFSectionHeader *sheader;
    ELFHeader *e;
    char *shstrtab;
    void *p;

    pHP->e = nullptr;
    pHP->next = nullptr;
    pHP->strtab = nullptr;
    pHP->resolved = false;
    pHP->symbols_num = 0;
    pHP->symtab = nullptr;
    pHP->sections = nullptr;
    pHP->chain = nullptr;
    pHP->isOriginal = nullptr;
    pHP->pSearchFunction = nullptr;

    e = reinterpret_cast<ELFHeader *>(mpData);
    pHP->e = e;

    e->e_type = htotus(e->e_type);
    e->e_machine = htotus(e->e_machine);
    e->e_version = htotul(e->e_version);
    e->e_entry = htotul(e->e_entry);
    e->e_phoff = htotul(e->e_phoff);
    e->e_shoff = htotul(e->e_shoff);
    e->e_flags = htotul(e->e_flags);
    e->e_ehsize = htotus(e->e_ehsize);
    e->e_phentsize = htotus(e->e_phentsize);
    e->e_phnum = htotus(e->e_phnum);
    e->e_shentsize = htotus(e->e_shentsize);
    e->e_shnum = htotus(e->e_shnum);
    e->e_shstrndx = htotus(e->e_shstrndx);

    p = ELFAddr(e->e_shoff);

    pHP->symtab = nullptr;
    pHP->sections = reinterpret_cast<ELFSectionHeader *>(p);

    for (i = 0; i < e->e_shnum; i++) {

        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_name = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_name);
        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_type = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_type);
        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_flags = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_flags);
        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_addr = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_addr);
        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_offset = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_offset);
        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_size = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_size);
        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_link = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_link);
        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_info = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_info);
        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_addralign = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_addralign);
        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_entsize = htotul(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_entsize);

        reinterpret_cast<ELFSectionHeader *>(p)[i].sh_voffset = ELFAddr(reinterpret_cast<ELFSectionHeader *>(p)[i].sh_offset);
    }

    shstrtab = reinterpret_cast<char *>(reinterpret_cast<ELFSectionHeader *>(p)[e->e_shstrndx].sh_voffset);

    for (i = 0; i < e->e_shnum; i++) {

        sheader = reinterpret_cast<ELFSectionHeader *>(i * sizeof(ELFSectionHeader) + reinterpret_cast<unsigned int>(p));

        // ====================================================================
        // r64-eagl: RESUELTO.  Los 4 B de mas YA NO ESTAN: Initialize mide
        // 2.352/2.352 y `linkdelta` de zEagl4Anim pasa a `.text +0`.
        // LA VEDA DE LA r47/r60 ERA FALSA, y esta es la razon exacta: las dos
        // rondas barrieron ~200 combinaciones de casos ANADIDOS dejando `case
        // SHT_SYMTAB:` FIJO, y el objetivo necesita TRES cosas A LA VEZ.
        // El objetivo emite en BAJO
        //     cmpwi  3 ; beq  <cuerpo STRTAB>     <- {3} de valor unico
        //     cmplwi 3 ; bgt  FIN                 <- GT high -> [4,7] acotado
        //     cmplwi 1 ; ble  FIN                 <- `LE high(1) -> code_label`
        //     <CAE al cuerpo de SHT_SYMTAB>       <- `emit_jump` BORRADO
        // Ese `ble 1` NO puede ser un `LT 2 -> default` (medido: GCC no
        // canonicaliza LTU C a LEU C-1 aqui; con el nodo [1,2] sale `blt 1`).
        // Solo lo emite la rama de RANGO de emit_case_nodes con
        // `node->right != 0 && node->left == 0` (stmt.c:6083), o sea un nodo
        // [0,1] --low == TYPE_MIN, por eso no lleva el `LT low`-- CON HIJO
        // DERECHO {2}.  Y {2}, colgado de [0,1] y de {3}, es `node_is_bounded`
        // (ancestro con high==1 y ancestro con low==3), asi que emite un
        // `emit_jump(<cuerpo SHT_SYMTAB>)` PELADO: ESE es el simplejump que le
        // faltaba a la condicion `JUMP_LABEL(range1end) == label2` de jump.c
        // (jump.c:1848) que la r47 nombro y no supo fabricar.  Con el, jump.c
        // invierte la raiz y permuta ALTO/BAJO, y el `b` se borra por ser salto
        // a la etiqueta siguiente: 17 instrucciones en vez de 18.
        // Para que balance_case_nodes construya {3} -> izq [0,1] -> der {2}
        // con la raiz todavia en {8} hacen falta ADEMAS dos cosas:
        //   (a) partir SHT_RELA de SHT_HASH..SHT_NOTE con sabores DISTINTOS
        //       (`break` contra `continue`) para que group_case_nodes NO los
        //       funda y el subarbol derecho de {3} sea el nodo acotado [5,7]
        //       precedido de {4}; jump.c se come luego las dos instrucciones
        //       muertas de ese subarbol;
        //   (b) DOS unidades mas de peso por encima de {9}, que aqui son
        //       SHT_SHLIB..SHT_DYNSYM, para que la caminata de
        //       balance_case_nodes siga parando en {8}.
        // Como se encontro: simulador de stmt.c (group + balance + emit) para
        // cribar, y luego 373 compilaciones de un micro con los cflags reales
        // --0,14 s cada una--: 48 aciertos, y el mas plausible es el que esta
        // escrito abajo.  El barrido CIEGO de 1.458 combinaciones de sabores
        // sin tocar `case SHT_SYMTAB` dio CERO, que es justo lo que le paso a
        // la r47 y a la r60.
        // LO QUE QUEDA (14 filas, todas de reparto/orden de operandos):
        //   * asociatividad de `&s[nameLength + 1]`: RE-MEDIDA sobre este arbol
        //     nuevo y sigue NEGATIVA.  `&s[nameLength] + 1`, `s + nameLength +
        //     1`, una local `char *tail` y `*(s + nameLength)` dan las CUATRO
        //     16 filas contra las 14 de la base (2.352 B en todas).
        //   * orden de operandos de `add`/`lwzx`/`stwx` indexados: el objetivo
        //     pone SIEMPRE el INDICE primero (`add r30,r11,r10`,
        //     `lwzx r11,r5,r4`) y nosotros la BASE.  Medido en micro con los
        //     cflags reales: la unica forma de fuente que da la vuelta al `add`
        //     es hacer la cuenta EN ENTEROS (`(S*)(i*sizeof(S) + (unsigned)p)`
        //     -> `add 3,0,3`); `&p[i]`, `p + i`, `i + p`, `(char*)p + i*sizeof`
        //     y el indice sin signo dan las cinco `add 3,3,0`.  Y NINGUNA de
        //     las seis mueve el `lwzx`, que sale `lwzx 4,3,0` en todas.  O sea:
        //     el orden del `lwzx` del objetivo NO sale de la forma de la
        //     indexacion; hay que buscarlo en otro sitio.
        // ====================================================================
        // r46 -- los 4 B de mas de Initialize estan AQUI, en la FORMA DEL ARBOL
        // de decision, no en el codigo de los cases. Objetivo contra nuestro:
        //   obj:  cmpwi 8/beq FIN | cmplwi 8/ble BAJO | <ALTO> | b FIN |
        //         BAJO: cmpwi 3/beq | cmplwi 3/bgt FIN | cmplwi 1/ble FIN |
        //         y CAE por debajo al cuerpo de SHT_SYMTAB           (14 insn)
        //   ntro: cmpwi 8/beq FIN | cmplwi 8/bgt ALTO | <BAJO acabado en
        //         cmpwi 2/beq SYMTAB> | b FIN | <ALTO> | b FIN       (15 insn)
        // Dos cosas a la vez: (a) el objetivo emite el subarbol ALTO en linea y
        // salta al BAJO, nosotros al reves; (b) su hoja del 2 es la forma de
        // RANGO de emit_case_nodes (`LT node->low -> default` = `cmplwi 1;ble`
        // y luego cae al cuerpo), la nuestra es la de VALOR UNICO (`beq` + `b
        // default`), que gasta una instruccion mas.
        // Medido en r46 (8 variantes sobre la unidad + 31 combinaciones en un
        // micro-fichero con los mismos cflags, 0,3 s cada una): NINGUNA emite
        // `ble` detras del `cmplwi 8`. Da IGUAL partir los rangos en cases
        // sueltos (GCC los vuelve a fundir: binario identico), quitar el
        // `default`, ponerlo primero, o usar `case X ... X`.
        //   + case 0 ... 1 (o 0, o 1) ....... 2.364 B / 32 filas (mueve el pivote)
        //   + case 10 / 10..11 / LOPROC..+4 . 2.320 B / 40 filas
        //   + case 12 ... LOPROC-1 .......... 2.336 B / 44 filas
        //   partir 4..7, 5..6 o LOPROC+7.. .. 2.356 B / 28 filas (identico)
        // r47 -- MECANISMO IDENTIFICADO, y no es emit_case_nodes. Reproducido el
        // arbol en un micro de 0,3 s (scratchpad/r47_ecs/micro.py, da nuestras 18
        // insns exactas) y leido stmt.c: con esta lista de `case` la rama que toma
        // emit_case_nodes es la de "neither node is bounded", que emite SIEMPRE
        // `GT high -> test_label`, el subarbol IZQUIERDO en linea, `b default`,
        // `test_label:`, el DERECHO. O sea: lo nuestro es lo unico que stmt.c sabe
        // emitir aqui. El `ble` del objetivo lo pone JUMP.C, la transformacion
        // /* Look for if (foo) bar; else break; */ (jump.c:1831), la unica de GCC
        // 2.95 que INVIERTE el salto y PERMUTA los dos rangos de insns. Condiciones:
        //   insn = condjump a label1, label1 == next_label(insn), NUSES(label1)==1
        //   range1end (ultima insn activa antes de label1) es un SIMPLEJUMP
        //   JUMP_LABEL(range1end) == label2 = next_label(label1)   <-- LA QUE FALTA
        //   range2end es JUMP_INSN seguido de BARRIER, y != range1end
        //   ! first (no en la primera vuelta de jump_optimize)
        // label2 es el cuerpo del PRIMER case en orden de fuente (SHT_SYMTAB). Su
        // BAJO acaba en `b <cuerpo SHT_SYMTAB>`; el nuestro acaba en `cmpwi 2;beq A`
        // (hoja de valor unico: emit_case_nodes solo emite el do_jump_if_equal y
        // CAE) y detras va el `b default` de emit_jump_if_reachable, cuyo JUMP_LABEL
        // es el default. Ese `emit_jump(code_label)` solo sale si el nodo {2} es
        // node_is_bounded (hace falta un ANTECESOR con high==1, o low==TYPE_MIN=0) o
        // si es un RANGO con cota alta (antecesor con low==high+1: solo {3} y {8},
        // luego high=2 -> no es rango, o high=7 -> se solapa con el case 3). Las dos
        // son inalcanzables sin cambiar la semantica: eso cierra el frente.
        // Barridos r47, todos negativos (firma normalizada, scratchpad/r47_ecs/sweep_sw.py):
        //   32 combinaciones break/continue de los 4 case vacios + default -> 4 firmas,
        //      ninguna con `ble`;  8 posiciones del `default:` -> arbol IDENTICO;
        //      6 permutaciones del orden de los case -> identico; case 2 como rango
        //      2..3/2..7 no compilan (solapan); case extra 0/1/0..1 mueven el pivote.
        // COLATERAL con cifra: group_case_nodes funde nodos CONSECUTIVOS cuyas
        // etiquetas llevan al mismo next_real_insn (o a dos saltos simples al mismo
        // sitio). Con `case 4...7` y `case 8` LOS DOS en break se funden en [4,8] y
        // el pivote pasa a {9}; con continue+break no se funden y el pivote es {8}.
        // Es lo que fija la forma entera del arbol.
        // r47 -- las OTRAS 14 filas, medidas:
        //   orden (offset,base) de `add r30,r11,r10` y los cuatro `lwzx/stwx`: la
        //     forma C NO lo toca -- pointer_int_sum (c-typeck.c) normaliza siempre a
        //     (PLUS ptr int); `(ELFSectionHeader*)p + i` y `pHP->symtab + i` dan el
        //     objeto IDENTICO (28 filas). Donde el destino coincide con el primer
        //     operando (add r9,r9,r29) el orden ya casa: lo decide el coalescing.
        //   `&s[nameLength + 1]`: el objetivo hace (s+nameLength)+1 y nosotros
        //     s+(nameLength+1). `strcpy(s + nameLength + 1, ...)`, `&s[nameLength]+1`
        //     y una local `char *tail` dan LAS TRES el mismo objeto: 98,70238 % / 30
        //     filas / 2.356 B -- arregla 4 filas (t->r29 y nameLength->r30, como el
        //     objetivo) y rompe 5 (&s[2] y type_separator se intercambian). No
        //     aplicado, pero la ASOCIATIVIDAD correcta es la del objetivo.
        // El pivote sale de balance_case_nodes: i=(nodos+rangos+1)/2 = 6 con
        // 7 nodos y 3 rangos, y la caminata para en el nodo {8}. Cualquier case
        // nuevo por debajo de 8 lo mueve. Base = 28 filas, es el mejor conocido.
        // r60 -- LA CAUSA ES stmt.c, NO jump.c, Y ES ESTRUCTURAL. La r47 nombraba la
        // condicion que falta en jump.c (JUMP_LABEL(range1end) == label2); esta ronda
        // dice POR QUE falta, y con eso el frente queda CERRADO con prueba.
        // El objetivo emite la hoja del nodo {2} como
        //     cmplwi r0,1 ; ble <default>      <- `LT 2 -> default` (LTU C -> LEU C-1)
        //     <CAIDA al cuerpo de SHT_SYMTAB>
        // y en emit_case_nodes (stmt.c:5875) esa forma sale de UN SOLO sitio: la rama
        // de RANGO (`node->low != node->high`), que acaba en `emit_jump(code_label)`.
        // Ese `emit_jump` es lo que convierte a range1end en un simplejump a label2 y
        // dispara la permutacion de jump.c; luego jump.c se lo come por ser salto al
        // siguiente. UNA causa explica las DOS diferencias (el `ble` y la instruccion
        // de mas). Nuestro {2} es de VALOR UNICO: add_case_node hace `r->high = r->low`
        // cuando los limites son iguales, asi que emite `do_jump_if_equal` + el
        // `b default` de emit_jump_if_reachable: 3 instrucciones contra 2.
        // Para que {2} sea RANGO hace falta low=2 (lo fija el `cmplwi 1`) y high>2 con
        // node_has_high_bound, o sea un ANTECESOR con low==high+1. Los antecesores de
        // {2} son {3} (low 3) y la raiz {8} (low 8) -> high=2 (imposible: seria valor
        // unico) o high=7 (imposible: se solapa con {3} y con [4,7]).
        // La otra forma que sale igual es que {2} cuelgue de un RANGO acabado en 1:
        // arbol {3} -> izq [0,1] -> der {2}; entonces [0,1] emite `LE 1 -> su cuerpo`
        // y {2} es node_is_bounded -> `emit_jump` (borrado). PERO balance_case_nodes
        // no lo construye: con la lista [0,1],{2},{3},[4,7] da i=(4 nodos+2 rangos+1)/2
        // = 3 y parte en {2}, no en {3}; para partir en {3} harian falta 3 rangos entre
        // esos cuatro nodos, y {2} y {3} no pueden serlo (no hay valores libres).
        // Y no vale meter nodos: el hijo DERECHO de {3} tiene que ser [4,7] SOLO para
        // que node_is_bounded le de el `bgt` directo.
        // BARRIDO r60, 161 combinaciones de case ANADIDOS semanticamente neutros
        // (SHT_NULL / SHT_PROGBITS / [0,1] x SHLIB / DYNSYM / [10,11] / LOPROC+k ...,
        // con break y con continue, medidas con micro.py sobre este .cpp): NINGUNA baja
        // de 28 filas ni de 2.356 B. Las que restauran el pivote {8} vuelven a 28
        // exactas -- la aritmetica de balance_case_nodes esta comprobada asi.
        // DOS medidas mas que valen para el que vuelva:
        //   `case SHT_PROGBITS ... SHT_SYMTAB:` (nodo [1,2]) da 2.352 B EXACTOS y baja
        //   a 15 filas -- la unica diferencia del arbol pasa a ser `blt` contra `ble`,
        //   porque low=1 y no 2. NO APLICADO: cambia la semantica (un PROGBITS entraria
        //   por el cuerpo de SHT_SYMTAB) y sigue sin casar.
        //   `case SHT_NULL ... SHT_SYMTAB:` da 2.344 B (low==TYPE_MIN quita el test).
        // O sea: los 4 B de .text de zEagl4Anim son ESTA instruccion, y ademas quedan
        // 14 filas de reparto/asociatividad que la r47 ya documenta aqui arriba.
        switch (sheader->sh_type) {

        case SHT_SYMTAB:

            sheader->sh_vlink = reinterpret_cast<ELFSectionHeader *>(p)[sheader->sh_link].sh_voffset;

            if (strcmp(".symtab", &shstrtab[sheader->sh_name]) == 0) {

                pHP->symtab = reinterpret_cast<ELF32_Sym *>(sheader->sh_voffset);
                pHP->symbols_num = static_cast<int>(sheader->sh_size) / static_cast<int>(sizeof(ELF32_Sym));
            }
            break;

        case SHT_STRTAB:

            if (strcmp(".strtab", &shstrtab[sheader->sh_name]) == 0) {

                int len;
                char *s;
                int slen;

                pHP->strtab = reinterpret_cast<char *>(sheader->sh_voffset);
                s = pHP->strtab;
                len = sheader->sh_size;

                while (len > 0) {

                    const char *TYPE_SEPARATOR = ":::";

                    slen = strlen(s);

                    if (s[0] == '_' && s[1] == '_') {

                        char *tail = &s[2];
                        char *type_separator = strstr(tail, TYPE_SEPARATOR);
                        char typebuf[128];

                        if (type_separator) {

                            *type_separator = 0;
                            strcpy(typebuf, tail);

                            tail = type_separator + strlen(TYPE_SEPARATOR);

                            unsigned int nameLength = strlen(tail) + 1;

                            memmove(s, tail, nameLength);
                            tail = &s[nameLength];
                            s[nameLength] = 0x7F;
                            strcpy(tail + 1, typebuf);
                        }
                    }

                    len -= slen + 1;
                    s += slen + 1;
                }
            }
            break;

        case SHT_REL:

            sheader->sh_vinfo = reinterpret_cast<ELFSectionHeader *>(p)[sheader->sh_info].sh_voffset;
            sheader->sh_vlink = reinterpret_cast<ELFSectionHeader *>(p)[sheader->sh_link].sh_voffset;
            break;

        case SHT_NULL:
        case SHT_PROGBITS:
            break;

        case SHT_RELA:
            break;

        case SHT_HASH:
        case SHT_DYNAMIC:
        case SHT_NOTE:
            continue;

        case SHT_NOBITS:
            break;

        case SHT_SHLIB:
        case SHT_DYNSYM:
            break;

        case SHT_LOPROC + 5:
        case SHT_LOPROC + 6:
            break;

        case SHT_HIPROC:
        case SHT_LOUSER:
        case SHT_HIUSER:
            continue;

        default:
            break;
        }
    }

    pHP->pSearchFunction = pSearchFunction;

    for (i = 0; i < 256; i++) {
        pHP->hash[i] = -1;
    }

    pHP->chain = reinterpret_cast<long unsigned int *>(EAGL4Internal::EAGL4Malloc(pHP->symbols_num * sizeof(*pHP->chain), "EAGL4::dynamic symbols"));
    pHP->isOriginal = reinterpret_cast<bool *>(EAGL4Internal::EAGL4Malloc(pHP->symbols_num * sizeof(*pHP->isOriginal), "EAGL4::dynamic symbols 2"));

    {
        ELF32_Sym *sym;
        int j;

        for (i = 0; i < pHP->symbols_num; i++) {

            sym = reinterpret_cast<ELF32_Sym *>(i * sizeof(ELF32_Sym) + reinterpret_cast<unsigned int>(pHP->symtab));

            sym->st_name = htotul(sym->st_name);
            sym->st_value = htotul(sym->st_value);
            sym->st_size = htotul(sym->st_size);
            sym->st_shndx = htotus(sym->st_shndx);

            j = elfhash(&pHP->strtab[sym->st_name]);

            pHP->chain[i] = pHP->hash[j];
            pHP->hash[j] = i;

            if (sym->st_shndx > 0 && sym->st_shndx < e->e_shnum) {
                pHP->isOriginal[i] = true;
            } else {
                pHP->isOriginal[i] = false;
            }
        }
    }

    pHP->next = hashhead;

    if (hashhead) {
        hashhead->prev = pHP;
    }

    pHP->prev = nullptr;
    hashhead = pHP;

    handle = pHP;
}

int DynamicLoader::GetCount() const {
    if (!handle) {
        return 0;
    }
    HashPointer *h = reinterpret_cast<HashPointer *>(handle);
    return h->symbols_num;
}

// TODO
DynamicLoader::Symbol DynamicLoader::GetSymbol(int i) const {
    DynamicLoader::Symbol r;

    r.name = nullptr;
    r.data = nullptr;

    HashPointer *h = reinterpret_cast<HashPointer *>(handle);
    if (!h) {
        return r;
    }
    ELF32_Sym *s = h->symtab;
    if (i < 0 || i >= h->symbols_num) {
        return r;
    }
    r.name = &h->strtab[s[i].st_name];
    r.type = &h->strtab[s[i].st_name] + strlen(r.name);
    r.type++;
    if (r.type[0] == 0x7F) {
        r.type++;
    } else {
        r.type--;
    }
    r.isInternalRef = static_cast<unsigned int>(s[i].st_other - 2) > 3;

    int iIndex = s[i].st_shndx;
    if (s[i].st_other == 1) {
        r.data = reinterpret_cast<void *>(s[i].st_value);
    } else if (iIndex > 0 && iIndex < h->e->e_shnum) {
        r.data = reinterpret_cast<void *>(h->sections[iIndex].sh_offset + s[i].st_value);
    }

    return r;
}

}; // namespace EAGL4

static void *dlsym(void *handle, const char *name) {
    EAGL4::HashPointer *h = reinterpret_cast<EAGL4::HashPointer *>(handle);
    EAGL4::ELF32_Sym *s = h->symtab;
    unsigned long j = h->hash[EAGL4::elfhash(name)];

    while (j != -1) {
        if (h->isOriginal[j] && strcmp(name, &h->strtab[s[j].st_name]) == 0) {
            int iIndex = s[j].st_shndx;
            if (iIndex > 0 && iIndex < h->e->e_shnum) {
                return reinterpret_cast<void *>(h->sections[iIndex].sh_offset + s[j].st_value);
            }
        }
        j = h->chain[j];
    }
    return nullptr;
}

namespace EAGL4 {

bool DynamicLoader::GetNextSymbol(const char *type, int &iIndex, Symbol &result) const {
    for (; iIndex < GetCount(); iIndex++) {
        Symbol s = GetSymbol(iIndex);
        if (strcmp(type, s.type) == 0) {
            result = s;
            iIndex++;
            return true;
        }
    }
    return false;
}

bool DynamicLoader::GetNextAddr(const char *type, int &iIndex, void *&addr) const {
    Symbol s;
    if (GetNextSymbol(type, iIndex, s)) {
        addr = s.data;
        return true;
    } else {
        return false;
    }
}

}; // namespace EAGL4
