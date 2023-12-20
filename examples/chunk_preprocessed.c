







Chunk strings = ((Chunk){0, 0, 0, ((void*)0), ((void*)0), 0});
uint32_t add_chunk(Chunk* const chunk, char const* const str, uint64_t const n) {
    uint32_t const id = chunk->nStrings++;
    uint64_t const offset = (chunk->len += !!(chunk->len));
    if (append_chunk(chunk, str, n) == ((void*)0)) return 0xFFFFFFFF;
    chunk->stringOffsets[id] = offset;
    return id;
}
uint32_t addIndex_chunk(Chunk* const chunk, uint32_t const str_id) {
    uint32_t const id = add_chunk(chunk, "", 0);
    if (id == 0xFFFFFFFF) return 0xFFFFFFFF;
    if (appendIndex_chunk(chunk, str_id) == ((void*)0)) return 0xFFFFFFFF;
    return id;
}
char const* append_chunk(Chunk* const chunk, char const* const str, uint64_t const n) {
    char* const append_start = appendSpace_chunk(chunk, n);
    if (append_start < chunk->start) return ((void*)0);
    __builtin___memcpy_chk (append_start, str, n, __builtin_object_size (append_start, 0));
    chunk->start[(chunk->len += n)] = '\0';
    if (chunk->stringsCap <= (uint32_t)(chunk->nStrings)) { uint32_t _new_cap = ((uint32_t)(chunk->nStrings)) << 1; if (_new_cap <= (uint32_t)(chunk->nStrings)) return ((void*)0); chunk->stringsCap = _new_cap; if (!(reallocate((void**)&(chunk->stringOffsets), (size_t)(chunk->stringsCap) * sizeof(uint64_t)))) return ((void*)0); }
    if (chunk->nStrings == 0) {
        uint64_t const offset = (uint64_t)(append_start - chunk->start);
        chunk->stringOffsets[chunk->nStrings++] = offset;
        return chunk->start + offset;
    }
    return chunk->start + chunk->stringOffsets[chunk->nStrings - 1];
}
char const* appendIndex_chunk(Chunk* const chunk, uint32_t const str_id) {
    uint64_t const start_offset = chunk->stringOffsets[str_id];
    uint64_t const end_offset = str_id == chunk->nStrings - 1
        ? chunk->len
        : chunk->stringOffsets[str_id + 1] - 1;
    uint64_t const n = (uint64_t)(end_offset - start_offset);
    char* const append_start = appendSpace_chunk(chunk, n);
    if (append_start == ((void*)0)) return ((void*)0);
    __builtin___memcpy_chk (append_start, chunk->start + start_offset, n, __builtin_object_size (append_start, 0));
    chunk->start[(chunk->len += n)] = '\0';
    if (chunk->stringsCap <= (uint32_t)(chunk->nStrings)) { uint32_t _new_cap = ((uint32_t)(chunk->nStrings)) << 1; if (_new_cap <= (uint32_t)(chunk->nStrings)) return ((void*)0); chunk->stringsCap = _new_cap; if (!(reallocate((void**)&(chunk->stringOffsets), (size_t)(chunk->stringsCap) * sizeof(uint64_t)))) return ((void*)0); }
    return chunk->start + chunk->stringOffsets[chunk->nStrings - 1];
}
char* appendSpace_chunk(Chunk* const chunk, uint64_t const size) {
    if (size == 0) return chunk->start + chunk->len;
    uint64_t const pseudo_len = chunk->len + size;
    if (pseudo_len < chunk->len) return ((void*)0);
    if (chunk->cap <= (uint64_t)(pseudo_len)) { uint64_t _new_cap = ((uint64_t)(pseudo_len)) << 1; if (_new_cap <= (uint64_t)(pseudo_len)) return ((void*)0); chunk->cap = _new_cap; if (!(reallocate((void**)&(chunk->start), (size_t)(chunk->cap) * sizeof(char)))) return ((void*)0); }
    return chunk->start + chunk->len;
}
Chunk* constructEmpty_chunk(Chunk* chunk, uint64_t const initial_cap, uint32_t const initial_stringsCap) {
    if (
        initial_cap == 0 ||
        initial_cap == 0xFFFFFFFFFFFFFFFF ||
        initial_stringsCap == 0 ||
        initial_stringsCap == 0xFFFFFFFF
    ) return ((void*)0);
    if (chunk == ((void*)0)) {
        chunk = malloc(sizeof(chunk));
        if (chunk == ((void*)0)) return ((void*)0);
    }
    chunk->cap = initial_cap;
    chunk->stringsCap = initial_stringsCap;
    chunk->nStrings = 0;
    chunk->len = 0;
    chunk->stringOffsets = malloc(initial_stringsCap * sizeof(uint64_t));
    if (chunk->stringOffsets == ((void*)0)) return ((void*)0);
    chunk->start = malloc(initial_cap);
    if (chunk->start == ((void*)0)) return ((void*)0);
    chunk->start[0] = '\0';
    return chunk;
}
_Bool deleteLast_chunk(Chunk* const chunk) {
    if (chunk->nStrings == 0) return 0;
    uint32_t const lastStr_id = chunk->nStrings - 1;
    uint32_t const prevStr_id = lastStr_id - 1;
    uint64_t const max_len = chunk->stringOffsets[lastStr_id] - 1;
    uint64_t const min_len = lastStr_id == 0 ? 0 : chunk->stringOffsets[prevStr_id];
    chunk->nStrings--;
    for (
        chunk->start[chunk->len = max_len] = '\0';
        chunk->len > min_len && chunk->start[chunk->len - 1] == '\0';
        chunk->len--
    );
    return 1;
}
void flush_chunk(Chunk* const chunk) {
    chunk->nStrings = 0;
    chunk->len = 0;
    chunk->start[0] = '\0';
}
void free_chunk(Chunk chunk) {
    free(chunk.start);
    free(chunk.stringOffsets);
}
void free_strings(void) {
    free_chunk(strings);
}
uint32_t fromFile_chunk(Chunk* const chunk, char const* const file, char const* delimeters) {
    FILE* const stream = fopen(file, "r");
    uint32_t const str_id = fromStream_chunk(chunk, stream, delimeters);
    if (fclose(stream) == (-1)) return 0xFFFFFFFF;
    return str_id;
}
uint32_t fromFileAsWhole_chunk(Chunk* const chunk, char const* const file) {
    FILE* const stream = fopen(file, "r");
    uint32_t const str_id = fromStreamAsWhole_chunk(chunk, stream);
    if (fclose(stream) == (-1)) return 0xFFFFFFFF;
    return str_id;
}
uint32_t fromStream_chunk(Chunk* const chunk, FILE* const stream, char const* delimeters) {
    static char const defaultDelimeters[] = " \t\n\v\f\r";
    uint32_t const str_id = fromStreamAsWhole_chunk(chunk, stream);
    if (delimeters == ((void*)0)) delimeters = defaultDelimeters;
    if (str_id == 0xFFFFFFFF) return 0xFFFFFFFF;
    if (splitLast_chunk(chunk, delimeters) == 0) return 0xFFFFFFFF;
    return str_id;
}
uint32_t fromStreamAsWhole_chunk(Chunk* const chunk, FILE* const stream) {
    if (stream == ((void*)0)) return 0xFFFFFFFF;
    uint32_t str_id = add_chunk(chunk, "", 0);
    if (str_id == 0xFFFFFFFF) return 0xFFFFFFFF;
    if (fseek(stream, 0L, 2) != 0) return 0xFFFFFFFF;
    long size = ftell(stream);
    if (size < 0L) return 0xFFFFFFFF;
    if (fseek(stream, 0L, 0) != 0) return 0xFFFFFFFF;
    char* append_start = appendSpace_chunk(chunk, (uint64_t)size);
    if (append_start == ((void*)0)) return 0xFFFFFFFF;
    if (fread(append_start, (size_t)size, 1, stream) != 1) return 0xFFFFFFFF;
    chunk->len += (uint64_t)size;
    return str_id;
}
char const* get_chunk(Chunk const* const chunk, uint32_t const str_id) {
    if (str_id >= chunk->nStrings) return ((void*)0);
    return chunk->start + chunk->stringOffsets[str_id];
}
char const* getFirst_chunk(Chunk const* const chunk) {
    return get_chunk(chunk, 0);
}
char const* getLast_chunk(Chunk const* const chunk) {
    if (chunk->nStrings == 0) return ((void*)0);
    return get_chunk(chunk, chunk->nStrings - 1);
}
_Bool isValid_chunk(Chunk const* const chunk) {
    return chunk &&
           chunk->cap != 0 &&
           chunk->cap != 0xFFFFFFFFFFFFFFFF &&
           chunk->stringsCap != 0 &&
           chunk->stringsCap != 0xFFFFFFFF &&
           chunk->stringOffsets &&
           chunk->start &&
           chunk->len <= chunk->cap &&
           chunk->nStrings <= chunk->stringsCap;
}
size_t splitLast_chunk(Chunk* const chunk, char const* delimeters) {
    static char const defaultDelimeters[] = " \t\n\v\f\r";
    if (delimeters == ((void*)0)) delimeters = defaultDelimeters;
    if (chunk->nStrings == 0) return 0;
    uint32_t const str_id = chunk->nStrings - 1;
    char* str_start = chunk->start + chunk->stringOffsets[str_id];
    char* str_end = chunk->start + chunk->len;
    for (
        str_end[0] = '\0';
        str_end > str_start && (isspace(str_end[-1]) || str_end[-1] == '\0' || strchr(delimeters, str_end[-1]));
        chunk->start[--chunk->len] = '\0', str_end--
    );
    for (;
        isspace(str_start[0]) || str_start[0] == '\0' || strchr(delimeters, str_start[0]);
        chunk->stringOffsets[str_id]++, *(str_start++) = '\0'
    );
    size_t nSplitted = 1;
    for (char* chr = str_start; chr < str_end; chr++) {
        if (strchr(delimeters, *chr) == ((void*)0)) continue;
        if (chunk->stringsCap <= (uint32_t)(chunk->nStrings)) { uint32_t _new_cap = ((uint32_t)(chunk->nStrings)) << 1; if (_new_cap <= (uint32_t)(chunk->nStrings)) return 0; chunk->stringsCap = _new_cap; if (!(reallocate((void**)&(chunk->stringOffsets), (size_t)(chunk->stringsCap) * sizeof(uint64_t)))) return 0; }
        for (
            *(chr++) = '\0';
            (chr[0] == '\0' || strchr(delimeters, chr[0]));
            *(chr++) = '\0'
        ) if (chr >= str_end) return nSplitted;
        chunk->stringOffsets[chunk->nStrings++] = (uint64_t)(chr - chunk->start);
        nSplitted++;
    }
    return nSplitted;
}
uint64_t strlen_chunk(Chunk const* const chunk, uint32_t const str_id) {
    uint32_t const last_str_id = chunk->nStrings - 1;
    if (chunk->nStrings == 0 || str_id > last_str_id) return 0xFFFFFFFFFFFFFFFF;
    if (str_id == last_str_id) {
        return chunk->len - chunk->stringOffsets[str_id];
    } else {
        return chunk->stringOffsets[str_id + 1] - chunk->stringOffsets[str_id] - 1L;
    }
}
