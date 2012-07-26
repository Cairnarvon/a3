#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "a3.h"

int main(int argc, char **argv)
{
    int i, opt;
    struct a3 a3;
    char *out = "-";
    extern char *optarg;
    extern int optind;

    if (argc < 4) {
        fprintf(stderr, "%s [-o FILENAME] [-p PAUSE] frame1.txt...\n", argv[0]);
        return 1;
    }

    a3.pause = 100;

    while ((opt = getopt(argc, argv, "+o:p:")) != -1) {
        switch (opt) {
        case 'p':
            a3.pause = atoi(optarg);
            break;
        case 'o':
            out = optarg;
            break;
        }
    }

    if (a3.pause < 0)
        a3.pause = 0;

    a3.framec = argc - optind;
    a3.framev = malloc(sizeof(char*) * a3.framec);

    for (i = 0; i < a3.framec; ++i) {
        FILE *f;
        long sz;

        if ((f = fopen(argv[i + optind], "r")) == NULL) {
            fprintf(stderr, "Can't frame: %s\n", argv[i + optind]);
            return 2;
        }

        fseek(f, 0, SEEK_END);
        sz = ftell(f);
        rewind(f);

        a3.framev[i] = malloc(sz + 1);
        fread(a3.framev[i], 1, sz, f);
        a3.framev[i][sz] = 0;

        fclose(f);
    }

    if (a3_save(a3, out) != 0) {
        fprintf(stderr, "Can't save for some reason.\n");
        return 3;
    }

    return 0;
}
