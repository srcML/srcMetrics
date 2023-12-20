
_Bool isPrime(uint32_t const odd_x) {
    register uint32_t n = odd_x;
    while (n > odd_x / n) n >>= 1;
    while (n * n < odd_x) n++;
    for (n -= !(n % 2); n > 2; n -= 2)
        if (odd_x % n == 0) return 0;
    return 1;
}
uint32_t nextPrime(uint32_t const x) {
    uint32_t candidate = x + !(x % 2);
    while (!isPrime(candidate)) candidate += 2;
    if (candidate < x) return x;
    return candidate;
}
