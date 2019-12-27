
nghttp2_stream_get_next_sibling
===============================

Synopsis
--------

*#include <nghttp2/nghttp2.h>*

.. function:: nghttp2_stream * nghttp2_stream_get_next_sibling(nghttp2_stream *stream)

    
    Returns the next sibling stream of *stream* in dependency tree.
    Returns NULL if there is no such stream.
