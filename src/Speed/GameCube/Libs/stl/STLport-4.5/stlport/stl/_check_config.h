// This file is reserved to site configuration purpose
// and should NEVER be overridden by user

# if defined ( _STLP_NO_OWN_IOSTREAMS )

// User choose not to use SGI iostreams, which means no
// precompiled library will be used and he is free to override
// any STLport configuration flags

# else

// The following will be defined in stl_config.h :
// # define _STLP_OWN_IOSTREAMS 1
# endif

