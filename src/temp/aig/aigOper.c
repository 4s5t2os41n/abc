/**CFile****************************************************************

  FileName    [aigOper.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Minimalistic And-Inverter Graph package.]

  Synopsis    [AIG operations.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - May 11, 2006.]

  Revision    [$Id: aigOper.c,v 1.00 2006/05/11 00:00:00 alanmi Exp $]

***********************************************************************/

#include "aig.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

// procedure to detect an EXOR gate
static inline int Aig_ObjIsExorType( Aig_Obj_t * p0, Aig_Obj_t * p1, Aig_Obj_t ** ppFan0, Aig_Obj_t ** ppFan1 )
{
    if ( !Aig_IsComplement(p0) || !Aig_IsComplement(p1) )
        return 0;
    p0 = Aig_Regular(p0);
    p1 = Aig_Regular(p1);
    if ( !Aig_ObjIsAnd(p0) || !Aig_ObjIsAnd(p1) )
        return 0;
    if ( Aig_ObjFanin0(p0) != Aig_ObjFanin0(p1) || Aig_ObjFanin1(p0) != Aig_ObjFanin1(p1) )
        return 0;
    if ( Aig_ObjFaninC0(p0) == Aig_ObjFaninC0(p1) || Aig_ObjFaninC1(p0) == Aig_ObjFaninC1(p1) )
        return 0;
    *ppFan0 = Aig_ObjChild0(p0);
    *ppFan1 = Aig_ObjChild1(p0);
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Returns i-th elementary variable.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_IthVar( Aig_Man_t * p, int i )
{
    int v;
    for ( v = Aig_ManPiNum(p); v <= i; v++ )
        Aig_ObjCreatePi( p );
    assert( i < Vec_PtrSize(p->vPis) );
    return Aig_ManPi( p, i );
}

/**Function*************************************************************

  Synopsis    [Perform one operation.]

  Description [The argument nodes can be complemented.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_Oper( Aig_Man_t * p, Aig_Obj_t * p0, Aig_Obj_t * p1, Aig_Type_t Type )
{
    if ( Type == AIG_AND )
        return Aig_And( p, p0, p1 );
    if ( Type == AIG_EXOR )
        return Aig_Exor( p, p0, p1 );
    assert( 0 );
    return NULL;
}

/**Function*************************************************************

  Synopsis    [Performs canonicization step.]

  Description [The argument nodes can be complemented.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_And( Aig_Man_t * p, Aig_Obj_t * p0, Aig_Obj_t * p1 )
{
    Aig_Obj_t * pGhost, * pResult;
//    Aig_Obj_t * pFan0, * pFan1;
    // check trivial cases
    if ( p0 == p1 )
        return p0;
    if ( p0 == Aig_Not(p1) )
        return Aig_Not(p->pConst1);
    if ( Aig_Regular(p0) == p->pConst1 )
        return p0 == p->pConst1 ? p1 : Aig_Not(p->pConst1);
    if ( Aig_Regular(p1) == p->pConst1 )
        return p1 == p->pConst1 ? p0 : Aig_Not(p->pConst1);
    // check if it can be an EXOR gate
//    if ( Aig_ObjIsExorType( p0, p1, &pFan0, &pFan1 ) )
//        return Aig_Exor( p, pFan0, pFan1 );
    // check the table
    pGhost = Aig_ObjCreateGhost( p, p0, p1, AIG_AND );
    if ( pResult = Aig_TableLookup( p, pGhost ) )
        return pResult;
    return Aig_ObjCreate( p, pGhost );
}

/**Function*************************************************************

  Synopsis    [Performs canonicization step.]

  Description [The argument nodes can be complemented.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_Exor( Aig_Man_t * p, Aig_Obj_t * p0, Aig_Obj_t * p1 )
{
/*
    Aig_Obj_t * pGhost, * pResult;
    // check trivial cases
    if ( p0 == p1 )
        return Aig_Not(p->pConst1);
    if ( p0 == Aig_Not(p1) )
        return p->pConst1;
    if ( Aig_Regular(p0) == p->pConst1 )
        return Aig_NotCond( p1, p0 == p->pConst1 );
    if ( Aig_Regular(p1) == p->pConst1 )
        return Aig_NotCond( p0, p1 == p->pConst1 );
    // check the table
    pGhost = Aig_ObjCreateGhost( p, p0, p1, AIG_EXOR );
    if ( pResult = Aig_TableLookup( p, pGhost ) )
        return pResult;
    return Aig_ObjCreate( p, pGhost );
*/
    return Aig_Or( p, Aig_And(p, p0, Aig_Not(p1)), Aig_And(p, Aig_Not(p0), p1) );
}

/**Function*************************************************************

  Synopsis    [Implements Boolean OR.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_Or( Aig_Man_t * p, Aig_Obj_t * p0, Aig_Obj_t * p1 )
{
    return Aig_Not( Aig_And( p, Aig_Not(p0), Aig_Not(p1) ) );
}

/**Function*************************************************************

  Synopsis    [Implements ITE operation.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_Mux( Aig_Man_t * p, Aig_Obj_t * pC, Aig_Obj_t * p1, Aig_Obj_t * p0 )
{
    Aig_Obj_t * pTempA1, * pTempA2, * pTempB1, * pTempB2, * pTemp;
    int Count0, Count1;
    // consider trivial cases
    if ( p0 == Aig_Not(p1) )
        return Aig_Exor( p, pC, p0 );
    // other cases can be added
    // implement the first MUX (F = C * x1 + C' * x0)
    pTempA1 = Aig_TableLookup( p, Aig_ObjCreateGhost(p, pC,          p1, AIG_AND) );
    pTempA2 = Aig_TableLookup( p, Aig_ObjCreateGhost(p, Aig_Not(pC), p0, AIG_AND) );
    if ( pTempA1 && pTempA2 )
    {
        pTemp = Aig_TableLookup( p, Aig_ObjCreateGhost(p, Aig_Not(pTempA1), Aig_Not(pTempA2), AIG_AND) );
        if ( pTemp ) return Aig_Not(pTemp);
    }
    Count0 = (pTempA1 != NULL) + (pTempA2 != NULL);
    // implement the second MUX (F' = C * x1' + C' * x0')
    pTempB1 = Aig_TableLookup( p, Aig_ObjCreateGhost(p, pC,          Aig_Not(p1), AIG_AND) );
    pTempB2 = Aig_TableLookup( p, Aig_ObjCreateGhost(p, Aig_Not(pC), Aig_Not(p0), AIG_AND) );
    if ( pTempB1 && pTempB2 )
    {
        pTemp = Aig_TableLookup( p, Aig_ObjCreateGhost(p, Aig_Not(pTempB1), Aig_Not(pTempB2), AIG_AND) );
        if ( pTemp ) return pTemp;
    }
    Count1 = (pTempB1 != NULL) + (pTempB2 != NULL);
    // compare and decide which one to implement
    if ( Count0 >= Count1 )
    {
        pTempA1 = pTempA1? pTempA1 : Aig_And(p, pC,          p1);
        pTempA2 = pTempA2? pTempA2 : Aig_And(p, Aig_Not(pC), p0);
        return Aig_Or( p, pTempA1, pTempA2 );
    }
    pTempB1 = pTempB1? pTempB1 : Aig_And(p, pC,          Aig_Not(p1));
    pTempB2 = pTempB2? pTempB2 : Aig_And(p, Aig_Not(pC), Aig_Not(p0));
    return Aig_Not( Aig_Or( p, pTempB1, pTempB2 ) );

//    return Aig_Or( Aig_And(pC, p1), Aig_And(Aig_Not(pC), p0) );
}

/**Function*************************************************************

  Synopsis    [Implements ITE operation.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_Maj( Aig_Man_t * p, Aig_Obj_t * pA, Aig_Obj_t * pB, Aig_Obj_t * pC )
{
    return Aig_Or( p, Aig_Or(p, Aig_And(p, pA, pB), Aig_And(p, pA, pC)), Aig_And(p, pB, pC) );
}

/**Function*************************************************************

  Synopsis    [Constructs the well-balanced tree of gates.]

  Description [Disregards levels and possible logic sharing.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_Multi_rec( Aig_Man_t * p, Aig_Obj_t ** ppObjs, int nObjs, Aig_Type_t Type )
{
    Aig_Obj_t * pObj1, * pObj2;
    if ( nObjs == 1 )
        return ppObjs[0];
    pObj1 = Aig_Multi_rec( p, ppObjs,           nObjs/2,         Type );
    pObj2 = Aig_Multi_rec( p, ppObjs + nObjs/2, nObjs - nObjs/2, Type );
    return Aig_Oper( p, pObj1, pObj2, Type );
}

/**Function*************************************************************

  Synopsis    [Old code.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_Multi( Aig_Man_t * p, Aig_Obj_t ** pArgs, int nArgs, Aig_Type_t Type )
{
    assert( Type == AIG_AND || Type == AIG_EXOR );
    assert( nArgs > 0 );
    return Aig_Multi_rec( p, pArgs, nArgs, Type );
}

/**Function*************************************************************

  Synopsis    [Implements the miter.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_Miter( Aig_Man_t * p, Vec_Ptr_t * vPairs )
{
    int i;
    assert( vPairs->nSize > 0 );
    assert( vPairs->nSize % 2 == 0 );
    // go through the cubes of the node's SOP
    for ( i = 0; i < vPairs->nSize; i += 2 )
        vPairs->pArray[i/2] = Aig_Not( Aig_Exor( p, vPairs->pArray[i], vPairs->pArray[i+1] ) );
    vPairs->nSize = vPairs->nSize/2;
    return Aig_Not( Aig_Multi_rec( p, (Aig_Obj_t **)vPairs->pArray, vPairs->nSize, AIG_AND ) );
}

/**Function*************************************************************

  Synopsis    [Creates AND function with nVars inputs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_CreateAnd( Aig_Man_t * p, int nVars )
{
    Aig_Obj_t * pFunc;
    int i;
    pFunc = Aig_ManConst1( p );
    for ( i = 0; i < nVars; i++ )
        pFunc = Aig_And( p, pFunc, Aig_IthVar(p, i) );
    return pFunc;
}

/**Function*************************************************************

  Synopsis    [Creates AND function with nVars inputs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_CreateOr( Aig_Man_t * p, int nVars )
{
    Aig_Obj_t * pFunc;
    int i;
    pFunc = Aig_ManConst0( p );
    for ( i = 0; i < nVars; i++ )
        pFunc = Aig_Or( p, pFunc, Aig_IthVar(p, i) );
    return pFunc;
}

/**Function*************************************************************

  Synopsis    [Creates AND function with nVars inputs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_CreateExor( Aig_Man_t * p, int nVars )
{
    Aig_Obj_t * pFunc;
    int i;
    pFunc = Aig_ManConst0( p );
    for ( i = 0; i < nVars; i++ )
        pFunc = Aig_Exor( p, pFunc, Aig_IthVar(p, i) );
    return pFunc;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


