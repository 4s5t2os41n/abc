/**CFile****************************************************************

  FileName    [utilCex.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Handling counter-examples.]

  Synopsis    [Handling counter-examples.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - Feburary 13, 2011.]

  Revision    [$Id: utilCex.c,v 1.00 2011/02/11 00:00:00 alanmi Exp $]

***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "abc_global.h"
#include "utilCex.h"

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////


/**Function*************************************************************

  Synopsis    [Allocates a counter-example.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Cex_t * Abc_CexAlloc( int nRegs, int nRealPis, int nFrames )
{
    Abc_Cex_t * pCex;
    int nWords = Abc_BitWordNum( nRegs + nRealPis * nFrames );
    pCex = (Abc_Cex_t *)ABC_ALLOC( char, sizeof(Abc_Cex_t) + sizeof(unsigned) * nWords );
    memset( pCex, 0, sizeof(Abc_Cex_t) + sizeof(unsigned) * nWords );
    pCex->nRegs  = nRegs;
    pCex->nPis   = nRealPis;
    pCex->nBits  = nRegs + nRealPis * nFrames;
    return pCex;
}
Abc_Cex_t * Abc_CexAllocFull( int nRegs, int nRealPis, int nFrames )
{
    Abc_Cex_t * pCex;
    int nWords = Abc_BitWordNum( nRegs + nRealPis * nFrames );
    pCex = (Abc_Cex_t *)ABC_ALLOC( char, sizeof(Abc_Cex_t) + sizeof(unsigned) * nWords );
    memset( pCex, 0xFF, sizeof(Abc_Cex_t) + sizeof(unsigned) * nWords );
    pCex->nRegs  = nRegs;
    pCex->nPis   = nRealPis;
    pCex->nBits  = nRegs + nRealPis * nFrames;
    return pCex;
}

/**Function*************************************************************

  Synopsis    [Make the trivial counter-example for the trivially asserted output.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Cex_t * Abc_CexMakeTriv( int nRegs, int nTruePis, int nTruePos, int iFrameOut )
{
    Abc_Cex_t * pCex;
    int iPo, iFrame;
    assert( nRegs > 0 );
    iPo    = iFrameOut % nTruePos;
    iFrame = iFrameOut / nTruePos;
    // allocate the counter example
    pCex = Abc_CexAlloc( nRegs, nTruePis, iFrame + 1 );
    pCex->iPo    = iPo;
    pCex->iFrame = iFrame;
    return pCex;
}

/**Function*************************************************************

  Synopsis    [Derives the counter-example.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Cex_t * Abc_CexCreate( int nRegs, int nPis, int * pArray, int iFrame, int iPo, int fSkipRegs )
{
    Abc_Cex_t * pCex;
    int i;
    pCex = Abc_CexAlloc( nRegs, nPis, iFrame+1 );
    pCex->iPo    = iPo;
    pCex->iFrame = iFrame;
    if ( pArray == NULL )
        return pCex;
    if ( fSkipRegs )
    {
        for ( i = nRegs; i < pCex->nBits; i++ )
            if ( pArray[i-nRegs] )
                Abc_InfoSetBit( pCex->pData, i );
    }
    else
    {
        for ( i = 0; i < pCex->nBits; i++ )
            if ( pArray[i] )
                Abc_InfoSetBit( pCex->pData, i );
    }
    return pCex;
}

/**Function*************************************************************

  Synopsis    [Make the trivial counter-example for the trivially asserted output.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Cex_t * Abc_CexDup( Abc_Cex_t * p, int nRegsNew )
{
    Abc_Cex_t * pCex;
    int i;
    if ( nRegsNew == -1 )
        nRegsNew = p->nRegs;
    pCex = Abc_CexAlloc( nRegsNew, p->nPis, p->iFrame+1 );
    pCex->iPo    = p->iPo;
    pCex->iFrame = p->iFrame;
    for ( i = p->nRegs; i < p->nBits; i++ )
        if ( Abc_InfoHasBit(p->pData, i) )
            Abc_InfoSetBit( pCex->pData, pCex->nRegs + i - p->nRegs );
    return pCex;
}

/**Function*************************************************************

  Synopsis    [Derives CEX from comb model.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Cex_t * Abc_CexDeriveFromCombModel( int * pModel, int nPis, int nRegs, int iPo )
{
    Abc_Cex_t * pCex;
    int i;
    pCex = Abc_CexAlloc( nRegs, nPis, 1 );
    pCex->iPo = iPo;
    pCex->iFrame = 0;
    for ( i = 0; i < nPis; i++ )
        if ( pModel[i] )
            pCex->pData[i>>5] |= (1<<(i & 31));     
    return pCex;
}

/**Function*************************************************************

  Synopsis    [Derives CEX from comb model.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Cex_t * Abc_CexMerge( Abc_Cex_t * pCex, Abc_Cex_t * pPart, int iFrBeg, int iFrEnd )
{
    Abc_Cex_t * pNew;
    int nFramesGain;
    int i, f, iBit;

    if ( iFrBeg < 0 )
        { printf( "Starting frame is less than 0.\n" ); return NULL; }
    if ( iFrEnd < 0 )
        { printf( "Stopping frame is less than 0.\n" ); return NULL; }
    if ( iFrBeg > pCex->iFrame )
        { printf( "Starting frame is more than the last frame of CEX (%d).\n", pCex->iFrame ); return NULL; }
    if ( iFrEnd > pCex->iFrame )
        { printf( "Stopping frame is more than the last frame of CEX (%d).\n", pCex->iFrame ); return NULL; }
    if ( iFrBeg > iFrEnd )
        { printf( "Starting frame (%d) should be less than stopping frame (%d).\n", iFrBeg, iFrEnd ); return NULL; }
    assert( iFrBeg >= 0 && iFrBeg <= pCex->iFrame );
    assert( iFrEnd >= 0 && iFrEnd <= pCex->iFrame );
    assert( iFrBeg <= iFrEnd );

    assert( pCex->nPis == pPart->nPis );
    assert( iFrEnd - iFrBeg + pPart->iPo >= pPart->iFrame );

    nFramesGain = iFrEnd - iFrBeg + pPart->iPo - pPart->iFrame;
    pNew = Abc_CexAlloc( pCex->nRegs, pCex->nPis, pCex->iFrame + 1 - nFramesGain );
    pNew->iPo    = pCex->iPo;
    pNew->iFrame = pCex->iFrame - nFramesGain;

    for ( iBit = 0; iBit < pCex->nRegs; iBit++ )
        if ( Abc_InfoHasBit(pCex->pData, iBit) )
            Abc_InfoSetBit( pNew->pData, iBit );
    for ( f = 0; f < iFrBeg; f++ )
        for ( i = 0; i < pCex->nPis; i++, iBit++ )
            if ( Abc_InfoHasBit(pCex->pData, pCex->nRegs + pCex->nPis * f + i) )
                Abc_InfoSetBit( pNew->pData, iBit );
    for ( f = 0; f < pPart->iFrame; f++ )
        for ( i = 0; i < pCex->nPis; i++, iBit++ )
            if ( Abc_InfoHasBit(pPart->pData, pPart->nRegs + pCex->nPis * f + i) )
                Abc_InfoSetBit( pNew->pData, iBit );
    for ( f = iFrEnd; f <= pCex->iFrame; f++ )
        for ( i = 0; i < pCex->nPis; i++, iBit++ )
            if ( Abc_InfoHasBit(pCex->pData, pCex->nRegs + pCex->nPis * f + i) )
                Abc_InfoSetBit( pNew->pData, iBit );
    assert( iBit == pNew->nBits );

    return pNew;
}

/**Function*************************************************************

  Synopsis    [Prints out the counter-example.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_CexPrintStats( Abc_Cex_t * p )
{
    int k, Counter = 0;
    if ( p == NULL )
    {
        printf( "The counter example is NULL.\n" );
        return;
    }
    for ( k = 0; k < p->nBits; k++ )
        Counter += Abc_InfoHasBit(p->pData, k);
    printf( "CEX: iPo = %d  iFrame = %d  nRegs = %d  nPis = %d  nBits = %d  nOnes = %5d (%5.2f %%)\n", 
        p->iPo, p->iFrame, p->nRegs, p->nPis, p->nBits, Counter, 100.0 * Counter / (p->nBits - p->nRegs) );
}
void Abc_CexPrintStatsInputs( Abc_Cex_t * p, int nInputs )
{
    int k, Counter = 0, Counter2 = 0;
    if ( p == NULL )
    {
        printf( "The counter example is NULL.\n" );
        return;
    }
    for ( k = 0; k < p->nBits; k++ )
    {
        Counter += Abc_InfoHasBit(p->pData, k);
        if ( (k - p->nRegs) % p->nPis < nInputs )
            Counter2 += Abc_InfoHasBit(p->pData, k);
    }
    printf( "CEX: iPo = %d  iFrame = %d  nRegs = %d  nPis = %d  nBits = %d  nOnes = %5d (%5.2f %%)  nOnesIn = %5d (%5.2f %%)\n", 
        p->iPo, p->iFrame, p->nRegs, p->nPis, p->nBits, 
        Counter,  100.0 * Counter  / (p->nBits - p->nRegs), 
        Counter2, 100.0 * Counter2 / (p->nBits - p->nRegs - (p->iFrame + 1) * (p->nPis - nInputs)) );
}

/**Function*************************************************************

  Synopsis    [Prints out the counter-example.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_CexPrint( Abc_Cex_t * p )
{
    int i, f, k;
    if ( p == NULL )
    {
        printf( "The counter example is NULL.\n" );
        return;
    }
    Abc_CexPrintStats( p );
    printf( "State    : " );
    for ( k = 0; k < p->nRegs; k++ )
        printf( "%d", Abc_InfoHasBit(p->pData, k) );
    printf( "\n" );
    for ( f = 0; f <= p->iFrame; f++ )
    {
        printf( "Frame %3d : ", f );
        for ( i = 0; i < p->nPis; i++ )
            printf( "%d", Abc_InfoHasBit(p->pData, k++) );
        printf( "\n" );
    }
    assert( k == p->nBits );
}

/**Function*************************************************************

  Synopsis    [Frees the counter-example.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_CexFreeP( Abc_Cex_t ** p )
{
    if ( *p == NULL )
        return;
    ABC_FREE( *p );
}

/**Function*************************************************************

  Synopsis    [Frees the counter-example.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_CexFree( Abc_Cex_t * p )
{
    ABC_FREE( p );
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

