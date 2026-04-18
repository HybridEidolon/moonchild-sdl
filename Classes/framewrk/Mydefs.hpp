#ifndef _MYDEFS_H
#define _MYDEFS_H

typedef enum 
{
  VG_FALSE = 0,
  VG_TRUE  = 1
} 
VG_BOOLEAN;


typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned short u16;
typedef char s8;
typedef unsigned char u8;

typedef unsigned char  UBYTE;
typedef char  CHAR;
typedef unsigned char  BYTE;
typedef char           INT8;
typedef unsigned char  UINT8;
typedef short          INT16;
typedef unsigned short UINT16;
typedef int            INT32;
typedef unsigned int   UINT32;
typedef char * LPSTR;

typedef struct _rect 
{
    int left;
    int right;
    int top;
    int bottom;
} RECT;

typedef struct tagPALETTEENTRY {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
} PALETTEENTRY;

typedef struct tagPOINT {
    int x;
    int y;
} POINT, *PPOINT;

#endif /* _MYDEFS_H */
