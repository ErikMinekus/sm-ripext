
nghttp2_session_callbacks_set_send_data_callback
================================================

Synopsis
--------

*#include <nghttp2/nghttp2.h>*

.. function:: void nghttp2_session_callbacks_set_send_data_callback( nghttp2_session_callbacks *cbs, nghttp2_send_data_callback send_data_callback)

    
    Sets callback function invoked when
    :macro:`nghttp2_data_flag.NGHTTP2_DATA_FLAG_NO_COPY` is used in
    :type:`nghttp2_data_source_read_callback` to avoid data copy.
