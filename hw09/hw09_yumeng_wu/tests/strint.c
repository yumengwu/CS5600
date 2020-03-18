
#include <stdio.h>

#include "gc.h"
#include "str.h"

long
max(long xx, long yy)
{
    return xx > yy ? xx : yy;
}

long
get_digit(char* xx, long nn, long ii)
{
    char dd[2] = "0";

    if (ii < nn) {
        dd[0] = xx[ii];
    }

    return aton(dd);
}

char*
si_add(char* xx, char* yy)
{
    char* aa = rev(xx);
    char* bb = rev(yy);

    long alen = len(aa);
    long blen = len(bb);
    long clen = 1 + max(alen, blen);

    char* cc = gc_malloc(clen + 1);

    long carry = 0;

    for (long ii = 0; ii < clen; ++ii) {
        long ad = get_digit(aa, alen, ii);
        long bd = get_digit(bb, blen, ii);
        long cd = ad + bd + carry;

        if (cd > 9) {
            carry = 1;
            cd -= 10;
        }
        else {
            carry = 0;
        }

        //printf("%ld + %ld = %ld (carry %ld)\n", ad, bd, cd, carry);

        char* tmp = ntoa(cd);
        cc[ii] = tmp[0];
    }

    if (cc[clen-1] == '0') {
        cc[clen-1] = 0;
    }
    else {
        cc[clen] = 0;
    }

    return rev(cc);
}
