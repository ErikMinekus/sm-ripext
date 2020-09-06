#ifdef __linux__
# include "jansson_config-linux.h"
#elif defined(__APPLE__)
# include "jansson_config-mac.h"
#elif defined(WIN32)
# include "jansson_config-windows.h"
#endif

#ifndef JANSSON_USING_CMAKE /* disabled if using cmake */
# if JSON_INTEGER_IS_LONG_LONG
#  ifdef _MSC_VER  /* Microsoft Visual Studio */
#   define json_strtoint     _strtoi64
#  else
#   define json_strtoint     strtoll
#  endif
# else
#  define json_strtoint     strtol
# endif
#endif
