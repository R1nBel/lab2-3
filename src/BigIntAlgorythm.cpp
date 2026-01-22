#include "BigInt.h"

BigInt BigInt::mulMod(BigInt& x, BigInt& y, const BigInt& mod)
{
    BigInt result(0LL);

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