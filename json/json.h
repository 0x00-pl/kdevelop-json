/* JSON Library */

#ifndef _JSON_H_
#define _JSON_H_

#include <stdio.h>

struct json_node;
typedef struct json_node json_node_t;

typedef enum json_node_type
{
    JSON_NODE_TYPE_UNKNOWN,
    JSON_NODE_TYPE_OBJECT,
    JSON_NODE_TYPE_ARRAY,
    JSON_NODE_TYPE_STRING,
    JSON_NODE_TYPE_INTEGER,
    JSON_NODE_TYPE_DOUBLE,
    JSON_NODE_TYPE_FALSE,
    JSON_NODE_TYPE_TRUE,
    JSON_NODE_TYPE_NULL,
} json_node_type_t;

typedef struct json_node_array_node
{
    json_node_t *node;
    struct json_node_array_node *next;
} json_node_array_node_t;

typedef struct json_node_array
{
    json_node_array_node_t *begin, *end;
    size_t size;
} json_node_array_t;

typedef struct json_node_object_node
{
    json_node_t *name;
    json_node_t *value;
    struct json_node_object_node *next;
} json_node_object_node_t;

typedef struct json_node_object
{
    json_node_object_node_t *begin, *end;
    size_t size;
} json_node_object_t;

struct json_node
{
    json_node_type_t type;
    union
    {
        struct 
        {
            char *str;
            size_t len;
        } string_part;
        union
        {
            int int_part;
            double double_part;
        } number_part;
        json_node_array_t *array_part;
        json_node_object_t *object_part;
    } u;
};

json_node_t *json_node_new(json_node_type_t type);
void json_node_destroy(json_node_t *node);
json_node_t *json_node_new_integer(int value);
json_node_t *json_node_new_double(double value);
json_node_t *json_node_new_false(void);
json_node_t *json_node_new_true(void);
json_node_t *json_node_new_null(void);
json_node_t *json_node_new_string(char *str, size_t len);
json_node_t *json_node_new_array(void);
json_node_t *json_node_new_object(void);
int json_node_as_array_append(json_node_t *node_array, \
        json_node_t *new_element);
int json_node_as_object_append(json_node_t *node_object, \
        json_node_t *new_name, json_node_t *new_value);


typedef struct json
{
    json_node_t *root;
} json_t;

json_t *json_new(void);
void json_destroy(json_t *json);
void json_set_root(json_t *json, json_node_t *node);
int json_dump(json_t *json, char **str_out, size_t *len_out);
int json_load(json_t **json_out, char *str, size_t len);


#endif

