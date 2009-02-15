/**CFile****************************************************************

  FileName    [aigMan.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [AIG package.]

  Synopsis    [AIG manager.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - April 28, 2007.]

  Revision    [$Id: aigMan.c,v 1.00 2007/04/28 00:00:00 alanmi Exp $]

***********************************************************************/

#include "aig.h"
#include "tim.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Starts the AIG manager.]

  Description [The argument of this procedure is a soft limit on the
  the number of nodes, or 0 if the limit is unknown.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Man_t * Aig_ManStart( int nNodesMax )
{
    Aig_Man_t * p;
    if ( nNodesMax <= 0 )
        nNodesMax = 10007;
    // start the manager
    p = ABC_ALLOC( Aig_Man_t, 1 );
    memset( p, 0, sizeof(Aig_Man_t) );
    // perform initializations
    p->nTravIds = 1;
    p->fCatchExor = 0;
    // allocate arrays for nodes
    p->vPis  = Vec_PtrAlloc( 100 );
    p->vPos  = Vec_PtrAlloc( 100 );
    p->vObjs = Vec_PtrAlloc( 1000 );
    p->vBufs = Vec_PtrAlloc( 100 );
    // prepare the internal memory manager
    p->pMemObjs = Aig_MmFixedStart( sizeof(Aig_Obj_t), nNodesMax );
    // create the constant node
    p->pConst1 = Aig_ManFetchMemory( p );
    p->pConst1->Type = AIG_OBJ_CONST1;
    p->pConst1->fPhase = 1;
    p->nObjs[AIG_OBJ_CONST1]++;
    // start the table
    p->nTableSize = Aig_PrimeCudd( nNodesMax );
    p->pTable = ABC_ALLOC( Aig_Obj_t *, p->nTableSize );
    memset( p->pTable, 0, sizeof(Aig_Obj_t *) * p->nTableSize );
    return p;
}

/**Function*************************************************************

  Synopsis    [Duplicates the AIG manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Man_t * Aig_ManStartFrom( Aig_Man_t * p )
{
    Aig_Man_t * pNew;
    Aig_Obj_t * pObj, * pObjNew;
    int i;
    // create the new manager
    pNew = Aig_ManStart( Aig_ManObjNumMax(p) );
    pNew->pName = Aig_UtilStrsav( p->pName );
    pNew->pSpec = Aig_UtilStrsav( p->pSpec );
    // create the PIs
    Aig_ManConst1(p)->pData = Aig_ManConst1(pNew);
    Aig_ManForEachPi( p, pObj, i )
    {
        pObjNew = Aig_ObjCreatePi( pNew );
        pObjNew->Level = pObj->Level;
        pObjNew->pHaig = pObj->pHaig;
        pObj->pData = pObjNew;
    }
    return pNew;
}

/**Function*************************************************************

  Synopsis    [Duplicates the AIG manager recursively.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Aig_ManDup_rec( Aig_Man_t * pNew, Aig_Man_t * p, Aig_Obj_t * pObj )
{
    Aig_Obj_t * pObjNew;
    if ( pObj->pData )
        return pObj->pData;
    Aig_ManDup_rec( pNew, p, Aig_ObjFanin0(pObj) );
    if ( Aig_ObjIsBuf(pObj) )
        return pObj->pData = Aig_ObjChild0Copy(pObj);
    Aig_ManDup_rec( pNew, p, Aig_ObjFanin1(pObj) );
    pObjNew = Aig_Oper( pNew, Aig_ObjChild0Copy(pObj), Aig_ObjChild1Copy(pObj), Aig_ObjType(pObj) );
    Aig_Regular(pObjNew)->pHaig = pObj->pHaig;
    return pObj->pData = pObjNew;
}

/**Function*************************************************************

  Synopsis    [Extracts the miter composed of XOR of the two nodes.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Man_t * Aig_ManExtractMiter( Aig_Man_t * p, Aig_Obj_t * pNode1, Aig_Obj_t * pNode2 )
{
    Aig_Man_t * pNew;
    Aig_Obj_t * pObj;
    int i;
    // create the new manager
    pNew = Aig_ManStart( Aig_ManObjNumMax(p) );
    pNew->pName = Aig_UtilStrsav( p->pName );
    pNew->pSpec = Aig_UtilStrsav( p->pSpec );
    // create the PIs
    Aig_ManCleanData( p );
    Aig_ManConst1(p)->pData = Aig_ManConst1(pNew);
    Aig_ManForEachPi( p, pObj, i )
        pObj->pData = Aig_ObjCreatePi(pNew);
    // dump the nodes
    Aig_ManDup_rec( pNew, p, pNode1 );   
    Aig_ManDup_rec( pNew, p, pNode2 );   
    // construct the EXOR
    pObj = Aig_Exor( pNew, pNode1->pData, pNode2->pData ); 
    pObj = Aig_NotCond( pObj, Aig_Regular(pObj)->fPhase ^ Aig_IsComplement(pObj) );
    // add the PO
    Aig_ObjCreatePo( pNew, pObj );
    // check the resulting network
    if ( !Aig_ManCheck(pNew) )
        printf( "Aig_ManExtractMiter(): The check has failed.\n" );
    return pNew;
}


/**Function*************************************************************

  Synopsis    [Stops the AIG manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Aig_ManStop( Aig_Man_t * p )
{
    Aig_Obj_t * pObj;
    int i;
    if ( p->vMapped )
        Vec_PtrFree( p->vMapped );
    // print time
    if ( p->time1 ) { ABC_PRT( "time1", p->time1 ); }
    if ( p->time2 ) { ABC_PRT( "time2", p->time2 ); }
    // delete timing
    if ( p->pManTime )
        Tim_ManStop( p->pManTime );
    // delete fanout
    if ( p->pFanData ) 
        Aig_ManFanoutStop( p );
    // make sure the nodes have clean marks
    Aig_ManForEachObj( p, pObj, i )
        assert( !pObj->fMarkA && !pObj->fMarkB );
//    Aig_TableProfile( p );
    Aig_MmFixedStop( p->pMemObjs, 0 );
    if ( p->vPis )      Vec_PtrFree( p->vPis );
    if ( p->vPos )      Vec_PtrFree( p->vPos );
    if ( p->vObjs )     Vec_PtrFree( p->vObjs );
    if ( p->vBufs )     Vec_PtrFree( p->vBufs );
    if ( p->vLevelR )   Vec_IntFree( p->vLevelR );
    if ( p->vLevels )   Vec_VecFree( p->vLevels );
    if ( p->vFlopNums)  Vec_IntFree( p->vFlopNums );
    if ( p->vFlopReprs) Vec_IntFree( p->vFlopReprs );
    if ( p->pManExdc )  Aig_ManStop( p->pManExdc );
    if ( p->vOnehots )  Vec_VecFree( (Vec_Vec_t *)p->vOnehots );
    if ( p->vClockDoms) Vec_VecFree( p->vClockDoms );
    if ( p->vProbs )    Vec_IntFree( p->vProbs );
    ABC_FREE( p->pFastSim );
    ABC_FREE( p->pData );
    ABC_FREE( p->pSeqModel );
    ABC_FREE( p->pName );
    ABC_FREE( p->pSpec );
    ABC_FREE( p->pObjCopies );
    ABC_FREE( p->pReprs );
    ABC_FREE( p->pEquivs );
    ABC_FREE( p->pTable );
    ABC_FREE( p );
}

/**Function*************************************************************

  Synopsis    [Removes combinational logic that does not feed into POs.]

  Description [Returns the number of dangling nodes removed.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Aig_ManCleanup( Aig_Man_t * p )
{
    Vec_Ptr_t * vObjs;
    Aig_Obj_t * pNode;
    int i, nNodesOld = Aig_ManNodeNum(p);
    // collect roots of dangling nodes
    vObjs = Vec_PtrAlloc( 100 );
    Aig_ManForEachObj( p, pNode, i )
        if ( Aig_ObjIsNode(pNode) && Aig_ObjRefs(pNode) == 0 )
            Vec_PtrPush( vObjs, pNode );
    // recursively remove dangling nodes
    Vec_PtrForEachEntry( vObjs, pNode, i )
        Aig_ObjDelete_rec( p, pNode, 1 );
    Vec_PtrFree( vObjs );
    return nNodesOld - Aig_ManNodeNum(p);
}

/**Function*************************************************************

  Synopsis    [Adds POs for the nodes that otherwise would be dangling.]

  Description [Returns the number of POs added.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Aig_ManAntiCleanup( Aig_Man_t * p )
{
    Aig_Obj_t * pNode;
    int i, nNodesOld = Aig_ManPoNum(p);
    Aig_ManForEachObj( p, pNode, i )
        if ( Aig_ObjIsNode(pNode) && Aig_ObjRefs(pNode) == 0 )
            Aig_ObjCreatePo( p, pNode );
    return nNodesOld - Aig_ManPoNum(p);
}

/**Function*************************************************************

  Synopsis    [Removes PIs without fanouts.]

  Description [Returns the number of PIs removed.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Aig_ManPiCleanup( Aig_Man_t * p )
{
    Aig_Obj_t * pObj;
    int i, k = 0, nPisOld = Aig_ManPiNum(p);
    Vec_PtrForEachEntry( p->vPis, pObj, i )
    {
        if ( i >= Aig_ManPiNum(p) - Aig_ManRegNum(p) )
            Vec_PtrWriteEntry( p->vPis, k++, pObj );
        else if ( Aig_ObjRefs(pObj) > 0 )
            Vec_PtrWriteEntry( p->vPis, k++, pObj );
        else
            Vec_PtrWriteEntry( p->vObjs, pObj->Id, NULL );
    }
    Vec_PtrShrink( p->vPis, k );
    p->nObjs[AIG_OBJ_PI] = Vec_PtrSize( p->vPis );
    if ( Aig_ManRegNum(p) )
        p->nTruePis = Aig_ManPiNum(p) - Aig_ManRegNum(p);
    return nPisOld - Aig_ManPiNum(p);
}

/**Function*************************************************************

  Synopsis    [Removes POs with constant input.]

  Description [Returns the number of POs removed.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Aig_ManPoCleanup( Aig_Man_t * p )
{
    Aig_Obj_t * pObj;
    int i, k = 0, nPosOld = Aig_ManPoNum(p);
    Vec_PtrForEachEntry( p->vPos, pObj, i )
    {
        if ( i >= Aig_ManPoNum(p) - Aig_ManRegNum(p) )
            Vec_PtrWriteEntry( p->vPos, k++, pObj );
        else if ( !Aig_ObjIsConst1(Aig_ObjFanin0(pObj)) || !Aig_ObjFaninC0(pObj) ) // non-const or const1
            Vec_PtrWriteEntry( p->vPos, k++, pObj );
        else
        {
            Aig_ObjDisconnect( p, pObj );
            Vec_PtrWriteEntry( p->vObjs, pObj->Id, NULL );
        }
    }
    Vec_PtrShrink( p->vPos, k );
    p->nObjs[AIG_OBJ_PO] = Vec_PtrSize( p->vPos );
    if ( Aig_ManRegNum(p) )
        p->nTruePos = Aig_ManPoNum(p) - Aig_ManRegNum(p);
    return nPosOld - Aig_ManPoNum(p);
}

/**Function*************************************************************

  Synopsis    [Performs one iteration of AIG rewriting.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Aig_ManHaigCounter( Aig_Man_t * pAig )
{
    Aig_Obj_t * pObj;
    int Counter, i;
    Counter = 0;
    Aig_ManForEachNode( pAig, pObj, i )
        Counter += (pObj->pHaig != NULL);
    return Counter;
}

/**Function*************************************************************

  Synopsis    [Stops the AIG manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Aig_ManPrintStats( Aig_Man_t * p )
{
    int nChoices = Aig_ManChoiceNum(p);
    printf( "%-15s : ",      p->pName );
    printf( "pi = %5d  ",    Aig_ManPiNum(p) );
    printf( "po = %5d  ",    Aig_ManPoNum(p) );
    if ( Aig_ManRegNum(p) )
    printf( "lat = %5d  ", Aig_ManRegNum(p) );
    printf( "and = %7d  ",   Aig_ManAndNum(p) );
//    printf( "Eq = %7d  ",     Aig_ManHaigCounter(p) );
    if ( Aig_ManExorNum(p) )
    printf( "xor = %5d  ",    Aig_ManExorNum(p) );
    if ( nChoices )
    printf( "ch = %5d  ",  nChoices );
    if ( Aig_ManBufNum(p) )
    printf( "buf = %5d  ",    Aig_ManBufNum(p) );
//    printf( "Cre = %6d  ",  p->nCreated );
//    printf( "Del = %6d  ",  p->nDeleted );
//    printf( "Lev = %3d  ",  Aig_ManLevelNum(p) );
//    printf( "Max = %7d  ",  Aig_ManObjNumMax(p) );
    printf( "lev = %3d",  Aig_ManLevels(p) );
    printf( "\n" );
    fflush( stdout );
}

/**Function*************************************************************

  Synopsis    [Reports the reduction of the AIG.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Aig_ManReportImprovement( Aig_Man_t * p, Aig_Man_t * pNew )
{
    printf( "REG: Beg = %5d. End = %5d. (R =%5.1f %%)  ",
        Aig_ManRegNum(p), Aig_ManRegNum(pNew), 
        Aig_ManRegNum(p)? 100.0*(Aig_ManRegNum(p)-Aig_ManRegNum(pNew))/Aig_ManRegNum(p) : 0.0 );
    printf( "AND: Beg = %6d. End = %6d. (R =%5.1f %%)",
        Aig_ManNodeNum(p), Aig_ManNodeNum(pNew), 
        Aig_ManNodeNum(p)? 100.0*(Aig_ManNodeNum(p)-Aig_ManNodeNum(pNew))/Aig_ManNodeNum(p) : 0.0 );
    printf( "\n" );
}

/**Function*************************************************************

  Synopsis    [Sets the number of registers in the AIG manager.]

  Description [This procedure should be called after the manager is 
  fully constructed.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Aig_ManSetRegNum( Aig_Man_t * p, int nRegs )
{
    p->nRegs = nRegs;
    p->nTruePis = Aig_ManPiNum(p) - nRegs;
    p->nTruePos = Aig_ManPoNum(p) - nRegs;
    Aig_ManSetPioNumbers( p );
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Aig_ManFlipFirstPo( Aig_Man_t * p )
{
    Aig_ObjChild0Flip( Aig_ManPo(p, 0) ); 
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


