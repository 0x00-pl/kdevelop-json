/* JSON Library */

/* http://www.ietf.org/rfc/rfc4627.txt */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"

#define IS_DIGIT(ch) (('0'<=(ch))&&((ch)<='9'))
#define IS_ALPHA_LOWCASE(ch) (('a'<=(ch))&&((ch)<='z'))

/* Declarations */

void json_node_destroy(json_node_t *node);

json_node_array_node_t *json_node_array_node_new(json_node_t *node);
void json_node_array_node_destroy(json_node_array_node_t *node);
json_node_array_t *json_node_array_new(void);
void json_node_array_destroy(json_node_array_t *node_array);
void json_node_array_append(json_node_array_t *node_array, \
        json_node_array_node_t *new_node);

json_node_object_node_t *json_node_object_node_new(json_node_t *name, json_node_t *value);
void json_node_object_node_destroy(json_node_object_node_t *object_node);
json_node_object_t *json_node_object_new(void);
void json_node_object_destroy(json_node_object_t *object);
void json_node_object_append(json_node_object_t *object, \
        json_node_object_node_t *new_node);

static int json_node_dump_array(json_node_t *node, char **p_io);
static int json_node_dump_object(json_node_t *node, char **p_io);
static int json_node_dump_string(json_node_t *node, char **p_io);
static int json_node_dump_integer(json_node_t *node, char **p_io);
static int json_node_dump(json_node_t *node, char **p_io);

static int json_node_array_load(json_node_t **json_node_out, \
        char **str_io, char *str_endp);
/*static int json_node_object_load(json_node_t **json_node_out, \*/
/*char **str_io, char *str_endp);*/
static int json_node_load(json_node_t **json_node_out, \
        char **str_io, char *str_endp);


/* Array */

json_node_array_node_t *json_node_array_node_new(json_node_t *node)
{
    json_node_array_node_t *new_array_node = NULL;

    new_array_node = (json_node_array_node_t *)malloc( \
            sizeof(json_node_array_node_t));
    if (new_array_node == NULL) return NULL;
    new_array_node->node = node;
    new_array_node->next = NULL;

    return new_array_node;
}

void json_node_array_node_destroy(json_node_array_node_t *node)
{
    json_node_destroy(node->node);
    free(node);
}

json_node_array_t *json_node_array_new(void)
{
    json_node_array_t *new_array = NULL;

    new_array = (json_node_array_t *)malloc( \
            sizeof(json_node_array_t));
    if (new_array == NULL) return NULL;
    new_array->begin = new_array->end = NULL;
    new_array->size = 0;
    return new_array;
}

void json_node_array_destroy(json_node_array_t *node_array)
{
    json_node_array_node_t *node_cur = node_array->begin;
    json_node_array_node_t *node_next;

    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        json_node_array_node_destroy(node_cur);
        node_cur = node_next;
    }
    free(node_array);
}

void json_node_array_append(json_node_array_t *node_array, \
        json_node_array_node_t *new_node)
{
    if (node_array->begin == NULL)
    {
        node_array->begin = node_array->end = new_node;
    }
    else
    {
        node_array->end->next = new_node;
        node_array->end = new_node;
    }
    node_array->size++;
}

/* Object */

json_node_object_node_t *json_node_object_node_new(json_node_t *name, json_node_t *value)
{
    json_node_object_node_t *new_object_node = NULL;

    if ((new_object_node = (json_node_object_node_t *)malloc( \
                    sizeof(json_node_object_node_t))) == NULL)
    { return NULL; }
    new_object_node->name = name;
    new_object_node->value = value;
    new_object_node->next = NULL;
    
    return new_object_node;
}

void json_node_object_node_destroy(json_node_object_node_t *object_node)
{
    json_node_destroy(object_node->name);
    json_node_destroy(object_node->value);
    free(object_node);
}

json_node_object_t *json_node_object_new(void)
{
    json_node_object_t *new_object = NULL;

    if ((new_object = (json_node_object_t *)malloc( \
                    sizeof(json_node_object_t))) == NULL)
    { return NULL; }
    new_object->begin = new_object->end = NULL;
    new_object->size = 0;

    return new_object;
}

void json_node_object_destroy(json_node_object_t *object)
{
    json_node_object_node_t *node_cur = object->begin, *node_next;

    while (node_cur != NULL)
    {
        node_next = node_cur->next;
        json_node_object_node_destroy(node_cur);
        node_cur = node_next;
    }
    free(object);
}

void json_node_object_append(json_node_object_t *object, \
        json_node_object_node_t *new_node)
{
    if (object->begin == NULL)
    {
        object->begin = object->end = new_node;
    }
    else
    {
        object->end->next = new_node;
        object->end = new_node;
    }
    object->size++;
}


/* Node */

json_node_t *json_node_new(json_node_type_t type)
{
    json_node_t *new_json_node = (json_node_t *)malloc( \
            sizeof(json_node_t));
    if (new_json_node == NULL) return NULL;
    new_json_node->type = JSON_NODE_TYPE_UNKNOWN;
    switch (type)
    {
        case JSON_NODE_TYPE_OBJECT:
            new_json_node->u.object_part = NULL;
            break;
        case JSON_NODE_TYPE_ARRAY:
            new_json_node->u.array_part = NULL;
            break;
        case JSON_NODE_TYPE_STRING:
            new_json_node->u.string_part.str = NULL;
            break;
        case JSON_NODE_TYPE_UNKNOWN:
        case JSON_NODE_TYPE_INTEGER:
        case JSON_NODE_TYPE_DOUBLE:
        case JSON_NODE_TYPE_TRUE:
        case JSON_NODE_TYPE_FALSE:
        case JSON_NODE_TYPE_NULL:
            break;
    }
    new_json_node->type = type;
    return new_json_node;
}

void json_node_destroy(json_node_t *node)
{
    switch (node->type)
    {
        case JSON_NODE_TYPE_ARRAY:
            if (node->u.array_part != NULL)
            { json_node_array_destroy(node->u.array_part); }
            break;
        case JSON_NODE_TYPE_OBJECT:
            if (node->u.object_part != NULL)
            { json_node_object_destroy(node->u.object_part); }
            break;
        case JSON_NODE_TYPE_STRING:
            if (node->u.string_part.str != NULL)
            { free(node->u.string_part.str); }
            break;
        case JSON_NODE_TYPE_UNKNOWN:
        case JSON_NODE_TYPE_INTEGER:
        case JSON_NODE_TYPE_DOUBLE:
        case JSON_NODE_TYPE_TRUE:
        case JSON_NODE_TYPE_FALSE:
        case JSON_NODE_TYPE_NULL:
            break;
    }
    free(node);
}

static int json_node_length(json_node_t *node);

static int json_node_length_integer(int value)
{
    int len = 0;

    if (value == 0) return 1;
    else if (value < 0) 
    { value = -value; len++; }

    while (value != 0)
    { value /= 10; len++; } 

    return len;
}

static int json_node_length_string(char *str, size_t str_len)
{
    int len = 2;
    char *str_p = str, *str_endp = str_p + str_len;

    while (str_p != str_endp)
    {
        switch (*str_p)
        {
            case '\\':
                str_p++; len++;
                if (str_p == str_endp) return -1;
                if (*str_p == 'u')
                {
                    str_p++; len++;
                    if (str_endp - str_p < 4) return -1;
                    if (!((IS_DIGIT(*str_p))&& \
                                (IS_DIGIT(*(str_p+1)))&&
                                (IS_DIGIT(*(str_p+2)))&&
                                (IS_DIGIT(*(str_p+3)))))
                    { return -1; }
                    str_p += 4; len += 4;
                }
                else if (*str_p == '\"')
                { str_p++; len++; }
                else if (*str_p == '\\')
                { str_p++; len++; }
                else if (*str_p == '/')
                { str_p++; len++; }
                else if (*str_p == 'b')
                { str_p++; len++; }
                else if (*str_p == 'f')
                { str_p++; len++; }
                else if (*str_p == 'n')
                { str_p++; len++; }
                else if (*str_p == 'r')
                { str_p++; len++; }
                else if (*str_p == 't')
                { str_p++; len++; }
                else
                { return -1; }
                break;
            default:
                str_p++; len++;
                break;
        }
    }

    return len;
}

static int json_node_length_array(json_node_t *node)
{
    /* if size <= 1 then 
     *   '[' + ']'
     * else 
     *   '[' + ']' + ',' * (size - 1) 
     */
    int length;
    size_t idx;
    json_node_array_node_t *array_node_cur = node->u.array_part->begin;

    if (node->u.array_part->size <= 1)
    {
        length = 2;
    }
    else
    {
        length = 2 + ((int)(node->u.array_part->size) - 1);
    }

    for (idx = 0; idx != node->u.array_part->size; idx++)
    {
        length += json_node_length(array_node_cur->node);
        array_node_cur = array_node_cur->next;
    }

    return length;
}

static int json_node_length_object(json_node_t *node)
{
    /* if size <= 1 then 
     *   '{' + '}' + ':' * size
     * else 
     *   '[' + ']' + ',' * (size - 1) + ':' * size
     */
    int length;
    size_t idx;
    json_node_object_node_t *object_node_cur = node->u.object_part->begin;

    if (node->u.object_part->size <= 1)
    {
        length = 2 + (int)(node->u.object_part->size);
    }
    else
    {
        length = 2 + ((int)(node->u.object_part->size) - 1) + (int)(node->u.object_part->size);
    }

    for (idx = 0; idx != node->u.object_part->size; idx++)
    {
        length += json_node_length(object_node_cur->name);
        length += json_node_length(object_node_cur->value);
        object_node_cur = object_node_cur->next;
    }

    return length;
}

static int json_node_length(json_node_t *node)
{
    int length = 0;
    switch (node->type)
    {
        case JSON_NODE_TYPE_ARRAY:
            length = json_node_length_array(node);
            break;
        case JSON_NODE_TYPE_OBJECT:
            length = json_node_length_object(node);
            break;
        case JSON_NODE_TYPE_INTEGER: 
            length = json_node_length_integer(node->u.number_part.int_part); 
            break;
        case JSON_NODE_TYPE_STRING:
            length = json_node_length_string(node->u.string_part.str, \
                    node->u.string_part.len);
            break;
        case JSON_NODE_TYPE_UNKNOWN: length = 0; break;
        case JSON_NODE_TYPE_DOUBLE: length = 0; break;
        case JSON_NODE_TYPE_TRUE: length = 4; break;
        case JSON_NODE_TYPE_FALSE: length = 5; break;
        case JSON_NODE_TYPE_NULL: length = 4; break;
    }
    return length;
}

static int json_node_dump_array(json_node_t *node, char **p_io)
{
    int ret = 0;
    char *p = *p_io;
    json_node_array_node_t *array_node_cur = node->u.array_part->begin;
    int first = 1;

    *p++ = '[';
    while (array_node_cur != NULL)
    {
        if (first) first = 0;
        else *p++ = ',';
        if ((ret = json_node_dump(array_node_cur->node, &p)) != 0)
        { goto fail; }
        array_node_cur = array_node_cur->next;
    }
    *p++ = ']';

fail:
    *p_io = p;
    return ret;
}

static int json_node_dump_object(json_node_t *node, char **p_io)
{
    int ret = 0;
    char *p = *p_io;
    json_node_object_node_t *object_node_cur = node->u.object_part->begin;
    int first = 1;

    *p++ = '{';
    while (object_node_cur != NULL)
    {
        if (first) first = 0;
        else *p++ = ',';
        if ((ret = json_node_dump(object_node_cur->name, &p)) != 0)
        { goto fail; }
        *p++ = ':';
        if ((ret = json_node_dump(object_node_cur->value, &p)) != 0)
        { goto fail; }
        object_node_cur = object_node_cur->next;
    }
    *p++ = '}';

fail:
    *p_io = p;
    return ret;
}

static int json_node_dump_integer(json_node_t *node, char **p_io)
{
    int ret = 0;
    char *p = *p_io;
    int value = node->u.number_part.int_part;
    int len = json_node_length_integer(value);
    int idx;

    if (value == 0)
    {
        *p++ = '0';
        goto finish;
    }

    if (value < 0)
    {
        value = -value;
        *p++ = '-';
        len--;
    }
    idx = len;
    for (;;)
    {
        idx--;
        p[idx] = (char)((int)('0') + (int)(value % 10));
        value /= 10;
        if (idx == 0) break;
    }
    p += len;

finish:
    *p_io = p;
    return ret;
}

static int json_node_dump_string(json_node_t *node, char **p_io)
{
    int ret = 0;
    char *p = *p_io;
    char *str_p = node->u.string_part.str, *str_endp = str_p + node->u.string_part.len;

    *p++ = '\"';

    while (str_p != str_endp)
    {
        switch (*str_p)
        {
            case '\\':
                *p++ = *str_p++;
                if (*str_p == 'u')
                {
                    *p++ = *str_p++;
                    *p++ = *str_p++;
                    *p++ = *str_p++;
                    *p++ = *str_p++;
                    *p++ = *str_p++;
                }
                else if (*str_p == '\"')
                { *p++ = *str_p++; }
                else if (*str_p == '\\')
                { *p++ = *str_p++; }
                else if (*str_p == '/')
                { *p++ = *str_p++; }
                else if (*str_p == 'b')
                { *p++ = *str_p++; }
                else if (*str_p == 'f')
                { *p++ = *str_p++; }
                else if (*str_p == 'n')
                { *p++ = *str_p++; }
                else if (*str_p == 'r')
                { *p++ = *str_p++; }
                else if (*str_p == 't')
                { *p++ = *str_p++; }
                else
                { return -1; }
                break;
            default:
                *p++ = *str_p++;
                break;
        }
    }

    *p++ = '\"';

    *p_io = p;
    return ret;
}

static int json_node_dump(json_node_t *node, char **p_io)
{
    int ret = 0;
    char *p = *p_io;

    switch (node->type)
    {
        case JSON_NODE_TYPE_ARRAY:
            if ((ret = json_node_dump_array(node, &p)) != 0)
            { goto fail; }
            break;
        case JSON_NODE_TYPE_OBJECT:
            if ((ret = json_node_dump_object(node, &p)) != 0)
            { goto fail; }
            break;
        case JSON_NODE_TYPE_INTEGER: 
            if ((ret = json_node_dump_integer(node, &p)) != 0)
            { goto fail; }
            break;
        case JSON_NODE_TYPE_STRING:
            if ((ret = json_node_dump_string(node, &p)) != 0)
            { goto fail; }
            break;
        case JSON_NODE_TYPE_UNKNOWN: 
            break;
        case JSON_NODE_TYPE_DOUBLE:
            break;
        case JSON_NODE_TYPE_TRUE: 
            memcpy(p, "true", 4);
            p += 4;
            break;
        case JSON_NODE_TYPE_FALSE:
            memcpy(p, "false", 5);
            p += 5;
            break;
        case JSON_NODE_TYPE_NULL:
            memcpy(p, "null", 4);
            p += 4;
            break;
    }

fail:
    *p_io = p;
    return ret;
}

json_node_t *json_node_new_integer(int value)
{
    json_node_t *new_node = json_node_new(JSON_NODE_TYPE_INTEGER);
    if (new_node == NULL) return NULL;
    new_node->u.number_part.int_part = value;
    return new_node;
}

json_node_t *json_node_new_double(double value)
{
    json_node_t *new_node = json_node_new(JSON_NODE_TYPE_DOUBLE);
    if (new_node == NULL) return NULL;
    new_node->u.number_part.double_part = value;
    return new_node;
}

json_node_t *json_node_new_false(void)
{
    return json_node_new(JSON_NODE_TYPE_FALSE);
}

json_node_t *json_node_new_true(void)
{
    return json_node_new(JSON_NODE_TYPE_TRUE);
}

json_node_t *json_node_new_null(void)
{
    return json_node_new(JSON_NODE_TYPE_NULL);
}

json_node_t *json_node_new_string(char *str, size_t len)
{
    json_node_t *new_node = json_node_new(JSON_NODE_TYPE_STRING);
    if (new_node == NULL) return NULL;
    new_node->u.string_part.str = (char *)malloc(sizeof(char) * (len + 1));
    if (new_node->u.string_part.str == NULL)
    { json_node_destroy(new_node); return NULL; }
    memcpy(new_node->u.string_part.str, str, len);
    new_node->u.string_part.str[len] = '\0';
    new_node->u.string_part.len = len;
    return new_node;
}

json_node_t *json_node_new_array(void)
{
    json_node_t *new_node = json_node_new(JSON_NODE_TYPE_ARRAY);
    if (new_node == NULL) return NULL;
    new_node->u.array_part = json_node_array_new();
    if (new_node->u.array_part == NULL)
    { json_node_destroy(new_node); return NULL; }
    return new_node;
}

json_node_t *json_node_new_object(void)
{
    json_node_t *new_node = json_node_new(JSON_NODE_TYPE_OBJECT);
    if (new_node == NULL) return NULL;
    new_node->u.array_part = json_node_array_new();
    if (new_node->u.array_part == NULL)
    { json_node_destroy(new_node); return NULL; }
    return new_node;
}

int json_node_as_array_append(json_node_t *node_array, \
        json_node_t *new_element)
{
    json_node_array_node_t *new_array_node = json_node_array_node_new(new_element);
    if (new_array_node == NULL) return -1;
    json_node_array_append(node_array->u.array_part, new_array_node);
    return 0;
}

int json_node_as_object_append(json_node_t *node_object, \
        json_node_t *new_name, json_node_t *new_value)
{
    json_node_object_node_t *new_object_node = json_node_object_node_new( \
            new_name, new_value);
    if (new_object_node == NULL) return -1;
    json_node_object_append(node_object->u.object_part, new_object_node);
    return 0;
}


/* JSON */

json_t *json_new(void)
{
    json_t *new_json = (json_t *)malloc(sizeof(json_t));
    if (new_json == NULL) return NULL; 
    new_json->root = NULL;
    return new_json;
}

void json_destroy(json_t *json)
{
    json_node_destroy(json->root);
    free(json);
}

void json_set_root(json_t *json, json_node_t *node)
{
    json->root = node;
}

int json_dump(json_t *json, char **str_out, size_t *len_out)
{
    int ret = 0;
    int length = json_node_length(json->root);
    char *str = NULL;
    char *str_p = NULL;

    if (length < 0) return -1;

    str = (char *)malloc(sizeof(char) * ((size_t)length + 1));
    if (str == NULL) 
    { ret = -1; goto fail; }
    str_p = str;

    if ((ret = json_node_dump(json->root, &str_p)) != 0)
    { goto fail; }
    str[length] = '\0';

    *str_out = str;
    *len_out = (size_t)length;
    goto done;
fail:
    if (str != NULL)
    { free(str); }
done:
    return ret;
}


static int json_node_array_load(json_node_t **json_node_out, \
        char **str_io, char *str_endp)
{
    int ret = 0;
    char *str_p = *str_io;
    json_node_t *new_array = NULL;
    json_node_t *new_array_node = NULL;

    /* Skip '[' */
    str_p++;

    if ((new_array = json_node_new_array()) == NULL)
    { ret = -1; goto fail; }

    for (;;)
    {
        if (str_p == str_endp)
        { ret = -1; goto fail; }
        if (*str_p == ']') break;

        if ((ret = json_node_load(&new_array_node, \
                        &str_p, str_endp)) != 0)
        { goto fail; }
        if ((ret = json_node_as_array_append(new_array, \
                        new_array_node)) != 0)
        { goto fail; }
        new_array_node = NULL;

        /* ',' */
        if (str_p == str_endp) 
        { ret = -1; goto fail; }
        if (*str_p == ']') break;
        else if (*str_p == ',')
        {
            /* Skip ',' */
            str_p++;
        }
        else
        { ret = -1; goto fail; }
    }

    /* Skip ']' */
    str_p++;

    *json_node_out = new_array;

    goto done;
fail:
    if (new_array != NULL) json_node_destroy(new_array);
    if (new_array_node != NULL) json_node_destroy(new_array_node);
done:
    *str_io = str_p;
    return ret;
}

static int json_node_object_load(json_node_t **json_node_out, \
        char **str_io, char *str_endp)
{
    int ret = 0;
    char *str_p = *str_io;
    json_node_t *new_object = NULL;
    json_node_t *new_object_name = NULL;
    json_node_t *new_object_value = NULL;

    /* Skip '{' */
    str_p++;

    if ((new_object = json_node_new_object()) == NULL)
    { ret = -1; goto fail; }

    for (;;)
    {
        if (str_p == str_endp)
        { ret = -1; goto fail; }
        if (*str_p == '}') break;

        /* Name */
        if ((ret = json_node_load(&new_object_name, \
                        &str_p, str_endp)) != 0)
        { goto fail; }

        /* ':' */
        if (str_p == str_endp) 
        { ret = -1; goto fail; }
        if (*str_p != ':') break;
        str_p++;

        /* Value */
        if ((ret = json_node_load(&new_object_value, \
                        &str_p, str_endp)) != 0)
        { goto fail; }

        if ((ret = json_node_as_object_append(new_object, \
                        new_object_name, new_object_value)) != 0)
        { goto fail; }
        new_object_name = NULL;
        new_object_value = NULL;

        /* ',' */
        if (str_p == str_endp) 
        { ret = -1; goto fail; }
        if (*str_p == '}') break;
        else if (*str_p == ',')
        {
            /* Skip ',' */
            str_p++;
        }
        else
        { ret = -1; goto fail; }
    }

    /* Skip '}' */
    str_p++;

    *json_node_out = new_object;

    goto done;
fail:
    if (new_object != NULL) json_node_destroy(new_object);
    if (new_object_name != NULL) json_node_destroy(new_object_name);
    if (new_object_value != NULL) json_node_destroy(new_object_value);
done:
    *str_io = str_p;
    return ret;
}

static int json_node_string_load(json_node_t **json_node_out, \
        char **str_io, char *str_endp)
{
    int ret = 0;
    char *str_p = *str_io;
    char *str_start_p;
    json_node_t *new_json_node = NULL;

    /* Skip \" */
    str_p++;
    str_start_p = str_p;

    while (str_p != str_endp)
    {
        if (*str_p == '\"') break;
        else if (*str_p == '\\')
        {
            /* Escape */
            if (++str_p == str_endp) return -1;

            if (*str_p == 'u')
            {
                if (str_endp - str_p >= 5)
                {
                    /* FIXME */
                    return -1;
                }
                str_p += 5;
            }
            else if (*str_p == '\"')
            {
                return -1;
            }
        }
        str_p++;
    }
    if (str_p == str_endp) return -1;

    if ((new_json_node = json_node_new_string( \
                    str_start_p, \
                    (size_t)(str_p - str_start_p))) == NULL)
    { ret = -1; goto fail; }

    /* Skip \" */
    str_p++;

    *json_node_out = new_json_node;

    goto done;
fail:
    if (new_json_node != NULL)
    { json_node_destroy(new_json_node); }
done:
    *str_io = str_p;
    return ret;
}

static int json_node_number_load(json_node_t **json_node_out, \
        char **str_io, char *str_endp)
{
    int ret = 0;
    char *str_p = *str_io;
    int value = 0;
    json_node_t *new_json_node = NULL;
    int negative = 0;

    if (str_p == str_endp) goto fail;

    if (*str_p == '-')
    { negative = 1; str_p++; }

    while (str_p != str_endp)
    {
        if (IS_DIGIT(*str_p))
        {
            value = value * 10 + ((int)(*str_p) - (int)'0');
            str_p++;
        }
        else { break; }
    }

    if (negative) value = -value;
    if ((new_json_node = json_node_new_integer(value)) == NULL)
    { ret = -1; goto fail; }

    *json_node_out = new_json_node;

    goto done;
fail:
    if (new_json_node != NULL)
    { json_node_destroy(new_json_node); }
done:
    *str_io = str_p;
    return ret;
}

static int json_node_alpha_lowcase_load(json_node_t **json_node_out, \
        char **str_io, char *str_endp)
{
    int ret = 0;
    char *str_p = *str_io;
    char *str_start_p = str_p;
    size_t len;
    json_node_t *new_json_node = NULL;

    if (str_p == str_endp) goto fail;

    while (str_p != str_endp)
    {
        if (IS_ALPHA_LOWCASE(*str_p))
        { str_p++; }
        else { break; }
    }

    len = (size_t)(str_p - str_start_p);
    if ((len == 4) && (strncmp(str_start_p, "null", 4) == 0))
    {
        if ((new_json_node = json_node_new_null()) == NULL)
        { ret = -1; goto fail; }
    }
    else if ((len == 4) && (strncmp(str_start_p, "true", 4) == 0))
    {
        if ((new_json_node = json_node_new_true()) == NULL)
        { ret = -1; goto fail; }
    }
    else if ((len == 5) && (strncmp(str_start_p, "false", 5) == 0))
    {
        if ((new_json_node = json_node_new_false()) == NULL)
        { ret = -1; goto fail; }
    }

    *json_node_out = new_json_node;

    goto done;
fail:
    if (new_json_node != NULL)
    { json_node_destroy(new_json_node); }
done:
    *str_io = str_p;
    return ret;
}

static int json_node_load(json_node_t **json_node_out, \
        char **str_io, char *str_endp)
{
    char *str_p = *str_io;

    if (str_p == str_endp) return -1;

    if (*str_p == '[')
    {
        return json_node_array_load(json_node_out, \
                str_io, str_endp);
    }
    else if (*str_p == '{')
    {
        return json_node_object_load(json_node_out, \
                str_io, str_endp);
    }
    else if (*str_p == '\"')
    {
        return json_node_string_load(json_node_out, \
                str_io, str_endp);
    }
    else if ((IS_DIGIT(*str_p))||(*str_p == '-'))
    {
        return json_node_number_load(json_node_out, \
                str_io, str_endp);
    }
    else if (IS_ALPHA_LOWCASE(*str_p))
    {
        return json_node_alpha_lowcase_load(json_node_out, \
                str_io, str_endp);
    }
    else
    {
        return -1;
    }
}

int json_load(json_t **json_out, char *str, size_t len)
{
    int ret = 0;
    char *str_p = str;
    char *str_endp = str_p + len;
    json_node_t *new_json_node_root = NULL;
    json_t *new_json = NULL;

    if ((ret = json_node_load(&new_json_node_root, \
                    &str_p, str_endp)) != 0)
    { goto fail; }

    if ((new_json = json_new()) == NULL)
    { goto fail; }
    json_set_root(new_json, new_json_node_root);
    new_json_node_root = NULL;

    *json_out = new_json;
    new_json = NULL;

    goto done;
fail:
done:
    if (new_json != NULL) json_destroy(new_json);
    if (new_json_node_root != NULL) json_node_destroy(new_json_node_root);
    return ret;
}

