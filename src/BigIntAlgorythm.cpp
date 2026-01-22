#include "BigInt.h"

BigInt BigInt::mulMod(const BigInt& a, const BigInt& b, const BigInt& mod)
{
    BigInt result(0LL);
    BigInt x = a;
    BigInt y = b;
    x.sign = y.sign = 1;

    while (!y.isZero())
    {
        if (y.a[0] & 1u)
        {
            result = result + x;
            if (result >= mod)
                result = result - mod;
        }

        x = x + x;
        if (x >= mod)
            x = x - mod;

        u32 carry = 0;
        for (int i = (int)y.size - 1; i >= 0; --i)
        {
            u64 cur = (u64)carry * BASE + y.a[i];
            y.a[i] = (u32)(cur >> 1);
            carry = (u32)(cur & 1);
        }
        y.trim();
    }
    return result;
}