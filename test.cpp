#ifdef __GNUG__
int kecske = __GNUG__;
#endif

struct Outer {
    struct Inner {
        struct InnerInner {
            int c;
        };
        InnerInner b;
    };

    Inner a;

    void asd2(Outer &test) {
        test.a.b.c = 2;
    }

    void asd3();
};

void asd(Outer &test) {
    test.a.b.c = 2;
}

void Outer::asd3() {}
