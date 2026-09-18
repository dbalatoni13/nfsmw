/* locale.c -- newlib 1.8 con MB_CAPABLE, tal cual.  _setlocale_r y setlocale
 * no llegan al binario (nadie las llama), pero sus cadenas si: la .rodata de
 * la unidad es lconv, "", ".", "C", "C-JIS", "C-EUCJP" y "C-SJIS". */
#include <locale.h>
#include <string.h>
#include <limits.h>
#include <reent.h>

int __mb_cur_max = 1;

static _CONST struct lconv lconv =
{
  ".", "", "", "", "", "", "", "", "", "",
  CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
  CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
};


char *
_DEFUN(_setlocale_r, (p, category, locale),
       struct _reent *p _AND
       int category _AND
       _CONST char *locale)
{
  if (locale)
    {
      if (strcmp (locale, "C") && strcmp (locale, "") &&
          strcmp (locale, "C-JIS") && strcmp (locale, "C-EUCJP") &&
          strcmp (locale, "C-SJIS"))
        return 0;

      if (!strcmp (locale, "C-JIS"))
        __mb_cur_max = 8;
      else if (strlen (locale) > 1)
        __mb_cur_max = 2;
      else
        __mb_cur_max = 1;

      p->_current_category = category;
      p->_current_locale = locale;
    }
  return "C";
}


struct lconv *
_DEFUN(_localeconv_r, (data),
      struct _reent *data)
{
  return (struct lconv *) &lconv;
}

#ifndef _REENT_ONLY

char *
_DEFUN(setlocale, (category, locale),
       int category _AND
       _CONST char *locale)
{
  return _setlocale_r (_REENT, category, locale);
}


struct lconv *
_DEFUN_VOID(localeconv)
{
  return _localeconv_r (_REENT);
}

#endif
