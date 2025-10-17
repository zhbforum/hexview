#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int hv_hex_digits_u64(uint64_t v)
{
    int d = 1;
    while (v >= 0x10)
    {
        v >>= 4;
        ++d;
    }
    return d;
}

int hv_parse_u64(const char *s, uint64_t *out)
{
    if (!s || !*s)
        return 0;
    int base = 10;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
    {
        base = 16;
        s += 2;
    }
    char *end = NULL;
    unsigned long long v = strtoull(s, &end, base);
    if (end == s || *end != '\0')
        return 0;
    *out = (uint64_t)v;
    return 1;
}
