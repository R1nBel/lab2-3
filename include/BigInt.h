#ifndef BIGINT_H
#define BIGINT_H

#include <iostream>
#include <cstdint>
#include <cstddef>

using u32 = uint32_t;
using u64 = uint64_t;

struct DivModResult;

class BigInt
{
public:
    static const u32 BASE = 1000000000u;
    static const int BASE_DIGS = 9;

    u32* a;
    size_t size;
    size_t capacity;
    int sign;

    BigInt();
    BigInt(long long v);
    BigInt(const char* s);
    BigInt(const BigInt& other);
    ~BigInt();

    BigInt& operator=(const BigInt& other);
    BigInt& operator=(long long v);
    BigInt operator-() const;

    friend BigInt operator+(const BigInt& x, const BigInt& y);
    friend BigInt operator-(const BigInt& x, const BigInt& y);
    BigInt mulShort(u32 v) const;

    DivModResult divModShort(u32 v) const;

    static BigInt mulMod(const BigInt& x, const BigInt& y, const BigInt& mod);

    friend bool operator==(const BigInt& x, const BigInt& y);
    friend bool operator!=(const BigInt& x, const BigInt& y);
    friend bool operator<(const BigInt& x, const BigInt& y);
    friend bool operator<=(const BigInt& x, const BigInt& y);
    friend bool operator>(const BigInt& x, const BigInt& y);
    friend bool operator>=(const BigInt& x, const BigInt& y);

    friend std::istream& operator>>(std::istream& is, BigInt& v);
    friend std::ostream& operator<<(std::ostream& os, const BigInt& v);

    bool isZero() const;

    char* toChar() const;
    static BigInt fromChar(const char* s);

    bool isOdd() const;
    BigInt div2() const;
    BigInt operator%(const BigInt& other) const;
    static bool needDecrement(u64 q, u32 v1, u32 v2, u32 u0, u32 u1, u32 u2);

    void addAssign(const BigInt& other);  
    void subAssign(const BigInt& other);   
    void mul2Assign();                     
    void div2Assign();

private:
    void reserve(size_t new_capacity);
    void push_back(u32 x);
    void trim();
    void read(const char* s);
    void swap(BigInt& other) noexcept;

    void addTo(const BigInt& other);
    void subFrom(const BigInt& other);

    static void addArrays(const u32* A, size_t sizeA, const u32* B, size_t sizeB, BigInt& res);
    static void subArrays(const u32* A, size_t sizeA, const u32* B, size_t sizeB, BigInt& res);
    static int absCompare(const BigInt& x, const BigInt& y);
    static char* readWordFromStream(std::istream& is);
};

struct DivModResult
{
    BigInt q;
    uint32_t rem;
};

#endif
