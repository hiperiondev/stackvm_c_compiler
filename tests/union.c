/* Test union */

int expect(int a, int b)
{
    if (!(a == b)) {
        printf("Failed\n");
        printf("  %d expected, but got %d\n", a, b);
        exit(1);
    }
}

int t1()
{
    union {
        int a;
        int b;
    } x;
    x.a = 90;
    expect(90, x.b);
}

int t2()
{
    union {
        char a[4];
        int b;
    } x;
    x.b = 0;
    x.a[1] = 1;
    expect(256, x.b);
}

int t3()
{
    union {
        char a[4];
        int b;
    } x;
    x.a[0] = x.a[1] = x.a[2] = x.a[3] = 0;
    x.a[1] = 1;
    expect(256, x.b);
}

int main()
{
    t1();
    t2();
    t3();

    return 0;
}
