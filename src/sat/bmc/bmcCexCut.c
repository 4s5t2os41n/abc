/**CFile****************************************************************

  FileName    [bmcCexCut.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [SAT-based bounded model checking.]

  Synopsis    []

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: bmcCexCut.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "bmc.h"

ABC_NAMESPACE_IMPL_START


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Generate GIA for target bad states.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Gia_Man_t * Bmc_GiaTargetStates( Gia_Man_t * p, Abc_Cex_t * pCex, int iFrBeg, int iFrEnd, int fVerbose )
{
    Gia_Man_t * pNew, * pTemp;
    Gia_Obj_t * pObj, * pObjRo, * pObjRi;
    Vec_Bit_t * vInitNew;
    int i, k, iBit = 0;

    if ( iFrBeg < 0 )
        { printf( "Starting frame is less than 0.\n" ); return NULL; }
    if ( iFrEnd < 0 )
        { printf( "Stopping frame is less than 0.\n" ); return NULL; }
    if ( iFrBeg > pCex->iFrame )
        { printf( "Starting frame is more than the last frame of CEX (%d).\n", pCex->iFrame ); return NULL; }
    if ( iFrEnd > pCex->iFrame )
        { printf( "Stopping frame is more than the last frame of CEX (%d).\n", pCex->iFrame ); return NULL; }
    if ( iFrBeg >= iFrEnd )
        { printf( "Starting frame (%d) should be less than stopping frame (%d).\n", iFrBeg, iFrEnd ); return NULL; }
    assert( iFrBeg >= 0 && iFrBeg <= pCex->iFrame );
    assert( iFrEnd >= 0 && iFrEnd <= pCex->iFrame );
    assert( iFrBeg < iFrEnd );

    // skip trough the first iFrEnd frames
    Gia_ManCleanMark0(p);
    Gia_ManForEachRo( p, pObj, i )
        pObj->fMark0 = Abc_InfoHasBit(pCex->pData, iBit++);
    vInitNew = Vec_BitStart( Gia_ManRegNum(p) );
    for ( i = 0; i < iFrEnd; i++ )
    {
        // remember values in frame iFrBeg
        if ( i == iFrBeg )
            Gia_ManForEachRo( p, pObjRo, k )
                if ( pObjRo->fMark0 )
                    Vec_BitWriteEntry( vInitNew, k, 1 );
        // simulate other values
        Gia_ManForEachPi( p, pObj, k )
            pObj->fMark0 = Abc_InfoHasBit(pCex->pData, iBit++);
        Gia_ManForEachAnd( p, pObj, k )
            pObj->fMark0 = (Gia_ObjFanin0(pObj)->fMark0 ^ Gia_ObjFaninC0(pObj)) & 
                           (Gia_ObjFanin1(pObj)->fMark0 ^ Gia_ObjFaninC1(pObj));
        Gia_ManForEachCo( p, pObj, k )
            pObj->fMark0 = Gia_ObjFanin0(pObj)->fMark0 ^ Gia_ObjFaninC0(pObj);
        Gia_ManForEachRiRo( p, pObjRi, pObjRo, k )
            pObjRo->fMark0 = pObjRi->fMark0;
    }
    assert( i == iFrEnd );

    // create new AIG manager
    pNew = Gia_ManStart( 10000 );
    Gia_ManConst0(p)->Value = 1;
    Gia_ManForEachPi( p, pObj, k )
        pObj->Value = 1;
    Gia_ManForEachRo( p, pObjRo, i )
        pObj->Value = Abc_LitNotCond( Gia_ManAppendCi(pNew), !pObjRo->fMark0 );
    Gia_ManHashStart( p );
    for ( i = iFrEnd; i <= pCex->iFrame; i++ )
    {
        Gia_ManForEachPi( p, pObj, k )
            pObj->fMark0 = Abc_InfoHasBit(pCex->pData, iBit++);
        Gia_ManForEachAnd( p, pObj, k )
        {
            pObj->fMark0 = (Gia_ObjFanin0(pObj)->fMark0 ^ Gia_ObjFaninC0(pObj)) & 
                           (Gia_ObjFanin1(pObj)->fMark0 ^ Gia_ObjFaninC1(pObj));
            if ( pObj->fMark0 )
                pObj->Value = Gia_ManHashAnd( p, Gia_ObjFanin0(pObj)->Value, Gia_ObjFanin1(pObj)->Value );
            else if ( !Gia_ObjFanin0(pObj)->fMark0 && !Gia_ObjFanin1(pObj)->fMark0 )
                pObj->Value = Gia_ManHashOr( p, Gia_ObjFanin0(pObj)->Value, Gia_ObjFanin1(pObj)->Value );
            else if ( !Gia_ObjFanin0(pObj)->fMark0 )
                pObj->Value = Gia_ObjFanin0(pObj)->Value;
            else if ( !Gia_ObjFanin1(pObj)->fMark0 )
                pObj->Value = Gia_ObjFanin1(pObj)->Value;
            else assert( 0 );
        }
        Gia_ManForEachCo( p, pObj, k )
        {
            pObj->fMark0 = Gia_ObjFanin0(pObj)->fMark0 ^ Gia_ObjFaninC0(pObj);
            pObj->Value = Gia_ObjFanin0(pObj)->Value;
        }
        if ( i == pCex->iFrame )
            break;
        Gia_ManForEachRiRo( p, pObjRi, pObjRo, k )
        {
            pObjRo->fMark0 = pObjRi->fMark0;
            pObjRo->Value = pObjRi->Value;
        }
    }
    Gia_ManHashStop( p );
    assert( iBit == pCex->nBits );

    // create PO
    Gia_ManAppendCo( pNew, Gia_ManPo(p, pCex->iPo)->Value );
    // cleanup
    pNew = Gia_ManCleanup( pTemp = pNew );
    Gia_ManStop( pTemp );

    // create new GIA
    pNew = Gia_ManDupWithNewPo( p, pTemp = pNew ); 
    Gia_ManStop( pTemp );

    // create new initial state
    pNew = Gia_ManDupFlip( pTemp = pNew, Vec_BitArray(vInitNew) );
    Gia_ManStop( pTemp );
    Vec_BitFree( vInitNew );
    return pNew;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

