/*
 * This file is part of an interview test.
 * See the file LICENSE in the source root for licensing terms.
 * Website: http://nxlog.org
 * Author: Renato Moraes <renatomoraesdossantos@gmail.com>
 */

#ifndef JSONPACKER_NXLOG_H
#define JSONPACKER_NXLOG_H

#include <stdio.h>
#include <stdlib.h>
#include <apr_pools.h>

#define JSONPACKERVERSION "1.00"

#define NULL_TYPE   0x00
#define CHAR_TYPE   0x01
#define SHORT_TYPE  0x02
#define INT_TYPE    0x03
#define LONG_TYPE   0x04
#define FLOAT_TYPE  0x05
#define DOUBLE_TYPE 0x06
#define STRING_TYPE 0x07
#define ARRAY_TYPE  0x08
#define OBJECT_TYPE 0x09

/**
 * NXLOG Struct
 */
typedef struct nxlog_t {
    apr_pool_t 		*pool;
    const char   *filepath;
    //const char   *log_priority;
    const char   *output;
} nxlog_t;

/**
 * NXLOG Struct GET
 * @return
 */
nxlog_t *nxlog_get();

/**
 * NXLOG Struct SET
 * @param nxlog struct for options management
 */
void nxlog_set(nxlog_t **nxlog);

/**
 * File Reader
 * @param filename
 * @return File content
 */
char* read_file(const char *filename);

#endif	/* JSONPACKER_NXLOG_H */
