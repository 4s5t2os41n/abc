/**CFile****************************************************************

  FileName    [kitDsd.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Computation kit.]

  Synopsis    [Performs disjoint-support decomposition based on truth tables.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - Dec 6, 2006.]

  Revision    [$Id: kitDsd.c,v 1.00 2006/12/06 00:00:00 alanmi Exp $]

***********************************************************************/

#include "kit.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Allocates the DSD manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Kit_DsdMan_t * Kit_DsdManAlloc( int nVars )
{
    Kit_DsdMan_t * p;
    p = ALLOC( Kit_DsdMan_t, 1 );
    memset( p, 0, sizeof(Kit_DsdMan_t) );
    p->nVars = nVars;
    p->nWords = Kit_TruthWordNum( p->nVars );
    p->vTtElems = Vec_PtrAllocTruthTables( p->nVars );
    p->vTtNodes = Vec_PtrAllocSimInfo( 64, p->nWords );
    return p;
}

/**Function*************************************************************

  Synopsis    [Deallocates the DSD manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdManFree( Kit_DsdMan_t * p )
{
    Vec_PtrFree( p->vTtElems );
    Vec_PtrFree( p->vTtNodes );
    free( p );
}

/**Function*************************************************************

  Synopsis    [Allocates the DSD node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Kit_DsdObj_t * Kit_DsdObjAlloc( Kit_DsdNtk_t * pNtk, Kit_Dsd_t Type, int nFans )
{
    Kit_DsdObj_t * pObj;
    int nSize = sizeof(Kit_DsdObj_t) + sizeof(unsigned) * (Kit_DsdObjOffset(nFans) + (Type == KIT_DSD_PRIME) * Kit_TruthWordNum(nFans));
    pObj = (Kit_DsdObj_t *)ALLOC( char, nSize );
    memset( pObj, 0, nSize );
    pObj->Id = pNtk->nVars + pNtk->nNodes;
    pObj->Type = Type;
    pObj->nFans = nFans;
    pObj->Offset = Kit_DsdObjOffset( nFans );
    // add the object
    assert( pNtk->nNodes < pNtk->nNodesAlloc );
    pNtk->pNodes[pNtk->nNodes++] = pObj;
    return pObj;
}

/**Function*************************************************************

  Synopsis    [Deallocates the DSD node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdObjFree( Kit_DsdNtk_t * p, Kit_DsdObj_t * pObj )
{
    free( pObj );
}

/**Function*************************************************************

  Synopsis    [Allocates the DSD network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Kit_DsdNtk_t * Kit_DsdNtkAlloc( int nVars )
{
    Kit_DsdNtk_t * pNtk;
    int nSize = sizeof(Kit_DsdNtk_t) + sizeof(void *) * nVars;
    // allocate the network
    pNtk = (Kit_DsdNtk_t *)ALLOC( char, nSize );
    memset( pNtk, 0, nSize );
    pNtk->nVars = nVars;
    pNtk->nNodesAlloc = nVars;
    pNtk->pMem = ALLOC( unsigned, 6 * Kit_TruthWordNum(nVars) );
    return pNtk;
}

/**Function*************************************************************

  Synopsis    [Deallocate the DSD network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdNtkFree( Kit_DsdNtk_t * pNtk )
{
    Kit_DsdObj_t * pObj;
    unsigned i;
    Kit_DsdNtkForEachObj( pNtk, pObj, i )
        free( pObj );
    free( pNtk->pMem );
    free( pNtk );
}

/**Function*************************************************************

  Synopsis    [Prints the hex unsigned into a file.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdPrintHex( FILE * pFile, unsigned * pTruth, int nFans )
{
    int nDigits, Digit, k;
    nDigits = (1 << nFans) / 4;
    for ( k = nDigits - 1; k >= 0; k-- )
    {
        Digit = ((pTruth[k/8] >> ((k%8) * 4)) & 15);
        if ( Digit < 10 )
            fprintf( pFile, "%d", Digit );
        else
            fprintf( pFile, "%c", 'A' + Digit-10 );
    }
}

/**Function*************************************************************

  Synopsis    [Recursively print the DSD formula.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdPrint_rec( FILE * pFile, Kit_DsdNtk_t * pNtk, int Id )
{
    Kit_DsdObj_t * pObj;
    unsigned iLit, i;
    char Symbol;

    pObj = Kit_DsdNtkObj( pNtk, Id );
    if ( pObj == NULL )
    {
        assert( Id < pNtk->nVars );
        fprintf( pFile, "%c", 'a' + Id );
        return;
    }

    if ( pObj->Type == KIT_DSD_CONST1 )
    {
        assert( pObj->nFans == 0 );
        fprintf( pFile, "Const1" );
        return;
    }

    if ( pObj->Type == KIT_DSD_VAR )
        assert( pObj->nFans == 1 );

    if ( pObj->Type == KIT_DSD_AND )
        Symbol = '*';
    else if ( pObj->Type == KIT_DSD_XOR )
        Symbol = '+';
    else 
        Symbol = ',';

    if ( pObj->Type == KIT_DSD_PRIME )
        Kit_DsdPrintHex( stdout, Kit_DsdObjTruth(pObj), pObj->nFans );

    fprintf( pFile, "(" );
    Kit_DsdObjForEachFanin( pNtk, pObj, iLit, i )
    {
        if ( Kit_DsdLitIsCompl(iLit) ) 
            fprintf( pFile, "!" );
        Kit_DsdPrint_rec( pFile, pNtk, Kit_DsdLit2Var(iLit) );
        if ( i < pObj->nFans - 1 )
            fprintf( pFile, "%c", Symbol );
    }
    fprintf( pFile, ")" );
}

/**Function*************************************************************

  Synopsis    [Print the DSD formula.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdPrint( FILE * pFile, Kit_DsdNtk_t * pNtk )
{
    fprintf( pFile, "F = " );
    if ( Kit_DsdLitIsCompl(pNtk->Root) )
        fprintf( pFile, "!" );
    Kit_DsdPrint_rec( pFile, pNtk, Kit_DsdLit2Var(pNtk->Root) );
    fprintf( pFile, "\n" );
}

/**Function*************************************************************

  Synopsis    [Derives the truth table of the DSD node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
unsigned * Kit_DsdTruthComputeNode_rec( Kit_DsdMan_t * p, Kit_DsdNtk_t * pNtk, int Id )
{
    Kit_DsdObj_t * pObj;
    unsigned * pTruthRes, * pTruthPrime, * pTruthMint, * pTruthFans[16];
    unsigned i, m, iLit, nMints, fCompl;

    // get the node with this ID
    pObj = Kit_DsdNtkObj( pNtk, Id );
    pTruthRes = Vec_PtrEntry( p->vTtNodes, Id );

    // special case: literal of an internal node
    if ( pObj == NULL )
    {
        assert( Id < pNtk->nVars );
        return pTruthRes;
    }

    // constant node
    if ( pObj->Type == KIT_DSD_CONST1 )
    {
        assert( pObj->nFans == 0 );
        Kit_TruthFill( pTruthRes, pNtk->nVars );
        return pTruthRes;
    }

    // elementary variable node
    if ( pObj->Type == KIT_DSD_VAR )
    {
        assert( pObj->nFans == 1 );
        iLit = pObj->pFans[0];
        pTruthFans[0] = Kit_DsdTruthComputeNode_rec( p, pNtk, Kit_DsdLit2Var(iLit) );
        if ( Kit_DsdLitIsCompl(iLit) )
            Kit_TruthNot( pTruthRes, pTruthFans[0], pNtk->nVars );
        else
            Kit_TruthCopy( pTruthRes, pTruthFans[0], pNtk->nVars );
        return pTruthRes;
    }

    // collect the truth tables of the fanins
    Kit_DsdObjForEachFanin( pNtk, pObj, iLit, i )
        pTruthFans[i] = Kit_DsdTruthComputeNode_rec( p, pNtk, Kit_DsdLit2Var(iLit) );
    // create the truth table

    // simple gates
    if ( pObj->Type == KIT_DSD_AND )
    {
        Kit_TruthFill( pTruthRes, pNtk->nVars );
        Kit_DsdObjForEachFanin( pNtk, pObj, iLit, i )
            Kit_TruthAndPhase( pTruthRes, pTruthRes, pTruthFans[i], pNtk->nVars, 0, Kit_DsdLitIsCompl(iLit) );
        return pTruthRes;
    }
    if ( pObj->Type == KIT_DSD_XOR )
    {
        Kit_TruthClear( pTruthRes, pNtk->nVars );
        fCompl = 0;
        Kit_DsdObjForEachFanin( pNtk, pObj, iLit, i )
        {
            Kit_TruthXor( pTruthRes, pTruthRes, pTruthFans[i], pNtk->nVars );
            fCompl ^= Kit_DsdLitIsCompl(iLit);
        }
        if ( fCompl )
            Kit_TruthNot( pTruthRes, pTruthRes, pNtk->nVars );
        return pTruthRes;
    }
    assert( pObj->Type == KIT_DSD_PRIME );

    // get the truth table of the prime node
    pTruthPrime = Kit_DsdObjTruth( pObj );
    // get storage for the temporary minterm
    pTruthMint = Vec_PtrEntry(p->vTtNodes, pNtk->nVars + pNtk->nNodes);

    // go through the minterms
    nMints = (1 << pObj->nFans);
    Kit_TruthClear( pTruthRes, pNtk->nVars );
    for ( m = 0; m < nMints; m++ )
    {
        if ( !Kit_TruthHasBit(pTruthPrime, m) )
            continue;
        Kit_TruthFill( pTruthMint, pNtk->nVars );
        Kit_DsdObjForEachFanin( pNtk, pObj, iLit, i )
            Kit_TruthAndPhase( pTruthMint, pTruthMint, pTruthFans[i], pNtk->nVars, 0, ((m & (1<<i)) == 0) ^ Kit_DsdLitIsCompl(iLit) );
        Kit_TruthOr( pTruthRes, pTruthRes, pTruthMint, pNtk->nVars );
    }
    return pTruthRes;
}

/**Function*************************************************************

  Synopsis    [Derives the truth table of the DSD network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
unsigned * Kit_DsdTruthCompute( Kit_DsdMan_t * p, Kit_DsdNtk_t * pNtk )
{
    unsigned * pTruthRes;
    int i;
    // assign elementary truth ables
    assert( pNtk->nVars <= p->nVars );
    for ( i = 0; i < (int)pNtk->nVars; i++ )
        Kit_TruthCopy( Vec_PtrEntry(p->vTtNodes, i), Vec_PtrEntry(p->vTtElems, i), p->nVars );
    // compute truth table for each node
    pTruthRes = Kit_DsdTruthComputeNode_rec( p, pNtk, Kit_DsdLit2Var(pNtk->Root) );
    // complement the truth table if needed
    if ( Kit_DsdLitIsCompl(pNtk->Root) )
        Kit_TruthNot( pTruthRes, pTruthRes, pNtk->nVars );
    return pTruthRes;
}


/**Function*************************************************************

  Synopsis    [Counts the number of blocks of the given number of inputs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdCountLuts_rec( Kit_DsdNtk_t * pNtk, int nLutSize, int Id, int * pCounter )
{
    Kit_DsdObj_t * pObj;
    unsigned iLit, i, Res0, Res1;
    pObj = Kit_DsdNtkObj( pNtk, Id );
    if ( pObj == NULL )
        return 0;
    if ( pObj->Type == KIT_DSD_AND || pObj->Type == KIT_DSD_XOR )
    {
        assert( pObj->nFans == 2 );
        Res0 = Kit_DsdCountLuts_rec( pNtk, nLutSize, Kit_DsdLit2Var(pObj->pFans[0]), pCounter );
        Res1 = Kit_DsdCountLuts_rec( pNtk, nLutSize, Kit_DsdLit2Var(pObj->pFans[1]), pCounter );
        if ( Res0 == 0 && Res1 > 0 )
            return Res1 - 1;
        if ( Res0 > 0 && Res1 == 0 )
            return Res0 - 1;
        (*pCounter)++;
        return nLutSize - 2;
    }
    assert( pObj->Type == KIT_DSD_PRIME );
    if ( (int)pObj->nFans > nLutSize ) //+ 1 )
    {
        *pCounter = 1000;
        return 0;
    }
    Kit_DsdObjForEachFanin( pNtk, pObj, iLit, i )
        Kit_DsdCountLuts_rec( pNtk, nLutSize, Kit_DsdLit2Var(iLit), pCounter );
    (*pCounter)++;
//    if ( (int)pObj->nFans == nLutSize + 1 )
//        (*pCounter)++;
    return nLutSize - pObj->nFans;
}

/**Function*************************************************************

  Synopsis    [Counts the number of blocks of the given number of inputs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdCountLuts( Kit_DsdNtk_t * pNtk, int nLutSize )
{
    int Counter = 0;
    if ( Kit_DsdNtkRoot(pNtk)->Type == KIT_DSD_CONST1 )
        return 0;
    if ( Kit_DsdNtkRoot(pNtk)->Type == KIT_DSD_VAR )
        return 0;
    Kit_DsdCountLuts_rec( pNtk, nLutSize, Kit_DsdLit2Var(pNtk->Root), &Counter );
    if ( Counter >= 1000 )
        return -1;
    return Counter;
}

/**Function*************************************************************

  Synopsis    [Counts the number of blocks of the given number of inputs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdNonDsdSizeMax( Kit_DsdNtk_t * pNtk )
{
    Kit_DsdObj_t * pObj;
    unsigned i, nSizeMax = 0;
    Kit_DsdNtkForEachObj( pNtk, pObj, i )
    {
        if ( pObj->Type != KIT_DSD_PRIME )
            continue;
        if ( nSizeMax < pObj->nFans )
            nSizeMax = pObj->nFans;
    }
    return nSizeMax;
}


/**Function*************************************************************

  Synopsis    [Expands the node.]

  Description [Returns the new literal.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdExpandCollectAnd_rec( Kit_DsdNtk_t * p, int iLit, int * piLitsNew, int * nLitsNew )
{
    Kit_DsdObj_t * pObj;
    unsigned i, iLitFanin;
    // check the end of the supergate
    pObj = Kit_DsdNtkObj( p, Kit_DsdLit2Var(iLit) );
    if ( Kit_DsdLitIsCompl(iLit) || Kit_DsdLit2Var(iLit) < p->nVars || pObj->Type != KIT_DSD_AND )
    {
        piLitsNew[(*nLitsNew)++] = iLit;
        return;
    }
    // iterate through the fanins
    Kit_DsdObjForEachFanin( p, pObj, iLitFanin, i )
        Kit_DsdExpandCollectAnd_rec( p, iLitFanin, piLitsNew, nLitsNew );
}

/**Function*************************************************************

  Synopsis    [Expands the node.]

  Description [Returns the new literal.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdExpandCollectXor_rec( Kit_DsdNtk_t * p, int iLit, int * piLitsNew, int * nLitsNew )
{
    Kit_DsdObj_t * pObj;
    unsigned i, iLitFanin;
    // check the end of the supergate
    pObj = Kit_DsdNtkObj( p, Kit_DsdLit2Var(iLit) );
    if ( Kit_DsdLit2Var(iLit) < p->nVars || pObj->Type != KIT_DSD_XOR )
    {
        piLitsNew[(*nLitsNew)++] = iLit;
        return;
    }
    // iterate through the fanins
    pObj = Kit_DsdNtkObj( p, Kit_DsdLit2Var(iLit) );
    Kit_DsdObjForEachFanin( p, pObj, iLitFanin, i )
        Kit_DsdExpandCollectXor_rec( p, iLitFanin, piLitsNew, nLitsNew );
    // if the literal was complemented, pass the complemented attribute somewhere
    if ( Kit_DsdLitIsCompl(iLit) )
        piLitsNew[0] = Kit_DsdLitNot( piLitsNew[0] );
}

/**Function*************************************************************

  Synopsis    [Expands the node.]

  Description [Returns the new literal.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdExpandNode_rec( Kit_DsdNtk_t * pNew, Kit_DsdNtk_t * p, int iLit )
{
    unsigned * pTruth, * pTruthNew;
    unsigned i, fCompl, iLitFanin, piLitsNew[16], nLitsNew = 0;
    Kit_DsdObj_t * pObj, * pObjNew;

    // remember the complement
    fCompl = Kit_DsdLitIsCompl(iLit); 
    iLit = Kit_DsdLitRegular(iLit); 
    assert( !Kit_DsdLitIsCompl(iLit) );

    // consider the case of simple gate
    pObj = Kit_DsdNtkObj( p, Kit_DsdLit2Var(iLit) );
    if ( pObj == NULL )
        return Kit_DsdLitNotCond( iLit, fCompl );
    if ( pObj->Type == KIT_DSD_AND )
    {
        Kit_DsdExpandCollectAnd_rec( p, iLit, piLitsNew, &nLitsNew );
        pObjNew = Kit_DsdObjAlloc( pNew, KIT_DSD_AND, nLitsNew );
        for ( i = 0; i < pObjNew->nFans; i++ )
            pObjNew->pFans[i] = Kit_DsdExpandNode_rec( pNew, p, piLitsNew[i] );
        return Kit_DsdVar2Lit( pObjNew->Id, fCompl );
    }
    if ( pObj->Type == KIT_DSD_XOR )
    {
        Kit_DsdExpandCollectXor_rec( p, iLit, piLitsNew, &nLitsNew );
        pObjNew = Kit_DsdObjAlloc( pNew, KIT_DSD_XOR, nLitsNew );
        for ( i = 0; i < pObjNew->nFans; i++ )
        {
            pObjNew->pFans[i] = Kit_DsdExpandNode_rec( pNew, p, Kit_DsdLitRegular(piLitsNew[i]) );
            fCompl ^= Kit_DsdLitIsCompl(piLitsNew[i]);
        }
        return Kit_DsdVar2Lit( pObjNew->Id, fCompl );
    }
    assert( pObj->Type == KIT_DSD_PRIME );

    // create new PRIME node
    pObjNew = Kit_DsdObjAlloc( pNew, KIT_DSD_PRIME, pObj->nFans );
    // copy the truth table
    pTruth = Kit_DsdObjTruth( pObj );
    pTruthNew = Kit_DsdObjTruth( pObjNew );
    Kit_TruthCopy( pTruthNew, pTruth, pObj->nFans );
    // create fanins
    Kit_DsdObjForEachFanin( pNtk, pObj, iLitFanin, i )
    {
        pObjNew->pFans[i] = Kit_DsdExpandNode_rec( pNew, p, iLitFanin );
        // complement the corresponding inputs of the truth table
        if ( Kit_DsdLitIsCompl(pObjNew->pFans[i]) )
        {
            pObjNew->pFans[i] = Kit_DsdLitRegular(pObjNew->pFans[i]);
            Kit_TruthChangePhase( pTruthNew, pObjNew->nFans, i );
        }
    }
    // if the incoming phase is complemented, absorb it into the prime node
    if ( fCompl )
        Kit_TruthNot( pTruthNew, pTruthNew, pObj->nFans );
    return Kit_DsdVar2Lit( pObjNew->Id, 0 );
}

/**Function*************************************************************

  Synopsis    [Expands the network.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Kit_DsdNtk_t * Kit_DsdExpand( Kit_DsdNtk_t * p )
{
    Kit_DsdNtk_t * pNew;
    Kit_DsdObj_t * pObjNew;
    assert( p->nVars <= 16 );
    // create a new network
    pNew = Kit_DsdNtkAlloc( p->nVars );
    // consider simple special cases
    if ( Kit_DsdNtkRoot(p)->Type == KIT_DSD_CONST1 )
    {
        pObjNew = Kit_DsdObjAlloc( pNew, KIT_DSD_CONST1, 0 );
        pNew->Root = Kit_DsdVar2Lit( pObjNew->Id, Kit_DsdLitIsCompl(p->Root) );
        return pNew;
    }
    if ( Kit_DsdNtkRoot(p)->Type == KIT_DSD_VAR )
    {
        pObjNew = Kit_DsdObjAlloc( pNew, KIT_DSD_VAR, 1 );
        pObjNew->pFans[0] = Kit_DsdNtkRoot(p)->pFans[0];
        pNew->Root = Kit_DsdVar2Lit( pObjNew->Id, Kit_DsdLitIsCompl(p->Root) );
        return pNew;
    }
    // convert the root node
    pNew->Root = Kit_DsdExpandNode_rec( pNew, p, p->Root );
    return pNew;
}

/**Function*************************************************************

  Synopsis    [Returns 1 if there is a component with more than 3 inputs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdFindLargeBox_rec( Kit_DsdNtk_t * pNtk, int Id, int Size )
{
    Kit_DsdObj_t * pObj;
    unsigned iLit, i, RetValue;
    pObj = Kit_DsdNtkObj( pNtk, Id );
    if ( pObj == NULL )
        return 0;
    if ( pObj->Type == KIT_DSD_PRIME && (int)pObj->nFans > Size )
        return 1;
    RetValue = 0;
    Kit_DsdObjForEachFanin( pNtk, pObj, iLit, i )
        RetValue |= Kit_DsdFindLargeBox_rec( pNtk, Kit_DsdLit2Var(iLit), Size );
    return RetValue;
}

/**Function*************************************************************

  Synopsis    [Returns 1 if there is a component with more than 3 inputs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdFindLargeBox( Kit_DsdNtk_t * pNtk, int Size )
{
    return Kit_DsdFindLargeBox_rec( pNtk, Kit_DsdLit2Var(pNtk->Root), Size );
}

/**Function*************************************************************

  Synopsis    [Returns 1 if the non-DSD 4-var func is implementable with two 3-LUTs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdRootNodeHasCommonVars( Kit_DsdObj_t * pObj0, Kit_DsdObj_t * pObj1 )
{
    unsigned i, k;
    for ( i = 0; i < pObj0->nFans; i++ )
    {
        if ( Kit_DsdLit2Var(pObj0->pFans[i]) >= 4 )
            continue;
        for ( k = 0; k < pObj1->nFans; k++ )
            if ( Kit_DsdLit2Var(pObj0->pFans[i]) == Kit_DsdLit2Var(pObj1->pFans[k]) )
                return 1;
    }
    return 0;
}

/**Function*************************************************************

  Synopsis    [Returns 1 if the non-DSD 4-var func is implementable with two 3-LUTs.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdCheckVar4Dec2( Kit_DsdNtk_t * pNtk0, Kit_DsdNtk_t * pNtk1 )
{
    assert( pNtk0->nVars == 4 );
    assert( pNtk1->nVars == 4 );
    if ( Kit_DsdFindLargeBox(pNtk0, 2) )
        return 0;
    if ( Kit_DsdFindLargeBox(pNtk1, 2) )
        return 0;
    return Kit_DsdRootNodeHasCommonVars( Kit_DsdNtkRoot(pNtk0), Kit_DsdNtkRoot(pNtk1) );
}

/**Function*************************************************************

  Synopsis    [Performs decomposition of the node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdDecompose_rec( Kit_DsdNtk_t * pNtk, Kit_DsdObj_t * pObj, unsigned uSupp, unsigned char * pPar )
{
    Kit_DsdObj_t * pRes, * pRes0, * pRes1;
    int nWords = Kit_TruthWordNum(pObj->nFans);
    unsigned * pTruth = Kit_DsdObjTruth(pObj);
    unsigned * pCofs2[2] = { pNtk->pMem, pNtk->pMem + nWords };
    unsigned * pCofs4[2][2] = { {pNtk->pMem + 2 * nWords, pNtk->pMem + 3 * nWords}, {pNtk->pMem + 4 * nWords, pNtk->pMem + 5 * nWords} };
    int i, iLit0, iLit1, nFans0, nFans1, nPairs;
    int fEquals[2][2], fOppos, fPairs[4][4];
    unsigned j, k, nFansNew, uSupp0, uSupp1;

    assert( pObj->nFans > 0 );
    assert( pObj->Type == KIT_DSD_PRIME );
    assert( uSupp == (uSupp0 = (unsigned)Kit_TruthSupport(pTruth, pObj->nFans)) );

    // compress the truth table
    if ( uSupp != Kit_BitMask(pObj->nFans) )
    {
        nFansNew = Kit_WordCountOnes(uSupp);
        Kit_TruthShrink( pNtk->pMem, pTruth, nFansNew, pObj->nFans, uSupp, 1 );
        for ( j = k = 0; j < pObj->nFans; j++ )
            if ( uSupp & (1 << j) )
                pObj->pFans[k++] = pObj->pFans[j];
        assert( k == nFansNew );
        pObj->nFans = k;
        uSupp = Kit_BitMask(pObj->nFans);
    }

    // consider the single variable case
    if ( pObj->nFans == 1 )
    {
        pObj->Type = KIT_DSD_NONE;
        if ( pTruth[0] == 0x55555555 )
            pObj->pFans[0] = Kit_DsdLitNot(pObj->pFans[0]);
        else
            assert( pTruth[0] == 0xAAAAAAAA );
        // update the parent pointer
        *pPar = Kit_DsdLitNotCond( pObj->pFans[0], Kit_DsdLitIsCompl(*pPar) );
        return;
    }

    // decompose the output
    if ( !pObj->fMark )
    for ( i = pObj->nFans - 1; i >= 0; i-- )
    {
        // get the two-variable cofactors
        Kit_TruthCofactor0New( pCofs2[0], pTruth, pObj->nFans, i );
        Kit_TruthCofactor1New( pCofs2[1], pTruth, pObj->nFans, i );
//        assert( !Kit_TruthVarInSupport( pCofs2[0], pObj->nFans, i) );
//        assert( !Kit_TruthVarInSupport( pCofs2[1], pObj->nFans, i) );
        // get the constant cofs
        fEquals[0][0] = Kit_TruthIsConst0( pCofs2[0], pObj->nFans );
        fEquals[0][1] = Kit_TruthIsConst0( pCofs2[1], pObj->nFans );
        fEquals[1][0] = Kit_TruthIsConst1( pCofs2[0], pObj->nFans );
        fEquals[1][1] = Kit_TruthIsConst1( pCofs2[1], pObj->nFans );
        fOppos        = Kit_TruthIsOpposite( pCofs2[0], pCofs2[1], pObj->nFans );
        assert( !Kit_TruthIsEqual(pCofs2[0], pCofs2[1], pObj->nFans) );
        if ( fEquals[0][0] + fEquals[0][1] + fEquals[1][0] + fEquals[1][1] + fOppos == 0 )
        {
            // check the MUX decomposition
            uSupp0 = Kit_TruthSupport( pCofs2[0], pObj->nFans );
            uSupp1 = Kit_TruthSupport( pCofs2[1], pObj->nFans );
            assert( uSupp == (uSupp0 | uSupp1 | (1<<i)) );
            if ( uSupp0 & uSupp1 )
                continue;
            // perform MUX decomposition
            pRes0 = Kit_DsdObjAlloc( pNtk, KIT_DSD_PRIME, pObj->nFans );
            pRes1 = Kit_DsdObjAlloc( pNtk, KIT_DSD_PRIME, pObj->nFans );
            for ( k = 0; k < pObj->nFans; k++ )
            {
                pRes0->pFans[k] = (uSupp0 & (1 << k))? pObj->pFans[k] : 127;
                pRes1->pFans[k] = (uSupp1 & (1 << k))? pObj->pFans[k] : 127;
            }
            Kit_TruthCopy( Kit_DsdObjTruth(pRes0), pCofs2[0], pObj->nFans );        
            Kit_TruthCopy( Kit_DsdObjTruth(pRes1), pCofs2[1], pObj->nFans ); 
            // update the current one
            assert( pObj->Type == KIT_DSD_PRIME );
            pTruth[0] = 0xCACACACA;
            pObj->nFans = 3;
            pObj->pFans[2] = pObj->pFans[i];
            pObj->pFans[0] = 2*pRes0->Id; pRes0->nRefs++;
            pObj->pFans[1] = 2*pRes1->Id; pRes1->nRefs++;
            // call recursively
            Kit_DsdDecompose_rec( pNtk, pRes0, uSupp0, pObj->pFans + 0 );
            Kit_DsdDecompose_rec( pNtk, pRes1, uSupp1, pObj->pFans + 1 );
            return;
        }
//Extra_PrintBinary( stdout, pTruth, 1 << pObj->nFans ); printf( "\n" );

        // create the new node
        pRes = Kit_DsdObjAlloc( pNtk, KIT_DSD_AND, 2 );
        pRes->nRefs++;
        pRes->nFans = 2;
        pRes->pFans[0] = pObj->pFans[i];  pObj->pFans[i] = 127;  uSupp &= ~(1 << i);
        pRes->pFans[1] = 2*pObj->Id;
        // update the parent pointer
        *pPar = Kit_DsdLitNotCond( 2 * pRes->Id, Kit_DsdLitIsCompl(*pPar) );
        // consider different decompositions
        if ( fEquals[0][0] )
        {
            Kit_TruthCopy( pTruth, pCofs2[1], pObj->nFans );
        }
        else if ( fEquals[0][1] )
        {
            pRes->pFans[0] = Kit_DsdLitNot(pRes->pFans[0]);
            Kit_TruthCopy( pTruth, pCofs2[0], pObj->nFans );
        }
        else if ( fEquals[1][0] )
        {
            *pPar = Kit_DsdLitNot(*pPar);
            pRes->pFans[1] = Kit_DsdLitNot(pRes->pFans[1]);
            Kit_TruthCopy( pTruth, pCofs2[1], pObj->nFans );
        }
        else if ( fEquals[1][1] )
        {
            *pPar = Kit_DsdLitNot(*pPar);
            pRes->pFans[0] = Kit_DsdLitNot(pRes->pFans[0]);  
            pRes->pFans[1] = Kit_DsdLitNot(pRes->pFans[1]);
            Kit_TruthCopy( pTruth, pCofs2[0], pObj->nFans );
        }
        else if ( fOppos )
        {
            pRes->Type = KIT_DSD_XOR;
            Kit_TruthCopy( pTruth, pCofs2[0], pObj->nFans );
        }
        else
            assert( 0 );
        // decompose the remainder
        assert( Kit_DsdObjTruth(pObj) == pTruth );
        Kit_DsdDecompose_rec( pNtk, pObj, uSupp, pRes->pFans + 1 );
        return;
    }
    pObj->fMark = 1;

    // decompose the input
    for ( i = pObj->nFans - 1; i >= 0; i-- )
    {
        assert( Kit_TruthVarInSupport( pTruth, pObj->nFans, i ) );
        // get the single variale cofactors
        Kit_TruthCofactor0New( pCofs2[0], pTruth, pObj->nFans, i );
        Kit_TruthCofactor1New( pCofs2[1], pTruth, pObj->nFans, i );
        // check the existence of MUX decomposition
        uSupp0 = Kit_TruthSupport( pCofs2[0], pObj->nFans );
        uSupp1 = Kit_TruthSupport( pCofs2[1], pObj->nFans );
        assert( uSupp == (uSupp0 | uSupp1 | (1<<i)) );
        // if one of the cofs is a constant, it is time to check the output again
        if ( uSupp0 == 0 || uSupp1 == 0 )
        {
            pObj->fMark = 0;
            Kit_DsdDecompose_rec( pNtk, pObj, uSupp, pPar );
            return;
        }
        assert( uSupp0 && uSupp1 );
        // get the number of unique variables
        nFans0 = Kit_WordCountOnes( uSupp0 & ~uSupp1 );
        nFans1 = Kit_WordCountOnes( uSupp1 & ~uSupp0 );
        if ( nFans0 == 1 && nFans1 == 1 )
        {
            // get the cofactors w.r.t. the unique variables
            iLit0 = Kit_WordFindFirstBit( uSupp0 & ~uSupp1 );
            iLit1 = Kit_WordFindFirstBit( uSupp1 & ~uSupp0 );
            // get four cofactors                                        
            Kit_TruthCofactor0New( pCofs4[0][0], pCofs2[0], pObj->nFans, iLit0 );
            Kit_TruthCofactor1New( pCofs4[0][1], pCofs2[0], pObj->nFans, iLit0 );
            Kit_TruthCofactor0New( pCofs4[1][0], pCofs2[1], pObj->nFans, iLit1 );
            Kit_TruthCofactor1New( pCofs4[1][1], pCofs2[1], pObj->nFans, iLit1 );
            // check existence conditions
            fEquals[0][0] = Kit_TruthIsEqual( pCofs4[0][0], pCofs4[1][0], pObj->nFans );
            fEquals[0][1] = Kit_TruthIsEqual( pCofs4[0][1], pCofs4[1][1], pObj->nFans );
            fEquals[1][0] = Kit_TruthIsEqual( pCofs4[0][0], pCofs4[1][1], pObj->nFans );
            fEquals[1][1] = Kit_TruthIsEqual( pCofs4[0][1], pCofs4[1][0], pObj->nFans );
            if ( (fEquals[0][0] && fEquals[0][1]) || (fEquals[1][0] && fEquals[1][1]) )
            {
                // construct the MUX
                pRes = Kit_DsdObjAlloc( pNtk, KIT_DSD_PRIME, 3 );
                Kit_DsdObjTruth(pRes)[0] = 0xCACACACA;
                pRes->nRefs++;
                pRes->nFans = 3;
                pRes->pFans[0] = pObj->pFans[iLit0]; pObj->pFans[iLit0] = 127;  uSupp &= ~(1 << iLit0);
                pRes->pFans[1] = pObj->pFans[iLit1]; pObj->pFans[iLit1] = 127;  uSupp &= ~(1 << iLit1);
                pRes->pFans[2] = pObj->pFans[i];     pObj->pFans[i] = 2 * pRes->Id; // remains in support
                // update the node
//                if ( fEquals[0][0] && fEquals[0][1] )
//                    Kit_TruthMux( pTruth, pCofs4[0][0], pCofs4[0][1], pObj->nFans, i );
//                else
//                    Kit_TruthMux( pTruth, pCofs4[0][1], pCofs4[0][0], pObj->nFans, i );
                Kit_TruthMux( pTruth, pCofs4[1][0], pCofs4[1][1], pObj->nFans, i );
                if ( fEquals[1][0] && fEquals[1][1] )
                    pRes->pFans[0] = Kit_DsdLitNot(pRes->pFans[0]);
                // decompose the remainder
                Kit_DsdDecompose_rec( pNtk, pObj, uSupp, pPar );
                return;
            }
        }

        // try other inputs
        for ( k = i+1; k < pObj->nFans; k++ )
        {
            // get four cofactors                                                ik
            Kit_TruthCofactor0New( pCofs4[0][0], pCofs2[0], pObj->nFans, k ); // 00 
            Kit_TruthCofactor1New( pCofs4[0][1], pCofs2[0], pObj->nFans, k ); // 01 
            Kit_TruthCofactor0New( pCofs4[1][0], pCofs2[1], pObj->nFans, k ); // 10 
            Kit_TruthCofactor1New( pCofs4[1][1], pCofs2[1], pObj->nFans, k ); // 11 
            // compare equal pairs
            fPairs[0][1] = fPairs[1][0] = Kit_TruthIsEqual( pCofs4[0][0], pCofs4[0][1], pObj->nFans );
            fPairs[0][2] = fPairs[2][0] = Kit_TruthIsEqual( pCofs4[0][0], pCofs4[1][0], pObj->nFans );
            fPairs[0][3] = fPairs[3][0] = Kit_TruthIsEqual( pCofs4[0][0], pCofs4[1][1], pObj->nFans );
            fPairs[1][2] = fPairs[2][1] = Kit_TruthIsEqual( pCofs4[0][1], pCofs4[1][0], pObj->nFans );
            fPairs[1][3] = fPairs[3][1] = Kit_TruthIsEqual( pCofs4[0][1], pCofs4[1][1], pObj->nFans );
            fPairs[2][3] = fPairs[3][2] = Kit_TruthIsEqual( pCofs4[1][0], pCofs4[1][1], pObj->nFans );
            nPairs = fPairs[0][1] + fPairs[0][2] + fPairs[0][3] + fPairs[1][2] + fPairs[1][3] + fPairs[2][3];
            if ( nPairs != 3 && nPairs != 2 )
                continue;

            // decomposition exists
            pRes = Kit_DsdObjAlloc( pNtk, KIT_DSD_AND, 2 );
            pRes->nRefs++;
            pRes->nFans = 2;
            pRes->pFans[0] = pObj->pFans[k]; pObj->pFans[k] = 2 * pRes->Id;  // remains in support
            pRes->pFans[1] = pObj->pFans[i]; pObj->pFans[i] = 127;       uSupp &= ~(1 << i);
            if ( !fPairs[0][1] && !fPairs[0][2] && !fPairs[0][3] ) // 00
            {
                pRes->pFans[0] = Kit_DsdLitNot(pRes->pFans[0]);  
                pRes->pFans[1] = Kit_DsdLitNot(pRes->pFans[1]);
                Kit_TruthMux( pTruth, pCofs4[1][1], pCofs4[0][0], pObj->nFans, k );
            }
            else if ( !fPairs[1][0] && !fPairs[1][2] && !fPairs[1][3] ) // 01
            {
                pRes->pFans[1] = Kit_DsdLitNot(pRes->pFans[1]);  
                Kit_TruthMux( pTruth, pCofs4[0][0], pCofs4[0][1], pObj->nFans, k );
            }
            else if ( !fPairs[2][0] && !fPairs[2][1] && !fPairs[2][3] ) // 10
            {
                pRes->pFans[0] = Kit_DsdLitNot(pRes->pFans[0]);  
                Kit_TruthMux( pTruth, pCofs4[0][0], pCofs4[1][0], pObj->nFans, k );
            }
            else if ( !fPairs[3][0] && !fPairs[3][1] && !fPairs[3][2] ) // 11
            {
//                unsigned uSupp0 = Kit_TruthSupport(pCofs4[0][0], pObj->nFans);
//                unsigned uSupp1 = Kit_TruthSupport(pCofs4[1][1], pObj->nFans);
//                unsigned uSupp;
//                Extra_PrintBinary( stdout, &uSupp0, pObj->nFans ); printf( "\n" );
//                Extra_PrintBinary( stdout, &uSupp1, pObj->nFans ); printf( "\n" );
                Kit_TruthMux( pTruth, pCofs4[0][0], pCofs4[1][1], pObj->nFans, k );
//                uSupp = Kit_TruthSupport(pTruth, pObj->nFans);
//                Extra_PrintBinary( stdout, &uSupp, pObj->nFans ); printf( "\n" ); printf( "\n" );
            }
            else
            {
                assert( fPairs[0][3] && fPairs[1][2] );
                pRes->Type = KIT_DSD_XOR;;
                Kit_TruthMux( pTruth, pCofs4[0][0], pCofs4[0][1], pObj->nFans, k );
            }
            // decompose the remainder
            Kit_DsdDecompose_rec( pNtk, pObj, uSupp, pPar );
            return;
        }
    }
}

/**Function*************************************************************

  Synopsis    [Performs decomposition of the truth table.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Kit_DsdNtk_t * Kit_DsdDecompose( unsigned * pTruth, int nVars )
{
    Kit_DsdNtk_t * pNtk;
    Kit_DsdObj_t * pObj;
    unsigned uSupp;
    int i, nVarsReal;
    assert( nVars <= 16 );
    pNtk = Kit_DsdNtkAlloc( nVars );
    pNtk->Root = Kit_DsdVar2Lit( pNtk->nVars, 0 );
    // create the first node
    pObj = Kit_DsdObjAlloc( pNtk, KIT_DSD_PRIME, nVars );
    assert( pNtk->pNodes[0] == pObj );
    for ( i = 0; i < nVars; i++ )
       pObj->pFans[i] = Kit_DsdVar2Lit( i, 0 );
    Kit_TruthCopy( Kit_DsdObjTruth(pObj), pTruth, nVars );
    uSupp = Kit_TruthSupport( pTruth, nVars );
    // consider special cases
    nVarsReal = Kit_WordCountOnes( uSupp );
    if ( nVarsReal == 0 )
    {
        pObj->Type = KIT_DSD_CONST1;
        pObj->nFans = 0;
        if ( pTruth[0] == 0 )
             pNtk->Root = Kit_DsdLitNot(pNtk->Root);
        return pNtk;
    }
    if ( nVarsReal == 1 )
    {
        pObj->Type = KIT_DSD_VAR;
        pObj->nFans = 1;
        pObj->pFans[0] = Kit_DsdVar2Lit( Kit_WordFindFirstBit(uSupp), (pTruth[0] & 1) );
        return pNtk;
    }
    Kit_DsdDecompose_rec( pNtk, pNtk->pNodes[0], uSupp, &pNtk->Root );
    return pNtk;
}

/**Function*************************************************************

  Synopsis    [Performs decomposition of the truth table.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdTestCofs( Kit_DsdNtk_t * pNtk, unsigned * pTruthInit )
{
    Kit_DsdNtk_t * pNtk0, * pNtk1, * pTemp;
//    Kit_DsdObj_t * pRoot;
    unsigned * pCofs2[2] = { pNtk->pMem, pNtk->pMem + Kit_TruthWordNum(pNtk->nVars) };
    unsigned i, * pTruth;
    int fVerbose = 0;
    int RetValue = 0;

    pTruth = pTruthInit;
//    pRoot = Kit_DsdNtkRoot(pNtk);
//    pTruth = Kit_DsdObjTruth(pRoot);
//    assert( pRoot->nFans == pNtk->nVars );

    if ( fVerbose )
    {
        printf( "Function: " );
//        Extra_PrintBinary( stdout, pTruth, (1 << pNtk->nVars) ); 
        Extra_PrintHexadecimal( stdout, pTruth, pNtk->nVars ); 
        printf( "\n" );
        Kit_DsdPrint( stdout, pNtk );
    }
    for ( i = 0; i < pNtk->nVars; i++ )
    {
        Kit_TruthCofactor0New( pCofs2[0], pTruth, pNtk->nVars, i );
        pNtk0 = Kit_DsdDecompose( pCofs2[0], pNtk->nVars );
        pNtk0 = Kit_DsdExpand( pTemp = pNtk0 );
        Kit_DsdNtkFree( pTemp );

        if ( fVerbose )
        {
            printf( "Cof%d0: ", i );
            Kit_DsdPrint( stdout, pNtk0 );
        }

        Kit_TruthCofactor1New( pCofs2[1], pTruth, pNtk->nVars, i );
        pNtk1 = Kit_DsdDecompose( pCofs2[1], pNtk->nVars );
        pNtk1 = Kit_DsdExpand( pTemp = pNtk1 );
        Kit_DsdNtkFree( pTemp );

        if ( fVerbose )
        {
            printf( "Cof%d1: ", i );
            Kit_DsdPrint( stdout, pNtk1 );
        }

        if ( Kit_DsdCheckVar4Dec2( pNtk0, pNtk1 ) )
            RetValue = 1;

        Kit_DsdNtkFree( pNtk0 );
        Kit_DsdNtkFree( pNtk1 );
    }
    if ( fVerbose )
        printf( "\n" );

    return RetValue;
}

/**Function*************************************************************

  Synopsis    [Performs decomposition of the truth table.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Kit_DsdEval( unsigned * pTruth, int nVars, int nLutSize )
{
    Kit_DsdMan_t * p;
    Kit_DsdNtk_t * pNtk;
    unsigned * pTruthC;
    int Result;

    // decompose the function
    pNtk = Kit_DsdDecompose( pTruth, nVars );
    Result = Kit_DsdCountLuts( pNtk, nLutSize );
//    printf( "\n" );
//    Kit_DsdPrint( stdout, pNtk );
//    printf( "Eval = %d.\n", Result );

    // recompute the truth table
    p = Kit_DsdManAlloc( nVars );
    pTruthC = Kit_DsdTruthCompute( p, pNtk );
    if ( !Extra_TruthIsEqual( pTruth, pTruthC, nVars ) )
        printf( "Verification failed.\n" );
    Kit_DsdManFree( p );

    Kit_DsdNtkFree( pNtk );
    return Result;
}

/**Function*************************************************************

  Synopsis    [Performs decomposition of the truth table.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Kit_DsdNtk_t * Kit_DsdDeriveNtk( unsigned * pTruth, int nVars, int nLutSize )
{
//    Kit_DsdMan_t * p;
    Kit_DsdNtk_t * pNtk;//, * pTemp;
//    unsigned * pTruthC;
//    int Result;

    // decompose the function
    pNtk = Kit_DsdDecompose( pTruth, nVars );

//    pNtk = Kit_DsdExpand( pTemp = pNtk );
//    Kit_DsdNtkFree( pTemp );

//    Result = Kit_DsdCountLuts( pNtk, nLutSize );

//    printf( "\n" );
//    Kit_DsdPrint( stdout, pNtk );
//    printf( "Eval = %d.\n", Result );

/*
    // recompute the truth table
    p = Kit_DsdManAlloc( nVars );
    pTruthC = Kit_DsdTruthCompute( p, pNtk );
    if ( !Extra_TruthIsEqual( pTruth, pTruthC, nVars ) )
        printf( "Verification failed.\n" );
    Kit_DsdManFree( p );
*/

//    Kit_DsdNtkFree( pNtk );
//    return Result;
    return pNtk;
}

/**Function*************************************************************

  Synopsis    [Performs decomposition of the truth table.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdTest( unsigned * pTruth, int nVars )
{
    Kit_DsdMan_t * p;
    unsigned * pTruthC;
    Kit_DsdNtk_t * pNtk, * pTemp;
    pNtk = Kit_DsdDecompose( pTruth, nVars );

//    if ( Kit_DsdFindLargeBox(pNtk, Kit_DsdLit2Var(pNtk->Root)) )
//        Kit_DsdPrint( stdout, pNtk );

//    if ( Kit_DsdNtkRoot(pNtk)->nFans == (unsigned)nVars && nVars == 6 )

    printf( "\n" );
    Kit_DsdPrint( stdout, pNtk );

    pNtk = Kit_DsdExpand( pTemp = pNtk );
    Kit_DsdNtkFree( pTemp );

    Kit_DsdPrint( stdout, pNtk );

//    if ( Kit_DsdFindLargeBox(pNtk, Kit_DsdLit2Var(pNtk->Root)) )
//        Kit_DsdTestCofs( pNtk, pTruth );

    // recompute the truth table
    p = Kit_DsdManAlloc( nVars );
    pTruthC = Kit_DsdTruthCompute( p, pNtk );
//    Extra_PrintBinary( stdout, pTruth, 1 << nVars ); printf( "\n" );
//    Extra_PrintBinary( stdout, pTruthC, 1 << nVars ); printf( "\n" );
    if ( Extra_TruthIsEqual( pTruth, pTruthC, nVars ) )
    {
//        printf( "Verification is okay.\n" );
    }
    else
        printf( "Verification failed.\n" );
    Kit_DsdManFree( p );


    Kit_DsdNtkFree( pNtk );
}

/**Function*************************************************************

  Synopsis    [Performs decomposition of the truth table.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Kit_DsdPrecompute4Vars()
{
    Kit_DsdMan_t * p;
    Kit_DsdNtk_t * pNtk, * pTemp;
    FILE * pFile;
    unsigned uTruth;
    unsigned * pTruthC;
    char Buffer[256];
    int i, RetValue;
    int Counter1 = 0, Counter2 = 0;

    pFile = fopen( "5npn/npn4.txt", "r" );
    for ( i = 0; fgets( Buffer, 100, pFile ); i++ )
    {
        Buffer[6] = 0;
        Extra_ReadHexadecimal( &uTruth, Buffer+2, 4 );
        uTruth = ((uTruth & 0xffff) << 16) | (uTruth & 0xffff);
        pNtk = Kit_DsdDecompose( &uTruth, 4 );

        pNtk = Kit_DsdExpand( pTemp = pNtk );
        Kit_DsdNtkFree( pTemp );


        if ( Kit_DsdFindLargeBox(pNtk, 3) )
        {
//            RetValue = 0;
            RetValue = Kit_DsdTestCofs( pNtk, &uTruth );
            printf( "\n" );
            printf( "%3d : Non-DSD function  %s  %s\n", i, Buffer + 2, RetValue? "implementable" : "" );
            Kit_DsdPrint( stdout, pNtk );

            Counter1++;
            Counter2 += RetValue;
        }

/*
        printf( "%3d : Function  %s   ", i, Buffer + 2 );
        if ( !Kit_DsdFindLargeBox(pNtk, 3) )
            Kit_DsdPrint( stdout, pNtk );
        else
            printf( "\n" );
*/

        p = Kit_DsdManAlloc( 4 );
        pTruthC = Kit_DsdTruthCompute( p, pNtk );
        if ( !Extra_TruthIsEqual( &uTruth, pTruthC, 4 ) )
            printf( "Verification failed.\n" );
        Kit_DsdManFree( p );

        Kit_DsdNtkFree( pNtk );
    }
    fclose( pFile );
    printf( "non-DSD = %d   implementable = %d\n", Counter1, Counter2 );
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


