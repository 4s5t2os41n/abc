/**CFile****************************************************************

  FileName    [abcMiter.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Network and node package.]

  Synopsis    [Procedures to derive the miter of two circuits.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: abcMiter.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "abc.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static Abc_Ntk_t * Abc_NtkMiterInt( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int fComb, int nPartSize );
static void        Abc_NtkMiterPrepare( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkMiter, int fComb, int nPartSize );
static void        Abc_NtkMiterAddOne( Abc_Ntk_t * pNtk, Abc_Ntk_t * pNtkMiter );
static void        Abc_NtkMiterFinalize( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkMiter, int fComb, int nPartSize );
static void        Abc_NtkAddFrame( Abc_Ntk_t * pNetNew, Abc_Ntk_t * pNet, int iFrame );

// to be exported 
typedef void (*AddFrameMapping)( Abc_Obj_t*, Abc_Obj_t*, int, void*);
extern Abc_Ntk_t * Abc_NtkFrames2( Abc_Ntk_t * pNtk, int nFrames, int fInitial, AddFrameMapping addFrameMapping, void* arg );
static void        Abc_NtkAddFrame2( Abc_Ntk_t * pNtkFrames, Abc_Ntk_t * pNtk, int iFrame, Vec_Ptr_t * vNodes, AddFrameMapping addFrameMapping, void* arg );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Derives the miter of two networks.]

  Description [Preprocesses the networks to make sure that they are strashed.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkMiter( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int fComb, int nPartSize )
{
    Abc_Ntk_t * pTemp = NULL;
    int fRemove1, fRemove2;
    assert( Abc_NtkHasOnlyLatchBoxes(pNtk1) );
    assert( Abc_NtkHasOnlyLatchBoxes(pNtk2) );
    // check that the networks have the same PIs/POs/latches
    if ( !Abc_NtkCompareSignals( pNtk1, pNtk2, 0, fComb ) )
        return NULL;
    // make sure the circuits are strashed 
    fRemove1 = (!Abc_NtkIsStrash(pNtk1)) && (pNtk1 = Abc_NtkStrash(pNtk1, 0, 0, 0));
    fRemove2 = (!Abc_NtkIsStrash(pNtk2)) && (pNtk2 = Abc_NtkStrash(pNtk2, 0, 0, 0));
    if ( pNtk1 && pNtk2 )
        pTemp = Abc_NtkMiterInt( pNtk1, pNtk2, fComb, nPartSize );
    if ( fRemove1 )  Abc_NtkDelete( pNtk1 );
    if ( fRemove2 )  Abc_NtkDelete( pNtk2 );
    return pTemp;
}

/**Function*************************************************************

  Synopsis    [Derives the miter of two sequential networks.]

  Description [Assumes that the networks are strashed.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkMiterInt( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int fComb, int nPartSize )
{
    char Buffer[1000];
    Abc_Ntk_t * pNtkMiter;

    assert( Abc_NtkIsStrash(pNtk1) );
    assert( Abc_NtkIsStrash(pNtk2) );

    // start the new network
    pNtkMiter = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
    sprintf( Buffer, "%s_%s_miter", pNtk1->pName, pNtk2->pName );
    pNtkMiter->pName = Extra_UtilStrsav(Buffer);

    // perform strashing
    Abc_NtkMiterPrepare( pNtk1, pNtk2, pNtkMiter, fComb, nPartSize );
    Abc_NtkMiterAddOne( pNtk1, pNtkMiter );
    Abc_NtkMiterAddOne( pNtk2, pNtkMiter );
    Abc_NtkMiterFinalize( pNtk1, pNtk2, pNtkMiter, fComb, nPartSize );
    Abc_AigCleanup(pNtkMiter->pManFunc);

    // make sure that everything is okay
    if ( !Abc_NtkCheck( pNtkMiter ) )
    {
        printf( "Abc_NtkMiter: The network check has failed.\n" );
        Abc_NtkDelete( pNtkMiter );
        return NULL;
    }
    return pNtkMiter;
}

/**Function*************************************************************

  Synopsis    [Prepares the network for mitering.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkMiterPrepare( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkMiter, int fComb, int nPartSize )
{
    Abc_Obj_t * pObj, * pObjNew;
    int i;
    // clean the copy field in all objects
//    Abc_NtkCleanCopy( pNtk1 );
//    Abc_NtkCleanCopy( pNtk2 );
    Abc_AigConst1(pNtk1)->pCopy = Abc_AigConst1(pNtkMiter);
    Abc_AigConst1(pNtk2)->pCopy = Abc_AigConst1(pNtkMiter);

    if ( fComb )
    {
        // create new PIs and remember them in the old PIs
        Abc_NtkForEachCi( pNtk1, pObj, i )
        {
            pObjNew = Abc_NtkCreatePi( pNtkMiter );
            // remember this PI in the old PIs
            pObj->pCopy = pObjNew;
            pObj = Abc_NtkCi(pNtk2, i);  
            pObj->pCopy = pObjNew;
            // add name
            Abc_ObjAssignName( pObjNew, Abc_ObjName(pObj), NULL );
        }
        if ( nPartSize <= 0 )
        {
            // create the only PO
            pObjNew = Abc_NtkCreatePo( pNtkMiter );
            // add the PO name
            Abc_ObjAssignName( pObjNew, "miter", NULL );
        }
    }
    else
    {
        // create new PIs and remember them in the old PIs
        Abc_NtkForEachPi( pNtk1, pObj, i )
        {
            pObjNew = Abc_NtkCreatePi( pNtkMiter );
            // remember this PI in the old PIs
            pObj->pCopy = pObjNew;
            pObj = Abc_NtkPi(pNtk2, i);  
            pObj->pCopy = pObjNew;
            // add name
            Abc_ObjAssignName( pObjNew, Abc_ObjName(pObj), NULL );
        }
        if ( nPartSize <= 0 )
        {
            // create the only PO
            pObjNew = Abc_NtkCreatePo( pNtkMiter );
            // add the PO name
            Abc_ObjAssignName( pObjNew, "miter", NULL );
        }
        // create the latches
        Abc_NtkForEachLatch( pNtk1, pObj, i )
        {
            pObjNew = Abc_NtkDupBox( pNtkMiter, pObj, 0 );
            // add names
            Abc_ObjAssignName( pObjNew, Abc_ObjName(pObj), "_1" );
            Abc_ObjAssignName( Abc_ObjFanin0(pObjNew),  Abc_ObjName(Abc_ObjFanin0(pObj)), "_1" );
            Abc_ObjAssignName( Abc_ObjFanout0(pObjNew), Abc_ObjName(Abc_ObjFanout0(pObj)), "_1" );
        }
        Abc_NtkForEachLatch( pNtk2, pObj, i )
        {
            pObjNew = Abc_NtkDupBox( pNtkMiter, pObj, 0 );
            // add name
            Abc_ObjAssignName( pObjNew, Abc_ObjName(pObj), "_2" );
            Abc_ObjAssignName( Abc_ObjFanin0(pObjNew),  Abc_ObjName(Abc_ObjFanin0(pObj)), "_2" );
            Abc_ObjAssignName( Abc_ObjFanout0(pObjNew), Abc_ObjName(Abc_ObjFanout0(pObj)), "_2" );
        }
    }
}

/**Function*************************************************************

  Synopsis    [Performs mitering for one network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkMiterAddOne( Abc_Ntk_t * pNtk, Abc_Ntk_t * pNtkMiter )
{
    Abc_Obj_t * pNode;
    int i;
    assert( Abc_NtkIsDfsOrdered(pNtk) );
    Abc_AigForEachAnd( pNtk, pNode, i )
        pNode->pCopy = Abc_AigAnd( pNtkMiter->pManFunc, Abc_ObjChild0Copy(pNode), Abc_ObjChild1Copy(pNode) );
}

/**Function*************************************************************

  Synopsis    [Performs mitering for one network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkMiterAddCone( Abc_Ntk_t * pNtk, Abc_Ntk_t * pNtkMiter, Abc_Obj_t * pRoot )
{
    Vec_Ptr_t * vNodes;
    Abc_Obj_t * pNode;
    int i;
    // map the constant nodes
    Abc_AigConst1(pNtk)->pCopy = Abc_AigConst1(pNtkMiter);
    // perform strashing
    vNodes = Abc_NtkDfsNodes( pNtk, &pRoot, 1 );
    Vec_PtrForEachEntry( vNodes, pNode, i )
        if ( Abc_AigNodeIsAnd(pNode) )
            pNode->pCopy = Abc_AigAnd( pNtkMiter->pManFunc, Abc_ObjChild0Copy(pNode), Abc_ObjChild1Copy(pNode) );
    Vec_PtrFree( vNodes );
}


/**Function*************************************************************

  Synopsis    [Finalizes the miter by adding the output part.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkMiterFinalize( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Abc_Ntk_t * pNtkMiter, int fComb, int nPartSize )
{
    Vec_Ptr_t * vPairs;
    Abc_Obj_t * pMiter, * pNode;
    int i;
    // collect the PO pairs from both networks
    vPairs = Vec_PtrAlloc( 100 );
    if ( fComb )
    {
        // collect the CO nodes for the miter
        Abc_NtkForEachCo( pNtk1, pNode, i )
        {
            Vec_PtrPush( vPairs, Abc_ObjChild0Copy(pNode) );
            pNode = Abc_NtkCo( pNtk2, i );
            Vec_PtrPush( vPairs, Abc_ObjChild0Copy(pNode) );
        }
    }
    else
    {
        // collect the PO nodes for the miter
        Abc_NtkForEachPo( pNtk1, pNode, i )
        {
            Vec_PtrPush( vPairs, Abc_ObjChild0Copy(pNode) );
            pNode = Abc_NtkPo( pNtk2, i );
            Vec_PtrPush( vPairs, Abc_ObjChild0Copy(pNode) );
        }
        // connect new latches
        Abc_NtkForEachLatch( pNtk1, pNode, i )
            Abc_ObjAddFanin( Abc_ObjFanin0(pNode)->pCopy, Abc_ObjChild0Copy(Abc_ObjFanin0(pNode)) );
        Abc_NtkForEachLatch( pNtk2, pNode, i )
            Abc_ObjAddFanin( Abc_ObjFanin0(pNode)->pCopy, Abc_ObjChild0Copy(Abc_ObjFanin0(pNode)) );
    }
    // add the miter
    if ( nPartSize <= 0 )
    {
        pMiter = Abc_AigMiter( pNtkMiter->pManFunc, vPairs );
        Abc_ObjAddFanin( Abc_NtkPo(pNtkMiter,0), pMiter );
        Vec_PtrFree( vPairs );
    }
    else
    {
        char Buffer[1024];
        Vec_Ptr_t * vPairsPart;
        int nParts, i, k, iCur;
        assert( Vec_PtrSize(vPairs) == 2 * Abc_NtkCoNum(pNtk1) );
        // create partitions
        nParts = Abc_NtkCoNum(pNtk1) / nPartSize + (int)((Abc_NtkCoNum(pNtk1) % nPartSize) > 0);
        vPairsPart = Vec_PtrAlloc( nPartSize );
        for ( i = 0; i < nParts; i++ )
        {
            Vec_PtrClear( vPairsPart );
            for ( k = 0; k < nPartSize; k++ )
            {
                iCur = i * nPartSize + k;
                if ( iCur >= Abc_NtkCoNum(pNtk1) )
                    break;
                Vec_PtrPush( vPairsPart, Vec_PtrEntry(vPairs, 2*iCur  ) );
                Vec_PtrPush( vPairsPart, Vec_PtrEntry(vPairs, 2*iCur+1) );
            }
            pMiter = Abc_AigMiter( pNtkMiter->pManFunc, vPairsPart );
            pNode = Abc_NtkCreatePo( pNtkMiter );
            Abc_ObjAddFanin( pNode, pMiter );
            // assign the name to the node
            if ( nPartSize == 1 )
                sprintf( Buffer, "%s", Abc_ObjName(Abc_NtkCo(pNtk1,i)) );
            else
                sprintf( Buffer, "%d", i );
            Abc_ObjAssignName( pNode, "miter_", Buffer );
        }
        Vec_PtrFree( vPairsPart );
        Vec_PtrFree( vPairs );
    }
}



/**Function*************************************************************

  Synopsis    [Derives the AND of two miters.]

  Description [The network should have the same names of PIs.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkMiterAnd( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int fOr, int fCompl2 )
{
    char Buffer[1000];
    Abc_Ntk_t * pNtkMiter;
    Abc_Obj_t * pOutput1, * pOutput2;
    Abc_Obj_t * pRoot1, * pRoot2, * pMiter;

    assert( Abc_NtkIsStrash(pNtk1) );
    assert( Abc_NtkIsStrash(pNtk2) );
    assert( 1 == Abc_NtkCoNum(pNtk1) );
    assert( 1 == Abc_NtkCoNum(pNtk2) );
    assert( 0 == Abc_NtkLatchNum(pNtk1) );
    assert( 0 == Abc_NtkLatchNum(pNtk2) );
    assert( Abc_NtkCiNum(pNtk1) == Abc_NtkCiNum(pNtk2) );
    assert( Abc_NtkHasOnlyLatchBoxes(pNtk1) );
    assert( Abc_NtkHasOnlyLatchBoxes(pNtk2) );

    // start the new network
    pNtkMiter = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
//    sprintf( Buffer, "%s_%s_miter", pNtk1->pName, pNtk2->pName );
    sprintf( Buffer, "product" );
    pNtkMiter->pName = Extra_UtilStrsav(Buffer);

    // perform strashing
    Abc_NtkMiterPrepare( pNtk1, pNtk2, pNtkMiter, 1, -1 );
    Abc_NtkMiterAddOne( pNtk1, pNtkMiter );
    Abc_NtkMiterAddOne( pNtk2, pNtkMiter );
//    Abc_NtkMiterFinalize( pNtk1, pNtk2, pNtkMiter, 1 );
    pRoot1 = Abc_NtkPo(pNtk1,0);
    pRoot2 = Abc_NtkPo(pNtk2,0);
    pOutput1 = Abc_ObjNotCond( Abc_ObjFanin0(pRoot1)->pCopy, Abc_ObjFaninC0(pRoot1) );
    pOutput2 = Abc_ObjNotCond( Abc_ObjFanin0(pRoot2)->pCopy, Abc_ObjFaninC0(pRoot2) ^ fCompl2 );
    
    // create the miter of the two outputs
    if ( fOr )
        pMiter = Abc_AigOr( pNtkMiter->pManFunc, pOutput1, pOutput2 );
    else
        pMiter = Abc_AigAnd( pNtkMiter->pManFunc, pOutput1, pOutput2 );
    Abc_ObjAddFanin( Abc_NtkPo(pNtkMiter,0), pMiter );

    // make sure that everything is okay
    if ( !Abc_NtkCheck( pNtkMiter ) )
    {
        printf( "Abc_NtkMiterAnd: The network check has failed.\n" );
        Abc_NtkDelete( pNtkMiter );
        return NULL;
    }
    return pNtkMiter;
}


/**Function*************************************************************

  Synopsis    [Derives the cofactor of the miter w.r.t. the set of vars.]

  Description [The array of variable values contains -1/0/1 for each PI.
  -1 means this PI remains, 0/1 means this PI is set to 0/1.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkMiterCofactor( Abc_Ntk_t * pNtk, Vec_Int_t * vPiValues )
{
    char Buffer[1000];
    Abc_Ntk_t * pNtkMiter;
    Abc_Obj_t * pRoot, * pOutput1;
    int Value, i;

    assert( Abc_NtkIsStrash(pNtk) );
    assert( 1 == Abc_NtkCoNum(pNtk) );
    assert( Abc_NtkHasOnlyLatchBoxes(pNtk) );

    // start the new network
    pNtkMiter = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
    sprintf( Buffer, "%s_miter", pNtk->pName );
    pNtkMiter->pName = Extra_UtilStrsav(Buffer);

    // get the root output
    pRoot = Abc_NtkCo( pNtk, 0 );

    // perform strashing
    Abc_NtkMiterPrepare( pNtk, pNtk, pNtkMiter, 1, -1 );
    // set the first cofactor
    Vec_IntForEachEntry( vPiValues, Value, i )
    {
        if ( Value == -1 )
            continue;
        if ( Value == 0 )
        {
            Abc_NtkCi(pNtk, i)->pCopy = Abc_ObjNot( Abc_AigConst1(pNtkMiter) );
            continue;
        }
        if ( Value == 1 )
        {
            Abc_NtkCi(pNtk, i)->pCopy = Abc_AigConst1(pNtkMiter);
            continue;
        }
        assert( 0 );
    }
    // add the first cofactor
    Abc_NtkMiterAddCone( pNtk, pNtkMiter, pRoot );

    // save the output
    pOutput1 = Abc_ObjNotCond( Abc_ObjFanin0(pRoot)->pCopy, Abc_ObjFaninC0(pRoot) );

    // create the miter of the two outputs
    Abc_ObjAddFanin( Abc_NtkPo(pNtkMiter,0), pOutput1 );

    // make sure that everything is okay
    if ( !Abc_NtkCheck( pNtkMiter ) )
    {
        printf( "Abc_NtkMiterCofactor: The network check has failed.\n" );
        Abc_NtkDelete( pNtkMiter );
        return NULL;
    }
    return pNtkMiter;
}
/**Function*************************************************************

  Synopsis    [Derives the miter of two cofactors of one output.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkMiterForCofactors( Abc_Ntk_t * pNtk, int Out, int In1, int In2 )
{
    char Buffer[1000];
    Abc_Ntk_t * pNtkMiter;
    Abc_Obj_t * pRoot, * pOutput1, * pOutput2, * pMiter;

    assert( Abc_NtkIsStrash(pNtk) );
    assert( Out < Abc_NtkCoNum(pNtk) );
    assert( In1 < Abc_NtkCiNum(pNtk) );
    assert( In2 < Abc_NtkCiNum(pNtk) );
    assert( Abc_NtkHasOnlyLatchBoxes(pNtk) );

    // start the new network
    pNtkMiter = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
    sprintf( Buffer, "%s_miter", Abc_ObjName(Abc_NtkCo(pNtk, Out)) );
    pNtkMiter->pName = Extra_UtilStrsav(Buffer);

    // get the root output
    pRoot = Abc_NtkCo( pNtk, Out );

    // perform strashing
    Abc_NtkMiterPrepare( pNtk, pNtk, pNtkMiter, 1, -1 );
    // set the first cofactor
    Abc_NtkCi(pNtk, In1)->pCopy = Abc_ObjNot( Abc_AigConst1(pNtkMiter) );
    if ( In2 >= 0 )
    Abc_NtkCi(pNtk, In2)->pCopy = Abc_AigConst1(pNtkMiter);
    // add the first cofactor
    Abc_NtkMiterAddCone( pNtk, pNtkMiter, pRoot );

    // save the output
    pOutput1 = Abc_ObjFanin0(pRoot)->pCopy;

    // set the second cofactor
    Abc_NtkCi(pNtk, In1)->pCopy = Abc_AigConst1(pNtkMiter);
    if ( In2 >= 0 )
    Abc_NtkCi(pNtk, In2)->pCopy = Abc_ObjNot( Abc_AigConst1(pNtkMiter) );
    // add the second cofactor
    Abc_NtkMiterAddCone( pNtk, pNtkMiter, pRoot );

    // save the output
    pOutput2 = Abc_ObjFanin0(pRoot)->pCopy;

    // create the miter of the two outputs
    pMiter = Abc_AigXor( pNtkMiter->pManFunc, pOutput1, pOutput2 );
    Abc_ObjAddFanin( Abc_NtkPo(pNtkMiter,0), pMiter );

    // make sure that everything is okay
    if ( !Abc_NtkCheck( pNtkMiter ) )
    {
        printf( "Abc_NtkMiter: The network check has failed.\n" );
        Abc_NtkDelete( pNtkMiter );
        return NULL;
    }
    return pNtkMiter;
}


/**Function*************************************************************

  Synopsis    [Derives the miter of two cofactors of one output.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkMiterQuantify( Abc_Ntk_t * pNtk, int In, int fExist )
{
    Abc_Ntk_t * pNtkMiter;
    Abc_Obj_t * pRoot, * pOutput1, * pOutput2, * pMiter;

    assert( Abc_NtkIsStrash(pNtk) );
    assert( 1 == Abc_NtkCoNum(pNtk) );
    assert( In < Abc_NtkCiNum(pNtk) );
    assert( Abc_NtkHasOnlyLatchBoxes(pNtk) );

    // start the new network
    pNtkMiter = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
    pNtkMiter->pName = Extra_UtilStrsav( Abc_ObjName(Abc_NtkCo(pNtk, 0)) );

    // get the root output
    pRoot = Abc_NtkCo( pNtk, 0 );

    // perform strashing
    Abc_NtkMiterPrepare( pNtk, pNtk, pNtkMiter, 1, -1 );
    // set the first cofactor
    Abc_NtkCi(pNtk, In)->pCopy = Abc_ObjNot( Abc_AigConst1(pNtkMiter) );
    // add the first cofactor
    Abc_NtkMiterAddCone( pNtk, pNtkMiter, pRoot );
    // save the output
//    pOutput1 = Abc_ObjFanin0(pRoot)->pCopy;
    pOutput1 = Abc_ObjNotCond( Abc_ObjFanin0(pRoot)->pCopy, Abc_ObjFaninC0(pRoot) );

    // set the second cofactor
    Abc_NtkCi(pNtk, In)->pCopy = Abc_AigConst1(pNtkMiter);
    // add the second cofactor
    Abc_NtkMiterAddCone( pNtk, pNtkMiter, pRoot );
    // save the output
//    pOutput2 = Abc_ObjFanin0(pRoot)->pCopy;
    pOutput2 = Abc_ObjNotCond( Abc_ObjFanin0(pRoot)->pCopy, Abc_ObjFaninC0(pRoot) );

    // create the miter of the two outputs
    if ( fExist ) 
        pMiter = Abc_AigOr( pNtkMiter->pManFunc, pOutput1, pOutput2 );
    else
        pMiter = Abc_AigAnd( pNtkMiter->pManFunc, pOutput1, pOutput2 );
    Abc_ObjAddFanin( Abc_NtkPo(pNtkMiter,0), pMiter );

    // make sure that everything is okay
    if ( !Abc_NtkCheck( pNtkMiter ) )
    {
        printf( "Abc_NtkMiter: The network check has failed.\n" );
        Abc_NtkDelete( pNtkMiter );
        return NULL;
    }
    return pNtkMiter;
}

/**Function*************************************************************

  Synopsis    [Quantifies all the PIs existentially from the only PO of the network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkMiterQuantifyPis( Abc_Ntk_t * pNtk )
{
    Abc_Ntk_t * pNtkTemp;
    Abc_Obj_t * pObj;
    int i;
    assert( Abc_NtkHasOnlyLatchBoxes(pNtk) );

    Abc_NtkForEachPi( pNtk, pObj, i )
    {
        if ( Abc_ObjFanoutNum(pObj) == 0 )
            continue;
        pNtk = Abc_NtkMiterQuantify( pNtkTemp = pNtk, i, 1 );
        Abc_NtkDelete( pNtkTemp );
    }

    return pNtk;
}




/**Function*************************************************************

  Synopsis    [Checks the status of the miter.]

  Description [Return 0 if the miter is sat for at least one output.
  Return 1 if the miter is unsat for all its outputs. Returns -1 if the
  miter is undecided for some outputs.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_NtkMiterIsConstant( Abc_Ntk_t * pMiter )
{
    Abc_Obj_t * pNodePo, * pChild;
    int i;
    assert( Abc_NtkIsStrash(pMiter) );
    Abc_NtkForEachPo( pMiter, pNodePo, i )
    {
        pChild = Abc_ObjChild0( pNodePo );
        // check if the output is constant 1
        if ( Abc_AigNodeIsConst(pChild) )
        {
            assert( Abc_ObjRegular(pChild) == Abc_AigConst1(pMiter) );
            if ( !Abc_ObjIsComplement(pChild) )
            {
                // if the miter is constant 1, return immediately
//                printf( "MITER IS CONSTANT 1!\n" );
                return 0;
            }
        }
/*
        // check if the output is not constant 0
        else if ( Abc_ObjRegular(pChild)->fPhase != (unsigned)Abc_ObjIsComplement(pChild) )
        {
            return 0;
        }
*/
        // if the miter is undecided (or satisfiable), return immediately
        else 
            return -1;
    }
    // return 1, meaning all outputs are constant zero
    return 1;
}

/**Function*************************************************************

  Synopsis    [Reports the status of the miter.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkMiterReport( Abc_Ntk_t * pMiter )
{
    Abc_Obj_t * pChild, * pNode;
    int i;
    if ( Abc_NtkPoNum(pMiter) == 1 )
    {
        pChild = Abc_ObjChild0( Abc_NtkPo(pMiter,0) );
        if ( Abc_AigNodeIsConst(pChild) )
        {
            if ( Abc_ObjIsComplement(pChild) )
                printf( "Unsatisfiable.\n" );
            else
                printf( "Satisfiable. (Constant 1).\n" );
        }
        else
            printf( "Satisfiable.\n" );
    }
    else
    {
        Abc_NtkForEachPo( pMiter, pNode, i )
        {
            pChild = Abc_ObjChild0( Abc_NtkPo(pMiter,i) );
            printf( "Output #%2d : ", i );
            if ( Abc_AigNodeIsConst(pChild) )
            {
                if ( Abc_ObjIsComplement(pChild) )
                    printf( "Unsatisfiable.\n" );
                else
                    printf( "Satisfiable. (Constant 1).\n" );
            }
            else
                printf( "Satisfiable.\n" );
        }
    }
}


/**Function*************************************************************

  Synopsis    [Derives the timeframes of the network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkFrames( Abc_Ntk_t * pNtk, int nFrames, int fInitial )
{
    char Buffer[1000];
    ProgressBar * pProgress;
    Abc_Ntk_t * pNtkFrames;
    Abc_Obj_t * pLatch, * pLatchOut;
    int i, Counter;
    assert( nFrames > 0 );
    assert( Abc_NtkIsStrash(pNtk) );
    assert( Abc_NtkIsDfsOrdered(pNtk) );
    assert( Abc_NtkHasOnlyLatchBoxes(pNtk) );
    // start the new network
    pNtkFrames = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
    sprintf( Buffer, "%s_%d_frames", pNtk->pName, nFrames );
    pNtkFrames->pName = Extra_UtilStrsav(Buffer);
    // map the constant nodes
    Abc_AigConst1(pNtk)->pCopy = Abc_AigConst1(pNtkFrames);
    // create new latches (or their initial values) and remember them in the new latches
    if ( !fInitial )
    {
        Abc_NtkForEachLatch( pNtk, pLatch, i )
            Abc_NtkDupBox( pNtkFrames, pLatch, 1 );
    }
    else
    {
        Counter = 0;
        Abc_NtkForEachLatch( pNtk, pLatch, i )
        {
            pLatchOut = Abc_ObjFanout0(pLatch);
            if ( Abc_LatchIsInitNone(pLatch) || Abc_LatchIsInitDc(pLatch) ) // don't-care initial value - create a new PI
            {
                pLatchOut->pCopy = Abc_NtkCreatePi(pNtkFrames);
                Abc_ObjAssignName( pLatchOut->pCopy, Abc_ObjName(pLatchOut), NULL );
                Counter++;
            }
            else
                pLatchOut->pCopy = Abc_ObjNotCond( Abc_AigConst1(pNtkFrames), Abc_LatchIsInit0(pLatch) );
        }
        if ( Counter )
            printf( "Warning: %d uninitialized latches are replaced by free PI variables.\n", Counter );
    }
    
    // create the timeframes
    pProgress = Extra_ProgressBarStart( stdout, nFrames );
    for ( i = 0; i < nFrames; i++ )
    {
        Extra_ProgressBarUpdate( pProgress, i, NULL );
        Abc_NtkAddFrame( pNtkFrames, pNtk, i );
    }
    Extra_ProgressBarStop( pProgress );
    
    // connect the new latches to the outputs of the last frame
    if ( !fInitial )
    {
        // we cannot use pLatch->pCopy here because pLatch->pCopy is used for temporary storage of strashed values
        Abc_NtkForEachLatch( pNtk, pLatch, i )
            Abc_ObjAddFanin( Abc_ObjFanin0(pLatch)->pCopy, Abc_ObjFanout0(pLatch)->pCopy );
    }

    // remove dangling nodes
    Abc_AigCleanup( pNtkFrames->pManFunc );
    // reorder the latches
    Abc_NtkOrderCisCos( pNtkFrames );
    // make sure that everything is okay
    if ( !Abc_NtkCheck( pNtkFrames ) )
    {
        printf( "Abc_NtkFrames: The network check has failed.\n" );
        Abc_NtkDelete( pNtkFrames );
        return NULL;
    }
    return pNtkFrames;
}

/**Function*************************************************************

  Synopsis    [Adds one time frame to the new network.]

  Description [Assumes that the latches of the old network point
  to the outputs of the previous frame of the new network (pLatch->pCopy). 
  In the end, updates the latches of the old network to point to the 
  outputs of the current frame of the new network.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkAddFrame( Abc_Ntk_t * pNtkFrames, Abc_Ntk_t * pNtk, int iFrame )
{
    char Buffer[10];
    Abc_Obj_t * pNode, * pLatch;
    int i;
    // create the prefix to be added to the node names
    sprintf( Buffer, "_%02d", iFrame );
    // add the new PI nodes
    Abc_NtkForEachPi( pNtk, pNode, i )
        Abc_ObjAssignName( Abc_NtkDupObj(pNtkFrames, pNode, 0), Abc_ObjName(pNode), Buffer );
    // add the internal nodes
    Abc_AigForEachAnd( pNtk, pNode, i )
        pNode->pCopy = Abc_AigAnd( pNtkFrames->pManFunc, Abc_ObjChild0Copy(pNode), Abc_ObjChild1Copy(pNode) );
    // add the new POs
    Abc_NtkForEachPo( pNtk, pNode, i )
    {
        Abc_ObjAssignName( Abc_NtkDupObj(pNtkFrames, pNode, 0), Abc_ObjName(pNode), Buffer );
        Abc_ObjAddFanin( pNode->pCopy, Abc_ObjChild0Copy(pNode) );
    }
    // add the new asserts
    Abc_NtkForEachAssert( pNtk, pNode, i )
    {
        Abc_ObjAssignName( Abc_NtkDupObj(pNtkFrames, pNode, 0), Abc_ObjName(pNode), Buffer );
        Abc_ObjAddFanin( pNode->pCopy, Abc_ObjChild0Copy(pNode) );
    }
    // transfer the implementation of the latch inputs to the latch outputs
    Abc_NtkForEachLatch( pNtk, pLatch, i )
        pLatch->pCopy = Abc_ObjChild0Copy(Abc_ObjFanin0(pLatch));
    Abc_NtkForEachLatch( pNtk, pLatch, i )
        Abc_ObjFanout0(pLatch)->pCopy = pLatch->pCopy;
}



/**Function*************************************************************

  Synopsis    [Derives the timeframes of the network.]

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Ntk_t * Abc_NtkFrames2( Abc_Ntk_t * pNtk, int nFrames, int fInitial, AddFrameMapping addFrameMapping, void* arg )
{
/*
    char Buffer[1000];
    ProgressBar * pProgress;
    Abc_Ntk_t * pNtkFrames;
    Vec_Ptr_t * vNodes;
    Abc_Obj_t * pLatch, * pLatchNew;
    int i, Counter;
    assert( nFrames > 0 );
    assert( Abc_NtkIsStrash(pNtk) );   
    // start the new network
    pNtkFrames = Abc_NtkAlloc( ABC_NTK_STRASH, ABC_FUNC_AIG, 1 );
    sprintf( Buffer, "%s_%d_frames", pNtk->pName, nFrames );
    pNtkFrames->pName = Extra_UtilStrsav(Buffer);
    // create new latches (or their initial values) and remember them in the new latches
    if ( !fInitial )
    {
        Abc_NtkForEachLatch( pNtk, pLatch, i ) {
            Abc_NtkDupObj( pNtkFrames, pLatch );
            if (addFrameMapping) addFrameMapping(pLatch->pCopy, pLatch, 0, arg);
        }
    }
    else
    {
        Counter = 0;
        Abc_NtkForEachLatch( pNtk, pLatch, i )
        {
            if ( Abc_LatchIsInitDc(pLatch) ) // don't-care initial value - create a new PI
            {
                pLatch->pCopy = Abc_NtkCreatePi(pNtkFrames);
                Abc_ObjAssignName( pLatch->pCopy, Abc_ObjName(pLatch), NULL );
                Counter++;
            }
            else {
                pLatch->pCopy = Abc_ObjNotCond( Abc_AigConst1(pNtkFrames), Abc_LatchIsInit0(pLatch) );
            }
 
            if (addFrameMapping) addFrameMapping(pLatch->pCopy, pLatch, 0, arg);
        }
        if ( Counter )
            printf( "Warning: %d uninitialized latches are replaced by free PI variables.\n", Counter );
    }
    
    // create the timeframes
    vNodes = Abc_NtkDfs( pNtk, 0 );
    pProgress = Extra_ProgressBarStart( stdout, nFrames );
    for ( i = 0; i < nFrames; i++ )
    {
        Extra_ProgressBarUpdate( pProgress, i, NULL );
        Abc_NtkAddFrame2( pNtkFrames, pNtk, i, vNodes, addFrameMapping, arg );
    }
    Extra_ProgressBarStop( pProgress );
    Vec_PtrFree( vNodes );
    
    // connect the new latches to the outputs of the last frame
    if ( !fInitial )
    {
        Abc_NtkForEachLatch( pNtk, pLatch, i )
        {
            pLatchNew = Abc_NtkBox(pNtkFrames, i);
            Abc_ObjAddFanin( pLatchNew, pLatch->pCopy );
            Abc_ObjAssignName( pLatchNew, Abc_ObjName(pLatch), NULL );
        }
    }
    Abc_NtkForEachLatch( pNtk, pLatch, i )
        pLatch->pNext = NULL;

    // remove dangling nodes
    Abc_AigCleanup( pNtkFrames->pManFunc );

    // reorder the latches
    Abc_NtkOrderCisCos( pNtkFrames );
    
    // make sure that everything is okay
    if ( !Abc_NtkCheck( pNtkFrames ) )
    {
        printf( "Abc_NtkFrames: The network check has failed.\n" );
        Abc_NtkDelete( pNtkFrames );
        return NULL;
    }
    return pNtkFrames;
*/
    return NULL;
}

/**Function*************************************************************

  Synopsis    [Adds one time frame to the new network.]

  Description [Assumes that the latches of the old network point
  to the outputs of the previous frame of the new network (pLatch->pCopy). 
  In the end, updates the latches of the old network to point to the 
  outputs of the current frame of the new network.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_NtkAddFrame2( Abc_Ntk_t * pNtkFrames, Abc_Ntk_t * pNtk, int iFrame, Vec_Ptr_t * vNodes, AddFrameMapping addFrameMapping, void* arg )
{
/*
    char Buffer[10];
    Abc_Obj_t * pNode, * pNodeNew, * pLatch;
    Abc_Obj_t * pConst1, * pConst1New;
    int i;
    // get the constant nodes
    pConst1    = Abc_AigConst1(pNtk);
    pConst1New = Abc_AigConst1(pNtkFrames);
    // create the prefix to be added to the node names
    sprintf( Buffer, "_%02d", iFrame );
    // add the new PI nodes
    Abc_NtkForEachPi( pNtk, pNode, i )
    {
        pNodeNew = Abc_NtkDupObj( pNtkFrames, pNode );       
        Abc_ObjAssignName( pNodeNew, Abc_ObjName(pNode), Buffer );
        if (addFrameMapping) addFrameMapping(pNodeNew, pNode, iFrame, arg);
    }
    // add the internal nodes
    Vec_PtrForEachEntry( vNodes, pNode, i )
    {
        if ( pNode == pConst1 )
            pNodeNew = pConst1New;
        else
            pNodeNew = Abc_AigAnd( pNtkFrames->pManFunc, Abc_ObjChild0Copy(pNode), Abc_ObjChild1Copy(pNode) );
        pNode->pCopy = pNodeNew;
        if (addFrameMapping) addFrameMapping(pNodeNew, pNode, iFrame, arg);
    }
    // add the new POs
    Abc_NtkForEachPo( pNtk, pNode, i )
    {
        pNodeNew = Abc_NtkDupObj( pNtkFrames, pNode );       
        Abc_ObjAddFanin( pNodeNew, Abc_ObjChild0Copy(pNode) );
        Abc_ObjAssignName( pNodeNew, Abc_ObjName(pNode), Buffer );
        if (addFrameMapping) addFrameMapping(pNodeNew, pNode, iFrame, arg);
    }
    // transfer the implementation of the latch drivers to the latches

    // it is important that these two steps are performed it two loops
    // and not in the same loop
    Abc_NtkForEachLatch( pNtk, pLatch, i ) 
        pLatch->pNext = Abc_ObjChild0Copy(pLatch);
    Abc_NtkForEachLatch( pNtk, pLatch, i ) 
        pLatch->pCopy = pLatch->pNext;

    Abc_NtkForEachLatch( pNtk, pLatch, i ) 
    {
        if (addFrameMapping) {
            // don't give Mike complemented pointers because he doesn't like it
            if (Abc_ObjIsComplement(pLatch->pCopy)) {            
                pNodeNew = Abc_NtkCreateNode( pNtkFrames );
                Abc_ObjAddFanin( pNodeNew, pLatch->pCopy );
                assert(Abc_ObjFaninNum(pNodeNew) == 1);
                pNodeNew->Level = 1 + Abc_ObjRegular(pLatch->pCopy)->Level;

                pLatch->pNext = pNodeNew;
                pLatch->pCopy = pNodeNew;
            }
            addFrameMapping(pLatch->pCopy, pLatch, iFrame+1, arg);
        }
    }
*/
}



/**Function*************************************************************

  Synopsis    [Splits the miter into two logic cones combined by an EXOR]

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_NtkDemiter( Abc_Ntk_t * pNtk )
{
    Abc_Obj_t * pNodeC, * pNodeA, * pNodeB, * pNode;
    Abc_Obj_t * pPoNew;
    Vec_Ptr_t * vNodes1, * vNodes2;
    int nCommon, i;

    assert( Abc_NtkIsStrash(pNtk) );
    assert( Abc_NtkPoNum(pNtk) == 1 );
    if ( !Abc_NodeIsExorType(Abc_ObjFanin0(Abc_NtkPo(pNtk,0))) )
    {
        printf( "The root of the miter is not an EXOR gate.\n" );
        return 0;
    }
    pNodeC = Abc_NodeRecognizeMux( Abc_ObjFanin0(Abc_NtkPo(pNtk,0)), &pNodeA, &pNodeB );
    assert( Abc_ObjRegular(pNodeA) == Abc_ObjRegular(pNodeB) );
    if ( Abc_ObjFaninC0(Abc_NtkPo(pNtk,0)) )
    {
        pNodeA = Abc_ObjNot(pNodeA);
        pNodeB = Abc_ObjNot(pNodeB);
    }

    // add the PO corresponding to control input
    pPoNew = Abc_NtkCreatePo( pNtk );
    Abc_ObjAddFanin( pPoNew, pNodeC );
    Abc_ObjAssignName( pPoNew, "addOut1", NULL );

    // add the PO corresponding to other input
    pPoNew = Abc_NtkCreatePo( pNtk );
    Abc_ObjAddFanin( pPoNew, pNodeB );
    Abc_ObjAssignName( pPoNew, "addOut2", NULL );

    // mark the nodes in the first cone
    pNodeB = Abc_ObjRegular(pNodeB);
    vNodes1 = Abc_NtkDfsNodes( pNtk, &pNodeC, 1 );
    vNodes2 = Abc_NtkDfsNodes( pNtk, &pNodeB, 1 );

    Vec_PtrForEachEntry( vNodes1, pNode, i )
        pNode->fMarkA = 1;
    nCommon = 0;
    Vec_PtrForEachEntry( vNodes2, pNode, i )
        nCommon += pNode->fMarkA;
    Vec_PtrForEachEntry( vNodes1, pNode, i )
        pNode->fMarkA = 0;

    printf( "First cone = %6d.  Second cone = %6d.  Common = %6d.\n", vNodes1->nSize, vNodes2->nSize, nCommon );
    Vec_PtrFree( vNodes1 );
    Vec_PtrFree( vNodes2 );

    // reorder the latches
    Abc_NtkOrderCisCos( pNtk );
    // make sure that everything is okay
    if ( !Abc_NtkCheck( pNtk ) )
        printf( "Abc_NtkDemiter: The network check has failed.\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    [Computes OR or AND of the POs.]

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_NtkCombinePos( Abc_Ntk_t * pNtk, int fAnd )
{
    Abc_Obj_t * pNode, * pMiter;
    int i;
    assert( Abc_NtkIsStrash(pNtk) );
//    assert( Abc_NtkLatchNum(pNtk) == 0 );
    if ( Abc_NtkPoNum(pNtk) == 1 )
        return 1;
    // start the result
    if ( fAnd )
        pMiter = Abc_AigConst1(pNtk);
    else
        pMiter = Abc_ObjNot( Abc_AigConst1(pNtk) );
    // perform operations on the POs
    Abc_NtkForEachPo( pNtk, pNode, i )
        if ( fAnd )
            pMiter = Abc_AigAnd( pNtk->pManFunc, pMiter, Abc_ObjChild0(pNode) );
        else
            pMiter = Abc_AigOr( pNtk->pManFunc, pMiter, Abc_ObjChild0(pNode) );
    // remove the POs and their names
    for ( i = Abc_NtkPoNum(pNtk) - 1; i >= 0; i-- )
        Abc_NtkDeleteObj( Abc_NtkPo(pNtk, i) );
    assert( Abc_NtkPoNum(pNtk) == 0 );
    // create the new PO
    pNode = Abc_NtkCreatePo( pNtk );
    Abc_ObjAddFanin( pNode, pMiter );
    Abc_ObjAssignName( pNode, "miter", NULL );
    // make sure that everything is okay
    if ( !Abc_NtkCheck( pNtk ) )
    {
        printf( "Abc_NtkOrPos: The network check has failed.\n" );
        return 0;
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


