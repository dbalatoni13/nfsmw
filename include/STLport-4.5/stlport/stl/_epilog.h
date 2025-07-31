/* NOTE : this header has no guards and is MEANT for multiple inclusion !
 * If you are using "header protection" option with your compiler,
 * please also find #pragma which disables it and put it here, to
 * allow reentrancy of this header.
 */

/* If the platform provides any specific epilog actions,
   like #pragmas, do include platform-specific prolog file */
# if defined (_STLP_HAS_SPECIFIC_PROLOG_EPILOG)
#  include <config/_epilog.h>
# endif

# ifndef _STLP_NO_POST_COMPATIBLE_SECTION
#  include <stl/_config_compat_post.h>
# endif

# if defined (_STLP_REDEFINE_STD) || ! defined (_STLP_USE_NAMESPACES)
/* We redefine "std" to "stlport", so that user code may use std:: transparently */
#  undef  std
#  define std STLPORT
# endif
