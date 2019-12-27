
nghttp2_priority_spec_init
==========================

Synopsis
--------

*#include <nghttp2/nghttp2.h>*

.. function:: void nghttp2_priority_spec_init(nghttp2_priority_spec *pri_spec, int32_t stream_id, int32_t weight, int exclusive)

    
    Initializes *pri_spec* with the *stream_id* of the stream to depend
    on with *weight* and its exclusive flag.  If *exclusive* is
    nonzero, exclusive flag is set.
    
    The *weight* must be in [:macro:`NGHTTP2_MIN_WEIGHT`,
    :macro:`NGHTTP2_MAX_WEIGHT`], inclusive.
