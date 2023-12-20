int global_x;

int decrement(int x) {
    x--;
    return x;
}

void init_global(void) {
    global_x = decrement(1);
}
