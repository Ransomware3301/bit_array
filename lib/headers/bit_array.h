/*
 *  File Type:  C header file
 *
 *  Program:    C library that enables support for bit operations by introducing
 *              a bit array of any length and functions to modify single bits
 *              inside each byte
 *
 *  Made by:    ransomware3301 (https://www.github.com/ransomware3301)
 *  Date:       17/02/2025
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


#define DEFAULT_SIZE BYTESIZE
#define DEFAULT_MODE ZEROES
#define GROUPING 4
#define BYTEMAXVAL 255
#define BYTESIZE 8
#define MALLOC_ERROR "ERROR: Memory allocation was unsuccessful"


/* ==== Type Definitions ==== */


/* Generic types definitions */
typedef unsigned long int uint64_t;
typedef unsigned char uint8_t;


/* n-bit array type definition */
typedef struct bitarr
{
    uint8_t *bits;
    uint64_t occupied;
    uint64_t size;
}
bitarr_t;


/* 
*  Bit Array initialization modes:
*      - ZEROES --> All bits set to 0
*      - ONES   --> All bits set to 1
*      - RAND   --> All bits set to 0 or 1 (random) 
*/
typedef enum init_mode
{
    ZEROES,
    ONES,
    RAND
}
init_mode_t;


/* Boolean type definition */
typedef enum bool
{
    FALSE,
    TRUE
}
bool_t;


/* ==== Function Declarations ==== */


/* IO Functions */
void        print_bitarr(bitarr_t*);
bitarr_t *  input_bitarr();


/* Helper functions */
bool_t is_boolean(long int);


/* Basic operations */
bitarr_t * create_bitarr(uint64_t, init_mode_t);
bitarr_t * delete_bitarr(bitarr_t*);
bitarr_t * copy_bitarr(bitarr_t*);
bitarr_t * extend_bitarr(bitarr_t**, uint64_t, init_mode_t);
bitarr_t * contract_bitarr(bitarr_t**, uint64_t);
void       set_bit_at_pos(bitarr_t*, uint64_t, bool_t);
int        get_bit_at_pos(bitarr_t*, uint64_t);
