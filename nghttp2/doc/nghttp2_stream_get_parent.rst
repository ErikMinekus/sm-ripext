
nghttp2_stream_get_parent
=========================

Synopsis
--------

*#include <nghttp2/nghttp2.h>*

.. function:: nghttp2_stream * nghttp2_stream_get_parent(nghttp2_stream *stream)

    
    Returns the parent stream of *stream* in dependency tree.  Returns
    NULL if there is no such stream.
