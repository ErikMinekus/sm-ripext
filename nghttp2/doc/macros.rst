
Macros
======
.. macro:: NGHTTP2_VERSION

    Version number of the nghttp2 library release
.. macro:: NGHTTP2_VERSION_NUM

    Numerical representation of the version number of the nghttp2 library
    release. This is a 24 bit number with 8 bits for major number, 8 bits
    for minor and 8 bits for patch. Version 1.2.3 becomes 0x010203.
.. macro:: NGHTTP2_PROTO_VERSION_ID

    
    The protocol version identification string of this library
    supports.  This identifier is used if HTTP/2 is used over TLS.
.. macro:: NGHTTP2_PROTO_VERSION_ID_LEN

    
    The length of :macro:`NGHTTP2_PROTO_VERSION_ID`.
.. macro:: NGHTTP2_PROTO_ALPN

    
    The serialized form of ALPN protocol identifier this library
    supports.  Notice that first byte is the length of following
    protocol identifier.  This is the same wire format of `TLS ALPN
    extension <https://tools.ietf.org/html/rfc7301>`_.  This is useful
    to process incoming ALPN tokens in wire format.
.. macro:: NGHTTP2_PROTO_ALPN_LEN

    
    The length of :macro:`NGHTTP2_PROTO_ALPN`.
.. macro:: NGHTTP2_CLEARTEXT_PROTO_VERSION_ID

    
    The protocol version identification string of this library
    supports.  This identifier is used if HTTP/2 is used over cleartext
    TCP.
.. macro:: NGHTTP2_CLEARTEXT_PROTO_VERSION_ID_LEN

    
    The length of :macro:`NGHTTP2_CLEARTEXT_PROTO_VERSION_ID`.
.. macro:: NGHTTP2_VERSION_AGE

    
    The age of :type:`nghttp2_info`
.. macro:: NGHTTP2_DEFAULT_WEIGHT

    
    The default weight of stream dependency.
.. macro:: NGHTTP2_MAX_WEIGHT

    
    The maximum weight of stream dependency.
.. macro:: NGHTTP2_MIN_WEIGHT

    
    The minimum weight of stream dependency.
.. macro:: NGHTTP2_MAX_WINDOW_SIZE

    
    The maximum window size
.. macro:: NGHTTP2_INITIAL_WINDOW_SIZE

    
    The initial window size for stream level flow control.
.. macro:: NGHTTP2_INITIAL_CONNECTION_WINDOW_SIZE

    
    The initial window size for connection level flow control.
.. macro:: NGHTTP2_DEFAULT_HEADER_TABLE_SIZE

    
    The default header table size.
.. macro:: NGHTTP2_CLIENT_MAGIC

    
    The client magic string, which is the first 24 bytes byte string of
    client connection preface.
.. macro:: NGHTTP2_CLIENT_MAGIC_LEN

    
    The length of :macro:`NGHTTP2_CLIENT_MAGIC`.
.. macro:: NGHTTP2_DEFAULT_MAX_SETTINGS

    
    The default max number of settings per SETTINGS frame
.. macro:: NGHTTP2_INITIAL_MAX_CONCURRENT_STREAMS

    
    .. warning::
    
      Deprecated.  The initial max concurrent streams is 0xffffffffu.
    
    Default maximum number of incoming concurrent streams.  Use
    `nghttp2_submit_settings()` with
    :macro:`nghttp2_settings_id.NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS`
    to change the maximum number of incoming concurrent streams.
    
    .. note::
    
      The maximum number of outgoing concurrent streams is 100 by
      default.
