#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "json.h"


static int test_int(int value, char *str_json)
{
    int ret = 0;
    char *result_str = NULL;
    size_t result_len;
    json_t *new_json = NULL;
    json_node_t *new_json_node = NULL;

    new_json = json_new();
    new_json_node = json_node_new_integer(value);
    json_set_root(new_json, new_json_node);
    new_json_node = NULL;

    json_dump(new_json, &result_str, &result_len);
    if (strlen(str_json) != result_len) 
    { ret = -1; goto fail; }
    if (strncmp(str_json, result_str, result_len) != 0)
    { ret = -1; goto fail; }

fail:
    if (new_json != NULL) json_destroy(new_json);
    if (new_json_node != NULL) json_node_destroy(new_json_node);
    if (result_str != NULL) free(result_str);
    return ret;
}

static int test_load_dump(char *str_json)
{
    int ret = 0;
    char *result_str = NULL;
    size_t result_len;
    json_t *new_json = NULL;

    fwrite(str_json, strlen(str_json), 1, stdout);
    printf(":");
    if ((ret = json_load(&new_json, str_json, strlen(str_json))) != 0)
    { goto fail; }
    if ((ret = json_dump(new_json, &result_str, &result_len)) != 0)
    { goto fail; }

    if (strlen(str_json) != result_len) 
    { ret = -1; goto fail; }
    if (strncmp(str_json, result_str, result_len) != 0)
    { ret = -1; goto fail; }

fail:
    if (new_json != NULL) json_destroy(new_json);
    if (result_str != NULL) free(result_str);
    return ret;
}

static int test_str(char *str_in, char *str_json)
{
    int ret = 0;
    char *result_str = NULL;
    size_t result_len;
    json_t *new_json = NULL;
    json_node_t *new_json_node = NULL;

    new_json = json_new();
    new_json_node = json_node_new_string(str_in, strlen(str_in));
    json_set_root(new_json, new_json_node);
    new_json_node = NULL;

    json_dump(new_json, &result_str, &result_len);
    if (strlen(str_json) != result_len) 
    { ret = -1; goto fail; }
    if (strncmp(str_json, result_str, result_len) != 0)
    { ret = -1; goto fail; }

fail:
    if (new_json != NULL) json_destroy(new_json);
    if (new_json_node != NULL) json_node_destroy(new_json_node);
    if (result_str != NULL) free(result_str);
    return ret;
}

static int test_null(void)
{
    int ret = 0;
    char *result_str = NULL;
    size_t result_len;
    char *str_json = "null";
    json_t *new_json = NULL;
    json_node_t *new_json_node = NULL;

    new_json = json_new();
    new_json_node = json_node_new_null();
    json_set_root(new_json, new_json_node);
    new_json_node = NULL;

    json_dump(new_json, &result_str, &result_len);
    if (strlen(str_json) != result_len) 
    { ret = -1; goto fail; }
    if (strncmp(str_json, result_str, result_len) != 0)
    { ret = -1; goto fail; }

fail:
    if (new_json != NULL) json_destroy(new_json);
    if (new_json_node != NULL) json_node_destroy(new_json_node);
    if (result_str != NULL) free(result_str);
    return ret;
}

static int test_false(void)
{
    int ret = 0;
    char *result_str = NULL;
    size_t result_len;
    char *str_json = "false";
    json_t *new_json = NULL;
    json_node_t *new_json_node = NULL;

    new_json = json_new();
    new_json_node = json_node_new_false();
    json_set_root(new_json, new_json_node);
    new_json_node = NULL;

    json_dump(new_json, &result_str, &result_len);
    if (strlen(str_json) != result_len) 
    { ret = -1; goto fail; }
    if (strncmp(str_json, result_str, result_len) != 0)
    { ret = -1; goto fail; }

fail:
    if (new_json != NULL) json_destroy(new_json);
    if (new_json_node != NULL) json_node_destroy(new_json_node);
    if (result_str != NULL) free(result_str);
    return ret;
}

static int test_true(void)
{
    int ret = 0;
    char *result_str = NULL;
    size_t result_len;
    char *str_json = "true";
    json_t *new_json = NULL;
    json_node_t *new_json_node = NULL;

    new_json = json_new();
    new_json_node = json_node_new_true();
    json_set_root(new_json, new_json_node);
    new_json_node = NULL;

    json_dump(new_json, &result_str, &result_len);
    if (strlen(str_json) != result_len) 
    { ret = -1; goto fail; }
    if (strncmp(str_json, result_str, result_len) != 0)
    { ret = -1; goto fail; }

fail:
    if (new_json != NULL) json_destroy(new_json);
    if (new_json_node != NULL) json_node_destroy(new_json_node);
    if (result_str != NULL) free(result_str);
    return ret;
}

static int test_array(void)
{
    int ret = 0;
    char *result_str = NULL;
    size_t result_len;
    char *str_json = "[0,1,2,3]";
    int idx;
    json_t *new_json = NULL;
    json_node_t *new_json_node = NULL;
    json_node_t *new_json_elem = NULL;

    new_json = json_new();
    new_json_node = json_node_new_array();
    for (idx = 0; idx != 4; idx++)
    {
        new_json_elem = json_node_new_integer(idx);
        json_node_as_array_append(new_json_node, \
                new_json_elem);
        new_json_elem = NULL;
    }
    json_set_root(new_json, new_json_node);
    new_json_node = NULL;

    json_dump(new_json, &result_str, &result_len);
    if (strlen(str_json) != result_len) 
    { ret = -1; goto fail; }
    if (strncmp(str_json, result_str, result_len) != 0)
    { ret = -1; goto fail; }

fail:
    if (new_json != NULL) json_destroy(new_json);
    if (new_json_node != NULL) json_node_destroy(new_json_node);
    if (new_json_elem != NULL) json_node_destroy(new_json_elem);
    if (result_str != NULL) free(result_str);
    return ret;
}

static int test_object(void)
{
    int ret = 0;
    char *result_str = NULL;
    size_t result_len;
    char *str_json = "{\"zero\":0,\"one\":1}";
    char *number_names[] = {
        "zero",
        "one",
        "two",
        "three",
    };
    int idx;
    json_t *new_json = NULL;
    json_node_t *new_json_node = NULL;
    json_node_t *new_json_elem_name = NULL;
    json_node_t *new_json_elem_value = NULL;

    new_json = json_new();
    new_json_node = json_node_new_object();
    for (idx = 0; idx != 2; idx++)
    {
        new_json_elem_name = json_node_new_string( \
                number_names[idx], \
                strlen(number_names[idx]));
        new_json_elem_value = json_node_new_integer(idx);
        if ((ret = json_node_as_object_append(new_json_node, \
                new_json_elem_name, \
                new_json_elem_value)) != 0)
        { goto fail; }
        new_json_elem_name = NULL;
        new_json_elem_value = NULL;
    }
    json_set_root(new_json, new_json_node);
    new_json_node = NULL;

    json_dump(new_json, &result_str, &result_len);
    if (strlen(str_json) != result_len) 
    { ret = -1; goto fail; }
    if (strncmp(str_json, result_str, result_len) != 0)
    { ret = -1; goto fail; }

fail:
    if (new_json != NULL) json_destroy(new_json);
    if (new_json_node != NULL) json_node_destroy(new_json_node);
    if (result_str != NULL) free(result_str);
    return ret;
}

int main(int argc, char** argv)
{
    printf("%d\n", test_int(123, "123"));
    printf("%d\n", test_int(-123, "-123"));
    printf("%d\n", test_int(0, "0"));
    printf("%d\n", test_str("abc", "\"abc\""));
    printf("%d\n", test_str("\"", "\"\"\""));
    printf("%d\n", test_null());
    printf("%d\n", test_false());
    printf("%d\n", test_true());
    printf("%d\n", test_array());
    printf("%d\n", test_object());

    printf("%d\n", test_load_dump("123"));
    printf("%d\n", test_load_dump("-123"));
    printf("%d\n", test_load_dump("0"));
    printf("%d\n", test_load_dump("\"abc\""));
    printf("%d\n", test_load_dump("null"));
    printf("%d\n", test_load_dump("false"));
    printf("%d\n", test_load_dump("true"));
    printf("%d\n", test_load_dump("[]"));
    printf("%d\n", test_load_dump("[1]"));
    printf("%d\n", test_load_dump("[1,2,3]"));
    printf("%d\n", test_load_dump("{}"));
    printf("%d\n", test_load_dump("{\"zero\":0}"));
    printf("%d\n", test_load_dump("{\"zero\":0,\"one\":1}"));
    printf("%d\n", test_load_dump("{\"zero\":0,\"one\":[]}"));

    /* BUGGY */
    /*printf("%d\n", test_load_dump("\"\\\"\""));*/
    return 0;
}

