#ifndef TYPES_INCLUDED
#define TYPES_INCLUDED


// unsigned number
typedef unsigned char          Number8;  // [0, 255]
typedef unsigned short int     Number16; // [0, 65 535]
typedef unsigned long int      Number32; // [0, 4 294 967 296]
typedef unsigned long long int Number64; // [0, 18 446 744 073 709 551 616]
typedef Number32               Number;

typedef char                   Signed_Number8;  // [-127, 127]
typedef short int              Signed_Number16; // [-32 767, 32 767]
typedef long int               Signed_Number32; // [-2 147 483 648, 2 147 483 648]
typedef long long int          Signed_Number64; // [-9 223 372 036 854 775 808, 9 223 372 036 854 775 808]
typedef Signed_Number32        Signed_Number;

typedef float                  Real_Number32;
typedef double                 Real_Number64;
typedef long double            Real_Number80;
typedef Real_Number32          Real_Number;

typedef Number8                Byte;
typedef Number                 Boolean;


#define stdcall __attribute__((__stdcall__))
#define cdecl   __attribute__((__cdecl__))
#define import  __attribute__((dllimport))
#define export  __attribute__((dllexport))


#pragma pack(push, 1) //no align for structs


#endif//TYPES_INCLUDED