#ifndef SUPPORT_MISC_STRINGHASH_H
#define SUPPORT_MISC_STRINGHASH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// clang-format off
template<int c00=0, int c01=0, int c02=0, int c03=0, int c04=0, int c05=0, int c06=0, int c07=0, 
            int c08=0, int c09=0, int c10=0, int c11=0, int c12=0, int c13=0, int c14=0, int c15=0, 
            int c16=0, int c17=0, int c18=0, int c19=0, int c20=0, int c21=0, int c22=0, int c23=0, 
            int c24=0, int c25=0, int c26=0, int c27=0, int c28=0, int c29=0, int c30=0, int c31=0>
class BinHash
{
private:
    #define M(x) (x) // This is a placeholder for modification of x.
    #define H(x, h) (h * 33 + x)
    enum {
        V00=0xFFFFFFFF,
        V01=H(M(c00),V00), V02=H(M(c01),V01), V03=H(M(c02),V02), V04=H(M(c03),V03), V05=H(M(c04),V04),
        V06=H(M(c05),V05), V07=H(M(c06),V06), V08=H(M(c07),V07), V09=H(M(c08),V08), V10=H(M(c09),V09),
        V11=H(M(c10),V10), V12=H(M(c11),V11), V13=H(M(c12),V12), V14=H(M(c13),V13), V15=H(M(c14),V14),
        V16=H(M(c15),V15), V17=H(M(c16),V16), V18=H(M(c17),V17), V19=H(M(c18),V18), V20=H(M(c19),V19),
        V21=H(M(c20),V20), V22=H(M(c21),V21), V23=H(M(c22),V22), V24=H(M(c23),V23), V25=H(M(c24),V24),
        V26=H(M(c25),V25), V27=H(M(c26),V26), V28=H(M(c27),V27), V29=H(M(c28),V28), V30=H(M(c29),V29),
        V31=H(M(c30),V30), V32=H(M(c31),V31)
    };

public:
    static const unsigned int value = (unsigned int)
        ((c00==0)?V00:((c01==0)?V01:((c02==0)?V02:((c03==0)?V03:((c04==0)?V04:((c05==0)?V05:
        ((c06==0)?V06:((c07==0)?V07:((c08==0)?V08:((c09==0)?V09:((c10==0)?V10:((c11==0)?V11:
        ((c12==0)?V12:((c13==0)?V13:((c14==0)?V14:((c15==0)?V15:((c16==0)?V16:((c17==0)?V17:
        ((c18==0)?V18:((c19==0)?V19:((c20==0)?V20:((c21==0)?V21:((c22==0)?V22:((c23==0)?V23:
        ((c24==0)?V24:((c25==0)?V25:((c26==0)?V26:((c27==0)?V27:((c28==0)?V28:((c29==0)?V29:
        ((c30==0)?V30:((c31==0)?V31:V32))))))))))))))))))))))))))))))));
};

// clang-format on

#endif
