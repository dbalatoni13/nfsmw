struct _rand_reent {
    char pad[0x58];
    unsigned long _rand_next;
};
extern struct _rand_reent *_impure_ptr;

int rand(void)
{
    _impure_ptr->_rand_next = _impure_ptr->_rand_next * 1103515245 + 12345;
    return (int)(_impure_ptr->_rand_next & 0x7FFFFFFF);
}
