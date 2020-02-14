// Author: Nat Tuck
// 3650 starter code
// don't change this file

#include <stdio.h>
#include <assert.h>

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage:\n");
        printf("\t%s file.dat\n", argv[1]);
    }

    const char* fname = argv[1];

    long count = 0;
    int  rv;

    FILE* data = fopen(fname, "r");
    assert(data != NULL);
    rv = fread(&count, sizeof(long), 1, data);
    assert(rv == 1);

    printf("count = %ld\n", count);

    if (count > 100) {
        printf("Truncating to 100 items.\n");
        count = 100;
    }

    for (long ii = 0; ii < count; ++ii) {
        float num = 0.0f;
        rv = fread(&num, sizeof(float), 1, data);
        assert(rv == 1);

        printf("%.04f\n", num);
    }
    rv = fclose(data);
    assert(rv == 0);
    return 0;
}


