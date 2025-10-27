/*
 * This file is part of an interview test.
 * See the file LICENSE in the source root for licensing terms.
 * Website: http://nxlog.org
 * Author: Renato Moraes <renatomoraesdossantos@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>

#include <cjson/cJSON.h>

#include <apr_general.h>
#include <apr_pools.h>
#include <apr_hash.h>
#include <apr_getopt.h>

#include <check.h>

#include "nxlog.h"
#include "tlv_box.h"
#include "log.h"


/**
 * Options Helper
 */
static void help()
{
    printf("\n"
        " Usage:\n"
        " jsonPacker [options]\n"
        " Available options:\n"
        "   [-h] [--help] Print help\n"
        "   [-f] [--file] JSON filepath to process\n"
        "   [-o] [--output] Configure output dir\n"
        "   [-v] [--version] Show version\n"
    );
}

/**
 * Parse CMD line
 * @param nxlog struct for options management
 * @param argc
 * @param argv
 */
static void cmd_line_options(nxlog_t *nxlog, int argc, const char * const *argv)
{
    apr_getopt_t *opt;
    const char *opt_arg;
    apr_status_t rv;
    int cmd;

    const apr_getopt_option_t options[] = {
            { "help", 'h', 0, "Print help" },
            { "file", 'f', 1, "JSON filepath to process" },
            { "output", 'o', 1, "Configure output dir" },
            { "version", 'v', 1, "Show version" },
            { NULL, 0, 0, NULL },
    };


    apr_getopt_init(&opt, nxlog->pool, argc, argv);
    while ( (rv = apr_getopt_long(opt, options, &cmd, &opt_arg)) == APR_SUCCESS )
    {
        switch ( cmd )
        {
            case 'h':
                help();
                exit(-1);
            case 'f':
                nxlog->filepath = opt_arg;
                break;
            case 'o':
                nxlog->output = opt_arg;
                break;
            case 'v':
                printf("%s", JSONPACKERVERSION);
                exit(-1);
        }
    }

    if ( (rv != APR_SUCCESS) && (rv != APR_EOF) )
    {
        printf("Error: Could not parse options. Try again.\n");
        log_error("Could not parse options.");
        exit(-1);
    }
}

/**
 * Parse JSON file
 * @param filename
 * @return JSON content parsed
 */
static cJSON *parse_file(const char *filename)
{
    log_info("Parsing...");
    char *content = read_file(filename);

    cJSON *parsed = cJSON_Parse(content);

    if (content != NULL)
    {
        free(content);
    } else {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
            log_error("Error before: %s\n", error_ptr);
        }
    }

    return parsed;
}


/**
 * Generate hashtable and TLV encoding
 * @param hash struct for hash tables
 * @param parsed struct for cJSON object
 * @param box struct for TLV object
 */
void create_hash_tlv(apr_hash_t *hash, cJSON *parsed, tlv_box_t *box)
{
    log_info("Encoding...");
    if (parsed == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
            log_error("Error before: %s\n", error_ptr);

        }
        exit(-1);
    }

    for (int i = 0; i < cJSON_GetArraySize(parsed); i++)
    {
        cJSON *jItem = cJSON_GetArrayItem(parsed, i);

        if (cJSON_IsString(jItem))
        {
            apr_hash_set (hash, jItem->string, APR_HASH_KEY_STRING, jItem->valuestring);
            tlv_box_put_string(box, STRING_TYPE, (char *)jItem->valuestring);
        }
        if (cJSON_IsBool(jItem))
        {
            if (cJSON_IsTrue(jItem))
            {
                apr_hash_set (hash, jItem->string, APR_HASH_KEY_STRING, TRUE);
                tlv_box_put_string(box, STRING_TYPE, (char *)"TRUE");
            }
            else
            {
                apr_hash_set(hash, jItem->string, APR_HASH_KEY_STRING, FALSE);
                tlv_box_put_string(box, STRING_TYPE, (char *) "FALSE");
            }
        }
        if (cJSON_IsNumber(jItem))
        {
            apr_hash_set (hash, jItem->string, APR_HASH_KEY_STRING, jItem->valueint);
            tlv_box_put_int(box, INT_TYPE, (int)jItem->valueint);
        }
    }

    if (tlv_box_serialize(box) != 0)
    {
        printf("box serialize failed !\n");
        log_error("box serialize failed !\n");
        exit(-1);
    }

    //printf("box serialize success, %d bytes \n", tlv_box_get_size(box));

    cJSON_Delete(parsed);
}

/**
 * Iterate hashtable
 * @param pool struct for pool management
 * @param hash struct for hash tables
 */
void iterate_hash(apr_pool_t *pool, apr_hash_t *hash)
{
    apr_hash_index_t *idx;
    const void *key;
    void *val;

    for (idx = apr_hash_first (pool, hash); idx; idx = apr_hash_next (idx))
    {
        apr_hash_this (idx, &key, NULL, &val);
        printf ("key = '%s', val = '%s'\n", (const char *) key, (const char *) val);
    }
}

int main(int argc, const char * const *argv)
{
    apr_pool_t *pool;
    nxlog_t *nxlog;
    nxlog_set(&nxlog);

    apr_status_t rv;
    log_info("Initializing ...");
    apr_initialize();

    //Create APR pool
    if ( (rv = apr_pool_create(&pool, NULL)) != APR_SUCCESS )
    {
        apr_terminate();
        log_error("Create APR Pool error.");
        return 0;
    }

    nxlog->pool = pool;
    cmd_line_options(nxlog, argc, argv);

    //Parse JSON file
    cJSON *parsed = parse_file(nxlog->filepath);

    //Encode parsed file and insert the data in a hashtable
    apr_hash_t *hash = apr_hash_make (nxlog->pool);
    tlv_box_t *box = tlv_box_create();
    create_hash_tlv (hash, parsed, box);

    //Write binary file
    FILE *write_ptr;
    write_ptr = fopen(nxlog->output,"wb");
    fwrite(&box->m_serialized_buffer, tlv_box_get_size(box), 1, write_ptr);

    tlv_box_destroy(box);

    /* the hash table is destroyed when @pool is destroyed */
    apr_pool_destroy(nxlog->pool);

    apr_terminate();

    return 0;
}