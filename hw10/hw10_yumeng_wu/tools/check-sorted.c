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
    int  code = 0;

    FILE* data = fopen(fname, "r");
    assert(data != NULL);
    rv = fread(&count, sizeof(long), 1, data);
    assert(rv == 1);

    float prev = 0.0f;

    for (long ii = 0; ii < count; ++ii) {
        float num = 0.0f;
        rv = fread(&num, sizeof(float), 1, data);
        assert(rv == 1);

        if (num < prev) {
            printf("Numbers out of order:\n");
            printf("  data[%ld] = %.04f\n", ii - 1, prev);
            printf("  data[%ld] = %.04f\n", ii, num);
            code = 1;
            break;
        }

        prev = num;
    }

    rv = fclose(data);
    assert(rv == 0);
    return code;
}


