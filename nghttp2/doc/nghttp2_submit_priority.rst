
nghttp2_submit_priority
=======================

Synopsis
--------

*#include <nghttp2/nghttp2.h>*

.. function:: int nghttp2_submit_priority(nghttp2_session *session, uint8_t flags, int32_t stream_id, const nghttp2_priority_spec *pri_spec)

    
    Submits PRIORITY frame to change the priority of stream *stream_id*
    to the priority specification *pri_spec*.
    
    The *flags* is currently ignored and should be
    :macro:`nghttp2_flag.NGHTTP2_FLAG_NONE`.
    
    The *pri_spec* is priority specification of this request.  ``NULL``
    is not allowed for this function. To specify the priority, use
    `nghttp2_priority_spec_init()`.  This function will copy its data
    members.
    
    The ``pri_spec->weight`` must be in [:macro:`NGHTTP2_MIN_WEIGHT`,
    :macro:`NGHTTP2_MAX_WEIGHT`], inclusive.  If ``pri_spec->weight``
    is strictly less than :macro:`NGHTTP2_MIN_WEIGHT`, it becomes
    :macro:`NGHTTP2_MIN_WEIGHT`.  If it is strictly greater than
    :macro:`NGHTTP2_MAX_WEIGHT`, it becomes
    :macro:`NGHTTP2_MAX_WEIGHT`.
    
    This function returns 0 if it succeeds, or one of the following
    negative error codes:
    
    :macro:`nghttp2_error.NGHTTP2_ERR_NOMEM`
        Out of memory.
    :macro:`nghttp2_error.NGHTTP2_ERR_INVALID_ARGUMENT`
        The *stream_id* is 0; or the *pri_spec* is NULL; or trying to
        depend on itself.
