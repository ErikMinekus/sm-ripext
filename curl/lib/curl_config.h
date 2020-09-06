#ifdef __linux__
# include "curl_config-linux.h"
#elif defined(__APPLE__)
# include "curl_config-mac.h"
#elif defined(WIN32)
# include "curl_config-windows.h"
#endif
