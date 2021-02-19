
nghttp2_session_change_stream_priority
======================================

Synopsis
--------

*#include <nghttp2/nghttp2.h>*

.. function:: int nghttp2_session_change_stream_priority(nghttp2_session *session, int32_t stream_id, const nghttp2_priority_spec *pri_spec)

    
    Changes priority of existing stream denoted by *stream_id*.  The
    new priority specification is *pri_spec*.
    
    The priority is changed silently and instantly, and no PRIORITY
    frame will be sent to notify the peer of this change.  This
    function may be useful for server to change the priority of pushed
    stream.
    
    If *session* is initialized as server, and ``pri_spec->stream_id``
    points to the idle stream, the idle stream is created if it does
    not exist.  The created idle stream will depend on root stream
    (stream 0) with weight 16.
    
    Otherwise, if stream denoted by ``pri_spec->stream_id`` is not
    found, we use default priority instead of given *pri_spec*.  That
    is make stream depend on root stream with weight 16.
    
    This function returns 0 if it succeeds, or one of the following
    negative error codes:
    
    :macro:`nghttp2_error.NGHTTP2_ERR_NOMEM`
        Out of memory.
    :macro:`nghttp2_error.NGHTTP2_ERR_INVALID_ARGUMENT`
        Attempted to depend on itself; or no stream exist for the given
        *stream_id*; or *stream_id* is 0
