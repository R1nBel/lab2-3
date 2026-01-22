#include "BigInt.h"

void BigInt::reserve(size_t new_capacity)
{
    if (new_capacity <= capacity) return;

    size_t target = new_capacity;
    if (target < 4) target = 4;
    if (capacity > 0 && target < capacity * 2) target = capacity * 2;

    u32* new_a = new u32[target];
    for (size_t i = 0; i < size; ++i) new_a[i] = a[i];

    delete[] a;
    a = new_a;
    capacity = target;
}

void BigInt::push_back(u32 x)
{
    if (size >= capacity) reserve(size + 1);
    a[size++] = x;
}

void BigInt::swap(BigInt& other) noexcept
{
    u32* ta = a; a = other.a; other.a = ta;
    size_t ts = size; size = other.size; other.size = ts;
    size_t tc = capacity; capacity = other.capacity; other.capacity = tc;
    int tsign = sign; sign = other.sign; other.sign = tsign;
}

bool BigInt::isZero() const
{
    return size == 0;
}

void BigInt::trim()
{
    while (size > 0 && a[size - 1] == 0) --size;
    if (size == 0) sign = 1;
}