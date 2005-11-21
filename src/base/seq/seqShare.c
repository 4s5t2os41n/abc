/**CFile****************************************************************

  FileName    [seqShare.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Construction and manipulation of sequential AIGs.]

  Synopsis    [Latch sharing at the fanout stems.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: seqShare.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "seqInt.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static void Seq_NodeShareFanouts( Abc_Obj_t * pNode, Vec_Ptr_t * vNodes );
static void Seq_NodeShareOne( Abc_Obj_t * pNode, Abc_InitType_t Init, Vec_Ptr_t * vNodes );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Transforms the sequential AIG to take fanout sharing into account.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Seq_NtkShareFanouts( Abc_Ntk_t * pNtk )
{
    Vec_Ptr_t * vNodes;
    Abc_Obj_t * pObj;
    int i;
    vNodes = Vec_PtrAlloc( 10 );
    // share the PI latches
    Abc_NtkForEachPi( pNtk, pObj, i )
        Seq_NodeShareFanouts( pObj, vNodes );
    // share the node latches
    Abc_NtkForEachNode( pNtk, pObj, i )
        Seq_NodeShareFanouts( pObj, vNodes );
    Vec_PtrFree( vNodes );
}

/**Function*************************************************************

  Synopsis    [Transforms the node to take fanout sharing into account.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Seq_NodeShareFanouts( Abc_Obj_t * pNode, Vec_Ptr_t * vNodes )
{
    Abc_Obj_t * pFanout;
    Abc_InitType_t Type;
    int nLatches[4], i;
    // skip the node with only one fanout
    if ( Abc_ObjFanoutNum(pNode) < 2 )
        return;
    // clean the the fanout counters
    for ( i = 0; i < 4; i++ )
        nLatches[i] = 0;
    // find the number of fanouts having latches of each type
    Abc_ObjForEachFanout( pNode, pFanout, i )  
    {
        if ( Seq_ObjFanoutL(pNode, pFanout) == 0 )
            continue;
        Type = Seq_NodeGetInitLast( pFanout, Abc_ObjFanoutEdgeNum(pNode, pFanout) );
        nLatches[Type]++;
    }
    // decide what to do
    if ( nLatches[ABC_INIT_ZERO] > 1 && nLatches[ABC_INIT_ONE] > 1 ) // 0-group and 1-group
    {
        Seq_NodeShareOne( pNode, ABC_INIT_ZERO, vNodes );     // shares 0 and DC
        Seq_NodeShareOne( pNode, ABC_INIT_ONE,  vNodes );     // shares 1 and DC
    }
    else if ( nLatches[ABC_INIT_ZERO] > 1 ) // 0-group
        Seq_NodeShareOne( pNode, ABC_INIT_ZERO, vNodes );     // shares 0 and DC
    else if ( nLatches[ABC_INIT_ONE] > 1 ) // 1-group
        Seq_NodeShareOne( pNode, ABC_INIT_ONE,  vNodes );     // shares 1 and DC
    else if ( nLatches[ABC_INIT_DC] > 1 ) // DC-group
    {
        if ( nLatches[ABC_INIT_ZERO] > 0 )
            Seq_NodeShareOne( pNode, ABC_INIT_ZERO, vNodes ); // shares 0 and DC
        else 
            Seq_NodeShareOne( pNode, ABC_INIT_ONE,  vNodes ); // shares 1 and DC
    }
}

/**Function*************************************************************

  Synopsis    [Transforms the node to take fanout sharing into account.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Seq_NodeShareOne( Abc_Obj_t * pNode, Abc_InitType_t Init, Vec_Ptr_t * vNodes )
{
    Vec_Int_t * vNums  = Seq_ObjLNums( pNode );
    Vec_Ptr_t * vInits = Seq_NodeLats( pNode );
    Abc_Obj_t * pFanout, * pBuffer;
    Abc_InitType_t Type, InitNew;
    int i;
    // collect the fanouts that satisfy the property (have initial value Init or DC)
    InitNew = ABC_INIT_DC;
    Vec_PtrClear( vNodes );
    Abc_ObjForEachFanout( pNode, pFanout, i )
    {
        if ( Seq_ObjFanoutL(pNode, pFanout) == 0 )
            continue;
        Type = Seq_NodeGetInitLast( pFanout, Abc_ObjFanoutEdgeNum(pNode, pFanout) );
        if ( Type == Init )
            InitNew = Init;
        if ( Type == Init || Type == ABC_INIT_DC )
        {
            Vec_PtrPush( vNodes, pFanout );
            Seq_NodeDeleteLast( pFanout, Abc_ObjFanoutEdgeNum(pNode, pFanout) );
        }
    }
    // create the new buffer
    pBuffer = Abc_NtkCreateNode( pNode->pNtk );
    Abc_ObjAddFanin( pBuffer, pNode );

    // grow storage for initial states
    Vec_PtrGrow( vInits, 2 * pBuffer->Id + 2 );
    for ( i = Vec_PtrSize(vInits); i < 2 * (int)pBuffer->Id + 2; i++ )
        Vec_PtrPush( vInits, NULL );
    // grow storage for numbers of latches
    Vec_IntGrow( vNums, 2 * pBuffer->Id + 2 );
    for ( i = Vec_IntSize(vNums); i < 2 * (int)pBuffer->Id + 2; i++ )
        Vec_IntPush( vNums, 0 );
    // insert the new latch
    Seq_NodeInsertFirst( pBuffer, 0, InitNew );

    // redirect the fanouts
    Vec_PtrForEachEntry( vNodes, pFanout, i )
        Abc_ObjPatchFanin( pFanout, pNode, pBuffer );
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


