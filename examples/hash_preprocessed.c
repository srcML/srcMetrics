
uint_fast64_t hash_str(char const* restrict str, size_t register n) {
    uint_fast64_t register hash = 5381;
    for (; n && *str; str++, n--) {
        hash += (hash << 5) + (uint_fast64_t)(*str);
    }
    return hash;
}
