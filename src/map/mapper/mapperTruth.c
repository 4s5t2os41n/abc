/**CFile****************************************************************

  FileName    [mapperTruth.c]

  PackageName [MVSIS 1.3: Multi-valued logic synthesis system.]

  Synopsis    [Generic technology mapping engine.]

  Author      [MVSIS Group]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 2.0. Started - June 1, 2004.]

  Revision    [$Id: mapperTruth.c,v 1.8 2005/01/23 06:59:45 alanmi Exp $]

***********************************************************************/

#include "mapperInt.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static void Map_TruthsCut( Map_Man_t * pMan, Map_Cut_t * pCut );
static void Map_TruthsCutOne( Map_Man_t * p, Map_Cut_t * pCut, unsigned uTruth[] );
static void Map_CutsCollect_rec( Map_Cut_t * pCut, Map_NodeVec_t * vVisited );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFITIONS                           ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Derives truth tables for each cut.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Map_MappingTruths( Map_Man_t * pMan )
{
    ProgressBar * pProgress;
    Map_Node_t * pNode;
    Map_Cut_t * pCut;
    int nNodes, i;
    // compute the cuts for the POs
    nNodes = pMan->vAnds->nSize;
    pProgress = Extra_ProgressBarStart( stdout, nNodes );
    for ( i = 0; i < nNodes; i++ )
    {
        pNode = pMan->vAnds->pArray[i];
        if ( !Map_NodeIsAnd( pNode ) )
            continue;
        assert( pNode->pCuts );
        assert( pNode->pCuts->nLeaves == 1 );

        // match the simple cut
        pNode->pCuts->M[0].uPhase     = 0;
        pNode->pCuts->M[0].pSupers    = pMan->pSuperLib->pSuperInv;
        pNode->pCuts->M[0].uPhaseBest = 0;
        pNode->pCuts->M[0].pSuperBest = pMan->pSuperLib->pSuperInv;

        pNode->pCuts->M[1].uPhase     = 0;
        pNode->pCuts->M[1].pSupers    = pMan->pSuperLib->pSuperInv;
        pNode->pCuts->M[1].uPhaseBest = 1;
        pNode->pCuts->M[1].pSuperBest = pMan->pSuperLib->pSuperInv;

        // match the rest of the cuts
        for ( pCut = pNode->pCuts->pNext; pCut; pCut = pCut->pNext )
             Map_TruthsCut( pMan, pCut );
        Extra_ProgressBarUpdate( pProgress, i, "Tables ..." );
    }
    Extra_ProgressBarStop( pProgress );
}

/**Function*************************************************************

  Synopsis    [Derives the truth table for one cut.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Map_TruthsCut( Map_Man_t * p, Map_Cut_t * pCut )
{
    unsigned uTruth[2], uCanon[2];
    unsigned char uPhases[16];

    // generally speaking, 1-input cut can be matched into a wire!
    if ( pCut->nLeaves == 1 )
        return;
    Map_TruthsCutOne( p, pCut, uTruth );

    // compute the canonical form for the positive phase
    Map_CanonComputeSlow( p->uTruths, p->nVarsMax, pCut->nLeaves, uTruth, uPhases, uCanon );
    pCut->M[1].pSupers = Map_SuperTableLookupC( p->pSuperLib, uCanon );
    pCut->M[1].uPhase  = uPhases[0];
    p->nCanons++;

    // compute the canonical form for the negative phase
    uTruth[0] = ~uTruth[0];
    uTruth[1] = ~uTruth[1];
    Map_CanonComputeSlow( p->uTruths, p->nVarsMax, pCut->nLeaves, uTruth, uPhases, uCanon );
    pCut->M[0].pSupers = Map_SuperTableLookupC( p->pSuperLib, uCanon );
    pCut->M[0].uPhase  = uPhases[0];
    p->nCanons++;

    // restore the truth table
    uTruth[0] = ~uTruth[0];
    uTruth[1] = ~uTruth[1];
}

/**Function*************************************************************

  Synopsis    [Computes the truth table of one cut.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Map_TruthsCutOne( Map_Man_t * p, Map_Cut_t * pCut, unsigned uTruth[] )
{
    unsigned uTruth1[2], uTruth2[2];
    Map_Cut_t * pTemp;
    int i;
    // mark the cut leaves
    for ( i = 0; i < pCut->nLeaves; i++ )
    {
        pTemp = pCut->ppLeaves[i]->pCuts;
        pTemp->fMark = 1;
        pTemp->M[0].uPhaseBest = p->uTruths[i][0];
        pTemp->M[1].uPhaseBest = p->uTruths[i][1];
    }
    assert( pCut->fMark == 0 );

    // collect the cuts in the cut cone
    p->vVisited->nSize = 0;
    Map_CutsCollect_rec( pCut, p->vVisited );
    assert( p->vVisited->nSize > 0 );
    pCut->nVolume = p->vVisited->nSize;

    // compute the tables and unmark
    for ( i = 0; i < pCut->nLeaves; i++ )
    {
        pTemp = pCut->ppLeaves[i]->pCuts;
        pTemp->fMark = 0;
    }
    for ( i = 0; i < p->vVisited->nSize; i++ )
    {
        // get the cut
        pTemp = (Map_Cut_t *)p->vVisited->pArray[i];
        pTemp->fMark = 0;
        // get truth table of the first branch
        if ( Map_CutIsComplement(pTemp->pOne) )
        {
            uTruth1[0] = ~Map_CutRegular(pTemp->pOne)->M[0].uPhaseBest;
            uTruth1[1] = ~Map_CutRegular(pTemp->pOne)->M[1].uPhaseBest;
        }
        else
        {
            uTruth1[0] = Map_CutRegular(pTemp->pOne)->M[0].uPhaseBest;
            uTruth1[1] = Map_CutRegular(pTemp->pOne)->M[1].uPhaseBest;
        }
        // get truth table of the second branch
        if ( Map_CutIsComplement(pTemp->pTwo) )
        {
            uTruth2[0] = ~Map_CutRegular(pTemp->pTwo)->M[0].uPhaseBest;
            uTruth2[1] = ~Map_CutRegular(pTemp->pTwo)->M[1].uPhaseBest;
        }
        else
        {
            uTruth2[0] = Map_CutRegular(pTemp->pTwo)->M[0].uPhaseBest;
            uTruth2[1] = Map_CutRegular(pTemp->pTwo)->M[1].uPhaseBest;
        }
        // get the truth table of the output
        if ( !pTemp->Phase )
        {
            pTemp->M[0].uPhaseBest = uTruth1[0] & uTruth2[0];
            pTemp->M[1].uPhaseBest = uTruth1[1] & uTruth2[1];
        }
        else
        {
            pTemp->M[0].uPhaseBest = ~(uTruth1[0] & uTruth2[0]);
            pTemp->M[1].uPhaseBest = ~(uTruth1[1] & uTruth2[1]);
        }
    }
    uTruth[0] = pTemp->M[0].uPhaseBest;
    uTruth[1] = pTemp->M[1].uPhaseBest;
}

/**Function*************************************************************

  Synopsis    [Recursively collect the cuts.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Map_CutsCollect_rec( Map_Cut_t * pCut, Map_NodeVec_t * vVisited )
{
    if ( pCut->fMark )
        return;
    Map_CutsCollect_rec( Map_CutRegular(pCut->pOne), vVisited );
    Map_CutsCollect_rec( Map_CutRegular(pCut->pTwo), vVisited );
    assert( pCut->fMark == 0 );
    pCut->fMark = 1;
    Map_NodeVecPush( vVisited, (Map_Node_t *)pCut );
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


