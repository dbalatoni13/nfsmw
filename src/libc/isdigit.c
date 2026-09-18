extern const char _ctype_[];

int isdigit(int c)
{
    return (_ctype_ + 1)[(unsigned)c] & 0x04;
}
