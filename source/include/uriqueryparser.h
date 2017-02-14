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

#ifndef URIQUERYPARSER_H
#define URIQUERYPARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ns_types.h"

/**
 * \brief Returns string containing query parameters.
 * \param uri URL
 * \return Query parameters or NULL if there is no parameters.
 */
char *query_string(char *uri);

/**
 * \brief Return count of query parameters.
 * \param query Query string
 * \return Count of query parameters
 */
int8_t query_param_count(char *query);

/**
 * \brief Extract queries from the query string.
 * \param query Query string
 * \param uri_query_parameters [OUT] List of queries
 * \return True if parsing success otherwise False.
 */
bool uri_query_parameters(char* query, char *uri_query_parameters[]);

#ifdef __cplusplus
}
#endif

#endif // URIQUERYPARSER_H
