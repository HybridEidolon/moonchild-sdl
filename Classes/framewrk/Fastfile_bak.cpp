/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       fastfile.c
 *  Content:    Fast file I/O for large numbers of files.
 *              Uses a single file built using FFCREATE.EXE; this file
 *              contains a directory + all the files.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 ***************************************************************************/
#include <windows.h>
//#include <io.h>
//#include <fcntl.h>
//#include <search.h>
#include <stdio.h>
#include "fastfile.h"
#include "ffent.h"
#include "log.hpp"

// strip the directory path of all files provided to open
#define USEBASENAME 1

#define TESTCASE

//#define LOG

#define FLOG



#ifdef __WATCOMC__
#define _stricmp stricmp
#endif

typedef struct {
    BOOL        inuse;
    LONG        pos;
    LONG        size;
    LPFILEENTRY pfe;
} FILEHANDLE, FAR *LPFILEHANDLE;

static int              LockCount;
static HANDLE           hFile;
static HANDLE           hFileMapping;
static LPFILEENTRY      pFE;
static LPBYTE           pBase;
static DWORD            dwFECnt;	// nr of files
static LPFILEHANDLE     lpFH;		// fileentries
static DWORD            dwFHCnt;
static long             lFileEnd;

static FILE *fHndl = 0;

/*
 * Compare 
 *
 * bsearch comparison routine
 */
int __cdecl Compare( LPFILEENTRY p1, LPFILEENTRY p2 )
{
    int i;
    i = ( _stricmp( (p1)->name,(p2)->name ) );
    return i;
    
} /* Compare */

int stricmp( char* s1, char* s2 )
{
	char* a = s1;
	char* b = s2;
	while ((*a) || (*b))
	{
		char c1 = *a;
		char c2 = *b;
		if ((c1 >= 'A') && (c1 <= 'Z')) c1 -= 'A' - 'a';
		if ((c2 >= 'A') && (c2 <= 'Z')) c2 -= 'A' - 'a';
		if (c1 == c2)
		{
			a++;
			b++;
		}
		else if (c1 < c2) return -1; else return 1; 
	}

	if((*a)==0 && (*b)==0) return 0;
	if((*a)!=0 && (*b)==0) return 1;
	if((*a)==0 && (*b)!=0) return -1;

	// hier kan ie niet komen
	return 0;
}


/*
 * FastFileInit
 *
 * Initialize for fast file access. The master file and maximum number
 * of open "files" are specified.
 */
int FastFileInit( LPCWSTR fname, int max_handles )
{
    HRSRC  h;
	return TRUE;

    LockCount = 0;
    FastFileFini();

    /*
     * get a file handle array
     */
    lpFH = (FILEHANDLE *)LocalAlloc( LPTR, max_handles * sizeof( FILEHANDLE ) );
    if( lpFH == NULL ) {

        return FALSE;
    }
    dwFHCnt = max_handles;


	fHndl = _wfopen(fname, L"rb");
	if(fHndl)
	{
		int t;
		/*
		 * get initial data from the memory mapped file
		 */

		fread(&dwFECnt, 1, 4, fHndl);

		pBase = (BYTE *) malloc(sizeof(FILEENTRY) * dwFECnt + 4);
		memcpy((void *)pBase, (void *)&dwFECnt, 4);

		pFE = (FILEENTRY *)(pBase+4);
		t = fread(pFE, 1, sizeof(FILEENTRY) * dwFECnt, fHndl);
		fseek(fHndl, 0, SEEK_SET );		// rewind
	    lFileEnd = pFE[dwFECnt-1].offset;

	}
	else
	{
	    return FALSE;
	}


#if 0

    /*
     * try our resourse file first
     */
//    if (h = FindResource(NULL, fname, RT_RCDATA))
	if(0)
    {
        pBase = (unsigned char *)LockResource(LoadResource(NULL, h));

        if (pBase == NULL)
        {
            FastFileFini();
            return FALSE;
        }

    }
    else   
    {

        /*
         * create a memory mapped file for the master file
         */
			hFile = CreateFile( fname, GENERIC_READ, FILE_SHARE_READ, NULL,
                                OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0 );

        if( hFile == NULL || hFile == (HANDLE)HFILE_ERROR )
        {
            hFile = NULL;
            FastFileFini();
            return FALSE;
        }
        hFileMapping = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 0, NULL );
        if( hFileMapping == NULL ) {
            FastFileFini();
            return FALSE;
        }
        pBase = (unsigned char *)MapViewOfFile( hFileMapping, FILE_MAP_READ, 0, 0, 0 );
        if( pBase == NULL ) {
            FastFileFini();
            return FALSE;
        }
    }

#endif
    return TRUE;

} /* FastFileInit */

/*
 * FastFileFini
 *
 * Clean up resources
 */
void FastFileFini( void )
{
 	return;
   //
    //  dont unmap things if we have locks out standing
    //
    if (LockCount != 0)
        return;

    if( hFileMapping != NULL && pBase ) {
        UnmapViewOfFile( pBase );
    }
    if( hFileMapping != NULL ) {
        CloseHandle( hFileMapping );
        hFileMapping = NULL;
    }
    if( hFile != NULL ) {
        CloseHandle( hFile );
        hFile = NULL;
    }
    if( lpFH != NULL ) {
        LocalFree( lpFH );
        lpFH = NULL;
    }
	if(fHndl)
	{
		fclose(fHndl);
		fHndl = 0;
	}
    dwFHCnt = 0;
    pBase = NULL;
    dwFECnt = 0;
    pFE = NULL;

} /* FastFileFini */

/*
 * FastFileOpen
 *
 * Search the directory for the file, and return a file handle if found.
 */
HFASTFILE FastFileOpen( LPSTR name )
{
    FILEENTRY   fe;
    LPFILEENTRY pfe;
#if USEBASENAME
    char *baseptr;
#endif // USEBASENAME

    
	
	if( pFE == NULL ) {
        return NULL;
    }
    if( name == NULL || name[0] == 0 ) {
        return NULL;
    }
	return 0;

#if USEBASENAME
    baseptr = name + strlen(name)-1; // set at last char
    while (baseptr > name) {
        if (*baseptr == '/' || *baseptr == '\\' || *baseptr == ':') {
            baseptr++;
            break;
        }
        baseptr--;
    }
    strncpy( fe.name, baseptr,15 );
    fe.name[15] = 0;
#else // !USEBASENAME
    strncpy( fe.name, name,15 );
    fe.name[15] = 0;
#endif // USEBASENAME


#if 1
  {
        DWORD i;

        pfe = NULL;
        for (i=0; i<dwFECnt; i++) {
            int rc;
            rc = stricmp(pFE[i].name, fe.name); 

            if (rc == 0) {
                pfe = &pFE[i];
                break;
            }
        }
  }
#else        
    pfe = bsearch( &fe, pFE, dwFECnt, sizeof( FILEENTRY ), (LPVOID) Compare );
#endif
    
    if( pfe != NULL ) {
        DWORD   i;
        for( i=0;i<dwFHCnt;i++ ) {
            if( !lpFH[i].inuse ) {
                lpFH[i].inuse = TRUE;
                lpFH[i].pos = pfe->offset;
                lpFH[i].size = (pfe+1)->offset - pfe->offset;
                lpFH[i].pfe = pfe;

                return &lpFH[i];
            }
        }
    } else {
    }

    return NULL;

} /* FastFileOpen */

/*
 * FastFileClose
 *
 * Mark a fast file handle as closed
 */
BOOL FastFileClose( HFASTFILE pfh )
{
	return TRUE;
	LPFILEHANDLE _pfh;

	_pfh = (LPFILEHANDLE) pfh;
    if( _pfh == NULL || _pfh->inuse != TRUE ) {
        return FALSE;
    }
    _pfh->inuse = FALSE;
    return TRUE;

} /* FastFileClose */

/*
 * FastFileLock
 *
 * return a memory pointer into a fast file
 */
LPVOID FastFileLock( LPFILEHANDLE pfh, int pos, int size )
{
	return 0;
    if( pfh == NULL || pfh->inuse != TRUE ) {
        return NULL;
    }
    if( size < 0 ) {
        return NULL;
    }
    if( (pos + size) > ((pfh->pfe)+1)->offset ) {
        return NULL;
    }
    LockCount++;
    return pBase + pfh->pos + pos;

} /* FastFileLock */

/*
 * FastFileUnlock
 *
 */
BOOL FastFileUnlock( LPFILEHANDLE pfh, int pos, int size )
{
	return TRUE;
    if( pfh == NULL || pfh->inuse != TRUE ) {
        return FALSE;
    }
    if( size < 0 ) {
        return FALSE;
    }
    if( (pos + size) > ((pfh->pfe)+1)->offset ) {
        return FALSE;
    }

    LockCount--;
    return TRUE;

} /* FastFileUnlock */

/*
 * FastFileRead
 *
 * read from a fast file (memcpy!)
 */
BOOL FastFileRead( HFASTFILE pfh, LPVOID ptr, int size )
{
	return TRUE;
 	LPFILEHANDLE _pfh;

	_pfh = (LPFILEHANDLE) pfh;
   if( _pfh == NULL || _pfh->inuse != TRUE ) {
        return FALSE;
    }
    if( size < 0 ) {
        return FALSE;
    }
    if( (_pfh->pos + size) > ((_pfh->pfe)+1)->offset ) {
        return FALSE;
    }
//    memcpy( ptr, pBase + _pfh->pos, size );

	fseek(fHndl, _pfh->pos, SEEK_SET);
	fread(ptr, 1, size, fHndl);

#if 0
#if DO_XORDATA
    {
      int i;
      unsigned char *p = ptr;
      for (i=0; i<size; i++) {
  *p++ ^= XORVALUE;
      }
    }
#endif // DO_XORDATA
#endif

    _pfh->pos += size;
    return TRUE;

} /* FastFileRead */

/*
 * FastFileSeek
 *
 * Set to a new position in a fast file.  Uses standard SEEK_SET, SEEK_CUR,
 * SEEK_END definitions.
 */
BOOL FastFileSeek( HFASTFILE pfh, int off, int how )
{
	return TRUE;
 	LPFILEHANDLE _pfh;
    LPFILEENTRY pfe;

	_pfh = (LPFILEHANDLE) pfh;

    if( _pfh == NULL || _pfh->inuse != TRUE ) {
        return FALSE;
    }
    pfe = _pfh->pfe;
    if( how == SEEK_SET ) {
        if( off < 0 || off >= _pfh->size ) {
            return FALSE;
        }
        off += pfe->offset;
    } else if( how == SEEK_END ) {
        if( off >= _pfh->size ) {
            return FALSE;
        }
        off = (pfe+1)->offset + off;      // RVV -=> changed to +!!!  otherwise not standard!
    } else if( how == SEEK_CUR ) {
        off = _pfh->pos + off;
        if( off < pfe->offset || off >= (pfe+1)->offset ) {
            return FALSE;
        }
    } else {
        return FALSE;
    }
    _pfh->pos = off;
    return TRUE;

} /* FastFileSeek */

/*
 * FastFileTell
 *
 * Get the current position in a fast file
 */
long FastFileTell( LPFILEHANDLE pfh )
{
	return 0;
    if( pfh == NULL || pfh->inuse != TRUE ) {
        return -1;
    }
    return pfh->pos - pfh->pfe->offset;

} /* FastFileTell */
