/**CFile****************************************************************

  FileName    [mfsSat.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [The good old minimization with complete don't-cares.]

  Synopsis    [Procedures to compute don't-cares using SAT.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: mfsSat.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "mfsInt.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Enumerates through the SAT assignments.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_NtkMfsSolveSat_iter( Mfs_Man_t * p )
{
    int Lits[MFS_FANIN_MAX];
    int RetValue, nBTLimit, iVar, b, Mint;
    if ( p->nTotConfLim && p->nTotConfLim <= p->pSat->stats.conflicts )
        return -1;
    nBTLimit = p->nTotConfLim? p->nTotConfLim - p->pSat->stats.conflicts : 0;
    RetValue = sat_solver_solve( p->pSat, NULL, NULL, (sint64)nBTLimit, (sint64)0, (sint64)0, (sint64)0 );
    assert( RetValue == l_Undef || RetValue == l_True || RetValue == l_False );
    if ( RetValue == l_Undef )
        return -1;
    if ( RetValue == l_False )
        return 0;
    p->nCares++;
    // add SAT assignment to the solver
    Mint = 0;
    Vec_IntForEachEntry( p->vProjVars, iVar, b )
    {
        Lits[b] = toLit( iVar );
        if ( sat_solver_var_value( p->pSat, iVar ) )
        {
            Mint |= (1 << b);
            Lits[b] = lit_neg( Lits[b] );
        }
    }
    assert( !Aig_InfoHasBit(p->uCare, Mint) );
    Aig_InfoSetBit( p->uCare, Mint );
    // add the blocking clause
    RetValue = sat_solver_addclause( p->pSat, Lits, Lits + Vec_IntSize(p->vProjVars) );
    if ( RetValue == 0 )
        return 0;
    return 1;
}

/**Function*************************************************************

  Synopsis    [Enumerates through the SAT assignments.]

  Description []
               
  SideEffects []

  SeeAlso     []
 
***********************************************************************/
int Abc_NtkMfsSolveSat( Mfs_Man_t * p, Abc_Obj_t * pNode )
{
    Aig_Obj_t * pObjPo;
    int RetValue, i;
    // collect projection variables
    Vec_IntClear( p->vProjVars );
    Vec_PtrForEachEntryStart( p->pAigWin->vPos, pObjPo, i, Aig_ManPoNum(p->pAigWin) - Abc_ObjFaninNum(pNode) )
    {
        assert( p->pCnf->pVarNums[pObjPo->Id] >= 0 );
        Vec_IntPush( p->vProjVars, p->pCnf->pVarNums[pObjPo->Id] );
    }

    // prepare the truth table of care set
    p->nFanins = Vec_IntSize( p->vProjVars );
    p->nWords = Aig_TruthWordNum( p->nFanins );
    memset( p->uCare, 0, sizeof(unsigned) * p->nWords );

    // iterate through the SAT assignments
    p->nCares = 0;
    p->nTotConfLim = p->pPars->nBTLimit;
    while ( (RetValue = Abc_NtkMfsSolveSat_iter(p)) == 1 );
    if ( RetValue == -1 )
        return 0;

    // write statistics
    p->nMintsCare  += p->nCares;
    p->nMintsTotal += (1<<p->nFanins);

    if ( p->pPars->fVeryVerbose )
    {
        printf( "Node %4d : Care = %2d. Total = %2d.  ", pNode->Id, p->nCares, (1<<p->nFanins) );
        Extra_PrintBinary( stdout, p->uCare, (1<<p->nFanins) );
        printf( "\n" );
    }

    // map the care 
    if ( p->nFanins > 4 )
        return 1;
    if ( p->nFanins == 4 )
        p->uCare[0] = p->uCare[0] | (p->uCare[0] << 16);
    if ( p->nFanins == 3 )
        p->uCare[0] = p->uCare[0] | (p->uCare[0] << 8) | (p->uCare[0] << 16) | (p->uCare[0] << 24);
    if ( p->nFanins == 2 )
        p->uCare[0] = p->uCare[0] | (p->uCare[0] <<  4) | (p->uCare[0] <<  8) | (p->uCare[0] << 12) |
              (p->uCare[0] << 16) | (p->uCare[0] << 20) | (p->uCare[0] << 24) | (p->uCare[0] << 28);
    assert( p->nFanins != 1 );
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


