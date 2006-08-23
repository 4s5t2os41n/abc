/**CFile****************************************************************

  FileName    [ver.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Verilog parser.]

  Synopsis    [External declarations.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - August 19, 2006.]

  Revision    [$Id: ver.h,v 1.00 2006/08/19 00:00:00 alanmi Exp $]

***********************************************************************/

#ifndef __VER_H__
#define __VER_H__

#ifdef __cplusplus
extern "C" {
#endif 

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "abc.h"

////////////////////////////////////////////////////////////////////////
///                         PARAMETERS                               ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                         BASIC TYPES                              ///
////////////////////////////////////////////////////////////////////////

typedef struct Ver_Man_t_       Ver_Man_t;
typedef struct Ver_Stream_t_    Ver_Stream_t;

struct Ver_Man_t_
{
    // input file stream
    char *          pFileName;
    Ver_Stream_t *  pReader;
    ProgressBar *   pProgress;
    // current network and library
    Abc_Ntk_t *     pNtkCur;       // the network under construction
    Abc_Lib_t *     pDesign;       // the current design 
    // parameters
    int             fUseMemMan;    // allocate memory manager in the networks
    int             fCheck;        // checks network for currectness
    // error recovery
    FILE *          Output;
    int             fTopLevel;
    int             fError;
    char            sError[2000];
    // intermediate structures
    Vec_Ptr_t *     vNames;  
    Vec_Ptr_t *     vStackFn;
    Vec_Int_t *     vStackOp;
};

////////////////////////////////////////////////////////////////////////
///                      MACRO DEFINITIONS                           ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                             ITERATORS                            ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DECLARATIONS                         ///
////////////////////////////////////////////////////////////////////////

/*=== verCore.c ========================================================*/
extern Abc_Lib_t *    Ver_ParseFile( char * pFileName, Abc_Lib_t * pGateLib, int fCheck, int fUseMemMan );
extern void           Ver_ParsePrintErrorMessage( Ver_Man_t * p );
/*=== verFormula.c ========================================================*/
extern void *         Ver_FormulaParser( char * pFormula, void * pMan, Vec_Ptr_t * vNames, Vec_Ptr_t * vStackFn, Vec_Int_t * vStackOp, char * pErrorMessage );
extern void *         Ver_FormulaReduction( char * pFormula, void * pMan, Vec_Ptr_t * vNames, char * pErrorMessage );
/*=== verParse.c ========================================================*/
extern int            Ver_ParseSkipComments( Ver_Man_t * p );
extern char *         Ver_ParseGetName( Ver_Man_t * p );
/*=== verStream.c ========================================================*/
extern Ver_Stream_t * Ver_StreamAlloc( char * pFileName );
extern void           Ver_StreamFree( Ver_Stream_t * p );
extern char *         Ver_StreamGetFileName( Ver_Stream_t * p );
extern int            Ver_StreamGetFileSize( Ver_Stream_t * p );
extern int            Ver_StreamGetCurPosition( Ver_Stream_t * p );
extern int            Ver_StreamGetLineNumber( Ver_Stream_t * p );

extern int            Ver_StreamIsOkey( Ver_Stream_t * p );
extern char           Ver_StreamScanChar( Ver_Stream_t * p );
extern char           Ver_StreamPopChar( Ver_Stream_t * p );
extern void           Ver_StreamSkipChars( Ver_Stream_t * p, char * pCharsToSkip );
extern void           Ver_StreamSkipToChars( Ver_Stream_t * p, char * pCharsToStop );
extern char *         Ver_StreamGetWord( Ver_Stream_t * p, char * pCharsToStop );

#ifdef __cplusplus
}
#endif

#endif

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

