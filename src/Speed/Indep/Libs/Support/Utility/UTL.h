//
//
#ifndef UTL_H
#define UTL_H // Decl: 4

#define UTL_INNER_H // Decl: 38

// Decl: 41
#define DECLAREHANDLE(_h_)                                                                                                                           \
    struct _h_##__ {                                                                                                                                 \
        int unused;                                                                                                                                  \
    };                                                                                                                                               \
    typedef struct _h_##__ *_h_;

#define UTL_ALIGNMENT __attribute__((aligned(16))) // Decl: 50
#undef UTL_ASSERT                                  // Decl: 67
#define UTL_ASSERT(_a_)                            // Decl: 68

#define UTL_PRINT_ALLOC(_Size_, _Count_, _Name_)   // Decl: 85
#define UTL_PRINT_DEALLOC(_Size_, _Count_, _Name_) // Decl: 86

#define UTL_WARN(_condition_, _who_, _string_) // Decl: 90

#define UTL_TEMPLATE_IMPL template <> // Decl: 99

#define UTL_VERSION 0x00010010 // Decl: 104

#undef UTL_INNER_H // Decl: 128

#endif
