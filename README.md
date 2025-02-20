# bit_array
A C11 library that implements a few functions to operate with bit arrays.

It was born as an attempt to define a `bit_t` in C but, since `malloc` allocates memory in amounts of bytes, there was still the problem of dealing with bytes.
The library was then designed to allocate memory in chunks of bytes and perform bit-level operations. 
The result is the possibility to define "bit arrays" (of type `bitarr_t`) that, for example, can be used to store 8 `boolean` values at a time, thus
avoiding to store a single `boolean` value in 8 bits of memory (assuming that `booleans` are defined as: `typedef unsigned char bool_t`).
