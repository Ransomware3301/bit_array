/*
 *  File Type:  C source file  
 *
 *  Program:    C library that enables support for bit operations by introducing
 *              a bit array of any length and functions to modify single bits
 *              inside each byte
 *
 *  Made by:    ransomware3301 (https://www.github.com/ransomware3301)
 *  Date:       17/02/2025
 */


#include "headers/bit_array.h"


/* ==== Function Definitions ==== */


/*
 *  Prints to stdout the content of the given bit array
 */
void print_bitarr(bitarr_t *bitarr)
{
    uint8_t byte, mask, tmp;
    int i, j;

    #ifndef GROUPING
        uint64_t byte_count;
    #endif

    if (bitarr)
    {
        if (bitarr->bits)
        {
            printf("\nBit Array Contents (occupied=%lu, total=%lu):\n [", 
                bitarr->occupied, 
                bitarr->size
            );

            #ifdef GROUPING
                // If GROUPING is defined, then the function will output
                // bits in groups of amount defined by the value GROUPING
                i = 0;
                j = 0;
                mask = 0;
                byte = 0;
                while (j < bitarr->size)
                {
                    if ((j % GROUPING) == 0) { printf(" "); }

                    if ((j % BYTESIZE) == 0)
                    {
                        mask = (uint8_t) 128;
                        byte = *(bitarr->bits + i);
                        i++;
                    }

                    tmp = (byte & mask);
                    tmp = tmp >> (BYTESIZE - (j % BYTESIZE) - 1);
                    printf("%u", tmp % 2);
                    mask = mask >> 1;
                    j++;
                }
            #else    
                // Else the function will output the entire stream of bits (left-to-right (LTR))
                // (basically this means that GROUPING = 1 by default if not defined previously)
                byte_count = bitarr->size / BYTESIZE;

                for (i = 0; i < byte_count; i++)
                {
                    byte = *(bitarr->bits + i);
                    mask = (uint8_t) 128;
                                
                    for (j = 0; j < BYTESIZE; j++)
                    {
                        tmp = (byte & mask);
                        tmp = tmp >> (BYTESIZE - j - 1);
                        printf("%u ", tmp % 2);
                        mask = mask >> 1;
                    }
                }
            #endif

            printf("]\n\n");
        }
        else
        {
            printf("\n[EMPTY]\n");
        }
    }
    else
    {
        printf("\n[NULL]\n");
    }
}


/*
 *  Initializes a new bit array and prompts the user to 
 *  insert data into it
 */
bitarr_t * input_bitarr(bitarr_t *new)
{
    uint64_t bytes, i;
    long int occupied;
    int value;


    do
    {
        printf("Insert bits to add (must be > 0): ");
        scanf("%ld", &occupied);
    } 
    while (occupied <= 0);

    if (new == NULL)
    {
        if (( new = (bitarr_t*)malloc(sizeof(bitarr_t)) ))
        {
            bytes = (uint64_t) ceil(((double) occupied) / BYTESIZE);
            new->occupied = occupied;
            new->size = bytes * BYTESIZE;
    
            if (( new->bits = (uint8_t*)malloc(bytes) ))
            {
                // Zeroing the bit array before input
                for (i = 0; i < bytes; i++)
                {
                    *(new->bits + i) = 0;
                }
    
                i = 0;
                while (i < new->occupied)
                {
                    printf("Insert bit at index %lu: ", i);
                    scanf("%d", &value);
    
                    if (is_boolean(value))
                    {
                        // Set the bit at intex i to 1 only if given in input
                        // otherwise its 0 by default (from initialization)
                        if (value == TRUE) { set_bit_at_pos(new, i, value); }
                        i++;
                    }
                    else
                    {
                        printf("[ERROR] \"%d\" is not 0 or 1 (NotBooleanException)\n", value);
                    }
                }
            }
            else
            {
                printf("[input_bitarr(): new->bits] %s\n", MALLOC_ERROR);
                new = delete_bitarr(new);
            }
        }
        else
        {
            printf("[input_bitarr(): new] %s\n", MALLOC_ERROR);
        }
    }

    return new;
}


/* 
 *  Returns TRUE if x is either 0 or 1
 */
bool_t is_boolean(long int x)
{
    return (x == FALSE || x == TRUE);
}


/*
 *  Allocates the minimum amount of bytes to store a number of bits
 *  equal to the given occupied parameter, and initializes those bits
 *  as instructed by the mode parameter.
 */
bitarr_t * create_bitarr(uint64_t occupied, init_mode_t mode)
{
    bitarr_t *bitarr;
    uint64_t i, bytes;


    bitarr = NULL;

    if (( bitarr = (bitarr_t*)malloc(sizeof(bitarr_t)) ))
    {   
        // Excess rounding the amount of bytes to allocate
        // based on the amount of bits requesed (size)
        bytes = (uint64_t) ceil( ((double) occupied) / BYTESIZE);

        bitarr->occupied = occupied;
        bitarr->size = bytes * BYTESIZE;

        if (( bitarr->bits = (uint8_t*)malloc(bytes) ))
        {
            // Zeroing the newly allocated memory
            for (i = 0; i < bytes; i++) { *(bitarr->bits + i) = 0; }

            switch (mode)
            {
                // RAND --> Instantiating the new bit array to a random mix of 0s and 1s 
                case RAND:
                    // Instantiating seed for RNG
                    srand(time(NULL));
                    for (i = 0; i < bitarr->occupied; i++)
                    { 
                        set_bit_at_pos(bitarr, i, rand() % 2);
                    }
                    break;
                
                // ONES --> Instantiating the new bit array to all 1s
                case ONES:
                    i = 0;
                    if (bytes > 1)
                    {
                        bytes -= (bitarr->occupied != bitarr->size);
                        for (i = 0; i < bytes; i++)
                        {
                            *(bitarr->bits + i) = BYTEMAXVAL;
                        }
                    }
                    for (i *= BYTESIZE; i < bitarr->occupied; i++) 
                    {
                        set_bit_at_pos(bitarr, i, TRUE);
                    }
                    break;

                // ZEROES --> Instantiating the new bit array to all 0s
                // (default behaviour)
                default:
                case ZEROES:
                    // The bits are already zeroed at the beginning
                    break;
            }
        }
        else
        {
            printf("[create_bitarr(): bitarr->bits]%s\n", MALLOC_ERROR);
            free(bitarr);
            bitarr = NULL;
        }
    }
    else
    {
        printf("[create_bitarr(): bitarr]%s\n", MALLOC_ERROR);
    }

    return bitarr;
}


/*
 *  Deallocates the given bit array from memory
 */
bitarr_t * delete_bitarr(bitarr_t *bitarr)
{
    if (bitarr)
    {
        if (bitarr->bits) { free(bitarr->bits); }
        free(bitarr);
        bitarr = NULL;
    }

    return bitarr;
}


/*
 *  Returns a copy of the given bit array
 */
bitarr_t * copy_bitarr(bitarr_t *bitarr)
{
    bitarr_t *copy;
    uint64_t bytes;
    uint64_t i;


    copy = NULL;

    if (( copy = (bitarr_t*)malloc(sizeof(bitarr_t)) ))
    {        
        bytes = bitarr->size / BYTESIZE;

        if (( copy->bits = (uint8_t*)malloc(bytes) ))
        {
            copy->occupied = bitarr->occupied;
            copy->size = bitarr->size;

            for (i = 0; i < bytes; i++)
            {
                *(copy->bits + i) = *(bitarr->bits + i);
            }
        }
        else
        {
            printf("[copy_bitarr(): copy->bitarr]%s\n", MALLOC_ERROR);
            free(copy);
            copy = NULL;
        }
    }
    else
    {
        printf("[copy_bitarr(): copy]%s\n", MALLOC_ERROR);
    }

    return copy;
}


/*
 *  Extends the given bit array by the given amount. 
 *  The newly allocated bits are initialized as instructed by the mode parameter
 */
bitarr_t * extend_bitarr(bitarr_t **bitarr, uint64_t amount, init_mode_t mode)
{
    bitarr_t *new, *old;
    uint64_t i, new_bytes, old_bytes, ext_bytes;


    new = NULL;

    if (amount > 0)
    {
        if (bitarr && *(bitarr))
        {
            old = *(bitarr);
    
            if (old->bits)
            {
                if (( new = (bitarr_t*)malloc(sizeof(bitarr_t)) ))
                {
                    // Bit array sizes are always multiples of 8 by construction, therefore
                    // dividing by BYTESIZE yields the correct amount of bytes (no rounding necessary)
                    old_bytes = old->size / BYTESIZE;
                    ext_bytes = (((uint64_t) ceil((double) amount / BYTESIZE)) * BYTESIZE);
                    new_bytes = old_bytes + ext_bytes;
    
                    // The field occupied is the same since the function doesn't ask
                    // the user for inputs, therefore the new bits are considered as available
                    new->occupied = old->occupied;
                    new->size = old->size + ext_bytes;
    
                    if (( new->bits = (uint8_t*)malloc(new_bytes) ))
                    {
                        // Copying the old bytes over to the new bit array
                        for (i = 0; i < old_bytes; i++)
                        {
                            *(new->bits + i) = *(old->bits + i);
                        }
                        
                        // Zeroing the bytes added with the extension
                        for (i = old_bytes; i < new_bytes; i++)
                        {
                            *(new->bits + i) = 0;
                        }
                        
                        switch (mode)
                        {
                            // RAND --> Instantiating the new bits to 0 or 1
                            case RAND:
                                // Instantiating seed for RNG
                                srand(time(NULL));
                                for (i = 0; i < amount; i++) { set_bit_at_pos(new, new->occupied + i, rand() % 2); }
                                break;
                            
                            // ONES --> Instantiating the new bits to all 1s
                            case ONES:
                                for (i = 0; i < amount; i++) { set_bit_at_pos(new, new->occupied + i, TRUE); }
                                break;
    
                            // ZEROES --> Instantiating the new bits to all 0s
                            // (default behaviour)
                            default:
                            case ZEROES:
                                // The bits are already zeroed at the beginning
                                break;
                        }
    
                        // Deleting the old bit array
                        *(bitarr) = delete_bitarr(*(bitarr));
                    }
                    else
                    {
                        printf("[extend_bitarr(): new->bits]%s\n", MALLOC_ERROR);
                        free(new);
                        new = old;
                    }
                }
                else
                {
                    printf("[extend_bitarr(): new]%s\n", MALLOC_ERROR);
                    new = old;
                }
            }
            else
            {
                // If the given bit array has the field bits NULL, then delete it 
                // and create a new bit array with the given parameters
                *(bitarr) = delete_bitarr(*(bitarr));
                new = create_bitarr(amount, mode);
            }
        }
        else
        {
            // Creating a new bit array if the given one is NULL
            new = create_bitarr(amount, mode);
        }
    }
    else
    {
        // No extension
        new = *(bitarr);
    }

    return new;
}


/*
 *  Contracts the given bit array of the given amount.
 *  If amount > bitarr->size, then the bit array is deleted.
 *  If bitarr == NULL, the function returns NULL.
 */
bitarr_t * contract_bitarr(bitarr_t **bitarr, uint64_t amount)
{
    bitarr_t *new, *old;
    uint64_t i, new_bytes;


    new = NULL;

    if (bitarr && *(bitarr))
    {
        old = *(bitarr);

        if (old->bits)
        {
            if (old->size > amount)
            {
                if (( new = (bitarr_t*)malloc(sizeof(bitarr_t)) ))
                {
                    new->size = old->size - (((uint64_t) ceil(((double) amount) / BYTESIZE)) * BYTESIZE);
                    new_bytes = new->size / BYTESIZE;
                    new->occupied = (new->size < old->occupied) ? new->size : old->occupied;

                    if (( new->bits = (uint8_t*)malloc(new_bytes) ))
                    {
                        // Transfering the surviving bytes to the shrunken bit array
                        for (i = 0; i < new_bytes; i++)
                        {
                            *(new->bits + i) = *(old->bits + i);
                        }

                        // Deleting the old bit array
                        *(bitarr) = delete_bitarr(*(bitarr));
                    }
                    else
                    {
                        new = delete_bitarr(new);
                        new = old;
                    }
                }
                else
                {
                    printf("[contract_bitarr(): new] %s\n", MALLOC_ERROR);
                    new = old;
                }
            }
            else
            {
                *(bitarr) = delete_bitarr(*(bitarr));
            }
        }
        else
        {
            new = old;
        }
    }

    return new;
}


/*
 *  Sets the value of the bit at the given index in the bit
 *  array to the given value. If the index is OUT-OF-BOUNDS 
 *  or the given bitarr doesn't exist, then it doesn't set any value
 */
void set_bit_at_pos(bitarr_t *bitarr, uint64_t index, bool_t value)
{
    uint8_t *byte;
    uint8_t mask, sub;
    uint64_t byte_index;


    if (bitarr && index < bitarr->size)
    {
        byte_index = index / BYTESIZE;
        byte = bitarr->bits + byte_index;
        mask = (uint8_t) 255;   // mask = 1111 1111
        sub = ((uint8_t) 128) >> (index % BYTESIZE);
        
        // Sets to 0 the bit selected by the selector_mask = mask - sub
        // (NOTE: mask - sub is of the form '1110 1111', where the '0' can be anywhere)
        *(byte) = *(byte) & (mask - sub);
    
        // If the bit needed to be set to 1, then add the subtracted mask (sub)
        if (value == TRUE)
        {
            *(byte) = *(byte) + sub;
        }
    }
}


/*
 *  Returns the value of the bit in the given bit array
 *  and returns -1 if the given index is OUT-OF-BOUNDS
 */
int get_bit_at_pos(bitarr_t *bitarr, uint64_t index)
{
    uint8_t mask;
    uint64_t offset;
    int res;


    if (bitarr && index < bitarr->size)
    {
        offset = index % BYTESIZE;
        mask = ((uint8_t) 128) >> offset;
        res = (*(bitarr->bits + (index / BYTESIZE)) & mask) >> (BYTESIZE - offset - 1);
    }
    else
    {
        res = -1;
    }

    return res;
}
