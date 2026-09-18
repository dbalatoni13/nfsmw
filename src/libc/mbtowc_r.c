/* mbtowc_r.c -- newlib.  Como math_support.c, esta unidad la compila el JUEGO
 * (pool de constantes en .rodata con `lis @ha`), no viene de la libm de SN. */
#include <stddef.h>
#include <reent.h>

extern int strcmp(const char *, const char *);
extern unsigned int strlen(const char *);

#define ESC_CHAR 0x1b

#define _issjis1(c) ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xef))
#define _issjis2(c) ((c >= 0x40 && c <= 0x7e) || (c >= 0x80 && c <= 0xfc))
#define _iseucjp(c) (c >= 0xa1 && c <= 0xfe)
#define _isjis(c) (c >= 0x21 && c <= 0x7e)

typedef enum {
    ESCAPE,
    DOLLAR,
    BRACKET,
    AT,
    B,
    J,
    NUL,
    JIS_CHAR,
    OTHER,
    JIS_C_NUM
} JIS_CHAR_TYPE;

typedef enum {
    ASCII,
    A_ESC,
    A_ESC_DL,
    JIS,
    JIS_1,
    JIS_2,
    J_ESC,
    J_ESC_BR,
    J2_ESC,
    J2_ESC_BR,
    DONE,
    INV,
    JIS_S_NUM
} JIS_STATE;

typedef enum {
    COPY_A,
    COPY_J1,
    COPY_J2,
    MAKE_J,
    MAKE_A,
    NOOP,
    EMPTY,
    ERROR
} JIS_ACTION;

/* JIS state machine: newlib solo inicializa las filas con contenido; las de
 * DONE e INV quedan a cero. */
static JIS_STATE JIS_state_table[JIS_S_NUM][JIS_C_NUM] = {
/*               ESCAPE     DOLLAR     BRACKET    AT         B          J          NUL        JIS_CHAR   OTHER */
/* ASCII      */ { A_ESC,     DONE,      DONE,      DONE,      DONE,      DONE,      DONE,      DONE,      DONE },
/* A_ESC      */ { DONE,      A_ESC_DL,  DONE,      DONE,      DONE,      DONE,      DONE,      DONE,      DONE },
/* A_ESC_DL   */ { DONE,      DONE,      DONE,      JIS,       JIS,       DONE,      DONE,      DONE,      DONE },
/* JIS        */ { J_ESC,     JIS_1,     JIS_1,     JIS_1,     JIS_1,     JIS_1,     INV,       JIS_1,     INV },
/* JIS_1      */ { INV,       JIS_2,     JIS_2,     JIS_2,     JIS_2,     JIS_2,     INV,       JIS_2,     INV },
/* JIS_2      */ { J2_ESC,    DONE,      DONE,      DONE,      DONE,      DONE,      INV,       DONE,      DONE },
/* J_ESC      */ { INV,       INV,       J_ESC_BR,  INV,       INV,       INV,       INV,       INV,       INV },
/* J_ESC_BR   */ { INV,       INV,       INV,       INV,       ASCII,     ASCII,     INV,       INV,       INV },
/* J2_ESC     */ { INV,       INV,       J2_ESC_BR, INV,       INV,       INV,       INV,       INV,       INV },
/* J2_ESC_BR  */ { INV,       INV,       INV,       INV,       DONE,      DONE,      INV,       INV,       INV },
};

static JIS_ACTION JIS_action_table[JIS_S_NUM][JIS_C_NUM] = {
/*               ESCAPE    DOLLAR    BRACKET   AT        B         J         NUL       JIS_CHAR  OTHER */
/* ASCII      */ { NOOP,     COPY_A,   COPY_A,   COPY_A,   COPY_A,   COPY_A,   EMPTY,    COPY_A,   COPY_A },
/* A_ESC      */ { COPY_A,   NOOP,     COPY_A,   COPY_A,   COPY_A,   COPY_A,   COPY_A,   COPY_A,   COPY_A },
/* A_ESC_DL   */ { COPY_A,   COPY_A,   COPY_A,   MAKE_A,   MAKE_A,   COPY_A,   COPY_A,   COPY_A,   COPY_A },
/* JIS        */ { NOOP,     NOOP,     NOOP,     NOOP,     NOOP,     NOOP,     ERROR,    NOOP,     ERROR },
/* JIS_1      */ { ERROR,    NOOP,     NOOP,     NOOP,     NOOP,     NOOP,     ERROR,    NOOP,     ERROR },
/* JIS_2      */ { NOOP,     COPY_J2,  COPY_J2,  COPY_J2,  COPY_J2,  COPY_J2,  ERROR,    COPY_J2,  COPY_J2 },
/* J_ESC      */ { ERROR,    ERROR,    NOOP,     ERROR,    ERROR,    ERROR,    ERROR,    ERROR,    ERROR },
/* J_ESC_BR   */ { ERROR,    ERROR,    ERROR,    ERROR,    NOOP,     NOOP,     ERROR,    ERROR,    ERROR },
/* J2_ESC     */ { ERROR,    ERROR,    NOOP,     ERROR,    ERROR,    ERROR,    ERROR,    ERROR,    ERROR },
/* J2_ESC_BR  */ { ERROR,    ERROR,    ERROR,    ERROR,    COPY_J1,  COPY_J1,  ERROR,    ERROR,    ERROR },
};

int _mbtowc_r(struct _reent *r, wchar_t *pwc, const char *s, size_t n,
              int *state) {
    wchar_t dummy;
    unsigned char *t = (unsigned char *)s;

    if (pwc == NULL)
        pwc = &dummy;

    if (s != NULL && n == 0)
        return -1;

    if (r->_current_locale == NULL || (strlen(r->_current_locale) <= 1)) {
        /* fall-through */
    } else if (!strcmp(r->_current_locale, "C-SJIS")) {
        int char1 = *t;
        if (s == NULL)
            return 0;
        if (_issjis1(char1)) {
            int char2 = t[1];
            if (n <= 1)
                return -1;
            if (_issjis2(char2)) {
                *pwc = (((wchar_t)*t) << 8) + (wchar_t)(*(t + 1));
                return 2;
            } else
                return -1;
        }
    } else if (!strcmp(r->_current_locale, "C-EUCJP")) {
        int char1 = *t;
        if (s == NULL)
            return 0;
        if (_iseucjp(char1)) {
            int char2 = t[1];
            if (n <= 1)
                return -1;
            if (_iseucjp(char2)) {
                *pwc = (((wchar_t)*t) << 8) + (wchar_t)(*(t + 1));
                return 2;
            } else
                return -1;
        }
    } else if (!strcmp(r->_current_locale, "C-JIS")) {
        JIS_STATE curr_state;
        JIS_ACTION action;
        JIS_CHAR_TYPE ch;
        unsigned char *ptr;
        unsigned int i;
        int curr_ch;

        if (s == NULL) {
            *state = 0;
            return 1;
        }

        curr_state = (*state == 0 ? ASCII : JIS);
        ptr = t;

        for (i = 0; i < n; i++) {
            curr_ch = t[i];
            switch (curr_ch) {
            case ESC_CHAR:
                ch = ESCAPE;
                break;
            case '$':
                ch = DOLLAR;
                break;
            case '@':
                ch = AT;
                break;
            case '(':
                ch = BRACKET;
                break;
            case 'B':
                ch = B;
                break;
            case 'J':
                ch = J;
                break;
            case '\0':
                ch = NUL;
                break;
            default:
                if (_isjis(curr_ch))
                    ch = JIS_CHAR;
                else
                    ch = OTHER;
            }

            action = JIS_action_table[curr_state][ch];
            curr_state = JIS_state_table[curr_state][ch];

            switch (action) {
            case EMPTY:
                *state = 0;
                *pwc = (wchar_t)0;
                return i;
            case COPY_A:
                *state = 0;
                *pwc = (wchar_t)*ptr;
                return (i + 1);
            case COPY_J1:
                *state = 0;
                *pwc = (((wchar_t)*ptr) << 8) + (wchar_t)(*(ptr + 1));
                return (i + 1);
            case COPY_J2:
                *state = 1;
                *pwc = (((wchar_t)*ptr) << 8) + (wchar_t)(*(ptr + 1));
                return (ptr - t + 2);
            case MAKE_J:
            case MAKE_A:
                ptr = (unsigned char *)(t + i + 1);
                break;
            case NOOP:
                break;
            case ERROR:
            default:
                return -1;
            }
        }
        return -1;
    }

    if (s == NULL)
        return 0;

    *pwc = (wchar_t)*t;

    if (*t == '\0')
        return 0;

    return 1;
}
