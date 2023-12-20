void trap() {


    int a = 5, b = 4;
    if (a > b) {
        b = a;
    } else if (a == b) {
        b--;
        if (b == 0)
            b++;
    } return a;
}

void empty() {


    if (x == 5)
        ;
    else if (y == 5)
        ;
}

unsigned gcd2(unsigned a, unsigned b) {


    if (b == 0)
        return a;
    else return gcd2(a, a % b);
}

int max3(int a, int b, int c) {


    int max;
    if (a > b) {
        if (a > c) {
            max = a;
        } else max = c;
    } else if (b > c) {
        max = b;
    } else max = c;
    return max;
}

int max4(int a, int b, int c, int d) {


    int max = a;
    if (b > max)
        max = b;
    if (c > max)
        max = c;
    if (d > max)
        max = d;
    return max;
}

unsigned gcd(unsigned a, unsigned b) {


    if (b == 0)
        return a;
    return gcd(a, a % b);
}

void largeIF() {
    if (x == 5) {
        if (y == 4)
            z = 9;
        else if (y == 2)
            z = 7;
    } else if (x == 4)
        if (y == 5)
            z = 9;
        else;
    z = x + y;
}

unsigned gcd3(unsigned a, unsigned b) {


    while (a != b) {
        if (a > b) a -= b;
        else b -= a;
    }
    return a;
}

unsigned gcd4(unsigned a, unsigned b) {


    if (b == 0) return a;
    do {
        unsigned t = b;
        b = a % b;
        a = t;
    } while (b != 0);
    return a;
}

int maxN(int n, int array[]) {


    if (n == 0)
        return INT_MIN;
    int max = array[0];
    for (
        int i = 1;
        i < n;
        i++
    ) { if (array[i] > max) {
        max = array[i];
    }} return max;
}

/*
int max2(int a, int b) {
    if (a > b) goto FORWARD;
BACKWARD:
    return b;
FORWARD:
    if (a == b) goto BACKWARD;
    return a;
}
*/
