/**CFile****************************************************************

  FileName    [seqUtil.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Construction and manipulation of sequential AIGs.]

  Synopsis    [Various utilities working with sequential AIGs.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: seqUtil.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "seqInt.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Returns the maximum latch number on any of the fanouts.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Seq_ObjFanoutLMax( Abc_Obj_t * pObj )
{
    Abc_Obj_t * pFanout;
    int i, nLatchCur, nLatchRes;
    if ( Abc_ObjFanoutNum(pObj) == 0 )
        return 0;
    nLatchRes = 0;
    Abc_ObjForEachFanout( pObj, pFanout, i )
    {
        nLatchCur = Abc_ObjFanoutL(pObj, pFanout);
        if ( nLatchRes < nLatchCur )
            nLatchRes = nLatchCur;
    }
    assert( nLatchRes >= 0 );
    return nLatchRes;
}

/**Function*************************************************************

  Synopsis    [Returns the minimum latch number on any of the fanouts.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Seq_ObjFanoutLMin( Abc_Obj_t * pObj )
{
    Abc_Obj_t * pFanout;
    int i, nLatchCur, nLatchRes;
    if ( Abc_ObjFanoutNum(pObj) == 0 )
        return 0;
    nLatchRes = ABC_INFINITY;
    Abc_ObjForEachFanout( pObj, pFanout, i )
    {
        nLatchCur = Abc_ObjFanoutL(pObj, pFanout);
        if ( nLatchRes > nLatchCur )
            nLatchRes = nLatchCur;
    }
    assert( nLatchRes < ABC_INFINITY );
    return nLatchRes;
}

/**Function*************************************************************

  Synopsis    [Returns the sum of latches on the fanout edges.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Seq_ObjFanoutLSum( Abc_Obj_t * pObj )
{
    Abc_Obj_t * pFanout;
    int i, nSum = 0;
    Abc_ObjForEachFanout( pObj, pFanout, i )
        nSum += Abc_ObjFanoutL(pObj, pFanout);
    return nSum;
}

/**Function*************************************************************

  Synopsis    [Returns the sum of latches on the fanin edges.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Seq_ObjFaninLSum( Abc_Obj_t * pObj )
{
    Abc_Obj_t * pFanin;
    int i, nSum = 0;
    Abc_ObjForEachFanin( pObj, pFanin, i )
        nSum += Abc_ObjFaninL(pObj, i);
    return nSum;
}

/**Function*************************************************************

  Synopsis    [Generates the printable edge label with the initial state.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
char * Seq_ObjFaninGetInitPrintable( Abc_Obj_t * pObj, int Edge )
{
    static char Buffer[1000];
    Abc_InitType_t Init;
    int nLatches, i;
    nLatches = Abc_ObjFaninL( pObj, Edge );
    for ( i = 0; i < nLatches; i++ )
    {
        Init = Seq_LatInit( Seq_NodeGetLat(pObj, Edge, i) );
        if ( Init == ABC_INIT_NONE )
            Buffer[i] = '_';
        else if ( Init == ABC_INIT_ZERO )
            Buffer[i] = '0';
        else if ( Init == ABC_INIT_ONE )
            Buffer[i] = '1';
        else if ( Init == ABC_INIT_DC )
            Buffer[i] = 'x';
        else assert( 0 );
    }
    Buffer[nLatches] = 0;
    return Buffer;
}

/**Function*************************************************************

  Synopsis    [Sets the given value to all the latches of the edge.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Seq_NodeLatchSetValues( Abc_Obj_t * pObj, int Edge, Abc_InitType_t Init )
{ 
    Seq_Lat_t * pLat, * pRing;
    int c; 
    pRing = Seq_NodeGetRing(pObj, Edge);
    if ( pRing == NULL ) 
        return; 
    for ( c = 0, pLat = pRing; !c || pLat != pRing; c++, pLat = pLat->pNext )
        Seq_LatSetInit( pLat, Init );
}

/**Function*************************************************************

  Synopsis    [Sets the given value to all the latches of the edge.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Seq_NtkLatchSetValues( Abc_Ntk_t * pNtk, Abc_InitType_t Init )
{ 
    Abc_Obj_t * pObj;
    int i;
    assert( Abc_NtkIsSeq( pNtk ) );
    Abc_NtkForEachPo( pNtk, pObj, i )
        Seq_NodeLatchSetValues( pObj, 0, Init );
    Abc_NtkForEachNode( pNtk, pObj, i )
    {
        Seq_NodeLatchSetValues( pObj, 0, Init );
        Seq_NodeLatchSetValues( pObj, 1, Init );
    }
}


/**Function*************************************************************

  Synopsis    [Counts the number of latches in the sequential AIG.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Seq_NtkLatchNum( Abc_Ntk_t * pNtk )
{
    Abc_Obj_t * pObj;
    int i, Counter;
    assert( Abc_NtkIsSeq( pNtk ) );
    Counter = 0;
    Abc_NtkForEachNode( pNtk, pObj, i )
        Counter += Seq_ObjFaninLSum( pObj );
    Abc_NtkForEachPo( pNtk, pObj, i )
        Counter += Seq_ObjFaninLSum( pObj );
    return Counter;
}

/**Function*************************************************************

  Synopsis    [Counts the number of latches in the sequential AIG.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Seq_NtkLatchNumMax( Abc_Ntk_t * pNtk )
{
    Abc_Obj_t * pObj;
    int i, Max, Cur;
    assert( Abc_NtkIsSeq( pNtk ) );
    Max = 0;
    Abc_AigForEachAnd( pNtk, pObj, i )
    {
        Cur = Abc_ObjFaninLMax( pObj );
        if ( Max < Cur )
            Max = Cur;
    }
    Abc_NtkForEachPo( pNtk, pObj, i )
    {
        Cur = Abc_ObjFaninL0( pObj );
        if ( Max < Cur )
            Max = Cur;
    }
    return Max;
}

/**Function*************************************************************

  Synopsis    [Counts the number of latches in the sequential AIG.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Seq_NtkLatchNumShared( Abc_Ntk_t * pNtk )
{
    Abc_Obj_t * pObj;
    int i, Counter;
    assert( Abc_NtkIsSeq( pNtk ) );
    Counter = 0;
    Abc_NtkForEachPi( pNtk, pObj, i )
        Counter += Seq_ObjFanoutLMax( pObj );
    Abc_NtkForEachNode( pNtk, pObj, i )
        Counter += Seq_ObjFanoutLMax( pObj );
    return Counter;
}

/**Function*************************************************************

  Synopsis    [Counts the number of latches in the sequential AIG.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Seq_ObjLatchGetInitNums( Abc_Obj_t * pObj, int Edge, int * pInits )
{
    Abc_InitType_t Init;
    int nLatches, i;
    nLatches = Abc_ObjFaninL( pObj, Edge );
    for ( i = 0; i < nLatches; i++ )
    {
        Init = Seq_NodeGetInitOne( pObj, Edge, i );
        pInits[Init]++;
    }
}

/**Function*************************************************************

  Synopsis    [Counts the number of latches in the sequential AIG.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Seq_NtkLatchGetInitNums( Abc_Ntk_t * pNtk, int * pInits )
{
    Abc_Obj_t * pObj;
    int i;
    assert( Abc_NtkIsSeq( pNtk ) );
    for ( i = 0; i < 4; i++ )    
        pInits[i] = 0;
    Abc_NtkForEachPo( pNtk, pObj, i )
        Seq_ObjLatchGetInitNums( pObj, 0, pInits );
    Abc_NtkForEachNode( pNtk, pObj, i )
    {
        if ( Abc_ObjFaninNum(pObj) > 0 )
            Seq_ObjLatchGetInitNums( pObj, 0, pInits );
        if ( Abc_ObjFaninNum(pObj) > 1 )
            Seq_ObjLatchGetInitNums( pObj, 1, pInits );
    }
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


