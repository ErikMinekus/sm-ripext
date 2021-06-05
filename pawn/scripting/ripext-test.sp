#include <sourcemod>
#include <ripext>

#pragma newdecls required
#pragma semicolon 1

#define API_BASE_URL "https://nghttp2.org/httpbin"

public Plugin myinfo =
{
    name        = "REST in Pawn - Tests",
    author      = "Tsunami",
    description = "Test HTTP and JSON natives",
    version     = "1.0.0",
    url         = "http://www.tsunami-productions.nl"
};


char sHTTPTags[][] = {
    "GET",
    "POST",
    "PUT",
    "PATCH",
    "DELETE",
    "GZIP",
    "AUTH",
    "BEARER",
    "REDIRECT",
    "FORM",
};


public void OnPluginStart()
{
    HTTPRequest hHTTPRequest;
    JSONObject hJSONObject = CreateJSONObject();

    char sImagePath[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, sImagePath, sizeof(sImagePath), "data/ripext-test.jpg");

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/get");
    hHTTPRequest.AppendQueryParam("name[]", "%s", "%&✓");
    hHTTPRequest.AppendQueryParam("name[]", "%s", "<=>");
    hHTTPRequest.Get(OnHTTPResponse, 0);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/post");
    hHTTPRequest.Post(hJSONObject, OnHTTPResponse, 1);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/put");
    hHTTPRequest.Put(hJSONObject, OnHTTPResponse, 2);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/patch");
    hHTTPRequest.Patch(hJSONObject, OnHTTPResponse, 3);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/delete");
    hHTTPRequest.Delete(OnHTTPResponse, 4);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/gzip");
    hHTTPRequest.Get(OnHTTPResponse, 5);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/basic-auth/user/pass");
    hHTTPRequest.SetBasicAuth("user", "pass");
    hHTTPRequest.Get(OnHTTPResponse, 6);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/bearer");
    hHTTPRequest.SetHeader("Authorization", "Bearer %s", "token");
    hHTTPRequest.Get(OnHTTPResponse, 7);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/redirect/1");
    hHTTPRequest.Get(OnHTTPResponse, 8);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/post");
    hHTTPRequest.AppendFormParam("name[]", "%s", "%&✓");
    hHTTPRequest.AppendFormParam("name[]", "%s", "<=>");
    hHTTPRequest.PostForm(OnHTTPResponse, 9);

    hHTTPRequest = new HTTPRequest(API_BASE_URL..."/image/jpeg");
    hHTTPRequest.DownloadFile(sImagePath, OnImageDownloaded);

    JSONObjectKeys hJSONObjectKeys = hJSONObject.Keys();
    char sKey[64];

    while (hJSONObjectKeys.ReadKey(sKey, sizeof(sKey))) {
        PrintToServer("[JSON] Read Object Key: %s", sKey);
    }

    delete hJSONObjectKeys;
    delete hJSONObject;
}

void OnHTTPResponse(HTTPResponse response, any value)
{
    if (response.Status != HTTPStatus_OK) {
        PrintToServer("[ERR] %s Status: %d", sHTTPTags[value], response.Status);
        return;
    }

    char sData[1024];
    response.Data.ToString(sData, sizeof(sData), JSON_INDENT(4));

    PrintToServer("[OK] %s Response:\n%s", sHTTPTags[value], sData);
}

void OnImageDownloaded(HTTPStatus status, any value)
{
    if (status != HTTPStatus_OK) {
        PrintToServer("[ERR] Download Status: %d", status);
        return;
    }

    PrintToServer("[OK] Download Complete");
}

JSONObject CreateJSONObject()
{
    JSONObject hJSONObject = new JSONObject();
    hJSONObject.SetInt("id", 1);
    hJSONObject.SetInt64("int64", "9223372036854775800");
    hJSONObject.SetFloat("jsonrpc", 2.0);
    hJSONObject.SetBool("bool", true);
    hJSONObject.SetString("method", "subtract");
    hJSONObject.SetNull("null");

    JSONObject hJSONObjectInArray = new JSONObject();
    hJSONObjectInArray.SetInt("id", 1);

    JSONArray hJSONArray = new JSONArray();
    hJSONArray.Push(hJSONObjectInArray);
    hJSONArray.PushInt(1);
    hJSONArray.PushInt64("9223372036854775800");
    hJSONArray.PushFloat(2.0);
    hJSONArray.PushBool(true);
    hJSONArray.PushString("string");
    hJSONArray.PushNull();

    hJSONObject.Set("params", hJSONArray);

    delete hJSONObjectInArray;
    delete hJSONArray;
    return hJSONObject;
}
