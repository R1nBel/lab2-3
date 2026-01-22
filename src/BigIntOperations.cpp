#include "BigInt.h"

BigInt& BigInt::operator=(const BigInt& other)
{
    if (this == &other) return *this;

    if (other.size > capacity)
    {
        delete[] a;
        a = nullptr;
        size = capacity = 0;
        if (other.size > 0) reserve(other.size);
    }

    if (other.size > 0)
    {
        for (size_t i = 0; i < other.size; ++i) a[i] = other.a[i];
        size = other.size;
        sign = other.sign;
    }
    else
    {
        size = 0;
        sign = 1;
    }

    return *this;
}

BigInt& BigInt::operator=(long long v)
{
    delete[] a;
    a = nullptr;
    size = capacity = 0;
    sign = 1;

    if (v == 0) return *this;

    unsigned long long uv;
    if (v < 0) {
        sign = -1;
        uv = (unsigned long long)(-(v + 1)) + 1ULL;
    }
    else {
        sign = 1;
        uv = (unsigned long long)v;
    }

    unsigned long long tmp = uv;
    int cnt = 0;
    while (tmp > 0) { tmp /= BASE; ++cnt; }

    reserve((size_t)cnt);
    size = 0;
    while (uv > 0)
    {
        a[size++] = (u32)(uv % BASE);
        uv /= BASE;
    }

    trim();
    return *this;
}

BigInt BigInt::operator-() const
{
    BigInt r = *this;

    if (!r.isZero()) r.sign = -r.sign;

    return r;
}

BigInt operator+(const BigInt& x, const BigInt& y)
{
    if (x.sign == y.sign)
    {
        BigInt r;

        r.sign = x.sign;
        BigInt::addArrays(x.a, x.size, y.a, y.size, r);

        r.trim();

        return r;
    }
    else
    {
        int cmp = BigInt::absCompare(x, y);

        if (cmp == 0) return BigInt(0LL);

        BigInt r;

        if (cmp > 0) {
            r.sign = x.sign;
            BigInt::subArrays(x.a, x.size, y.a, y.size, r);
        }
        else
        {
            r.sign = y.sign;
            BigInt::subArrays(y.a, y.size, x.a, x.size, r);
        }

        r.trim();

        return r;
    }
}

BigInt operator-(const BigInt& x, const BigInt& y)
{
    return x + (-y);
}


BigInt BigInt::mulShort(u32 v) const
{
    if (v == 0 || isZero()) return BigInt(0LL);

    BigInt r;
    r.sign = sign;
    r.reserve(size + 1);
    r.size = size;

    u64 carry = 0;
    for (size_t i = 0; i < size; ++i)
    {
        u64 cur = carry + (u64)a[i] * v;
        r.a[i] = (u32)(cur % BASE);
        carry = cur / BASE;
    }
    if (carry)
    {
        if (r.size >= r.capacity) r.reserve(r.size + 1);
        r.a[r.size++] = (u32)carry;
    }

    r.trim();

    return r;
}

DivModResult BigInt::divModShort(u32 v) const
{
    if (v == 0) throw std::runtime_error("Division by zero");

    BigInt q;
    q.sign = sign;
    q.reserve(size);
    q.size = size;

    u64 rem = 0;
    for (int i = (int)size - 1; i >= 0; --i)
    {
        u64 cur = a[i] + rem * (u64)BASE;
        q.a[i] = (u32)(cur / v);
        rem = cur % v;
    }

    q.trim();

    DivModResult r; r.q = q; r.rem = (u32)rem;

    return r;
}


std::istream& operator>>(std::istream& is, BigInt& v)
{
    char* s = BigInt::readWordFromStream(is);

    if (!s) return is;

    v.read(s);

    delete[] s;
    return is;
}

std::ostream& operator<<(std::ostream& os, const BigInt& v)
{
    if (v.isZero()) { os << '0'; return os; }

    if (v.sign == -1) os << '-';
    os << v.a[v.size - 1];

    char buf[BigInt::BASE_DIGS + 1];

    for (int i = (int)v.size - 2; i >= 0; --i)
    {
        u32 block = v.a[i];
        for (int j = BigInt::BASE_DIGS - 1; j >= 0; --j)
        {
            buf[j] = char('0' + (block % 10));
            block /= 10;
        }

        buf[BigInt::BASE_DIGS] = '\0';

        os << buf;
    }
    return os;
}


char* BigInt::toChar() const
{
    if (isZero())
    {
        char* cstr = new char[2];
        cstr[0] = '0'; cstr[1] = '\0';

        return cstr;
    }

    u32 highest = a[size - 1];
    int digits_in_highest = 0;
    u32 tmp = highest;

    do { ++digits_in_highest; tmp /= 10; } while (tmp > 0);

    int total_digits = digits_in_highest + (int)(size - 1) * BASE_DIGS;
    if (sign == -1) ++total_digits;

    char* cstr = new char[total_digits + 1];
    int pos = 0;
    if (sign == -1) cstr[pos++] = '-';

    char buf[32];
    int bp = 0;
    tmp = highest;
    do { buf[bp++] = char('0' + (tmp % 10)); tmp /= 10; } while (tmp > 0);
    for (int i = bp - 1; i >= 0; --i) cstr[pos++] = buf[i];

    for (int i = (int)size - 2; i >= 0; --i)
    {
        u32 block = a[i];
        for (int j = BASE_DIGS - 1; j >= 0; --j)
        {
            buf[j] = char('0' + (block % 10));
            block /= 10;
        }

        for (int j = 0; j < BASE_DIGS; ++j) cstr[pos++] = buf[j];
    }

    cstr[pos] = '\0';
    return cstr;
}

BigInt BigInt::fromChar(const char* s) { return BigInt(s); }


void BigInt::addArrays(const u32* A, size_t sizeA, const u32* B, size_t sizeB, BigInt& res)
{
    size_t n = (sizeA > sizeB) ? sizeA : sizeB;

    res.reserve(n + 1);
    res.size = 0;

    u64 carry = 0;
    for (size_t i = 0; i < n || carry; ++i)
    {
        u64 val = carry;
        if (i < sizeA) val += A[i];
        if (i < sizeB) val += B[i];

        res.push_back((u32)(val % BASE));
        carry = val / BASE;
    }
}

void BigInt::subArrays(const u32* A, size_t sizeA, const u32* B, size_t sizeB, BigInt& res)
{
    res.reserve(sizeA);
    res.size = sizeA;
    if (sizeA > 0) for (size_t i = 0; i < sizeA; ++i) res.a[i] = A[i];

    u64 carry = 0;
    for (size_t i = 0; i < sizeB || carry; ++i)
    {
        long long cur = (long long)res.a[i] - (long long)(i < sizeB ? B[i] : 0) - (long long)carry;
        if (cur < 0) { cur += BASE; carry = 1; }
        else carry = 0;

        res.a[i] = (u32)cur;
    }

    while (res.size > 0 && res.a[res.size - 1] == 0) --res.size;
}

int BigInt::absCompare(const BigInt& x, const BigInt& y)
{
    if (x.size != y.size) return (x.size < y.size) ? -1 : 1;

    for (int i = (int)x.size - 1; i >= 0; --i)
    {
        if (x.a[i] != y.a[i]) return (x.a[i] < y.a[i]) ? -1 : 1;
    }
    return 0;
}

void BigInt::read(const char* s)
{
    delete[] a; a = nullptr; size = capacity = 0; sign = 1;

    if (!s) return;

    int pos = 0;
    if (s[0] == '-' || s[0] == '+')
    {
        sign = (s[0] == '-') ? -1 : 1;
        pos = 1;
    }

    while (s[pos] == '0') ++pos;
    if (s[pos] == '\0') { sign = 1; return; }

    int len = 0;
    const char* p = s + pos;
    while (*p) { ++len; ++p; }

    int blocks = (len + BASE_DIGS - 1) / BASE_DIGS;
    reserve((size_t)blocks);
    size = 0;

    for (int i = len + pos - 1; i >= pos; i -= BASE_DIGS)
    {
        int l = (pos > i - BASE_DIGS + 1) ? pos : i - BASE_DIGS + 1;
        u32 x = 0;
        for (int j = l; j <= i; ++j) x = x * 10 + (u32)(s[j] - '0');

        push_back(x);
    }

    trim();
}

char* BigInt::readWordFromStream(std::istream& is)
{
    int ch = is.get();

    while (ch != EOF && std::isspace(ch)) ch = is.get();
    if (ch == EOF) return nullptr;

    size_t cap = 64, len = 0;
    char* buf = new char[cap];

    while (ch != EOF && !std::isspace(ch))
    {
        if (len + 1 >= cap)
        {
            size_t newcap = cap * 2;
            char* nb = new char[newcap];
            for (size_t i = 0; i < len; ++i) nb[i] = buf[i];

            delete[] buf;
            buf = nb;
            cap = newcap;
        }

        buf[len++] = (char)ch;
        ch = is.get();
    }

    buf[len] = '\0';

    if (ch != EOF) is.unget();

    return buf;
}

bool BigInt::isOdd() const {
    if (isZero()) return false;
    return (a[0] & 1u) != 0;
}

BigInt BigInt::div2() const
{
    if (isZero()) return BigInt(0LL);

    BigInt result;
    result.sign = sign;
    result.reserve(size);
    result.size = size;

    u64 carry = 0;
    for (int i = (int)size - 1; i >= 0; --i)
    {
        u64 cur = carry * (u64)BASE + a[i];
        result.a[i] = (u32)(cur / 2);
        carry = cur % 2;
    }

    result.trim();
    return result;
}

BigInt BigInt::operator%(const BigInt& other) const
{
    if (other.isZero())
        throw std::runtime_error("Division by zero");

    BigInt a = *this;
    BigInt b = other;

    a.sign = 1;
    b.sign = 1;

    if (a < b) return a;

    u32 norm = 1;
    while ((u64)b.a[b.size - 1] * norm < BASE / 2)
        norm <<= 1;

    if (norm > 1) {
        a = a.mulShort(norm);
        b = b.mulShort(norm);
    }

    size_t n = b.size;
    size_t m = a.size - n;

    BigInt r = a;

    for (int j = (int)m; j >= 0; --j)
    {
        u32 u0 = (r.size > j + n) ? r.a[j + n] : 0;
        u32 u1 = (r.size > j + n - 1) ? r.a[j + n - 1] : 0;
        u32 u2 = (r.size > j + n - 2) ? r.a[j + n - 2] : 0;

        u32 v1 = b.a[n - 1];
        u32 v2 = (n > 1) ? b.a[n - 2] : 0;

        u64 dividend = (u64)u0 * BASE + u1;
        u64 qhat = dividend / v1;
        if (qhat >= BASE) qhat = BASE - 1;

        while (needDecrement(qhat, v1, v2, u0, u1, u2))
            --qhat;

        BigInt prod = b.mulShort((u32)qhat);

        if (j > 0) {
            prod.reserve(prod.size + j);
            for (int i = (int)prod.size - 1; i >= 0; --i)
                prod.a[i + j] = prod.a[i];
            for (size_t i = 0; i < j; ++i)
                prod.a[i] = 0;
            prod.size += j;
        }

        while (r < prod) {
            --qhat;
            prod = b.mulShort((u32)qhat);
            if (j > 0) {
                prod.reserve(prod.size + j);
                for (int i = (int)prod.size - 1; i >= 0; --i)
                    prod.a[i + j] = prod.a[i];
                for (size_t i = 0; i < j; ++i)
                    prod.a[i] = 0;
                prod.size += j;
            }
        }

        r = r - prod;
    }

    if (norm > 1) {
        r = r.divModShort(norm).q;
    }

    r.trim();
    return r;
}


bool BigInt::needDecrement(
    u64 q, u32 v1, u32 v2,
    u32 u0, u32 u1, u32 u2)
{
    u64 left_hi = q * v1;
    u64 left_lo = q * v2;

    if (left_hi > u0) return true;
    if (left_hi < u0) return false;

    u64 right_mid = (u64)u1 * BASE + u2;
    return left_lo > right_mid;
}

void BigInt::addAssign(const BigInt& other)
{
    size_t n = (size > other.size) ? size : other.size;
    reserve(n + 1);

    u64 carry = 0;
    size_t i = 0;
    for (; i < n || carry; ++i) {
        if (i >= size) a[i] = 0, size = i + 1;
        u64 sum = (u64)a[i] + carry;
        if (i < other.size) sum += other.a[i];
        a[i] = (u32)(sum % BASE);
        carry = sum / BASE;
    }
    trim();
}

void BigInt::subAssign(const BigInt& other)
{
    u64 borrow = 0;
    for (size_t i = 0; i < other.size || borrow; ++i) {
        u64 sub = borrow + (i < other.size ? other.a[i] : 0);
        if ((u64)a[i] >= sub) { a[i] -= (u32)sub; borrow = 0; }
        else { a[i] = (u32)((u64)a[i] + BASE - sub); borrow = 1; }
    }
    trim();
}

void BigInt::mul2Assign()
{
    u64 carry = 0;
    reserve(size + 1);
    for (size_t i = 0; i < size; ++i) {
        u64 cur = (u64)a[i] * 2 + carry;
        a[i] = (u32)(cur % BASE);
        carry = cur / BASE;
    }
    if (carry) a[size++] = (u32)carry;
    trim();
}

void BigInt::div2Assign()
{
    u64 carry = 0;
    for (int i = (int)size - 1; i >= 0; --i) {
        u64 cur = carry * (u64)BASE + a[i];
        a[i] = (u32)(cur / 2);
        carry = cur % 2;
    }
    trim();
}
