
nghttp2_option_set_no_closed_streams
====================================

Synopsis
--------

*#include <nghttp2/nghttp2.h>*

.. function:: void nghttp2_option_set_no_closed_streams(nghttp2_option *option, int val)

    
    This option prevents the library from retaining closed streams to
    maintain the priority tree.  If this option is set to nonzero,
    applications can discard closed stream completely to save memory.
