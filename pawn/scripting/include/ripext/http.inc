enum HTTPStatus
{
	HTTPStatus_Invalid = 0,

	// 1xx Informational
	HTTPStatus_Continue = 100,
	HTTPStatus_SwitchingProtocols = 101,

	// 2xx Success
	HTTPStatus_OK = 200,
	HTTPStatus_Created = 201,
	HTTPStatus_Accepted = 202,
	HTTPStatus_NonAuthoritativeInformation = 203,
	HTTPStatus_NoContent = 204,
	HTTPStatus_ResetContent = 205,
	HTTPStatus_PartialContent = 206,

	// 3xx Redirection
	HTTPStatus_MultipleChoices = 300,
	HTTPStatus_MovedPermanently = 301,
	HTTPStatus_Found = 302,
	HTTPStatus_SeeOther = 303,
	HTTPStatus_NotModified = 304,
	HTTPStatus_UseProxy = 305,
	HTTPStatus_TemporaryRedirect = 307,

	// 4xx Client Error
	HTTPStatus_BadRequest = 400,
	HTTPStatus_Unauthorized = 401,
	HTTPStatus_PaymentRequired = 402,
	HTTPStatus_Forbidden = 403,
	HTTPStatus_NotFound = 404,
	HTTPStatus_MethodNotAllowed = 405,
	HTTPStatus_NotAcceptable = 406,
	HTTPStatus_ProxyAuthenticationRequired = 407,
	HTTPStatus_RequestTimeout = 408,
	HTTPStatus_Conflict = 409,
	HTTPStatus_Gone = 410,
	HTTPStatus_LengthRequired = 411,
	HTTPStatus_PreconditionFailed = 412,
	HTTPStatus_RequestEntityTooLarge = 413,
	HTTPStatus_RequestURITooLarge = 414,
	HTTPStatus_UnsupportedMediaType = 415,
	HTTPStatus_RequestedRangeNotSatisfiable = 416,
	HTTPStatus_ExpectationFailed = 417,

	// 5xx Server Error
	HTTPStatus_InternalServerError = 500,
	HTTPStatus_NotImplemented = 501,
	HTTPStatus_BadGateway = 502,
	HTTPStatus_ServiceUnavailable = 503,
	HTTPStatus_GatewayTimeout = 504,
	HTTPStatus_HTTPVersionNotSupported = 505,
};

typeset HTTPRequestCallback
{
	function void (HTTPResponse response, any value);
	function void (HTTPResponse response, any value, const char[] error);
};

methodmap HTTPResponse
{
	// Retrieves a HTTP header from the response.
	//
	// @param name       Header name.
	// @param buffer     String buffer to store value.
	// @param maxlength  Maximum length of the string buffer.
	// @return           True on success, false if the header was not found.
	public native bool GetHeader(const char[] name, char[] buffer, int maxlength);

	// Retrieves the JSON data of the response.
	property JSON Data {
		public native get();
	}

	// Retrieves the HTTP status of the response.
	property HTTPStatus Status {
		public native get();
	}
};

methodmap HTTPClient < Handle
{
	// Creates a HTTP client.
	//
	// @param baseURL    Base URL to the REST API.
	public native HTTPClient(const char[] baseURL);

	// Sets a HTTP header to be used for all requests.
	//
	// @param name       Header name.
	// @param value      String value to set.
	public native void SetHeader(const char[] name, const char[] value);

	// Performs a HTTP GET request.
	//
	// @param endpoint   API endpoint to request.
	// @param callback   A function to use as a callback when the request has finished.
	// @param value      Optional value to pass to the callback function.
	public native void Get(const char[] endpoint, HTTPRequestCallback callback, any value = 0);

	// Performs a HTTP POST request.
	//
	// @param endpoint   API endpoint to request.
	// @param data       JSON data to send.
	// @param callback   A function to use as a callback when the request has finished.
	// @param value      Optional value to pass to the callback function.
	public native void Post(const char[] endpoint, JSON data, HTTPRequestCallback callback, any value = 0);

	// Performs a HTTP PUT request.
	//
	// @param endpoint   API endpoint to request.
	// @param data       JSON data to send.
	// @param callback   A function to use as a callback when the request has finished.
	// @param value      Optional value to pass to the callback function.
	public native void Put(const char[] endpoint, JSON data, HTTPRequestCallback callback, any value = 0);

	// Performs a HTTP PATCH request.
	//
	// @param endpoint   API endpoint to request.
	// @param data       JSON data to send.
	// @param callback   A function to use as a callback when the request has finished.
	// @param value      Optional value to pass to the callback function.
	public native void Patch(const char[] endpoint, JSON data, HTTPRequestCallback callback, any value = 0);

	// Performs a HTTP DELETE request.
	//
	// @param endpoint   API endpoint to request.
	// @param callback   A function to use as a callback when the request has finished.
	// @param value      Optional value to pass to the callback function.
	public native void Delete(const char[] endpoint, HTTPRequestCallback callback, any value = 0);

	// Connect timeout in seconds.
	property int ConnectTimeout {
		public native get();
		public native set(int connectTimeout);
	}

	// Follow HTTP 3xx redirects.
	property bool FollowLocation {
		public native get();
		public native set(bool followLocation);
	}

	// Timeout in seconds.
	property int Timeout {
		public native get();
		public native set(int timeout);
	}
};
