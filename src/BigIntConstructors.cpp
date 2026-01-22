#include "BigInt.h"

BigInt::BigInt() : a(nullptr), size(0), capacity(0), sign(1) {}

BigInt::BigInt(long long v) : a(nullptr), size(0), capacity(0), sign(1)
{
    *this = v;
}

BigInt::BigInt(const char* s) : a(nullptr), size(0), capacity(0), sign(1)
{
    read(s);
}

BigInt::BigInt(const BigInt& other) : a(nullptr), size(0), capacity(0), sign(1)
{
    if (other.size > 0)
    {
        reserve(other.size);

        for (size_t i = 0; i < other.size; ++i) a[i] = other.a[i];

        size = other.size;
        sign = other.sign;
    }
    else
    {
        sign = other.sign;
    }
}

BigInt::~BigInt()
{
    delete[] a;
}