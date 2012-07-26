#ifndef A3_H
#define A3_H

 struct a3 {
    char **framev;
    int framec;
    int pause;
};

int a3_save(struct a3 a3, char *fname);
struct a3 *a3_load(char *fname);

void a3_free(struct a3 *a3);

#endif
