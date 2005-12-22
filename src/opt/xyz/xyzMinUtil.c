/**CFile****************************************************************

  FileName    [xyzMinUtil.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Cover manipulation package.]

  Synopsis    [Utilities.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: xyzMinUtil.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "xyzInt.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Min_CubeWrite( FILE * pFile, Min_Cube_t * pCube )
{
    int i;
    assert( (int)pCube->nLits == Min_CubeCountLits(pCube) );
    for ( i = 0; i < (int)pCube->nVars; i++ )
        if ( Min_CubeHasBit(pCube, i*2) )
        {
            if ( Min_CubeHasBit(pCube, i*2+1) )
                fprintf( pFile, "-" );
            else
                fprintf( pFile, "0" );
        }
        else
        {
            if ( Min_CubeHasBit(pCube, i*2+1) )
                fprintf( pFile, "1" );
            else
                fprintf( pFile, "?" );
        }
    fprintf( pFile, " 1\n" );
//    fprintf( pFile, " %d\n", pCube->nLits );
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Min_CoverWrite( FILE * pFile, Min_Cube_t * pCover )
{
    Min_Cube_t * pCube;
    Min_CoverForEachCube( pCover, pCube )
        Min_CubeWrite( pFile, pCube );
    printf( "\n" );
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Min_CoverWriteFile( Min_Cube_t * pCover, char * pName, int fEsop )
{
    char Buffer[1000];
    Min_Cube_t * pCube;
    FILE * pFile;
    int i;
    sprintf( Buffer, "%s.esop", pName );
    for ( i = strlen(Buffer) - 1; i >= 0; i-- )
        if ( Buffer[i] == '<' || Buffer[i] == '>' )
            Buffer[i] = '_';
    pFile = fopen( Buffer, "w" );
    fprintf( pFile, "# %s cover for output %s generated by ABC on %s\n", fEsop? "ESOP":"SOP", pName, Extra_TimeStamp() );
    fprintf( pFile, ".i %d\n", pCover? pCover->nVars : 0 );
    fprintf( pFile, ".o %d\n", 1 );
    fprintf( pFile, ".p %d\n", Min_CoverCountCubes(pCover) );
    if ( fEsop ) fprintf( pFile, ".type esop\n" );
    Min_CoverForEachCube( pCover, pCube )
        Min_CubeWrite( pFile, pCube );
    fprintf( pFile, ".e\n" );
    fclose( pFile );
}

/**Function*************************************************************

  Synopsis    [Performs one round of rewriting using distance 2 cubes.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Min_CoverCheck( Min_Man_t * p )
{
    Min_Cube_t * pCube;
    int i;
    for ( i = 0; i <= p->nVars; i++ )
        Min_CoverForEachCube( p->ppStore[i], pCube )
            assert( i == (int)pCube->nLits );
}


/**Function*************************************************************

  Synopsis    [Converts the cover from the sorted structure.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Min_Cube_t * Min_CoverCollect( Min_Man_t * p, int nSuppSize )
{
    Min_Cube_t * pCov = NULL, ** ppTail = &pCov;
    Min_Cube_t * pCube, * pCube2;
    int i;
    for ( i = 0; i <= nSuppSize; i++ )
    {
        Min_CoverForEachCubeSafe( p->ppStore[i], pCube, pCube2 )
        {
            assert( i == (int)pCube->nLits );
            *ppTail = pCube; 
            ppTail = &pCube->pNext;
        }
    }
    *ppTail = NULL;
    return pCov;
}

/**Function*************************************************************

  Synopsis    [Sorts the cover in the increasing number of literals.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Min_CoverExpand( Min_Man_t * p, Min_Cube_t * pCover )
{
    Min_Cube_t * pCube, * pCube2;
    Min_ManClean( p, p->nVars );
    Min_CoverForEachCubeSafe( pCover, pCube, pCube2 )
    {
        pCube->pNext = p->ppStore[pCube->nLits];
        p->ppStore[pCube->nLits] = pCube;
        p->nCubes++;
    }
}

/**Function*************************************************************

  Synopsis    [Sorts the cover in the increasing number of literals.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Min_CoverSuppVarNum( Min_Man_t * p, Min_Cube_t * pCover )
{
    Min_Cube_t * pCube;
    int i, Counter;
    if ( pCover == NULL )
        return 0;
    // clean the cube
    for ( i = 0; i < (int)pCover->nWords; i++ )
        p->pTemp->uData[i]  = ~((unsigned)0);
    // add the bit data
    Min_CoverForEachCube( pCover, pCube )
        for ( i = 0; i < (int)pCover->nWords; i++ )
            p->pTemp->uData[i] &= pCube->uData[i];
    // count the vars
    Counter = 0;
    for ( i = 0; i < (int)pCover->nVars; i++ )
        Counter += ( Min_CubeGetVar(p->pTemp, i) != 3 );
    return Counter;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


