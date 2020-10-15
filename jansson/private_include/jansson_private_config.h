#ifdef __linux__
# include "jansson_private_config-linux.h"
#elif defined(__APPLE__)
# include "jansson_private_config-mac.h"
#elif defined(WIN32)
# include "jansson_private_config-windows.h"
#endif
