//----------------------------------------------------------------------------
// The confidential and proprietary information contained in this file may
// only be used by a person authorised under and to the extent permitted
// by a subsisting licensing agreement from ARM Limited or its affiliates.
//
// (C) COPYRIGHT 2016 ARM Limited or its affiliates.
// ALL RIGHTS RESERVED
//
// This entire notice must be reproduced on all copies of this file
// and copies of this file may only be made by a person if such person is
// permitted to do so under the terms of a subsisting license agreement
// from ARM Limited or its affiliates.
//----------------------------------------------------------------------------

#include <string.h>
#include "include/uriqueryparser.h"

char* query_string(const char* uri)
{
    char* query = strchr((char*)uri, '?');
    if (query != NULL) {
        query++;
        if (*query == '\0') {
            return NULL;
        } else {
            return query;
        }
        return query;
    } else {
        return NULL;
    }
}

int8_t query_param_count(char* query)
{
    int param_count = 0;
    while (NULL != (query = strchr(query, '='))) {
        param_count++;
        ++query;
    }
    return param_count;
}

bool uri_query_parameters(char* query, char *uri_query_parameters[])
{
    int i = 0;
    if (query == NULL || *query == '\0') {
        return false;
    }

    uri_query_parameters[i++] = query;
    while (NULL != (query = strchr(query, '&'))) {
        *query = '\0';
        uri_query_parameters[i] = ++query;
        i++;
    }
    return true;
}
