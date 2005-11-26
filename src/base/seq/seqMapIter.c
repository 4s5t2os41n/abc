/**CFile****************************************************************

  FileName    [seqMapIter.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Construction and manipulation of sequential AIGs.]

  Synopsis    [Iterative delay computation in SC mapping/retiming package.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: seqMapIter.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "seqInt.h"
#include "main.h"
#include "mio.h"
#include "mapperInt.h"

// the internal procedures
static float Seq_MapRetimeDelayLagsInternal( Abc_Ntk_t * pNtk, int fVerbose );
static float Seq_MapRetimeSearch_rec( Abc_Ntk_t * pNtk, float FiMin, float FiMax, float Delta, int fVerbose );
static int   Seq_MapRetimeForPeriod( Abc_Ntk_t * pNtk, float Fi, int fVerbose );
static int   Seq_MapNodeUpdateLValue( Abc_Obj_t * pObj, float Fi, float DelayInv );
static float Seq_MapCollectNode_rec( Abc_Obj_t * pAnd, float FiBest, Vec_Ptr_t * vMapping, Vec_Vec_t * vMapCuts );
static void  Seq_MapCanonicizeTruthTables( Abc_Ntk_t * pNtk );

extern Cut_Man_t * Abc_NtkSeqCuts( Abc_Ntk_t * pNtk, Cut_Params_t * pParams );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Computes the retiming lags for FPGA mapping.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Seq_MapRetimeDelayLags( Abc_Ntk_t * pNtk, int fVerbose )
{
    Abc_Seq_t * p = pNtk->pManFunc;
    Cut_Params_t Params, * pParams = &Params;
    Abc_Obj_t * pObj;
    float TotalArea, FiBest;
    int i, clk;

    // set defaults for cut computation
    memset( pParams, 0, sizeof(Cut_Params_t) );
    pParams->nVarsMax  = p->nVarsMax;  // the max cut size ("k" of the k-feasible cuts)
    pParams->nKeepMax  = 1000;  // the max number of cuts kept at a node
    pParams->fTruth    = 1;     // compute truth tables
    pParams->fFilter   = 1;     // filter dominated cuts
    pParams->fSeq      = 1;     // compute sequential cuts
    pParams->fVerbose  = fVerbose;     // the verbosiness flag

    // compute the cuts
clk = clock();
    p->pCutMan = Abc_NtkSeqCuts( pNtk, pParams );
p->timeCuts = clock() - clk;
    if ( fVerbose )
        Cut_ManPrintStats( p->pCutMan );

    // compute canonical forms of the truth tables of the cuts
    Seq_MapCanonicizeTruthTables( pNtk );

    // compute the delays
clk = clock();
    FiBest = Seq_MapRetimeDelayLagsInternal( pNtk, fVerbose );
p->timeDelay = clock() - clk;

    // collect the nodes and cuts used in the mapping
    p->vMapAnds = Vec_PtrAlloc( 1000 );
    p->vMapCuts = Vec_VecAlloc( 1000 );
    TotalArea = 0.0;
    Abc_NtkForEachPo( pNtk, pObj, i )
        TotalArea += Seq_MapCollectNode_rec( Abc_ObjChild0(pObj), FiBest, p->vMapAnds, p->vMapCuts );

    // clean the marks
    Abc_NtkForEachObj( pNtk, pObj, i )
        pObj->fMarkA = pObj->fMarkB = 0;

    if ( fVerbose )
        printf( "Total area = %6.2f.\n", TotalArea );

    // remove the cuts
    Cut_ManStop( p->pCutMan );
    p->pCutMan = NULL;
}

/**Function*************************************************************

  Synopsis    [Retimes AIG for optimal delay using Pan's algorithm.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
float Seq_MapRetimeDelayLagsInternal( Abc_Ntk_t * pNtk, int fVerbose )
{
    Abc_Seq_t * p = pNtk->pManFunc;
    Abc_Obj_t * pNode;
    float FiMax, FiBest, Delta;
    int i, RetValue;
    char NodeLag;

    assert( Abc_NtkIsSeq( pNtk ) );

    // assign the accuracy for min-period computation
    Delta = Mio_LibraryReadDelayNand2Max(Abc_FrameReadLibGen());
    if ( Delta == 0.0 )
    {
        Delta = Mio_LibraryReadDelayAnd2Max(Abc_FrameReadLibGen());
        if ( Delta == 0.0 )
        {
            printf( "Cannot retime/map if the library does not have NAND2 or AND2.\n" );
            return 0.0;
        }
    }

    // get the upper bound on the clock period
    FiMax = Delta * (2 + Seq_NtkLevelMax(pNtk));
    Delta /= 2;

    // make sure this clock period is feasible
    assert( Seq_MapRetimeForPeriod( pNtk, FiMax, fVerbose ) );

    // search for the optimal clock period between 0 and nLevelMax
    FiBest = Seq_MapRetimeSearch_rec( pNtk, 0.0, FiMax, Delta, fVerbose );

    // recompute the best l-values
    RetValue = Seq_MapRetimeForPeriod( pNtk, FiBest, fVerbose );
    assert( RetValue );

    // write the retiming lags for both phases of each node
    Vec_StrFill( p->vLags,  p->nSize, 0 );
    Vec_StrFill( p->vLagsN, p->nSize, 0 );
    Abc_AigForEachAnd( pNtk, pNode, i )
    {
        NodeLag = Seq_NodeComputeLagFloat( Seq_NodeGetLValueP(pNode), FiBest );
        Seq_NodeSetLag( pNode, NodeLag );
        NodeLag = Seq_NodeComputeLagFloat( Seq_NodeGetLValueN(pNode), FiBest );
        Seq_NodeSetLagN( pNode, NodeLag );
    }

    // print the result
    if ( fVerbose )
    printf( "The best clock period is %6.2f.\n", FiBest );
    return FiBest;
}

/**Function*************************************************************

  Synopsis    [Performs binary search for the optimal clock period.]

  Description [Assumes that FiMin is infeasible while FiMax is feasible.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
float Seq_MapRetimeSearch_rec( Abc_Ntk_t * pNtk, float FiMin, float FiMax, float Delta, int fVerbose )
{
    float Median;
    assert( FiMin < FiMax );
    if ( FiMin + Delta >= FiMax )
        return FiMax;
    Median = FiMin + (FiMax - FiMin)/2;
    if ( Seq_MapRetimeForPeriod( pNtk, Median, fVerbose ) )
        return Seq_MapRetimeSearch_rec( pNtk, FiMin, Median, Delta, fVerbose ); // Median is feasible
    else 
        return Seq_MapRetimeSearch_rec( pNtk, Median, FiMax, Delta, fVerbose ); // Median is infeasible
}

/**Function*************************************************************

  Synopsis    [Returns 1 if retiming with this clock period is feasible.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Seq_MapRetimeForPeriod( Abc_Ntk_t * pNtk, float Fi, int fVerbose )
{
    Abc_Seq_t * p = pNtk->pManFunc;
    Abc_Obj_t * pObj;
    float DelayInv = Mio_LibraryReadDelayInvMax(Abc_FrameReadLibGen());
    int i, c, RetValue, fChange, Counter;
    char * pReason = "";

    // set l-values of all nodes to be minus infinity
    Vec_IntFill( p->vLValues,  p->nSize, -ABC_INFINITY );
    Vec_IntFill( p->vLValuesN, p->nSize, -ABC_INFINITY );
    Vec_StrFill( p->vUses,     p->nSize, 0             );

    // set l-values of constants and PIs
    pObj = Abc_NtkObj( pNtk, 0 );
    Seq_NodeSetLValueP( pObj, 0.0 );
    Seq_NodeSetLValueN( pObj, 0.0 );
    Abc_NtkForEachPi( pNtk, pObj, i )
    {
        Seq_NodeSetLValueP( pObj, 0.0 );
        Seq_NodeSetLValueN( pObj, DelayInv );
    }

    // update all values iteratively
    Counter = 0;
    for ( c = 0; c < p->nMaxIters; c++ )
    {
        fChange = 0;
        Abc_AigForEachAnd( pNtk, pObj, i )
        {
            Counter++;
            RetValue = Seq_MapNodeUpdateLValue( pObj, Fi, DelayInv );
            if ( RetValue == SEQ_UPDATE_YES ) 
                fChange = 1;
        }
        Abc_NtkForEachPo( pNtk, pObj, i )
        {
            RetValue = Seq_MapNodeUpdateLValue( pObj, Fi, DelayInv );
            if ( RetValue == SEQ_UPDATE_FAIL )
                break;
        }
        if ( RetValue == SEQ_UPDATE_FAIL )
            break;
        if ( fChange == 0 )
            break;
    }
    if ( c == p->nMaxIters )
    {
        RetValue = SEQ_UPDATE_FAIL;
        pReason = "(timeout)";
    }
    else
        c++;

    // report the results
    if ( fVerbose )
    {
        if ( RetValue == SEQ_UPDATE_FAIL )
            printf( "Period = %6.2f.  Iterations = %3d.  Updates = %10d.    Infeasible %s\n", Fi, c, Counter, pReason );
        else
            printf( "Period = %6.2f.  Iterations = %3d.  Updates = %10d.      Feasible\n",    Fi, c, Counter );
    }
    return RetValue != SEQ_UPDATE_FAIL;
}




/**Function*************************************************************

  Synopsis    [Computes the l-value of the cut.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
float Seq_MapSuperGetArrival( Abc_Obj_t * pObj, float Fi, Seq_Match_t * pMatch, float DelayMax )
{
    Abc_Seq_t * p = pObj->pNtk->pManFunc;
    Abc_Obj_t * pFanin;
    float lValueCur, lValueMax;
    int i;
    lValueMax = -ABC_INFINITY;
    for ( i = pMatch->pCut->nLeaves - 1; i >= 0; i-- )
    {
        // get the arrival time of the fanin
        pFanin = Abc_NtkObj( pObj->pNtk, pMatch->pCut->pLeaves[i] >> 8 );
        if ( pMatch->uPhase & (1 << i) )
            lValueCur = Seq_NodeGetLValueN(pFanin) - Fi * (pMatch->pCut->pLeaves[i] & 255);
        else
            lValueCur = Seq_NodeGetLValueP(pFanin) - Fi * (pMatch->pCut->pLeaves[i] & 255);
        // add the arrival time of this pin
        if ( lValueMax < lValueCur + pMatch->pSuper->tDelaysR[i].Worst )
            lValueMax = lValueCur + pMatch->pSuper->tDelaysR[i].Worst;
        if ( lValueMax < lValueCur + pMatch->pSuper->tDelaysF[i].Worst )
            lValueMax = lValueCur + pMatch->pSuper->tDelaysF[i].Worst;
        if ( lValueMax > DelayMax + p->fEpsilon )
            return ABC_INFINITY;
    }
    return lValueMax;
}

/**Function*************************************************************

  Synopsis    [Computes the l-value of the cut.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
float Seq_MapNodeComputeCut(  Abc_Obj_t * pObj, Cut_Cut_t * pCut, int fCompl, float Fi, Seq_Match_t * pMatchBest )
{
    Seq_Match_t Match, * pMatchCur = &Match;
    Abc_Seq_t * p = pObj->pNtk->pManFunc;
    Map_Super_t * pSuper, * pSuperList;
    unsigned uCanon[2];
    float lValueBest, lValueCur;
    int i;
    assert( pCut->nLeaves < 6 );
    // get the canonical truth table of this cut
    uCanon[0] = uCanon[1] = (fCompl? pCut->uCanon0 : pCut->uCanon1);
    // match the given phase of the cut
    pSuperList = Map_SuperTableLookupC( p->pSuperLib, uCanon );
    // compute the arrival times of each supergate
    lValueBest = ABC_INFINITY;
    for ( pSuper = pSuperList; pSuper; pSuper = pSuper->pNext )
    {
        // create the match
        pMatchCur->pCut   = pCut;
        pMatchCur->pSuper = pSuper;
        // get the phase
        for ( i = 0; i < (int)pSuper->nPhases; i++ )
        {
            pMatchCur->uPhase = (fCompl? pCut->Num0 : pCut->Num1) ^ pSuper->uPhases[i];
            // find the arrival time of this match
            lValueCur = Seq_MapSuperGetArrival( pObj, Fi, pMatchCur, lValueBest );
            if ( lValueBest > lValueCur )
            {
                lValueBest = lValueCur;
                if ( pMatchBest )
                    *pMatchBest = *pMatchCur;
            }
        }
    }
    return lValueBest;
}

/**Function*************************************************************

  Synopsis    [Computes the l-value of the node.]

  Description [The node can be internal or a PO.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
float Seq_MapNodeComputePhase( Abc_Obj_t * pObj, int fCompl, float Fi, Seq_Match_t * pMatchBest )
{
    Seq_Match_t Match, * pMatchCur = &Match;
    Cut_Cut_t * pList, * pCut;
    float lValueNew, lValueCut;
    // get the list of cuts
    pList = Abc_NodeReadCuts( Seq_NodeCutMan(pObj), pObj );
    // get the arrival time of the best non-trivial cut
    lValueNew = ABC_INFINITY;
    for ( pCut = pList->pNext; pCut; pCut = pCut->pNext )
    {
        lValueCut = Seq_MapNodeComputeCut( pObj, pCut, fCompl, Fi, pMatchBest? pMatchCur : NULL );
        if ( lValueNew > lValueCut )
        {
            lValueNew = lValueCut;
            if ( pMatchBest )
                *pMatchBest = *pMatchCur;
        }
    }
    return lValueNew;
}

/**Function*************************************************************

  Synopsis    [Computes the l-value of the node.]

  Description [The node can be internal or a PO.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Seq_MapNodeUpdateLValue( Abc_Obj_t * pObj, float Fi, float DelayInv )
{
    Abc_Seq_t * p = pObj->pNtk->pManFunc;
    Cut_Cut_t * pList;
    char Use;
    float lValueOld0, lValueOld1, lValue0, lValue1, lValue;
    assert( !Abc_ObjIsPi(pObj) );
    assert( Abc_ObjFaninNum(pObj) > 0 );
    // consider the case of the PO
    if ( Abc_ObjIsPo(pObj) )
    {
        if ( Abc_ObjFaninC0(pObj) ) // PO requires negative polarity
            lValue = Seq_NodeGetLValueN(Abc_ObjFanin0(pObj)) - Fi * Seq_ObjFaninL0(pObj);
        else
            lValue = Seq_NodeGetLValueP(Abc_ObjFanin0(pObj)) - Fi * Seq_ObjFaninL0(pObj);
        return (lValue > Fi + p->fEpsilon)? SEQ_UPDATE_FAIL : SEQ_UPDATE_NO;
    }
    // get the cuts
    pList = Abc_NodeReadCuts( Seq_NodeCutMan(pObj), pObj );
    if ( pList == NULL )
        return SEQ_UPDATE_NO;
    // compute the arrival time of both phases
    lValue0 = Seq_MapNodeComputePhase( pObj, 1, Fi, NULL );
    lValue1 = Seq_MapNodeComputePhase( pObj, 0, Fi, NULL );
    // consider the case when negative phase is too slow
    if ( lValue0 > lValue1 + DelayInv + p->fEpsilon )
        lValue0 = lValue1 + DelayInv, Use = 2;
    else if ( lValue1 > lValue0 + DelayInv + p->fEpsilon )
        lValue1 = lValue0 + DelayInv, Use = 1;
    else
        Use = 3;
    // set the uses of the phases
    Seq_NodeSetUses( pObj, Use );
    // get the old arrival times
    lValueOld0 = Seq_NodeGetLValueN(pObj);
    lValueOld1 = Seq_NodeGetLValueP(pObj);
    // compare 
    if ( lValue0 <= lValueOld0 + p->fEpsilon && lValue1 <= lValueOld1 + p->fEpsilon )
        return SEQ_UPDATE_NO;
    // update the values
    if ( lValue0 > lValueOld0 + p->fEpsilon )
        Seq_NodeSetLValueN( pObj, lValue0 );
    if ( lValue1 > lValueOld1 + p->fEpsilon )
        Seq_NodeSetLValueP( pObj, lValue1 );
    return SEQ_UPDATE_YES;
}



/**Function*************************************************************

  Synopsis    [Derives the parameters of the best mapping/retiming for one node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
float Seq_MapCollectNode_rec( Abc_Obj_t * pAnd, float FiBest, Vec_Ptr_t * vMapping, Vec_Vec_t * vMapCuts )
{
    Seq_Match_t * pMatch;
    Abc_Obj_t * pFanin;
    int k, fCompl, Use;
    float Area;

    // get the polarity of the node
    fCompl = Abc_ObjIsComplement(pAnd);
    pAnd   = Abc_ObjRegular(pAnd);

    // skip visited nodes
    if ( fCompl )
    {
        if ( pAnd->fMarkB )
            return 0.0;
        pAnd->fMarkB = 1;
    }
    else
    {
        if ( pAnd->fMarkA )
            return 0.0;
        pAnd->fMarkA = 1;
    }

    // skip if this is a non-PI node
    if ( !Abc_NodeIsAigAnd(pAnd) )
    {
        if ( Abc_ObjIsPi(pAnd) && fCompl )
            return Mio_LibraryReadAreaInv(Abc_FrameReadLibGen());   
        return 0.0;
    }

    // check the uses of this node
    Use = Seq_NodeGetUses( pAnd );
    if ( fCompl && Use == 2 )  // the neg phase is required; the pos phase is used
    {
        Area = Seq_MapCollectNode_rec( pAnd, FiBest, vMapping, vMapCuts );
        return Area + Mio_LibraryReadAreaInv(Abc_FrameReadLibGen()); 
    }
    if ( !fCompl && Use == 1 )  // the pos phase is required; the neg phase is used
    {
        Area = Seq_MapCollectNode_rec( Abc_ObjNot(pAnd), FiBest, vMapping, vMapCuts );
        return Area + Mio_LibraryReadAreaInv(Abc_FrameReadLibGen()); 
    }

    // get the best match
    pMatch = ALLOC( Seq_Match_t, 1 );
    Seq_MapNodeComputePhase( pAnd, fCompl, FiBest, pMatch );
    pMatch->pAnd    = pAnd;
    pMatch->fCompl  = fCompl;
    pMatch->fCutInv = pMatch->pCut->fCompl;
    pMatch->PolUse  = Use;

    // call for the fanin cuts
    Area = pMatch->pSuper->Area;
    for ( k = 0; k < (int)pMatch->pCut->nLeaves; k++ )
    {
        pFanin = Abc_NtkObj( pAnd->pNtk, pMatch->pCut->pLeaves[k] >> 8 );
        if ( pMatch->uPhase & (1 << k) )
            pFanin = Abc_ObjNot( pFanin ); 
        Area += Seq_MapCollectNode_rec( pFanin, FiBest, vMapping, vMapCuts );
    }

    // add this node
    Vec_PtrPush( vMapping, pMatch );
    for ( k = 0; k < (int)pMatch->pCut->nLeaves; k++ )
        Vec_VecPush( vMapCuts, Vec_PtrSize(vMapping)-1, (void *)pMatch->pCut->pLeaves[k] );

    return Area;
}

/**Function*************************************************************

  Synopsis    [Computes the canonical versions of the truth tables.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Seq_MapCanonicizeTruthTables( Abc_Ntk_t * pNtk )
{
    Abc_Obj_t * pObj;
    Cut_Cut_t * pCut, * pList;
    int i;
    Abc_AigForEachAnd( pNtk, pObj, i )
    {
        pList = Abc_NodeReadCuts( Seq_NodeCutMan(pObj), pObj );
        for ( pCut = pList->pNext; pCut; pCut = pCut->pNext )
            Cut_TruthCanonicize( pCut );
    }
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////
