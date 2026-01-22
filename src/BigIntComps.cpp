#include "BigInt.h"

bool operator==(const BigInt& x, const BigInt& y)
{
    if (x.sign != y.sign || x.size != y.size) return false;

    for (size_t i = 0; i < x.size; ++i) if (x.a[i] != y.a[i]) return false;

    return true;
}

bool operator!=(const BigInt& x, const BigInt& y) { return !(x == y); }

bool operator<(const BigInt& x, const BigInt& y)
{
    if (x.sign != y.sign) return x.sign < y.sign;

    int cmp = BigInt::absCompare(x, y);

    return (x.sign == 1) ? (cmp < 0) : (cmp > 0);
}

bool operator<=(const BigInt& x, const BigInt& y) { return !(y < x); }

bool operator>(const BigInt& x, const BigInt& y) { return y < x; }

bool operator>=(const BigInt& x, const BigInt& y) { return !(x < y); }
