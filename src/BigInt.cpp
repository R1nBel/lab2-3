#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>

using namespace std;

class BigInt 
{
public:
    using u32 = uint32_t;
    using u64 = uint64_t;
    static const u32 BASE = 1000000000;
    static const int BASE_DIGS = 9;

    vector<u32> a;
    int sign;

    BigInt() : sign(1) 
    {
    }

    BigInt(long long v) 
    {
        *this = v; 
    }

    BigInt(const string& s) 
    {
        read(s); 
    }

    BigInt& operator=(long long v) 
    {
        sign = (v >= 0) ? 1 : -1;
        
        if (v < 0)
        {
            v = -v;
        }
            
        a.clear();

        while (v > 0) 
        {
            a.push_back((u32)(v % BASE));
            v /= BASE;
        }
        return *this;
    }

    bool isZero() const 
    { 
        
        return a.empty(); 
    }

    void trim() 
    {
        
        while (!a.empty() && a.back() == 0)
        {
            a.pop_back();
        }

        if (a.empty())
        {
            sign = 1;
        }
    }

    void read(const string& s) 
    {
        a.clear();
        
        sign = 1;

        int pos = 0;

        if (!s.empty() && (s[0] == '-' || s[0] == '+')) 
        {
            
            if (s[0] == '-')
            {
                sign = -1;
            }

            else
            {
                sign = 1;
            }

            pos = 1;
        }

        for (int i = (int)s.size() - 1; i >= pos; i -= BASE_DIGS) 
        {
            int l = max(pos, i - BASE_DIGS + 1);

            u32 x = 0;

            for (int j = l; j <= i; ++j)
            {
                x = x * 10 + (s[j] - '0'); 
            }

            a.push_back(x);
        }
        trim();
    }

    friend istream& operator>>(istream& is, BigInt& v)
    {
        string s; is >> s; v.read(s); return is;
    }

    friend ostream& operator<<(ostream& os, const BigInt& v) 
    {
        if (v.isZero()) { os << '0'; return os; }
        if (v.sign == -1) os << '-';
        os << v.a.back();
        char buf[32];
        for (int i = (int)v.a.size() - 2; i >= 0; --i) 
        {
            snprintf(buf, sizeof(buf), "%09u", v.a[i]);
            os << buf;
        }
        return os;
    }

    friend BigInt operator+(const BigInt& x, const BigInt& y) 
    {
        
        if (x.sign == y.sign) 
        {
            BigInt r;
            r.sign = x.sign;

            r.a = addAbs(x.a, y.a);
            
            r.trim();

            return r;
        }
        else 
        {
            int cmp = absCompare(x, y);

            if (cmp == 0) return BigInt(0);

            BigInt r;

            if (cmp > 0) 
            { 
                r.sign = x.sign;

                r.a = subAbs(x.a, y.a); 
            }
            else 
            { 
                r.sign = y.sign;

                r.a = subAbs(y.a, x.a);
            }
            r.trim();

            return r;
        }
    }

    friend BigInt operator-(const BigInt& x, const BigInt& y) 
    {
        return x + (-y);
    }

    BigInt operator-() const {
        BigInt r = *this;

        if (!r.isZero())
        {
            r.sign = -r.sign;
        }
        return r;
    }

    friend BigInt operator*(const BigInt& x, const BigInt& y)
    {   
        if (x.isZero() || y.isZero()) return BigInt(0);

        BigInt r;

        r.sign = x.sign * y.sign;

        r.a.assign(x.a.size() + y.a.size(), 0);

        for (size_t i = 0; i < x.a.size(); ++i)
        {
            u64 carry = 0;

            for (size_t j = 0; j < y.a.size() || carry; ++j)
            {
                u64 cur = r.a[i + j] + carry + (u64)x.a[i] * (j < y.a.size() ? (u64)y.a[j] : 0ULL);

                r.a[i + j] = (u32)(cur % BASE);

                carry = cur / BASE;
            }
        }

        r.trim();

        return r;
    }

    friend bool operator==(const BigInt& x, const BigInt& y) 
    {
        return x.sign == y.sign && x.a == y.a;
    }

    friend bool operator!=(const BigInt& x, const BigInt& y) 
    { 
        return !(x == y); 
    }

    friend bool operator<(const BigInt& x, const BigInt& y) 
    {
        if (x.sign != y.sign)
        {
            
            return x.sign < y.sign;
        }  
        int cmp = absCompare(x, y);

        return (x.sign == 1) ? cmp < 0 : cmp > 0;
    }

    friend bool operator<=(const BigInt& x, const BigInt& y) 
    { 
        return !(y < x); 
    }

    friend bool operator>(const BigInt& x, const BigInt& y) 
    { 
        return y < x; 
    }

    friend bool operator>=(const BigInt& x, const BigInt& y) 
    { 
        return !(x < y); 
    }

    BigInt mulShort(u32 v) const 
    {
        BigInt r;

        if (v == 0 || isZero())
        {
            return BigInt(0);
        }

        r.sign = sign;

        u64 carry = 0;

        for (size_t i = 0; i < a.size() || carry; ++i) 
        {  
            if (i == r.a.size())
            {
                r.a.push_back(0);
                
            }

            u64 cur = carry + (i < a.size() ? (u64)a[i] * v : 0);

            r.a[i] = (u32)(cur % BASE);

            carry = cur / BASE;
 
        }
        r.trim();
        
        return r;
    }

    pair<BigInt, u32> divModShort(u32 v) const 
    {
        BigInt q;
        q.sign = sign;

        q.a.resize(a.size());

        u64 rem = 0;

        for (int i = (int)a.size() - 1; i >= 0; --i) 
        {
            u64 cur = a[i] + rem * BASE;
            

            q.a[i] = (u32)(cur / v);
            

            rem = cur % v;

        }
        q.trim();
        return { q, (u32)rem };
    }

    char* toChar() const 
    {
        string s = toString();

        char* cstr = new char[s.size() + 1];

        strcpy_s(cstr, s.size() + 1, s.c_str());

        return cstr;
    }

    static BigInt fromChar(const char* s) 
    {
        return BigInt(string(s));
    }

    string toString() const
    {
        if (isZero()) return "0";

        string s = (sign == -1 ? "-" : "") + std::to_string(a.back());

        char buf[32];

        for (int i = (int)a.size() - 2; i >= 0; --i) 
        {
            snprintf(buf, sizeof(buf), "%09u", a[i]);

            s += buf;
        }
        return s;
    }

    bool isOdd() const
    {
        
        if (a.empty())
        { 
            return false;
        }
        return (a[0] & 1u) != 0;
    }

    BigInt bigPow(long long exponent) const
    {
        BigInt base = *this;

        BigInt result(1);

        while (exponent > 0)
        {
            
            if (exponent & 1LL)
            {
                result = result * base;
            }

            base = base * base;

            exponent >>= 1LL;
        }
        return result;
    }

    BigInt div2() const
    {
        BigInt q;
        q.sign = sign;
        
        if (a.empty())
        {  
            return BigInt(0); 
        }

        q.a.resize(a.size());

        u64 carry = 0;
        
        for (int i = (int)a.size() - 1; i >= 0; --i)
        {
            u64 cur = a[i] + carry * (u64)BASE;

            q.a[i] = (u32)(cur / 2);

            carry = cur % 2;
        }

        q.trim();

        return q;
    }

    const BigInt mulMod(const BigInt& x, const BigInt& y) const
    {
        BigInt mod = *this;

        BigInt a = x;

        BigInt b = y;

        BigInt res(0);

        if (a >= mod)
        {
            a = a - mod;
        }

        if (b >= mod)
        {
            b = b - mod;
        }

        while (!b.isZero())
        {
            
            if (b.isOdd())
            {
                res = res + a;

                if (res >= mod)
                {
                    res = res - mod;
                    
                }
            }

            a = a + a;
            
            if (a >= mod)
            {
                a = a - mod;
            }
            b = b.div2();
        }
        return res;
    }

private:
    static int absCompare(const BigInt& x, const BigInt& y) 
    {
        if (x.a.size() != y.a.size())
        {
            return x.a.size() < y.a.size() ? -1 : 1;
        }

        for (int i = (int)x.a.size() - 1; i >= 0; --i)
        {
            if (x.a[i] != y.a[i])
            {
                return x.a[i] < y.a[i] ? -1 : 1;
            }
        }
        return 0;
    }

    static vector<u32> addAbs(const vector<u32>& A, const vector<u32>& B) 
    {
        size_t n = max(A.size(), B.size());

        vector<u32> res(n);
        u64 carry = 0;
        
        for (size_t i = 0; i < n || carry; ++i) 
        {
            if (i == res.size())
            {
                res.push_back(0);
            }

            u64 sum = carry + (i < A.size() ? A[i] : 0) + (i < B.size() ? B[i] : 0);

            res[i] = (u32)(sum % BASE);

            carry = sum / BASE;
  
        }
        return res;
    }

    static vector<u32> subAbs(const vector<u32>& A, const vector<u32>& B) 
    {
        vector<u32> res = A;

        u64 carry = 0;

        for (size_t i = 0; i < B.size() || carry; ++i) 
        {
            u64 sub = (u64)res[i] - (i < B.size() ? B[i] : 0) - carry;

            if ((long long)sub < 0) 
            {
                sub += BASE;

                carry = 1;
            }
            else
            {
                carry = 0;
            }
                
            res[i] = (u32)sub;
        }

        while (!res.empty() && res.back() == 0)
        {
            res.pop_back();
        }
        return res;
    }
};
