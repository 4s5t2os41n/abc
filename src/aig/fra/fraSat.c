/**CFile****************************************************************

  FileName    [fraSat.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [New FRAIG package.]

  Synopsis    []

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 30, 2007.]

  Revision    [$Id: fraSat.c,v 1.00 2007/06/30 00:00:00 alanmi Exp $]

***********************************************************************/

#include "fra.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static int Fra_SetActivityFactors( Fra_Man_t * p, Dar_Obj_t * pOld, Dar_Obj_t * pNew );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Runs equivalence test for the two nodes.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Fra_NodesAreEquiv( Fra_Man_t * p, Dar_Obj_t * pOld, Dar_Obj_t * pNew )
{
    int pLits[4], RetValue, RetValue1, nBTLimit, clk, clk2 = clock();

    // make sure the nodes are not complemented
    assert( !Dar_IsComplement(pNew) );
    assert( !Dar_IsComplement(pOld) );
    assert( pNew != pOld );

    // if at least one of the nodes is a failed node, perform adjustments:
    // if the backtrack limit is small, simply skip this node
    // if the backtrack limit is > 10, take the quare root of the limit
    nBTLimit = p->pPars->nBTLimitNode;
/*
    if ( nBTLimit > 0 && (pOld->fFailTfo || pNew->fFailTfo) )
    {
        p->nSatFails++;
        // fail immediately
//        return -1;
        if ( nBTLimit <= 10 )
            return -1;
        nBTLimit = (int)pow(nBTLimit, 0.7);
    }
*/
    p->nSatCalls++;

    // make sure the solver is allocated and has enough variables
    if ( p->pSat == NULL )
    {
        p->pSat = sat_solver_new();
        p->nSatVars = 1;
        sat_solver_setnvars( p->pSat, 1000 );
    }

    // if the nodes do not have SAT variables, allocate them
    Fra_NodeAddToSolver( p, pOld, pNew );

    // prepare variable activity
    Fra_SetActivityFactors( p, pOld, pNew ); 

    // solve under assumptions
    // A = 1; B = 0     OR     A = 1; B = 1 
clk = clock();
    pLits[0] = toLitCond( Fra_ObjSatNum(pOld), 0 );
    pLits[1] = toLitCond( Fra_ObjSatNum(pNew), pOld->fPhase == pNew->fPhase );
//Sat_SolverWriteDimacs( p->pSat, "temp.cnf", pLits, pLits + 2, 1 );
    RetValue1 = sat_solver_solve( p->pSat, pLits, pLits + 2, 
        (sint64)nBTLimit, (sint64)0, 
        p->nBTLimitGlobal, p->nInsLimitGlobal );
p->timeSat += clock() - clk;
    if ( RetValue1 == l_False )
    {
p->timeSatUnsat += clock() - clk;
        pLits[0] = lit_neg( pLits[0] );
        pLits[1] = lit_neg( pLits[1] );
        RetValue = sat_solver_addclause( p->pSat, pLits, pLits + 2 );
        assert( RetValue );
        // continue solving the other implication
        p->nSatCallsUnsat++;
    }
    else if ( RetValue1 == l_True )
    {
p->timeSatSat += clock() - clk;
        Fra_SavePattern( p );
        p->nSatCallsSat++;
        return 0;
    }
    else // if ( RetValue1 == l_Undef )
    {
p->timeSatFail += clock() - clk;
        // mark the node as the failed node
//        if ( pOld != p->pManFraig->pConst1 ) 
//            pOld->fFailTfo = 1;
//        pNew->fFailTfo = 1;
        p->nSatFailsReal++;
        return -1;
    }

    // if the old node was constant 0, we already know the answer
    if ( pOld == p->pManFraig->pConst1 )
    {
        p->nSatProof++;
        return 1;
    }

    // solve under assumptions
    // A = 0; B = 1     OR     A = 0; B = 0 
clk = clock();
    pLits[0] = toLitCond( Fra_ObjSatNum(pOld), 1 );
    pLits[1] = toLitCond( Fra_ObjSatNum(pNew), pOld->fPhase ^ pNew->fPhase );
    RetValue1 = sat_solver_solve( p->pSat, pLits, pLits + 2, 
        (sint64)nBTLimit, (sint64)0, 
        p->nBTLimitGlobal, p->nInsLimitGlobal );
p->timeSat += clock() - clk;
    if ( RetValue1 == l_False )
    {
p->timeSatUnsat += clock() - clk;
        pLits[0] = lit_neg( pLits[0] );
        pLits[1] = lit_neg( pLits[1] );
        RetValue = sat_solver_addclause( p->pSat, pLits, pLits + 2 );
        assert( RetValue );
        p->nSatCallsUnsat++;
    }
    else if ( RetValue1 == l_True )
    {
p->timeSatSat += clock() - clk;
        Fra_SavePattern( p );
        p->nSatCallsSat++;
        return 0;
    }
    else // if ( RetValue1 == l_Undef )
    {
p->timeSatFail += clock() - clk;
        // mark the node as the failed node
//        pOld->fFailTfo = 1;
//        pNew->fFailTfo = 1;
        p->nSatFailsReal++;
        return -1;
    }
/*
    // check BDD proof
    {
        int RetVal;
        PRT( "Sat", clock() - clk2 );
        clk2 = clock();
        RetVal = Fra_NodesAreEquivBdd( pOld, pNew );
//        printf( "%d ", RetVal );
        assert( RetVal );
        PRT( "Bdd", clock() - clk2 );
        printf( "\n" );
    }
*/
    // return SAT proof
    p->nSatProof++;
    return 1;
}

/**Function*************************************************************

  Synopsis    [Runs equivalence test for one node.]

  Description [Returns the fraiged node.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Fra_NodeIsConst( Fra_Man_t * p, Dar_Obj_t * pNew )
{
    int pLits[2], RetValue1, RetValue, clk;

    // make sure the nodes are not complemented
    assert( !Dar_IsComplement(pNew) );
    assert( pNew != p->pManFraig->pConst1 );
    p->nSatCalls++;

    // make sure the solver is allocated and has enough variables
    if ( p->pSat == NULL )
    {
        p->pSat = sat_solver_new();
        p->nSatVars = 1;
        sat_solver_setnvars( p->pSat, 1000 );
    }

    // if the nodes do not have SAT variables, allocate them
    Fra_NodeAddToSolver( p, NULL, pNew );

    // prepare variable activity
    Fra_SetActivityFactors( p, NULL, pNew ); 

    // solve under assumptions
clk = clock();
    pLits[0] = toLitCond( Fra_ObjSatNum(pNew), pNew->fPhase );
    RetValue1 = sat_solver_solve( p->pSat, pLits, pLits + 1, 
        (sint64)p->pPars->nBTLimitMiter, (sint64)0, 
        p->nBTLimitGlobal, p->nInsLimitGlobal );
p->timeSat += clock() - clk;
    if ( RetValue1 == l_False )
    {
p->timeSatUnsat += clock() - clk;
        pLits[0] = lit_neg( pLits[0] );
        RetValue = sat_solver_addclause( p->pSat, pLits, pLits + 1 );
        assert( RetValue );
        // continue solving the other implication
        p->nSatCallsUnsat++;
    }
    else if ( RetValue1 == l_True )
    {
p->timeSatSat += clock() - clk;
        if ( p->pPatWords )
            Fra_SavePattern( p );
        p->nSatCallsSat++;
        return 0;
    }
    else // if ( RetValue1 == l_Undef )
    {
p->timeSatFail += clock() - clk;
        // mark the node as the failed node
//        pNew->fFailTfo = 1;
        p->nSatFailsReal++;
        return -1;
    }

    // return SAT proof
    p->nSatProof++;
    return 1;
}

/**Function*************************************************************

  Synopsis    [Sets variable activities in the cone.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Fra_SetActivityFactors_rec( Fra_Man_t * p, Dar_Obj_t * pObj, int LevelMin, int LevelMax )
{
    Vec_Ptr_t * vFanins;
    Dar_Obj_t * pFanin;
    int i, Counter = 0;
    assert( !Dar_IsComplement(pObj) );
    assert( Fra_ObjSatNum(pObj) );
    // skip visited variables
    if ( Dar_ObjIsTravIdCurrent(p->pManFraig, pObj) )
        return 0;
    Dar_ObjSetTravIdCurrent(p->pManFraig, pObj);
    // add the PI to the list
    if ( pObj->Level <= (unsigned)LevelMin || Dar_ObjIsPi(pObj) )
        return 0;
    // set the factor of this variable
    // (LevelMax-LevelMin) / (pObj->Level-LevelMin) = p->pPars->dActConeBumpMax / ThisBump
    p->pSat->factors[Fra_ObjSatNum(pObj)] = p->pPars->dActConeBumpMax * (pObj->Level - LevelMin)/(LevelMax - LevelMin);
    veci_push(&p->pSat->act_vars, Fra_ObjSatNum(pObj));
    // explore the fanins
    vFanins = Fra_ObjFaninVec( pObj );
    Vec_PtrForEachEntry( vFanins, pFanin, i )
        Counter += Fra_SetActivityFactors_rec( p, Dar_Regular(pFanin), LevelMin, LevelMax );
    return 1 + Counter;
}

/**Function*************************************************************

  Synopsis    [Sets variable activities in the cone.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Fra_SetActivityFactors( Fra_Man_t * p, Dar_Obj_t * pOld, Dar_Obj_t * pNew )
{
    int clk, LevelMin, LevelMax;
    assert( pOld || pNew );
clk = clock(); 
    // reset the active variables
    veci_resize(&p->pSat->act_vars, 0);
    // prepare for traversal
    Dar_ManIncrementTravId( p->pManFraig );
    // determine the min and max level to visit
    assert( p->pPars->dActConeRatio > 0 && p->pPars->dActConeRatio < 1 );
    LevelMax = DAR_MAX( (pNew ? pNew->Level : 0), (pOld ? pOld->Level : 0) );
    LevelMin = (int)(LevelMax * (1.0 - p->pPars->dActConeRatio));
    // traverse
    if ( pOld && !Dar_ObjIsConst1(pOld) )
        Fra_SetActivityFactors_rec( p, pOld, LevelMin, LevelMax );
    if ( pNew && !Dar_ObjIsConst1(pNew) )
        Fra_SetActivityFactors_rec( p, pNew, LevelMin, LevelMax );
//Fra_PrintActivity( p );
p->timeTrav += clock() - clk;
    return 1;
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


