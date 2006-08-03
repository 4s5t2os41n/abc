/**CFile****************************************************************

  FileName    [added.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [C-language MiniSat solver.]

  Synopsis    [Additional SAT solver procedures.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: added.c,v 1.4 2005/09/16 22:55:03 casem Exp $]

***********************************************************************/

#include <stdio.h>
#include <assert.h>
#include "solver.h"
#include "extra.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

struct clause_t
{
    int size_learnt;
    lit lits[0];
};

static inline int   clause_size       (clause* c)          { return c->size_learnt >> 1; }
static inline lit*  clause_begin      (clause* c)          { return c->lits; }

static inline int  lit_var(lit l)  { return l >> 1; }
static inline int  lit_sign(lit l) { return (l & 1); }

static void Asat_ClauseWriteDimacs( FILE * pFile, clause * pC, bool fIncrement );

extern void Io_WriteCnfOutputPiMapping( FILE * pFile, int incrementVars );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Write the clauses in the solver into a file in DIMACS format.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Asat_SolverWriteDimacs( solver * p, char * pFileName, lit* assumptionsBegin, lit* assumptionsEnd, int incrementVars )
{
    FILE * pFile;
    void ** pClauses;
    int nClauses, i;

    // count the number of clauses
    nClauses = p->clauses.size + p->learnts.size;
    for ( i = 0; i < p->size; i++ )
        if ( p->levels[i] == 0 && p->assigns[i] != l_Undef )
            nClauses++;

    // start the file
    pFile = fopen( pFileName, "wb" );
    if ( pFile == NULL )
    {
        printf( "Asat_SolverWriteDimacs(): Cannot open the ouput file.\n" );
        return;
    }
    fprintf( pFile, "c CNF generated by ABC on %s\n", Extra_TimeStamp() );
//    Io_WriteCnfOutputPiMapping( pFile, incrementVars );
    fprintf( pFile, "p cnf %d %d\n", p->size, nClauses );

    // write the original clauses
    nClauses = p->clauses.size;
    pClauses = p->clauses.ptr;
    for ( i = 0; i < nClauses; i++ )
        Asat_ClauseWriteDimacs( pFile, pClauses[i], incrementVars );

    // write the learned clauses
    nClauses = p->learnts.size;
    pClauses = p->learnts.ptr;
    for ( i = 0; i < nClauses; i++ )
        Asat_ClauseWriteDimacs( pFile, pClauses[i], incrementVars );

    // write zero-level assertions
    for ( i = 0; i < p->size; i++ )
        if ( p->levels[i] == 0 && p->assigns[i] != l_Undef )
            fprintf( pFile, "%s%d%s\n",
                     (p->assigns[i] == l_False)? "-": "",
                     i + (int)(incrementVars>0),
                     (incrementVars) ? " 0" : "");

    // write the assumptions
    if (assumptionsBegin) {
        for (; assumptionsBegin != assumptionsEnd; assumptionsBegin++) {
            fprintf( pFile, "%s%d%s\n",
                     lit_sign(*assumptionsBegin)? "-": "",
                     lit_var(*assumptionsBegin) + (int)(incrementVars>0),
                     (incrementVars) ? " 0" : "");
        }
    }

    fprintf( pFile, "\n" );
    fclose( pFile );
}   

/**Function*************************************************************

  Synopsis    [Writes the given clause in a file in DIMACS format.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Asat_ClauseWriteDimacs( FILE * pFile, clause * pC, bool fIncrement )
{
    lit * pLits = clause_begin(pC);
    int nLits = clause_size(pC);
    int i;

    for ( i = 0; i < nLits; i++ )
        fprintf( pFile, "%s%d ", (lit_sign(pLits[i])? "-": ""),  lit_var(pLits[i]) + (int)(fIncrement>0) );
    if ( fIncrement )
        fprintf( pFile, "0" );
    fprintf( pFile, "\n" );
}

/**Function*************************************************************

  Synopsis    [Returns a counter-example.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int * solver_get_model( solver * p, int * pVars, int nVars )
{
    int * pModel;
    int i;
    pModel = ALLOC( int, nVars );
    for ( i = 0; i < nVars; i++ )
    {
        assert( pVars[i] >= 0 && pVars[i] < p->size );
        pModel[i] = (int)(p->model.ptr[pVars[i]] == l_True);
    }
    return pModel;    
}

/**Function*************************************************************

  Synopsis    [Writes the given clause in a file in DIMACS format.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Asat_SatPrintStats( FILE * pFile, solver * p )
{
    printf( "Start = %4d. Conf = %6d. Dec = %6d. Prop = %7d. Insp = %8d.\n",
        (int)p->solver_stats.starts, 
        (int)p->solver_stats.conflicts,
        (int)p->solver_stats.decisions,
        (int)p->solver_stats.propagations,
        (int)p->solver_stats.inspects );
    printf( "Total runtime = %7.2f sec.  Var select = %7.2f sec.  Var update = %7.2f sec.\n",
        (float)(p->timeTotal)/(float)(CLOCKS_PER_SEC),
        (float)(p->timeSelect)/(float)(CLOCKS_PER_SEC),
        (float)(p->timeUpdate)/(float)(CLOCKS_PER_SEC) );
}

/**Function*************************************************************

  Synopsis    [Sets the preferred variables.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Asat_SolverSetPrefVars(solver * s, int * pPrefVars, int nPrefVars)
{
    int i;
    assert( s->pPrefVars == NULL );
    for ( i = 0; i < nPrefVars; i++ )
        assert( pPrefVars[i] < s->size );
    s->pPrefVars = pPrefVars;
    s->nPrefVars = nPrefVars;
}

/**Function*************************************************************

  Synopsis    [Sets the preferred variables.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Asat_SolverSetFactors(solver * s, int * pFactors)
{
    assert( s->factors == NULL );
    s->factors = pFactors;
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


