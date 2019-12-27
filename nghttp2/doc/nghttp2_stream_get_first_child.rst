
nghttp2_stream_get_first_child
==============================

Synopsis
--------

*#include <nghttp2/nghttp2.h>*

.. function:: nghttp2_stream * nghttp2_stream_get_first_child(nghttp2_stream *stream)

    
    Returns the first child stream of *stream* in dependency tree.
    Returns NULL if there is no such stream.
