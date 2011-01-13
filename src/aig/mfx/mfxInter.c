/**CFile****************************************************************

  FileName    [mfxInter.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [The good old minimization with complete don't-cares.]

  Synopsis    [Procedures for computing resub function by interpolation.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: mfxInter.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "mfxInt.h"
#include "kit.h"

ABC_NAMESPACE_IMPL_START


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Adds constraints for the two-input AND-gate.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Mfx_SatAddXor( sat_solver * pSat, int iVarA, int iVarB, int iVarC )
{
    lit Lits[3];

    Lits[0] = toLitCond( iVarA, 1 );
    Lits[1] = toLitCond( iVarB, 1 );
    Lits[2] = toLitCond( iVarC, 1 );
    if ( !sat_solver_addclause( pSat, Lits, Lits + 3 ) )
        return 0;

    Lits[0] = toLitCond( iVarA, 1 );
    Lits[1] = toLitCond( iVarB, 0 );
    Lits[2] = toLitCond( iVarC, 0 );
    if ( !sat_solver_addclause( pSat, Lits, Lits + 3 ) )
        return 0;

    Lits[0] = toLitCond( iVarA, 0 );
    Lits[1] = toLitCond( iVarB, 1 );
    Lits[2] = toLitCond( iVarC, 0 );
    if ( !sat_solver_addclause( pSat, Lits, Lits + 3 ) )
        return 0;

    Lits[0] = toLitCond( iVarA, 0 );
    Lits[1] = toLitCond( iVarB, 0 );
    Lits[2] = toLitCond( iVarC, 1 );
    if ( !sat_solver_addclause( pSat, Lits, Lits + 3 ) )
        return 0;

    return 1;
}

/**Function*************************************************************

  Synopsis    [Creates miter for checking resubsitution.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
sat_solver * Mfx_CreateSolverResub( Mfx_Man_t * p, int * pCands, int nCands, int fInvert )
{
    sat_solver * pSat;
    Aig_Obj_t * pObjPo;
    int Lits[2], status, iVar, i, c;

    // get the literal for the output of F
    pObjPo = Aig_ManPo( p->pAigWin, Aig_ManPoNum(p->pAigWin) - Vec_PtrSize(p->vDivs) - 1 );
    Lits[0] = toLitCond( p->pCnf->pVarNums[pObjPo->Id], fInvert );

    // collect the outputs of the divisors
    Vec_IntClear( p->vProjVarsCnf );
    Vec_PtrForEachEntryStart( Aig_Obj_t *, p->pAigWin->vPos, pObjPo, i, Aig_ManPoNum(p->pAigWin) - Vec_PtrSize(p->vDivs) )
    {
        assert( p->pCnf->pVarNums[pObjPo->Id] >= 0 );
        Vec_IntPush( p->vProjVarsCnf, p->pCnf->pVarNums[pObjPo->Id] );
    }
    assert( Vec_IntSize(p->vProjVarsCnf) == Vec_PtrSize(p->vDivs) );

    // start the solver
    pSat = sat_solver_new();
    sat_solver_setnvars( pSat, 2 * p->pCnf->nVars + Vec_PtrSize(p->vDivs) );
    if ( pCands )
        sat_solver_store_alloc( pSat );

    // load the first copy of the clauses
    for ( i = 0; i < p->pCnf->nClauses; i++ )
    {
        if ( !sat_solver_addclause( pSat, p->pCnf->pClauses[i], p->pCnf->pClauses[i+1] ) )
        {
            sat_solver_delete( pSat );
            return NULL;
        }
    }
    // add the clause for the first output of F
    if ( !sat_solver_addclause( pSat, Lits, Lits+1 ) )
    {
        sat_solver_delete( pSat );
        return NULL;
    }

    // bookmark the clauses of A
    if ( pCands )
        sat_solver_store_mark_clauses_a( pSat );

    // transform the literals
    for ( i = 0; i < p->pCnf->nLiterals; i++ )
        p->pCnf->pClauses[0][i] += 2 * p->pCnf->nVars;
    // load the second copy of the clauses
    for ( i = 0; i < p->pCnf->nClauses; i++ )
    {
        if ( !sat_solver_addclause( pSat, p->pCnf->pClauses[i], p->pCnf->pClauses[i+1] ) )
        {
            sat_solver_delete( pSat );
            return NULL;
        }
    }
    // transform the literals
    for ( i = 0; i < p->pCnf->nLiterals; i++ )
        p->pCnf->pClauses[0][i] -= 2 * p->pCnf->nVars;
    // add the clause for the second output of F
    Lits[0] = 2 * p->pCnf->nVars + lit_neg( Lits[0] );
    if ( !sat_solver_addclause( pSat, Lits, Lits+1 ) )
    {
        sat_solver_delete( pSat );
        return NULL;
    }

    if ( pCands )
    {
        // add relevant clauses for EXOR gates
        for ( c = 0; c < nCands; c++ )
        {
            // get the variable number of this divisor
            i = lit_var( pCands[c] ) - 2 * p->pCnf->nVars;
            // get the corresponding SAT variable
            iVar = Vec_IntEntry( p->vProjVarsCnf, i );
            // add the corresponding EXOR gate
            if ( !Mfx_SatAddXor( pSat, iVar, iVar + p->pCnf->nVars, 2 * p->pCnf->nVars + i ) )
            {
                sat_solver_delete( pSat );
                return NULL;
            }
            // add the corresponding clause
            if ( !sat_solver_addclause( pSat, pCands + c, pCands + c + 1 ) )
            {
                sat_solver_delete( pSat );
                return NULL;
            }
        }
        // bookmark the roots
        sat_solver_store_mark_roots( pSat );
    }
    else
    {
        // add the clauses for the EXOR gates - and remember their outputs
        Vec_IntClear( p->vProjVarsSat );
        Vec_IntForEachEntry( p->vProjVarsCnf, iVar, i )
        {
            if ( !Mfx_SatAddXor( pSat, iVar, iVar + p->pCnf->nVars, 2 * p->pCnf->nVars + i ) )
            {
                sat_solver_delete( pSat );
                return NULL;
            }
            Vec_IntPush( p->vProjVarsSat, 2 * p->pCnf->nVars + i );
        }
        assert( Vec_IntSize(p->vProjVarsCnf) == Vec_IntSize(p->vProjVarsSat) );
        // simplify the solver
        status = sat_solver_simplify(pSat);
        if ( status == 0 )
        {
//            printf( "Mfx_CreateSolverResub(): SAT solver construction has failed. Skipping node.\n" );
            sat_solver_delete( pSat );
            return NULL;
        }
    }
    return pSat;
}

/**Function*************************************************************

  Synopsis    [Performs interpolation.]

  Description [Derives the new function of the node.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
unsigned * Mfx_InterplateTruth( Mfx_Man_t * p, int * pCands, int nCands, int fInvert )
{
    sat_solver * pSat;
    Sto_Man_t * pCnf = NULL;
    unsigned * puTruth;
    int nFanins, status;
    int c, i, * pGloVars;

    // derive the SAT solver for interpolation
    pSat = Mfx_CreateSolverResub( p, pCands, nCands, fInvert );

    // solve the problem
    status = sat_solver_solve( pSat, NULL, NULL, (ABC_INT64_T)p->pPars->nBTLimit, (ABC_INT64_T)0, (ABC_INT64_T)0, (ABC_INT64_T)0 );
    if ( status != l_False )
    {
        p->nTimeOuts++;
        return NULL;
    }
    // get the learned clauses
    pCnf = (Sto_Man_t *)sat_solver_store_release( pSat );
    sat_solver_delete( pSat );

    // set the global variables
    pGloVars = Int_ManSetGlobalVars( p->pMan, nCands );
    for ( c = 0; c < nCands; c++ )
    {
        // get the variable number of this divisor
        i = lit_var( pCands[c] ) - 2 * p->pCnf->nVars;
        // get the corresponding SAT variable
        pGloVars[c] = Vec_IntEntry( p->vProjVarsCnf, i );
    }

    // derive the interpolant
    nFanins = Int_ManInterpolate( p->pMan, pCnf, 0, &puTruth );
    Sto_ManFree( pCnf );
    assert( nFanins == nCands );
    return puTruth;
}

/**Function*************************************************************

  Synopsis    [Performs interpolation.]

  Description [Derives the new function of the node.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Mfx_InterplateEval( Mfx_Man_t * p, int * pCands, int nCands )
{
    unsigned * pTruth, uTruth0[2], uTruth1[2];
    int nCounter;
    pTruth = Mfx_InterplateTruth( p, pCands, nCands, 0 );
    if ( nCands == 6 )
    {
        uTruth1[0] = pTruth[0];
        uTruth1[1] = pTruth[1];
    }
    else
    {
        uTruth1[0] = pTruth[0];
        uTruth1[1] = pTruth[0];
    }
    pTruth = Mfx_InterplateTruth( p, pCands, nCands, 1 );
    if ( nCands == 6 )
    {
        uTruth0[0] = ~pTruth[0];
        uTruth0[1] = ~pTruth[1];
    }
    else
    {
        uTruth0[0] = ~pTruth[0];
        uTruth0[1] = ~pTruth[0];
    }
    nCounter  = Extra_WordCountOnes( uTruth0[0] ^ uTruth1[0] );
    nCounter += Extra_WordCountOnes( uTruth0[1] ^ uTruth1[1] );
//    printf( "%d ", nCounter );
    return nCounter;
}


/**Function*************************************************************

  Synopsis    [Performs interpolation.]

  Description [Derives the new function of the node.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Hop_Obj_t * Mfx_Interplate( Mfx_Man_t * p, int * pCands, int nCands )
{
    extern Hop_Obj_t * Kit_GraphToHop( Hop_Man_t * pMan, Kit_Graph_t * pGraph );

    sat_solver * pSat;
    Sto_Man_t * pCnf = NULL;
    unsigned * puTruth;
    Kit_Graph_t * pGraph;
    Hop_Obj_t * pFunc;
    int nFanins, status;
    int c, i, * pGloVars;

//    p->nDcMints += Mfx_InterplateEval( p, pCands, nCands );

    // derive the SAT solver for interpolation
    pSat = Mfx_CreateSolverResub( p, pCands, nCands, 0 );

    // solve the problem
    status = sat_solver_solve( pSat, NULL, NULL, (ABC_INT64_T)p->pPars->nBTLimit, (ABC_INT64_T)0, (ABC_INT64_T)0, (ABC_INT64_T)0 );
    if ( status != l_False )
    {
        p->nTimeOuts++;
        return NULL;
    }
    // get the learned clauses
    pCnf = (Sto_Man_t *)sat_solver_store_release( pSat );
    sat_solver_delete( pSat );

    // set the global variables
    pGloVars = Int_ManSetGlobalVars( p->pMan, nCands );
    for ( c = 0; c < nCands; c++ )
    {
        // get the variable number of this divisor
        i = lit_var( pCands[c] ) - 2 * p->pCnf->nVars;
        // get the corresponding SAT variable
        pGloVars[c] = Vec_IntEntry( p->vProjVarsCnf, i );
    }

    // derive the interpolant
    nFanins = Int_ManInterpolate( p->pMan, pCnf, 0, &puTruth );
    Sto_ManFree( pCnf );
    assert( nFanins == nCands );

    // transform interpolant into AIG
    pGraph = Kit_TruthToGraph( puTruth, nFanins, p->vMem );
    pFunc = Kit_GraphToHop( p->pNtk->pManHop, pGraph );
    Kit_GraphFree( pGraph );
    return pFunc;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

