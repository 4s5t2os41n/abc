/**CFile****************************************************************

  FileName    [cnfUtil.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [AIG-to-CNF conversion.]

  Synopsis    []

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - April 28, 2007.]

  Revision    [$Id: cnfUtil.c,v 1.00 2007/04/28 00:00:00 alanmi Exp $]

***********************************************************************/

#include "cnf.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Computes area, references, and nodes used in the mapping.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Dar_ManScanMapping_rec( Cnf_Man_t * p, Dar_Obj_t * pObj, Vec_Ptr_t * vMapped )
{
    Dar_Obj_t * pLeaf;
    Dar_Cut_t * pCutBest;
    int aArea, i;
    if ( pObj->nRefs++ || Dar_ObjIsPi(pObj) || Dar_ObjIsConst1(pObj) )
        return 0;
    assert( Dar_ObjIsAnd(pObj) );
    // collect the node first to derive pre-order
    if ( vMapped )
        Vec_PtrPush( vMapped, pObj );
    // visit the transitive fanin of the selected cut
    pCutBest = Dar_ObjBestCut(pObj);
    aArea = pCutBest->Cost;
    Dar_CutForEachLeaf( p->pManAig, pCutBest, pLeaf, i )
        aArea += Dar_ManScanMapping_rec( p, pLeaf, vMapped );
    return aArea;
}

/**Function*************************************************************

  Synopsis    [Computes area, references, and nodes used in the mapping.]

  Description [Collects the nodes in reverse topological order.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Vec_Ptr_t * Dar_ManScanMapping( Cnf_Man_t * p, int fCollect )
{
    Vec_Ptr_t * vMapped = NULL;
    Dar_Obj_t * pObj;
    int i;
    // clean all references
    Dar_ManForEachObj( p->pManAig, pObj, i )
        pObj->nRefs = 0;
    // allocate the array
    if ( fCollect )
        vMapped = Vec_PtrAlloc( 1000 );
    // collect nodes reachable from POs in the DFS order through the best cuts
    p->aArea = 0;
    Dar_ManForEachPo( p->pManAig, pObj, i )
        p->aArea += Dar_ManScanMapping_rec( p, Dar_ObjFanin0(pObj), vMapped );
    printf( "Variables = %6d. Clauses = %8d.\n", vMapped? Vec_PtrSize(vMapped) : 0, p->aArea );
    return vMapped;
}

/**Function*************************************************************

  Synopsis    [Computes area, references, and nodes used in the mapping.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Cnf_ManScanMapping_rec( Cnf_Man_t * p, Dar_Obj_t * pObj, Vec_Ptr_t * vMapped )
{
    Dar_Obj_t * pLeaf;
    Cnf_Cut_t * pCutBest;
    int aArea, i;
    if ( pObj->nRefs++ || Dar_ObjIsPi(pObj) || Dar_ObjIsConst1(pObj) )
        return 0;
    assert( Dar_ObjIsAnd(pObj) );
    assert( pObj->pData != NULL );
    // visit the transitive fanin of the selected cut
    pCutBest = pObj->pData;
    assert( pCutBest->Cost < 127 );
    aArea = pCutBest->Cost;
    Cnf_CutForEachLeaf( p->pManAig, pCutBest, pLeaf, i )
        aArea += Cnf_ManScanMapping_rec( p, pLeaf, vMapped );
    // collect the node first to derive pre-order
    if ( vMapped )
        Vec_PtrPush( vMapped, pObj );
    return aArea;
}

/**Function*************************************************************

  Synopsis    [Computes area, references, and nodes used in the mapping.]

  Description [Collects the nodes in reverse topological order.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Vec_Ptr_t * Cnf_ManScanMapping( Cnf_Man_t * p, int fCollect )
{
    Vec_Ptr_t * vMapped = NULL;
    Dar_Obj_t * pObj;
    int i;
    // clean all references
    Dar_ManForEachObj( p->pManAig, pObj, i )
        pObj->nRefs = 0;
    // allocate the array
    if ( fCollect )
        vMapped = Vec_PtrAlloc( 1000 );
    // collect nodes reachable from POs in the DFS order through the best cuts
    p->aArea = 0;
    Dar_ManForEachPo( p->pManAig, pObj, i )
        p->aArea += Cnf_ManScanMapping_rec( p, Dar_ObjFanin0(pObj), vMapped );
    printf( "Variables = %6d. Clauses = %8d.\n", vMapped? Vec_PtrSize(vMapped) : 0, p->aArea );
    return vMapped;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


