#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <zlib.h>

#include "a3.h"

int a3_save(struct a3 a3, char *fname)
{
    int i, ret;
    unsigned long sz = 4, bsz;
    FILE *f;
    unsigned char *buffa, *zbuffa;

    if (strcmp(fname, "-") == 0)
        f = stdout;
    else if ((f = fopen(fname, "w")) == NULL)
        return -1;

    /* Magic bytes */
    fputc('A', f);
    fputc('3', f);
    fputc('\0', f);
    fputc('\0', f);

    /* zlib input buffer */
    buffa = malloc(sz);

    /* Pause in ms, 32-bit LE */
    memcpy(buffa, &htole32(a3.pause), 4);

    /* Each frame, plus closing NUL */
    for (i = 0; i < a3.framec; ++i) {
        int len = strlen(a3.framev[i]) + 1;

        buffa = realloc(buffa, sz + len * sizeof(char));
        memcpy(buffa + sz, a3.framev[i], len);

        sz += len;
    }

    /* Compress the whole thing with zlib */
    zbuffa = malloc(sz);
    bsz = sz;
    if (compress2(zbuffa, &bsz, buffa, sz, 9) != Z_OK) {
        ret = 1;
        goto cleanup;
    }
    fwrite(zbuffa, 1, bsz, f);
    ret = 0;

cleanup:
    fclose(f);
    free(buffa);
    free(zbuffa);

    return ret;
}

struct a3 *a3_load(char *fname)
{
    int i;
    unsigned long bsz, sz1, sz2;
    struct a3 *a3;
    FILE *f;
    char bytes[4];
    unsigned char *zbuffa, *buffa;

    if ((f = fopen(fname, "r")) == NULL)
        return NULL;

    /* Determine size of compressed buffer combined */
    fseek(f, 0, SEEK_END);
    bsz = ftell(f) - 4;
    rewind(f);

    /* Verify magic bytes */
    fread(bytes, 1, 4, f);
    if (bytes[0] != 'A' || bytes[1] != '3' || bytes[2] != 0 || bytes[3] != 0)
        return NULL;

    a3 = malloc(sizeof(struct a3));

    /* Read compressed buffer */
    zbuffa = malloc(bsz);
    fread(zbuffa, 1, bsz, f);
    fclose(f);

    /* Uncompress the buffer */
    sz1 = sz2 = bsz * 2;
    buffa = malloc(sz1);
    while ((i = uncompress(buffa, &sz2, zbuffa, bsz)) == Z_BUF_ERROR) {
        sz2 = sz1 *= 2;
        buffa = realloc(buffa, sz1);
    }
    free(zbuffa);
    if (i != Z_OK)
        goto fail;

    /* Read pause value */
    memcpy(&(a3->pause), buffa, 4);
    a3->pause = le32toh(a3->pause);
    if (a3->pause < 0)
        goto fail;

    /* Count number of frames */
    a3->framec = 0;
    for (i = 4;  i < (int)sz2; ++i)
        if (buffa[i] == 0)
            ++(a3->framec);
    if (a3->framec == 0)
        goto fail;

    /* Allocate enough space to store them all */
    a3->framev = malloc(sizeof(char*) * a3->framec);

    /* Copy all frames at once */
    a3->framev[0] = malloc(sz2 - 4);
    memcpy(a3->framev[0], buffa + 4, sz2 - 4);

    /* Divide up and assign pointers */
    for (i = 1; i < a3->framec; ++i) {
        a3->framev[i] = a3->framev[i - 1] + strlen((a3->framev)[i - 1]) + 1;
    }

    goto success;

fail:
    free(buffa);
    free(a3);
    return NULL;

success:
    return a3;
}

void a3_free(struct a3 *a3)
{
    free(a3->framev[0]);
    free(a3->framev);
    free(a3);
}
