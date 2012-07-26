#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "a3.h"

int main(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; ++i) {
        struct a3 *a3;
        int j, up = 0;

        a3 = a3_load(argv[i]);

        if (a3 == NULL) {
            fprintf(stderr, "Can't file: %s\n", argv[i]);
            continue;
        }

        for (j = 0; j < a3->framec; ++j) {
            char *f = a3->framev[j];

            if (up) {
                printf("\033[%dF\033[J", up);
                up = 0;
            }

            printf("\033[K");
            while (*f) {
                putchar(*f);
                if (*f == '\n')
                    ++up;
                ++f;
            }

            usleep(a3->pause * 1000);
        }

        a3_free(a3);
    }

    return 0;
}
