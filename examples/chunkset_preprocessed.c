











static uint32_t determineNRows(uint32_t const strCap, uint32_t const loadPercent) {
    uint32_t nRows = (strCap > loadPercent)
        ? (strCap / loadPercent) * 100U
        : (strCap * 100U) / loadPercent;
    if (nRows < strCap) nRows = strCap;
    if (nRows < 11U) nRows = 11U;
    return nextPrime(nRows);
}
static _Bool adjust(ChunkSet* const set) {
    Chunk* const chunk = (Chunk*)set;
    unsigned const newNRows = determineNRows(chunk->nStrings, set->loadPercent);
    if (newNRows <= set->nRows) return 1;
    free(set->rowSize);
    free(set->rowCap);
    for (uint32_t** row = set->table + set->nRows; --row >= set->table; free(*row));
    free(set->table);
    set->nRows = newNRows;
    set->rowSize = calloc(newNRows, sizeof(uint32_t));
    if (set->rowSize == ((void*)0)) return 0;
    set->rowCap = calloc(newNRows, sizeof(uint32_t));
    if (set->rowCap == ((void*)0)) return 0;
    set->table = calloc(newNRows, sizeof(uint32_t*));
    if (set->table == ((void*)0)) return 0;
    for (uint32_t key_id = 0; key_id < chunk->nStrings; key_id++) {
        char const* const key = get_chunk(chunk, key_id);
        uint64_t const key_len = strlen_chunk(chunk, key_id);
        unsigned const row_id = hash_str(key, key_len) % newNRows;
        if (set->rowSize[row_id] == 0) {
            uint32_t* const row_ptr = malloc(4 * sizeof(uint32_t));
            if (row_ptr == ((void*)0)) return 0;
            set->rowCap[row_id] = 4;
            set->table[row_id] = row_ptr;
        }
        if (set->rowCap[row_id] <= (unsigned)(set->rowSize[row_id])) { unsigned _new_cap = ((unsigned)(set->rowSize[row_id])) << 1; if (_new_cap <= (unsigned)(set->rowSize[row_id])) return 0; set->rowCap[row_id] = _new_cap; if (!(reallocate((void**)&(set->table[row_id]), (size_t)(set->rowCap[row_id]) * sizeof(int)))) return 0; }
        set->table[row_id][set->rowSize[row_id]++] = key_id;
    }
    return 1;
}
uint32_t addKey_cset(ChunkSet* const set, char const* const key, uint64_t const n) {
    Chunk* const chunk = (Chunk*)set;
    unsigned const row_id = hash_str(key, n) % set->nRows;
    if (set->rowSize[row_id] == 0) {
        uint32_t* const row_ptr = malloc(4 * sizeof(uint32_t));
        if (row_ptr == ((void*)0)) return 0xFFFFFFFF;
        set->rowCap[row_id] = 4;
        set->table[row_id] = row_ptr;
    }
    for (uint32_t* key_id = set->table[row_id] + set->rowSize[row_id]; --key_id >= set->table[row_id];) {
        char const* const candidate = get_chunk(chunk, *key_id);
        if (!str_eq_n(key, candidate, n)) continue;
        return *key_id;
    }
    if (set->rowCap[row_id] <= (unsigned)(set->rowSize[row_id])) { unsigned _new_cap = ((unsigned)(set->rowSize[row_id])) << 1; if (_new_cap <= (unsigned)(set->rowSize[row_id])) return 0xFFFFFFFF; set->rowCap[row_id] = _new_cap; if (!(reallocate((void**)&(set->table[row_id]), (size_t)(set->rowCap[row_id]) * sizeof(int)))) return 0xFFFFFFFF; }
    uint32_t const key_id = add_chunk(chunk, key, n);
    if (key_id == 0xFFFFFFFF) return 0xFFFFFFFF;
    set->table[row_id][set->rowSize[row_id]++] = key_id;
    if (adjust(set)) {
        return key_id;
    } else {
        return 0xFFFFFFFF;
    }
}
ChunkSet* constructEmpty_cset(ChunkSet* set, uint64_t const initial_cap, uint32_t const initial_stringsCap, uint32_t const loadPercent) {
    if (loadPercent == 0) return ((void*)0);
    set->loadPercent = loadPercent;
    if (set == ((void*)0)) {
        set = malloc(sizeof(ChunkSet));
        if (set == ((void*)0)) return ((void*)0);
    }
    if (constructEmpty_chunk((Chunk*)set, initial_cap, initial_stringsCap) == ((void*)0)) return ((void*)0);
    set->nRows = determineNRows(initial_stringsCap, loadPercent);
    set->rowSize = calloc(set->nRows, sizeof(uint32_t));
    if (set->rowSize == ((void*)0)) return ((void*)0);
    set->rowCap = calloc(set->nRows, sizeof(uint32_t));
    if (set->rowCap == ((void*)0)) return ((void*)0);
    set->table = calloc(set->nRows, sizeof(uint32_t*));
    if (set->table == ((void*)0)) return ((void*)0);
    return set;
}
void flush_cset(ChunkSet* const set) {
    flush_chunk((Chunk*)set);
    __builtin___memset_chk (set->rowSize, 0U, set->nRows * sizeof(uint32_t), __builtin_object_size (set->rowSize, 0));
}
void free_cset(ChunkSet set) {
    free_chunk(*((Chunk*)&set));
    free(set.rowSize);
    free(set.rowCap);
    for (uint32_t** row = set.table + set.nRows; --row >= set.table; free(*row));
    free(set.table);
}
char const* getKey_cset(ChunkSet const* const set, uint32_t const key_id) {
    return get_chunk((Chunk const*)set, key_id);
}
uint32_t getKeyCount_cset(ChunkSet const* const set) {
    return set->nStrings;
}
uint32_t getKeyId_cset(ChunkSet const* const set, char const* const key, uint64_t const n) {
    Chunk const* const chunk = (Chunk const*)set;
    unsigned const row_id = hash_str(key, n) % set->nRows;
    if (set->table[row_id] == ((void*)0)) return 0xFFFFFFFF;
    for (uint32_t* key_id = set->table[row_id] + set->rowSize[row_id]; --key_id >= set->table[row_id];) {
        char const* const candidate = get_chunk(chunk, *key_id);
        uint64_t const candidate_len = strlen_chunk(chunk, *key_id);
        if (!str_eq_n(key, candidate, candidate_len)) continue;
        return *key_id;
    }
    return 0xFFFFFFFF;
}
_Bool isValid_cset(ChunkSet const* const set) {
    return isValid_chunk((Chunk const*)set) &&
           set->loadPercent != 0 &&
           set->nRows != 0 &&
           set->rowSize &&
           set->rowCap &&
           set->table;
}
uint64_t strlen_cset(ChunkSet const* const set, uint32_t const key_id) {
    return strlen_chunk((Chunk const*)set, key_id);
}
