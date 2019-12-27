
nghttp2_session_get_root_stream
===============================

Synopsis
--------

*#include <nghttp2/nghttp2.h>*

.. function:: nghttp2_stream * nghttp2_session_get_root_stream(nghttp2_session *session)

    
    Returns root of dependency tree, which is imaginary stream with
    stream ID 0.  The returned pointer is valid until *session* is
    freed by `nghttp2_session_del()`.
