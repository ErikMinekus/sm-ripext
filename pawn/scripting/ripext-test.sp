#include <sourcemod>
#include <ripext>

#pragma newdecls required
#pragma semicolon 1

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
};

public void OnPluginStart()
{
    HTTPClient hHTTPClient = new HTTPClient("https://nghttp2.org/httpbin");
    JSONObject hJSONObject = CreateJSONObject();

    char sImagePath[PLATFORM_MAX_PATH];
    BuildPath(Path_SM, sImagePath, sizeof(sImagePath), "data/ripext-test.jpg");

    hHTTPClient.Get("get", OnHTTPResponse, 0);
    hHTTPClient.Post("post", hJSONObject, OnHTTPResponse, 1);
    hHTTPClient.Put("put", hJSONObject, OnHTTPResponse, 2);
    hHTTPClient.Patch("patch", hJSONObject, OnHTTPResponse, 3);
    hHTTPClient.Delete("delete", OnHTTPResponse, 4);
    hHTTPClient.Get("gzip", OnHTTPResponse, 5);
    hHTTPClient.DownloadFile("image/jpeg", sImagePath, OnImageDownloaded);

    // Params
    StringMap params = new StringMap();
    params.SetString("test", "test");
    params.SetString("test2", "test2");
    params.SetString("escape", "Hofbräuhaus / München");
    hHTTPClient.GetWithParams("get", params, OnHTTPResponse, 0);
    delete params;

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
    if (response.Data == null) {
        PrintToServer("[OK] %s No response", sHTTPTags[value]);
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
