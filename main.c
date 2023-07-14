#include <stdio.h>
#include <string.h>

#include <json-c/json.h>

int main()
{
    char json_string[100] = "{\"1\": 2, \"3\": 4, \"5\": null}";
    // char json_string[100] = "1";

    struct json_tokener* tok = json_tokener_new();
    enum json_tokener_error jerr;
    json_object* jobj;

    do
    {
        jobj = json_tokener_parse_ex(tok, json_string, strlen(json_string) + 1);
        jerr = json_tokener_get_error(tok);
    } while (jerr == json_tokener_continue);

    printf("jobj: %p\n", jobj);

    printf("hello world\n");
}
