#include "BigInt.h"

BigInt BigInt::mulMod(const BigInt& x, const BigInt& y, const BigInt& mod)
{
    if (mod.isZero()) throw std::invalid_argument("mod = 0");
    if (x.isZero() || y.isZero()) return BigInt(0LL);

    BigInt a = x % mod;
    if (a.sign < 0) a = a + mod;

    BigInt b = y;
    if (b.sign < 0) b.sign = 1;

    BigInt res(0LL);
    res.reserve(mod.size + 1);
    res.size = 1; res.a[0] = 0; res.sign = 1;

    a.reserve(mod.size + 1);

    while (!b.isZero()) {
        if (b.isOdd()) {
            res.addAssign(a);
            while (res >= mod) res.subAssign(mod);
        }

        a.mul2Assign();
        while (a >= mod) a.subAssign(mod);

        b.div2Assign();
    }

    return res;
}
