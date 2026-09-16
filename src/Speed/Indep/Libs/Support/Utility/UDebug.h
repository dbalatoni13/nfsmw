#ifndef UDEBUG_H
#define UDEBUG_H // Decl: 2

extern void (*UFoundation_AssertMessage)(const char *, ...); // Decl: 5

#define ABSTRACT_BASE __declspec(novtable) // Decl: 68
#define DEBUG_VALUE 0                      // Decl: 69
#define DBGPrintf(_a_)                     // Decl: 70
#define DBGPrintfIf(_a_, _b_)              // Decl: 71

#define RELPrintf(_a_)        // Decl: 78
#define RELPrintfIf(_a_, _b_) // Decl: 79

// TODO fake
#undef ASSERT

// Decl: 87
#define ASSERT(exp)                                                                                                                                  \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 88
#define ASSERT_MESSAGE(exp, msg)                                                                                                                     \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 89
#define ASSERT_MESSAGE1(exp, fmt, arg1)                                                                                                              \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 90
#define ASSERT_MESSAGE2(exp, fmt, arg1, arg2)                                                                                                        \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 91
#define ASSERT_MESSAGE3(exp, fmt, arg1, arg2, arg3)                                                                                                  \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 92
#define ASSERT_MESSAGE4(exp, fmt, arg1, arg2, arg3, arg4)                                                                                            \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 93
#define ABORT_MESSAGE(msg)                                                                                                                           \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 94
#define ABORT_MESSAGE1(fmt, arg1)                                                                                                                    \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 95
#define ABORT_MESSAGE2(fmt, arg1, arg2)                                                                                                              \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 96
#define ABORT_MESSAGE3(fmt, arg1, arg2, arg3)                                                                                                        \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }
// Decl: 97
#define ABORT_MESSAGE4(fmt, arg1, arg2, arg3, arg4)                                                                                                  \
    {                                                                                                                                                \
        (void)0;                                                                                                                                     \
    }

#endif
