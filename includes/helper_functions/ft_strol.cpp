#include "helpers.hpp"
#include <limits.h>

static int ft_isspace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || 
            c == '\r' || c == '\v' || c == '\f');
}

static int get_digit_value(char c, int base) {
    int val = -1;
    if (c >= '0' && c <= '9') val = c - '0';
    else if (c >= 'a' && c <= 'z') val = c - 'a' + 10;
    else if (c >= 'A' && c <= 'Z') val = c - 'A' + 10;
    if (val >= base) return -1;
    return val;
}

long ft_strtol(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    unsigned long acc = 0;
    unsigned long cutoff;
    int cutlim;
    int neg = 0, any = 0;

    while (ft_isspace(*s))
        s++;


    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+') {
        s++;
    }


    if ((base == 0 || base == 16) && *s == '0' && (*(s + 1) == 'x' || *(s + 1) == 'X')) {
        s += 2;
        base = 16;
    } else if (base == 0) {
        base = (*s == '0') ? 8 : 10;
    }

    if (base < 2 || base > 36) {
        if (endptr) *endptr = (char *)nptr;
        return 0;
    }


    cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
    cutlim = cutoff % (unsigned long)base;
    cutoff /= (unsigned long)base;

    while (*s != '\0') {
        int val = get_digit_value(*s, base);
        if (val == -1) break;

        if (any < 0 || acc > cutoff || (acc == cutoff && val > cutlim)) {
            any = -1; 
        } else {
            any = 1;
            acc *= base;
            acc += val;
        }
        s++;
    }

    if (any < 0) {
        acc = neg ? LONG_MIN : LONG_MAX;
    } else if (neg) {
        acc = -acc;
    }

    if (endptr != 0) {
        *endptr = (char *)(any ? s : nptr);
    }

    return (long)acc;
}