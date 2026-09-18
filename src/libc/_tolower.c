extern const char _ctype_[];

int _tolower(int c)
{
    if ((_ctype_ + 1)[(unsigned)c] & 0x01)
        return c - 'A' + 'a';
    return c;
}
