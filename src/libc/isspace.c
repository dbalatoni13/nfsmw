extern const char _ctype_[];

int isspace(int c)
{
    return (_ctype_ + 1)[(unsigned)c] & 0x08;
}
