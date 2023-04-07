/**** guard*/
#ifndef LSTD_TYPES_H
#define LSTD_TYPES_H

typedef unsigned char uint8,u8 ; // alias for uint8 or u8 for access  
typedef signed char sint8 ,s8;    
   
typedef unsigned short int uint16,u16 ;   
typedef signed short int sint16 ,s16 ; 


typedef unsigned int uint32 ,u32  ; 
typedef signed  int sint32  ,s32 ;

typedef unsigned long long int    uint64,u64;
typedef signed long  long int    sint64,s64;

typedef float     f32,float32 ;  
typedef double    f64,float64;  

typedef volatile sint8     VINT8;     /* Volatile signed 8 bits. Prefix ?? */
typedef volatile uint8    VUINT8;    /* Volatile unsigned 8 bits. Prefix ?? */
typedef volatile sint16    VINT16;    /* Volatile signed 16 bits. Prefix ?? */
typedef volatile uint16   VUINT16;   /* Volatile unsigned 16 bits. Prefix ?? */
typedef volatile sint32    VINT32;    /* Volatile signed 32 bits. Prefix ?? */
typedef volatile uint32   VUINT32;   /* Volatile unsigned 32 bits. Prefix ?? */
typedef volatile uint64   VINT64;    /* Volatile signed 64 bits. Prefix ?? */
typedef volatile sint64   VUINT64;   /* Volatile unsigned 64 bits. Prefix ?? */

// error state

typedef enum
{
	NOK=0,
	OK
}ErrorState;
	
#endif // guard
