#if defined _ripext_included_
	#endinput
#endif
#define _ripext_included_

#include <ripext/json>
#include <ripext/http>

/**
 * Do not edit below this line!
 */
public Extension __ext_rip =
{
	name = "REST in Pawn",
	file = "rip.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};

#if !defined REQUIRE_EXTENSIONS
public __ext_rip_SetNTVOptional()
{
	/* JSON */
	MarkNativeAsOptional("JSON.ToFile");
	MarkNativeAsOptional("JSON.ToString");

	/* JSONObject */
	MarkNativeAsOptional("JSONObject.JSONObject");
	MarkNativeAsOptional("JSONObject.FromFile");
	MarkNativeAsOptional("JSONObject.FromString");
	MarkNativeAsOptional("JSONObject.Get");
	MarkNativeAsOptional("JSONObject.GetBool");
	MarkNativeAsOptional("JSONObject.GetFloat");
	MarkNativeAsOptional("JSONObject.GetInt");
	MarkNativeAsOptional("JSONObject.GetInt64");
	MarkNativeAsOptional("JSONObject.GetString");
	MarkNativeAsOptional("JSONObject.IsNull");
	MarkNativeAsOptional("JSONObject.HasKey");
	MarkNativeAsOptional("JSONObject.Set");
	MarkNativeAsOptional("JSONObject.SetBool");
	MarkNativeAsOptional("JSONObject.SetFloat");
	MarkNativeAsOptional("JSONObject.SetInt");
	MarkNativeAsOptional("JSONObject.SetInt64");
	MarkNativeAsOptional("JSONObject.SetNull");
	MarkNativeAsOptional("JSONObject.SetString");
	MarkNativeAsOptional("JSONObject.Remove");
	MarkNativeAsOptional("JSONObject.Clear");
	MarkNativeAsOptional("JSONObject.Keys");
	MarkNativeAsOptional("JSONObject.Size.get");

	/* JSONObjectKeys */
	MarkNativeAsOptional("JSONObjectKeys.ReadKey");

	/* JSONArray */
	MarkNativeAsOptional("JSONArray.JSONArray");
	MarkNativeAsOptional("JSONArray.FromFile");
	MarkNativeAsOptional("JSONArray.FromString");
	MarkNativeAsOptional("JSONArray.Get");
	MarkNativeAsOptional("JSONArray.GetBool");
	MarkNativeAsOptional("JSONArray.GetFloat");
	MarkNativeAsOptional("JSONArray.GetInt");
	MarkNativeAsOptional("JSONArray.GetInt64");
	MarkNativeAsOptional("JSONArray.GetString");
	MarkNativeAsOptional("JSONArray.IsNull");
	MarkNativeAsOptional("JSONArray.Set");
	MarkNativeAsOptional("JSONArray.SetBool");
	MarkNativeAsOptional("JSONArray.SetFloat");
	MarkNativeAsOptional("JSONArray.SetInt");
	MarkNativeAsOptional("JSONArray.SetInt64");
	MarkNativeAsOptional("JSONArray.SetNull");
	MarkNativeAsOptional("JSONArray.SetString");
	MarkNativeAsOptional("JSONArray.Push");
	MarkNativeAsOptional("JSONArray.PushBool");
	MarkNativeAsOptional("JSONArray.PushFloat");
	MarkNativeAsOptional("JSONArray.PushInt");
	MarkNativeAsOptional("JSONArray.PushInt64");
	MarkNativeAsOptional("JSONArray.PushNull");
	MarkNativeAsOptional("JSONArray.PushString");
	MarkNativeAsOptional("JSONArray.Remove");
	MarkNativeAsOptional("JSONArray.Clear");
	MarkNativeAsOptional("JSONArray.Length.get");

	/* HTTPRequest */
	MarkNativeAsOptional("HTTPRequest.HTTPRequest");
	MarkNativeAsOptional("HTTPRequest.AppendFormParam");
	MarkNativeAsOptional("HTTPRequest.AppendQueryParam");
	MarkNativeAsOptional("HTTPRequest.SetBasicAuth");
	MarkNativeAsOptional("HTTPRequest.SetHeader");
	MarkNativeAsOptional("HTTPRequest.Get");
	MarkNativeAsOptional("HTTPRequest.Post");
	MarkNativeAsOptional("HTTPRequest.Put");
	MarkNativeAsOptional("HTTPRequest.Patch");
	MarkNativeAsOptional("HTTPRequest.Delete");
	MarkNativeAsOptional("HTTPRequest.DownloadFile");
	MarkNativeAsOptional("HTTPRequest.UploadFile");
	MarkNativeAsOptional("HTTPRequest.PostForm");
	MarkNativeAsOptional("HTTPRequest.ConnectTimeout.get");
	MarkNativeAsOptional("HTTPRequest.ConnectTimeout.set");
	MarkNativeAsOptional("HTTPRequest.MaxRedirects.get");
	MarkNativeAsOptional("HTTPRequest.MaxRedirects.set");
	MarkNativeAsOptional("HTTPRequest.MaxRecvSpeed.get");
	MarkNativeAsOptional("HTTPRequest.MaxRecvSpeed.set");
	MarkNativeAsOptional("HTTPRequest.MaxSendSpeed.get");
	MarkNativeAsOptional("HTTPRequest.MaxSendSpeed.set");
	MarkNativeAsOptional("HTTPRequest.Timeout.get");
	MarkNativeAsOptional("HTTPRequest.Timeout.set");

	/* HTTPResponse */
	MarkNativeAsOptional("HTTPResponse.GetHeader");
	MarkNativeAsOptional("HTTPResponse.Data.get");
	MarkNativeAsOptional("HTTPResponse.Status.get");

	/* HTTPClient */
	MarkNativeAsOptional("HTTPClient.HTTPClient");
	MarkNativeAsOptional("HTTPClient.SetHeader");
	MarkNativeAsOptional("HTTPClient.Get");
	MarkNativeAsOptional("HTTPClient.Post");
	MarkNativeAsOptional("HTTPClient.Put");
	MarkNativeAsOptional("HTTPClient.Patch");
	MarkNativeAsOptional("HTTPClient.Delete");
	MarkNativeAsOptional("HTTPClient.DownloadFile");
	MarkNativeAsOptional("HTTPClient.UploadFile");
	MarkNativeAsOptional("HTTPClient.ConnectTimeout.get");
	MarkNativeAsOptional("HTTPClient.ConnectTimeout.set");
	MarkNativeAsOptional("HTTPClient.FollowLocation.get");
	MarkNativeAsOptional("HTTPClient.FollowLocation.set");
	MarkNativeAsOptional("HTTPClient.Timeout.get");
	MarkNativeAsOptional("HTTPClient.Timeout.set");
	MarkNativeAsOptional("HTTPClient.MaxSendSpeed.get");
	MarkNativeAsOptional("HTTPClient.MaxSendSpeed.set");
	MarkNativeAsOptional("HTTPClient.MaxRecvSpeed.get");
	MarkNativeAsOptional("HTTPClient.MaxRecvSpeed.set");
}
#endif
