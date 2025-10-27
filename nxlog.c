/*
 * This file is part of an interview test.
 * See the file LICENSE in the source root for licensing terms.
 * Website: http://nxlog.org
 * Author: Renato Moraes <renatomoraesdossantos@gmail.com>
 */

#include "nxlog.h"

static nxlog_t *_nxlog = NULL;

void nxlog_set(nxlog_t **nxlog)
{
    nxlog_t *newStack;
    if (!(newStack = malloc(sizeof(*newStack)))) {
        fprintf (stderr, "%s() Error: Memory allocation failed.\n", __func__);
        exit (EXIT_FAILURE);
    }
    newStack->output = "";

    *nxlog = newStack;
}

nxlog_t *nxlog_get()
{
    return ( _nxlog );
}

char* read_file(const char *filename) {
    FILE *file = NULL;
    long length = 0;
    char *content = NULL;
    size_t read_chars = 0;

    /* open in read binary mode */
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        goto cleanup;
    }

    /* get the length */
    if (fseek(file, 0, SEEK_END) != 0)
    {
        goto cleanup;
    }
    length = ftell(file);
    if (length < 0)
    {
        goto cleanup;
    }
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        goto cleanup;
    }

    /* allocate content buffer */
    content = (char*)malloc((size_t)length + sizeof(""));
    if (content == NULL)
    {
        goto cleanup;
    }

    /* read the file into memory */
    read_chars = fread(content, sizeof(char), (size_t)length, file);
    if ((long)read_chars != length)
    {
        free(content);
        content = NULL;
        goto cleanup;
    }
    content[read_chars] = '\0';


    cleanup:
    if (file != NULL)
    {
        fclose(file);
    }

    return content;
}