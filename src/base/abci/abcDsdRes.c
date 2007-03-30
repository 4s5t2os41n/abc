/**CFile****************************************************************

  FileName    [abcDsdRes.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Network and node package.]

  Synopsis    []

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: abcDsdRes.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "abc.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

#define LUT_SIZE_MAX     16   // the largest size of the function
#define LUT_CUTS_MAX    128   // the largest number of cuts considered

typedef struct Lut_Man_t_ Lut_Man_t;
typedef struct Lut_Cut_t_ Lut_Cut_t;

struct Lut_Cut_t_
{
    unsigned     nLeaves       : 6;     // (L) the number of leaves
    unsigned     nNodes        : 6;     // (M) the number of nodes
    unsigned     nNodesMarked  : 6;     // (Q) nodes outside of MFFC
    unsigned     nNodesMax     : 6;     // the max number of nodes 
    unsigned     nLeavesMax    : 6;     // the max number of leaves
    unsigned     fHasDsd       : 1;     // set to 1 if the cut has structural DSD (and so cannot be used)
    unsigned     fMark         : 1;     // multipurpose mark
//    unsigned     uSign[2];              // the signature
    float        Weight;                // the weight of the cut: (M - Q)/N(V)   (the larger the better)
    int          pLeaves[LUT_SIZE_MAX]; // the leaves of the cut
    int          pNodes[LUT_SIZE_MAX];  // the nodes of the cut
};

struct Lut_Man_t_
{
    // parameters
    Lut_Par_t *  pPars;                 // the set of parameters
    // current representation
    Abc_Ntk_t *  pNtk;                  // the network
    Abc_Obj_t *  pObj;                  // the node to resynthesize 
    // cut representation
    int          nCuts;                 // the total number of cuts    
    int          nCutsMax;              // the largest possible number of cuts
    int          nEvals;                // the number of good cuts
    Lut_Cut_t    pCuts[LUT_CUTS_MAX];   // the storage for cuts
    int          pEvals[LUT_SIZE_MAX];  // the good cuts
    // temporary variables
    int          pRefs[LUT_SIZE_MAX];   // fanin reference counters 
    int          pCands[LUT_SIZE_MAX];  // internal nodes pointing only to the leaves
    // truth table representation
    Vec_Ptr_t *  vTtElems;              // elementary truth tables
    Vec_Ptr_t *  vTtNodes;              // storage for temporary truth tables of the nodes 
};

static int Abc_LutResynthesizeNode( Lut_Man_t * p );

#define Abc_LutCutForEachLeaf( pNtk, pCut, pObj, i )                                        \
    for ( i = 0; (i < (int)(pCut)->nLeaves) && (((pObj) = Abc_NtkObj(pNtk, (pCut)->pLeaves[i])), 1); i++ )
#define Abc_LutCutForEachNode( pNtk, pCut, pObj, i )                                        \
    for ( i = 0; (i < (int)(pCut)->nNodes) && (((pObj) = Abc_NtkObj(pNtk, (pCut)->pNodes[i])), 1); i++ )

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Lut_Man_t * Abc_LutManStart( Lut_Par_t * pPars )
{
    Lut_Man_t * p;
    int i;
    assert( pPars->nLutsMax <= 16 );
    p = ALLOC( Lut_Man_t, 1 );
    memset( p, 0, sizeof(Lut_Man_t) );
    p->pPars = pPars;
    p->nCutsMax = LUT_CUTS_MAX;
    for ( i = 0; i < p->nCuts; i++ )
        p->pCuts[i].nLeavesMax = p->pCuts[i].nNodesMax = LUT_SIZE_MAX;
    p->vTtElems = Vec_PtrAllocTruthTables( pPars->nLutsMax );
    p->vTtNodes = Vec_PtrAllocSimInfo( 256, Abc_TruthWordNum(pPars->nLutsMax) );
    return p;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_LutManStop( Lut_Man_t * p )
{
    Vec_PtrFree( p->vTtElems );
    Vec_PtrFree( p->vTtNodes );
    free( p );
}

/**Function*************************************************************

  Synopsis    [Performs resynthesis for one network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_LutResynthesize( Abc_Ntk_t * pNtk, Lut_Par_t * pPars )
{
    Lut_Man_t * p;
    Abc_Obj_t * pObj;
    int i;
    assert( Abc_NtkIsLogic(pNtk) );
    // convert logic to AIGs
    Abc_NtkToAig( pNtk );
    // compute the levels
    Abc_NtkLevel( pNtk );
    // start the manager
    p = Abc_LutManStart( pPars );
    p->pNtk = pNtk;
    // get the number of inputs
    p->pPars->nLutSize = Abc_NtkGetFaninMax( pNtk );
    p->pPars->nVarsMax = p->pPars->nLutsMax * (p->pPars->nLutSize - 1) + 1; // V = N * (K-1) + 1
    printf( "Resynthesis for %d %d-LUTs with %d non-MFFC LUTs, %d crossbars, and %d-input cuts.\n",
        p->pPars->nLutsMax, p->pPars->nLutSize, p->pPars->nLutsOver, p->pPars->nVarsShared, p->pPars->nVarsMax );
    // consider all nodes
    Abc_NtkForEachNode( pNtk, pObj, i )
    {
        p->pObj = pObj;
        Abc_LutResynthesizeNode( p );
    }
    Abc_LutManStop( p );
    // check the resulting network
    if ( !Abc_NtkCheck( pNtk ) )
    {
        printf( "Abc_LutResynthesize: The network check has failed.\n" );
        return 0;
    }
    return 1;
}

/**Function*************************************************************

  Synopsis    [Returns 1 if the cut has structural DSD.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_LutNodeCutsCheckDsd( Lut_Man_t * p, Lut_Cut_t * pCut )
{
    Abc_Obj_t * pObj, * pFanin;
    int i, k, nCands, fLeavesOnly, RetValue;
    assert( pCut->nLeaves > 0 );
    // clear ref counters
    memset( p->pRefs, 0, sizeof(int) * pCut->nLeaves );
    // mark cut leaves
    Abc_LutCutForEachLeaf( p->pNtk, pCut, pObj, i )
    {
        assert( pObj->fMarkA == 0 );
        pObj->fMarkA = 1;
        pObj->pCopy = (void *)i;
    }
    // ref leaves pointed from the internal nodes
    nCands = 0;
    Abc_LutCutForEachNode( p->pNtk, pCut, pObj, i )
    {
        fLeavesOnly = 1;
        Abc_ObjForEachFanin( pObj, pFanin, k )
            if ( pFanin->fMarkA )
                p->pRefs[(int)pFanin->pCopy]++;
            else
                fLeavesOnly = 0;
        if ( fLeavesOnly )
            p->pCands[nCands++] = pObj->Id;
    }
    // look at the nodes that only point to the leaves
    RetValue = 0;
    for ( i = 0; i < nCands; i++ )
    {
        pObj = Abc_NtkObj( p->pNtk, p->pCands[i] );
        Abc_ObjForEachFanin( pObj, pFanin, k )
        {
            assert( pFanin->fMarkA == 1 );
            if ( p->pRefs[(int)pFanin->pCopy] > 1 )
                break;
        }
        if ( k == Abc_ObjFaninNum(pFanin) )
        {
            RetValue = 1;
            break;
        }
    }
    // unmark cut leaves
    Abc_LutCutForEachLeaf( p->pNtk, pCut, pObj, i )
        pObj->fMarkA = 0;
    return RetValue;
}

/**Function*************************************************************

  Synopsis    [Returns 1 if pDom is contained in pCut.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
static inline int Abc_LutNodeCutsOneDominance( Lut_Cut_t * pDom, Lut_Cut_t * pCut )
{
    int i, k;
    for ( i = 0; i < (int)pDom->nLeaves; i++ )
    {
        for ( k = 0; k < (int)pCut->nLeaves; k++ )
            if ( pDom->pLeaves[i] == pCut->pLeaves[k] )
                break;
        if ( k == (int)pCut->nLeaves ) // node i in pDom is not contained in pCut
            return 0;
    }
    // every node in pDom is contained in pCut
    return 1;
}

/**Function*************************************************************

  Synopsis    [Check if the cut exists.]

  Description [Returns 1 if the cut exists.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_LutNodeCutsOneFilter( Lut_Cut_t * pCuts, int nCuts, Lut_Cut_t * pCutNew )
{
    Lut_Cut_t * pCut;
    int i, k;
//    assert( pCutNew->uHash );
    // try to find the cut
    for ( i = 0; i < nCuts; i++ )
    {
        pCut = pCuts + i;
        if ( pCut->nLeaves == 0 )
            continue;
        if ( pCut->nLeaves == pCutNew->nLeaves )
        {
//            if ( pCut->uHash[0] == pCutNew->uHash[0] && pCut->uHash[1] == pCutNew->uHash[1] )
            {
                for ( k = 0; k < (int)pCutNew->nLeaves; k++ )
                    if ( pCut->pLeaves[k] != pCutNew->pLeaves[k] )
                        break;
                if ( k == (int)pCutNew->nLeaves )
                    return 1;
            }
            continue;
        }
        if ( pCut->nLeaves < pCutNew->nLeaves )
        {
            // skip the non-contained cuts
//            if ( (pCut->uHash[0] & pCutNew->uHash[0]) != pCut->uHash[0] )
//                continue;
//            if ( (pCut->uHash[1] & pCutNew->uHash[1]) != pCut->uHash[1] )
//                continue;
            // check containment seriously
            if ( Abc_LutNodeCutsOneDominance( pCut, pCutNew ) )
                return 1;
            continue;
        }
        // check potential containment of other cut

        // skip the non-contained cuts
//        if ( (pCut->uHash[0] & pCutNew->uHash[0]) != pCutNew->uHash[0] )
//            continue;
//        if ( (pCut->uHash[1] & pCutNew->uHash[1]) != pCutNew->uHash[1] )
//            continue;
        // check containment seriously
        if ( Abc_LutNodeCutsOneDominance( pCutNew, pCut ) )
            pCut->nLeaves = 0; // removed
    }
    return 0;
}

/**Function*************************************************************

  Synopsis    [Computes the set of all cuts.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_LutNodeCutsOne( Lut_Man_t * p, Lut_Cut_t * pCut, int Node )
{
    Lut_Cut_t * pCutNew;
    Abc_Obj_t * pObj, * pFanin;
    int i, k, j;

    // check if the cut can stand adding one more internal node
    if ( pCut->nNodes == LUT_SIZE_MAX )
        return;

    // if the node is not in the MFFC, check the limit
    pObj = Abc_NtkObj( p->pNtk, Node );
    if ( !Abc_NodeIsTravIdCurrent(pObj) )
    {
        if ( (int)pCut->nNodesMarked == p->pPars->nLutsOver )
            return;
        assert( (int)pCut->nNodesMarked < p->pPars->nLutsOver );
    }

    // create the new set of leaves
    pCutNew = p->pCuts + p->nCuts;
    pCutNew->nLeaves = 0;
    for ( i = 0; i < (int)pCut->nLeaves; i++ )
        if ( pCut->pLeaves[i] != Node )
            pCutNew->pLeaves[pCutNew->nLeaves++] = pCut->pLeaves[i];

    // add new nodes
    Abc_ObjForEachFanin( pObj, pFanin, i )
    {
        // find the place where this node belongs
        for ( k = 0; k < (int)pCutNew->nLeaves; k++ )
            if ( pCutNew->pLeaves[k] >= pFanin->Id )
                break;
        if ( pCutNew->pLeaves[k] == pFanin->Id )
            continue;
        // check if there is room
        if ( (int)pCutNew->nLeaves == p->pPars->nVarsMax )
            return;
        // move all the nodes
        for ( j = pCutNew->nLeaves; j > k; j-- )
            pCutNew->pLeaves[j] = pCutNew->pLeaves[j-1];
        pCutNew->pLeaves[k] = pFanin->Id;
        pCutNew->nLeaves++;
        assert( pCutNew->nLeaves <= LUT_SIZE_MAX );
    }

    // skip the contained cuts
    if ( Abc_LutNodeCutsOneFilter( p->pCuts, p->nCuts, pCutNew ) )
        return;

    // update the set of internal nodes
    assert( pCut->nNodes < LUT_SIZE_MAX );
    memcpy( pCutNew->pNodes, pCutNew->pNodes, pCut->nNodes * sizeof(int) );
    pCutNew->pNodes[ pCut->nNodes++ ] = Node;

    // add the marked node
    pCutNew->nNodesMarked = pCut->nNodesMarked + !Abc_NodeIsTravIdCurrent(pObj);

    // add the cut to storage
    assert( p->nCuts < LUT_CUTS_MAX );
    p->nCuts++;

}

/**Function*************************************************************

  Synopsis    [Computes the set of all cuts.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_LutNodeCuts( Lut_Man_t * p )
{
    Abc_Obj_t * pFanin;
    Lut_Cut_t * pCut, * pCut2;
    int i, k, Temp, nMffc, fChanges;

    // mark the MFFC of the node with the current trav ID
    nMffc = Abc_NodeMffcLabel( p->pObj );
    assert( nMffc > 0 );
    if ( nMffc == 1 )
        return 0;

    // initialize the first cut
    pCut = p->pCuts;
    // assign internal nodes
    pCut->nNodes = 1; 
    pCut->pNodes[0] = p->pObj->Id;
    pCut->nNodesMarked = 0;
    // assign the leaves
    pCut->nLeaves = Abc_ObjFaninNum( p->pObj );
    Abc_ObjForEachFanin( p->pObj, pFanin, i )
        pCut->pLeaves[i] = pFanin->Id;
    // sort the leaves
    do {
        fChanges = 0;
        for ( i = 0; i < (int)pCut->nLeaves - 1; i++ )
        {
            if ( pCut->pLeaves[i] <= pCut->pLeaves[i+1] )
                continue;
            Temp = pCut->pLeaves[i];
            pCut->pLeaves[i] = pCut->pLeaves[i+1];
            pCut->pLeaves[i+1] = Temp;
            fChanges = 1;
        }
    } while ( fChanges );

    // perform the cut computation
    for ( i = 0; i < p->nCuts; i++ )
    {
        pCut = p->pCuts + p->pEvals[i];
        if ( pCut->nLeaves == 0 )
            continue;
        // try to expand each fanin of each cut
        for ( k = 0; k < (int)pCut->nLeaves; k++ )
        {
            Abc_LutNodeCutsOne( p, pCut, pCut->pLeaves[k] );
            if ( p->nCuts == LUT_CUTS_MAX )
                break;
        }
        if ( p->nCuts == LUT_CUTS_MAX )
            break;
    }

    // compress the cuts by removing empty ones, decomposable ones, and those with negative Weight
    p->nEvals = 0;
    for ( i = 0; i < p->nCuts; i++ )
    {
        pCut = p->pCuts + p->pEvals[i];
        pCut->Weight = (float)1.0 * (pCut->nNodes - pCut->nNodesMarked) / p->pPars->nLutsMax;
        pCut->fHasDsd = Abc_LutNodeCutsCheckDsd( p, pCut );
        if ( pCut->nLeaves == 0 || pCut->Weight <= 1.0 || pCut->fHasDsd )
            continue;
        p->pEvals[p->nEvals++] = i;
    }
    if ( p->nEvals == 0 )
        return 0;

    // sort the cuts by Weight
    do {
        fChanges = 0;
        for ( i = 0; i < p->nEvals - 1; i++ )
        {
            pCut = p->pCuts + p->pEvals[i];
            pCut2 = p->pCuts + p->pEvals[i+1];
            if ( pCut->Weight >= pCut2->Weight )
                continue;
            Temp = p->pEvals[i];
            p->pEvals[i] = p->pEvals[i+1];
            p->pEvals[i+1] = Temp;
            fChanges = 1;
        }
    } while ( fChanges );
    return 1;
}

/**Function*************************************************************

  Synopsis    [Computes the truth able of one cut.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
unsigned * Abc_LutCutTruth( Lut_Man_t * p, Lut_Cut_t * pCut )
{
    unsigned * pTruth = NULL;
    return pTruth;
}

/**Function*************************************************************

  Synopsis    [Implements the given DSD network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_LutCutUpdate( Lut_Man_t * p, Lut_Cut_t * pCut, void * pDsd )
{
    return 1;
}

/**Function*************************************************************

  Synopsis    [Performs resynthesis for one node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_LutResynthesizeNode( Lut_Man_t * p )
{
    Lut_Cut_t * pCut;
    unsigned * pTruth;
    void * pDsd;
    int i;
    // compute the cuts
    if ( !Abc_LutNodeCuts( p ) )
        return 0;
    // try the good cuts
    for ( i = 0; i < p->nEvals; i++ )
    {
        // get the cut
        pCut = p->pCuts + p->pEvals[i];
        // compute the truth table
        pTruth = Abc_LutCutTruth( p, pCut );
        // check decomposition
        pDsd = /***/ NULL;
        // if it is not DSD decomposable, return
        if ( pDsd == NULL )
            continue;
        // update the network
        Abc_LutCutUpdate( p, pCut, pDsd );
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


