/**CFile****************************************************************

  FileName    [intMan.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Interpolation engine.]

  Synopsis    [Interpolation manager procedures.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 24, 2008.]

  Revision    [$Id: intMan.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "intInt.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Creates the interpolation manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Inter_Man_t * Inter_ManCreate( Aig_Man_t * pAig, Inter_ManParams_t * pPars )
{
    Inter_Man_t * p;
    // create interpolation manager
    p = ABC_ALLOC( Inter_Man_t, 1 );
    memset( p, 0, sizeof(Inter_Man_t) );
    p->vVarsAB = Vec_IntAlloc( Aig_ManRegNum(pAig) );
    p->nConfLimit = pPars->nBTLimit;
    p->fVerbose = pPars->fVerbose;
    p->pAig = pAig;   
    return p;
}

/**Function*************************************************************

  Synopsis    [Cleans the interpolation manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Inter_ManClean( Inter_Man_t * p )
{
    if ( p->pCnfInter )
        Cnf_DataFree( p->pCnfInter );
    if ( p->pCnfFrames )
        Cnf_DataFree( p->pCnfFrames );
    if ( p->pInter )
        Aig_ManStop( p->pInter );
    if ( p->pFrames )
        Aig_ManStop( p->pFrames );
}

/**Function*************************************************************

  Synopsis    [Frees the interpolation manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Inter_ManStop( Inter_Man_t * p )
{
    if ( p->fVerbose )
    {
        p->timeOther = p->timeTotal-p->timeRwr-p->timeCnf-p->timeSat-p->timeInt-p->timeEqu;
        printf( "Runtime statistics:\n" );
        ABC_PRTP( "Rewriting  ", p->timeRwr,   p->timeTotal );
        ABC_PRTP( "CNF mapping", p->timeCnf,   p->timeTotal );
        ABC_PRTP( "SAT solving", p->timeSat,   p->timeTotal );
        ABC_PRTP( "Interpol   ", p->timeInt,   p->timeTotal );
        ABC_PRTP( "Containment", p->timeEqu,   p->timeTotal );
        ABC_PRTP( "Other      ", p->timeOther, p->timeTotal );
        ABC_PRTP( "TOTAL      ", p->timeTotal, p->timeTotal );
    }

    if ( p->pCnfAig )
        Cnf_DataFree( p->pCnfAig );
    if ( p->pCnfFrames )
        Cnf_DataFree( p->pCnfFrames );
    if ( p->pCnfInter )
        Cnf_DataFree( p->pCnfInter );
    Vec_IntFree( p->vVarsAB );
    if ( p->pAigTrans )
        Aig_ManStop( p->pAigTrans );
    if ( p->pFrames )
        Aig_ManStop( p->pFrames );
    if ( p->pInter )
        Aig_ManStop( p->pInter );
    if ( p->pInterNew )
        Aig_ManStop( p->pInterNew );
    ABC_FREE( p );
}



////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


