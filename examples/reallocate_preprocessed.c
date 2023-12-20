






_Bool reallocate(void** ptrptr, size_t const size) {
    if (ptrptr == ((void *)0) || *ptrptr == ((void *)0) || size == 0) return 0;
    void* const ptr = realloc(*ptrptr, size);
    if (ptr == ((void *)0)) return 0;
    *ptrptr = ptr;
    return 1;
}
_Bool recalloc(void** ptrptr, size_t const old_element_count, size_t const new_element_count, size_t const element_size) {
    if (
        ptrptr == ((void *)0) ||
        *ptrptr == ((void *)0) ||
        old_element_count == 0 ||
        element_size == 0 ||
        new_element_count <= old_element_count
    ) return 0;
    size_t const memcpy_size = old_element_count * element_size;
    if (memcpy_size < old_element_count || memcpy_size < element_size) return 0;
    void* const ptr = calloc(new_element_count, element_size);
    if (ptr == ((void *)0)) return 0;
    __builtin___memcpy_chk (ptr, *ptrptr, memcpy_size, __builtin_object_size (ptr, 0));
    free(*ptrptr);
    *ptrptr = ptr;
    return 1;
}
