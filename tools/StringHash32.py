def _hash32(buffer: bytes, initial_val: int) -> int:
    # uses ints instead of ctypes c_uint32 because all the ".value" accesses make it slower
    buffer_size_left = len(buffer)
    a = 0x9e3779b9
    b = 0x9e3779b9
    c = initial_val & 0xffff_ffff
    i = 0
    while (buffer_size_left >= 12):
        # load 3 ints as little endian
        a = (a + 
             ((buffer[i + 0x0] << 0x00) +
              (buffer[i + 0x1] << 0x08) +
              (buffer[i + 0x2] << 0x10) +
              (buffer[i + 0x3] << 0x18)
              )
            ) & 0xffff_ffff
        b = (b + 
             ((buffer[i + 0x4] << 0x00) + 
              (buffer[i + 0x5] << 0x08) +
              (buffer[i + 0x6] << 0x10) +
              (buffer[i + 0x7] << 0x18)
              )
            ) & 0xffff_ffff
        c = (c +
             ((buffer[i + 0x8] << 0x00) +
              (buffer[i + 0x9] << 0x08) +
              (buffer[i + 0xA] << 0x10) +
              (buffer[i + 0xB] << 0x18)
              )
            ) & 0xffff_ffff

        a = ((a - (b + c)) ^ (c >> 0x0D)) & 0xffff_ffff
        b = ((b - (c + a)) ^ (a << 0x08)) & 0xffff_ffff
        c = ((c - (a + b)) ^ (b >> 0x0D)) & 0xffff_ffff

        a = ((a - (b + c)) ^ (c >> 0x0C)) & 0xffff_ffff
        b = ((b - (c + a)) ^ (a << 0x10)) & 0xffff_ffff
        c = ((c - (a + b)) ^ (b >> 0x05)) & 0xffff_ffff

        a = ((a - (b + c)) ^ (c >> 0x03)) & 0xffff_ffff
        b = ((b - (c + a)) ^ (a << 0x0A)) & 0xffff_ffff
        c = ((c - (a + b)) ^ (b >> 0x0F)) & 0xffff_ffff

        buffer_size_left -= 12
        i += 12
    
    c = (c + len(buffer)) & 0xffff_ffff

    # handle leftover bytes in buffer
    # if one of the checks passes, it will keep passing checks
    if (buffer_size_left >= 0xb): c = (c + (buffer[i + 0xa] << 0x18)) & 0xffff_ffff
    if (buffer_size_left >= 0xa): c = (c + (buffer[i + 0x9] << 0x10)) & 0xffff_ffff
    if (buffer_size_left >= 0x9): c = (c + (buffer[i + 0x8] << 0x08)) & 0xffff_ffff
    if (buffer_size_left >= 0x8): b = (b + (buffer[i + 0x7] << 0x18)) & 0xffff_ffff
    if (buffer_size_left >= 0x7): b = (b + (buffer[i + 0x6] << 0x10)) & 0xffff_ffff
    if (buffer_size_left >= 0x6): b = (b + (buffer[i + 0x5] << 0x08)) & 0xffff_ffff
    if (buffer_size_left >= 0x5): b = (b + (buffer[i + 0x4] << 0x00)) & 0xffff_ffff
    if (buffer_size_left >= 0x4): a = (a + (buffer[i + 0x3] << 0x18)) & 0xffff_ffff
    if (buffer_size_left >= 0x3): a = (a + (buffer[i + 0x2] << 0x10)) & 0xffff_ffff
    if (buffer_size_left >= 0x2): a = (a + (buffer[i + 0x1] << 0x08)) & 0xffff_ffff
    if (buffer_size_left >= 0x1): a = (a + (buffer[i + 0x0] << 0x00)) & 0xffff_ffff

    a = ((a - (b + c)) ^ (c >> 0x0D)) & 0xffff_ffff
    b = ((b - (a + c)) ^ (a << 0x08)) & 0xffff_ffff
    c = ((c - (a + b)) ^ (b >> 0x0D)) & 0xffff_ffff

    a = ((a - (b + c)) ^ (c >> 0x0C)) & 0xffff_ffff
    b = ((b - (a + c)) ^ (a << 0x10)) & 0xffff_ffff
    c = ((c - (a + b)) ^ (b >> 0x05)) & 0xffff_ffff

    a = ((a - (b + c)) ^ (c >> 0x03)) & 0xffff_ffff
    b = ((b - (a + c)) ^ (a << 0x0A)) & 0xffff_ffff
    c = ((c - (a + b)) ^ (b >> 0x0F)) & 0xffff_ffff
    return c

def StringHash32(s: str) -> int:
    if (isinstance(s, str) and len(s) != 0):
        return _hash32(s.encode("ascii"), 0xabcdef00)
    return 0
