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

    hHTTPClient.Get("get", OnHTTPResponse, 0);
    hHTTPClient.Post("post", hJSONObject, OnHTTPResponse, 1);
    hHTTPClient.Put("put", hJSONObject, OnHTTPResponse, 2);
    hHTTPClient.Patch("patch", hJSONObject, OnHTTPResponse, 3);
    hHTTPClient.Delete("delete", OnHTTPResponse, 4);
    hHTTPClient.Get("gzip", OnHTTPResponse, 5);

    delete hJSONObject;
}

public void OnHTTPResponse(HTTPResponse response, any value)
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

JSONObject CreateJSONObject()
{
    JSONObject hJSONObject = new JSONObject();
    hJSONObject.SetInt("id", 1);
    hJSONObject.SetFloat("jsonrpc", 2.0);
    hJSONObject.SetString("method", "subtract");

    JSONArray hJSONArray = new JSONArray();
    hJSONArray.PushInt(42);
    hJSONArray.PushInt(23);
    hJSONObject.Set("params", hJSONArray);

    delete hJSONArray;
    return hJSONObject;
}
