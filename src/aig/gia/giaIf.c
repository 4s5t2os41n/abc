/**CFile****************************************************************

  FileName    [giaMap.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Scalable AIG package.]

  Synopsis    [Manipulation of mapping associated with the AIG.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: giaMap.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "gia.h"
#include "aig/aig/aig.h"
#include "map/if/if.h"
#include "bool/kit/kit.h"
#include "base/main/main.h"
#include "sat/bsat/satSolver.h"

#ifdef WIN32
#include <windows.h>
#endif

ABC_NAMESPACE_IMPL_START


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

extern int Kit_TruthToGia( Gia_Man_t * pMan, unsigned * pTruth, int nVars, Vec_Int_t * vMemory, Vec_Int_t * vLeaves, int fHash );
extern int Abc_RecToGia3( Gia_Man_t * pMan, If_Man_t * pIfMan, If_Cut_t * pCut, Vec_Int_t * vLeaves, int fHash );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Load the network into FPGA manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Gia_ManSetIfParsDefault( void * pp )
{
    If_Par_t * pPars = (If_Par_t *)pp;
//    extern void * Abc_FrameReadLibLut();
    If_Par_t * p = (If_Par_t *)pPars;
    // set defaults
    memset( p, 0, sizeof(If_Par_t) );
    // user-controlable paramters
    p->nLutSize    = -1;
//    p->nLutSize    =  6;
    p->nCutsMax    =  8;
    p->nFlowIters  =  1;
    p->nAreaIters  =  2;
    p->DelayTarget = -1;
    p->Epsilon     =  (float)0.005;
    p->fPreprocess =  1;
    p->fArea       =  0;
    p->fFancy      =  0;
    p->fExpRed     =  1; ////
    p->fLatchPaths =  0;
    p->fEdge       =  1;
    p->fPower      =  0;
    p->fCutMin     =  0;
    p->fVerbose    =  0;
    p->pLutStruct  =  NULL;
    // internal parameters
    p->fTruth      =  0;
    p->nLatchesCi  =  0;
    p->nLatchesCo  =  0;
    p->fLiftLeaves =  0;
    p->fUseCoAttrs =  1;   // use CO attributes
    p->pLutLib     =  NULL;
    p->pTimesArr   =  NULL; 
    p->pTimesReq   =  NULL;   
    p->pFuncCost   =  NULL;   
}


/**Function*************************************************************

  Synopsis    [Prints mapping statistics.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManLutFaninCount( Gia_Man_t * p )
{
    int i, Counter = 0;
    Gia_ManForEachLut( p, i )
        Counter += Gia_ObjLutSize(p, i);
    return Counter;
}

/**Function*************************************************************

  Synopsis    [Prints mapping statistics.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManLutSizeMax( Gia_Man_t * p )
{
    int i, nSizeMax = -1;
    Gia_ManForEachLut( p, i )
        nSizeMax = Abc_MaxInt( nSizeMax, Gia_ObjLutSize(p, i) );
    return nSizeMax;
}

/**Function*************************************************************

  Synopsis    [Prints mapping statistics.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManLutNum( Gia_Man_t * p )
{
    int i, Counter = 0;
    Gia_ManForEachLut( p, i )
        Counter ++;
    return Counter;
}

/**Function*************************************************************

  Synopsis    [Prints mapping statistics.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManLutLevel( Gia_Man_t * p )
{
    Gia_Obj_t * pObj;
    int i, k, iFan, Level;
    int * pLevels = ABC_CALLOC( int, Gia_ManObjNum(p) );
    Gia_ManForEachLut( p, i )
    {
        Level = 0;
        Gia_LutForEachFanin( p, i, iFan, k )
            if ( Level < pLevels[iFan] )
                Level = pLevels[iFan];
        pLevels[i] = Level + 1;
    }
    Level = 0;
    Gia_ManForEachCo( p, pObj, k )
        if ( Level < pLevels[Gia_ObjFaninId0p(p, pObj)] )
            Level = pLevels[Gia_ObjFaninId0p(p, pObj)];
    ABC_FREE( pLevels );
    return Level;
}

/**Function*************************************************************

  Synopsis    [Assigns levels.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Gia_ManSetRefsMapped( Gia_Man_t * p )  
{
    Gia_Obj_t * pObj;
    int i, k, iFan;
    ABC_FREE( p->pRefs );
    p->pRefs = ABC_CALLOC( int, Gia_ManObjNum(p) );
    Gia_ManForEachCo( p, pObj, i )
        Gia_ObjRefInc( p, Gia_ObjFanin0(pObj) );
    Gia_ManForEachLut( p, i )
        Gia_LutForEachFanin( p, i, iFan, k )
            Gia_ObjRefInc( p, Gia_ManObj(p, iFan) );
}

/**Function*************************************************************

  Synopsis    [Calculate mapping overlap.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManComputeOverlap2One_rec( Gia_Man_t * p, int iObj, Vec_Str_t * vLabel, Vec_Int_t * vVisit )
{
    Gia_Obj_t * pObj;
    int Counter;
    if ( Vec_StrEntry(vLabel, iObj) )
        return 0;
    Vec_StrWriteEntry( vLabel, iObj, 1 );
    pObj = Gia_ManObj( p, iObj );
    assert( Gia_ObjIsAnd(pObj) );
    Counter  = Gia_ManComputeOverlap2One_rec( p, Gia_ObjFaninId0(pObj, iObj), vLabel, vVisit );
    Counter += Gia_ManComputeOverlap2One_rec( p, Gia_ObjFaninId1(pObj, iObj), vLabel, vVisit );
    Vec_IntPush( vVisit, iObj );
    return Counter + 1;
}
int Gia_ManComputeOverlap2One( Gia_Man_t * p, int iObj, Vec_Str_t * vLabel, Vec_Int_t * vVisit )
{
    int iFan, k, Counter;
    Vec_IntClear( vVisit );
    Gia_LutForEachFanin( p, iObj, iFan, k )
        Vec_StrWriteEntry( vLabel, iFan, 1 );
    Counter = Gia_ManComputeOverlap2One_rec( p, iObj, vLabel, vVisit );
    Gia_LutForEachFanin( p, iObj, iFan, k )
        Vec_StrWriteEntry( vLabel, iFan, 0 );
    Vec_IntForEachEntry( vVisit, iFan, k )
        Vec_StrWriteEntry( vLabel, iFan, 0 );
    return Counter;
}
int Gia_ManComputeOverlap2( Gia_Man_t * p )
{
    Vec_Int_t * vVisit;
    Vec_Str_t * vLabel;
    int i, Count = -Gia_ManAndNum(p);
    assert( Gia_ManHasMapping(p) );
    vVisit = Vec_IntAlloc( 100 );
    vLabel = Vec_StrStart( Gia_ManObjNum(p) );
    Gia_ManForEachLut( p, i )
        Count += Gia_ManComputeOverlap2One( p, i, vLabel, vVisit );
    Vec_StrFree( vLabel );
    Vec_IntFree( vVisit );
    return Count;
}

/**Function*************************************************************

  Synopsis    [Calculate mapping overlap.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManComputeOverlapOne_rec( Gia_Man_t * p, int iObj )
{
    Gia_Obj_t * pObj;
    if ( Gia_ObjIsTravIdCurrentId(p, iObj) )
        return 0;
    Gia_ObjSetTravIdCurrentId( p, iObj );
    pObj = Gia_ManObj( p, iObj );
    assert( Gia_ObjIsAnd(pObj) );
    return 1 + Gia_ManComputeOverlapOne_rec( p, Gia_ObjFaninId0(pObj, iObj) )
          + Gia_ManComputeOverlapOne_rec( p, Gia_ObjFaninId1(pObj, iObj) );
}
int Gia_ManComputeOverlapOne( Gia_Man_t * p, int iObj )
{
    int iFan, k;
    Gia_ManIncrementTravId(p);
    Gia_LutForEachFanin( p, iObj, iFan, k )
        Gia_ObjSetTravIdCurrentId( p, iFan );
    return Gia_ManComputeOverlapOne_rec( p, iObj );
}
int Gia_ManComputeOverlap( Gia_Man_t * p )
{
    int i, Count = -Gia_ManAndNum(p);
    assert( Gia_ManHasMapping(p) );
    Gia_ManForEachLut( p, i )
        Count += Gia_ManComputeOverlapOne( p, i );
    return Count;
}

/**Function*************************************************************

  Synopsis    [Prints mapping statistics.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Gia_ManPrintMappingStats( Gia_Man_t * p, char * pDumpFile )
{
    Gia_Obj_t * pObj;
    int * pLevels;
    int i, k, iFan, nLutSize = 0, nLuts = 0, nFanins = 0, LevelMax = 0, Ave = 0;
    if ( !Gia_ManHasMapping(p) )
        return;
    pLevels = ABC_CALLOC( int, Gia_ManObjNum(p) );
    Gia_ManForEachLut( p, i )
    {
        nLuts++;
        nFanins += Gia_ObjLutSize(p, i);
        nLutSize = Abc_MaxInt( nLutSize, Gia_ObjLutSize(p, i) );
        Gia_LutForEachFanin( p, i, iFan, k )
            pLevels[i] = Abc_MaxInt( pLevels[i], pLevels[iFan] );
        pLevels[i]++;
        LevelMax = Abc_MaxInt( LevelMax, pLevels[i] );
    }
    Gia_ManForEachCo( p, pObj, i )
        Ave += pLevels[Gia_ObjFaninId0p(p, pObj)];
    ABC_FREE( pLevels );

#ifdef WIN32
    {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    Abc_Print( 1, "Mapping (K=%d)  :  ", nLutSize );
    SetConsoleTextAttribute( hConsole, 14 ); // yellow
    Abc_Print( 1, "lut =%7d  ",  nLuts );
    SetConsoleTextAttribute( hConsole, 10 ); // green
    Abc_Print( 1, "edge =%8d  ", nFanins );
    SetConsoleTextAttribute( hConsole, 12 ); // red
    Abc_Print( 1, "lev =%5d ",   LevelMax );
    Abc_Print( 1, "(%.2f)  ",    (float)Ave / Gia_ManCoNum(p) );
    SetConsoleTextAttribute( hConsole, 7 );  // normal
    Abc_Print( 1, "over =%5.1f %%  ", 100.0 * Gia_ManComputeOverlap(p) / Gia_ManAndNum(p) );
    Abc_Print( 1, "mem =%5.2f MB", 4.0*(Gia_ManObjNum(p) + 2*nLuts + nFanins)/(1<<20) );
    Abc_Print( 1, "\n" );
    }
#else
    Abc_Print( 1, "Mapping (K=%d)  :  ", nLutSize );
    Abc_Print( 1, "%slut =%7d%s  ",  "\033[1;33m", nLuts,    "\033[0m" );  // yellow
    Abc_Print( 1, "%sedge =%8d%s  ", "\033[1;32m", nFanins,  "\033[0m" );  // green
    Abc_Print( 1, "%slev =%5d%s ",   "\033[1;31m", LevelMax, "\033[0m" );  // red
    Abc_Print( 1, "%s(%.2f)%s  ",    "\033[1;31m", (float)Ave / Gia_ManCoNum(p), "\033[0m" );
    Abc_Print( 1, "over =%5.1f %%  ", 100.0 * Gia_ManComputeOverlap(p) / Gia_ManAndNum(p) );
    Abc_Print( 1, "mem =%5.2f MB", 4.0*(Gia_ManObjNum(p) + 2*nLuts + nFanins)/(1<<20) );
    Abc_Print( 1, "\n" );
#endif


    if ( pDumpFile )
    {
        static char FileNameOld[1000] = {0};
        static abctime clk = 0;
        FILE * pTable = fopen( pDumpFile, "a+" );
        if ( strcmp( FileNameOld, p->pName ) )
        {
            sprintf( FileNameOld, "%s", p->pName );
            fprintf( pTable, "\n" );
            fprintf( pTable, "%s  ", p->pName );
            fprintf( pTable, "%d ", Gia_ManCiNum(p) );
            fprintf( pTable, "%d ", Gia_ManCoNum(p) );
            fprintf( pTable, "%d  ", Gia_ManAndNum(p) );
            fprintf( pTable, "%d ", nLuts           );
            fprintf( pTable, "%d ", LevelMax        );
            clk = Abc_Clock();
        }
        else
        {
            fprintf( pTable, " " );
            fprintf( pTable, "%d ", nLuts           );
            fprintf( pTable, "%d ", LevelMax        );
            fprintf( pTable, "%.2f", 1.0*(Abc_Clock() - clk)/CLOCKS_PER_SEC );
            clk = Abc_Clock();
        }
        fclose( pTable );
    }

}

/**Function*************************************************************

  Synopsis    [Prints mapping statistics.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Gia_ManPrintPackingStats( Gia_Man_t * p )
{
    int fVerbose = 0;
    int nObjToShow = 200;
    int nNumStr[5] = {0};
    int i, k, Entry, nEntries, nEntries2, MaxSize = -1;
    if ( p->vPacking == NULL )
        return;
    nEntries = Vec_IntEntry( p->vPacking, 0 );
    nEntries2 = 0;
    Vec_IntForEachEntryStart( p->vPacking, Entry, i, 1 )
    {
        assert( Entry > 0 && Entry < 4 );
        nNumStr[Entry]++;
        i++;
        if ( fVerbose && nEntries2 < nObjToShow ) Abc_Print( 1, "{ " );
        for ( k = 0; k < Entry; k++, i++ )
            if ( fVerbose && nEntries2 < nObjToShow ) Abc_Print( 1, "%d ", Vec_IntEntry(p->vPacking, i) );
        if ( fVerbose && nEntries2 < nObjToShow ) Abc_Print( 1, "}\n" );
        i--;
        nEntries2++;
    }
    assert( nEntries == nEntries2 );
    if ( nNumStr[3] > 0 )
        MaxSize = 3;
    else if ( nNumStr[2] > 0 )
        MaxSize = 2;
    else if ( nNumStr[1] > 0 )
        MaxSize = 1;
    Abc_Print( 1, "Packing (N=%d)  :  ", MaxSize );
    for ( i = 1; i <= MaxSize; i++ )
        Abc_Print( 1, "%d x LUT = %d   ", i, nNumStr[i] );
    Abc_Print( 1, "Total = %d", nEntries2 );
    Abc_Print( 1, "\n" );
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Gia_ManPrintNodeProfile( int * pCounts, int nSizeMax )
{
    int i, SizeAll = 0, NodeAll = 0;
    for ( i = 0; i <= nSizeMax; i++ )
    {
        SizeAll += i * pCounts[i];
        NodeAll += pCounts[i];
    }
    Abc_Print( 1, "LUT = %d : ", NodeAll );
    for ( i = 2; i <= nSizeMax; i++ )
        Abc_Print( 1, "%d=%d %.1f %%  ", i, pCounts[i], 100.0*pCounts[i]/NodeAll );
    Abc_Print( 1, "Ave = %.2f\n", 1.0*SizeAll/(NodeAll ? NodeAll : 1) );
}
void Gia_ManPrintLutStats( Gia_Man_t * p )
{
    int i, nSizeMax, pCounts[33] = {0};
    nSizeMax = Gia_ManLutSizeMax( p );
    if ( nSizeMax > 32 )
    {
        Abc_Print( 1, "The max LUT size (%d) is too large.\n", nSizeMax );
        return;
    }
    Gia_ManForEachLut( p, i )
        pCounts[ Gia_ObjLutSize(p, i) ]++;
    Gia_ManPrintNodeProfile( pCounts, nSizeMax );
}

/**Function*************************************************************

  Synopsis    [Computes levels for AIG with choices and white boxes.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Gia_ManChoiceLevel_rec( Gia_Man_t * p, Gia_Obj_t * pObj )
{
    Tim_Man_t * pManTime = (Tim_Man_t *)p->pManTime;
    Gia_Obj_t * pNext;
    int i, iBox, iTerm1, nTerms, LevelMax = 0;
    if ( Gia_ObjIsTravIdCurrent( p, pObj ) )
        return;
    Gia_ObjSetTravIdCurrent( p, pObj );
    if ( Gia_ObjIsCi(pObj) )
    {
        if ( pManTime )
        {
            iBox = Tim_ManBoxForCi( pManTime, Gia_ObjCioId(pObj) );
            if ( iBox >= 0 ) // this is not a true PI
            {
                iTerm1 = Tim_ManBoxInputFirst( pManTime, iBox );
                nTerms = Tim_ManBoxInputNum( pManTime, iBox );
                for ( i = 0; i < nTerms; i++ )
                {
                    pNext = Gia_ManCo( p, iTerm1 + i );
                    Gia_ManChoiceLevel_rec( p, pNext );
                    if ( LevelMax < Gia_ObjLevel(p, pNext) )
                        LevelMax = Gia_ObjLevel(p, pNext);
                }
                LevelMax++;
            }
        }
//        Abc_Print( 1, "%d ", pObj->Level );
    }
    else if ( Gia_ObjIsCo(pObj) )
    {
        pNext = Gia_ObjFanin0(pObj);
        Gia_ManChoiceLevel_rec( p, pNext );
        if ( LevelMax < Gia_ObjLevel(p, pNext) )
            LevelMax = Gia_ObjLevel(p, pNext);
    }
    else if ( Gia_ObjIsAnd(pObj) )
    { 
        // get the maximum level of the two fanins
        pNext = Gia_ObjFanin0(pObj);
        Gia_ManChoiceLevel_rec( p, pNext );
        if ( LevelMax < Gia_ObjLevel(p, pNext) )
            LevelMax = Gia_ObjLevel(p, pNext);
        pNext = Gia_ObjFanin1(pObj);
        Gia_ManChoiceLevel_rec( p, pNext );
        if ( LevelMax < Gia_ObjLevel(p, pNext) )
            LevelMax = Gia_ObjLevel(p, pNext);
        LevelMax++;

        // get the level of the nodes in the choice node
        if ( (pNext = Gia_ObjSiblObj(p, Gia_ObjId(p, pObj))) )
        {
            Gia_ManChoiceLevel_rec( p, pNext );
            if ( LevelMax < Gia_ObjLevel(p, pNext) )
                LevelMax = Gia_ObjLevel(p, pNext);
        }
    }
    else if ( !Gia_ObjIsConst0(pObj) )
        assert( 0 );
    Gia_ObjSetLevel( p, pObj, LevelMax );
}
int Gia_ManChoiceLevel( Gia_Man_t * p )
{
    Gia_Obj_t * pObj;
    int i, LevelMax = 0;
//    assert( Gia_ManRegNum(p) == 0 );
    Gia_ManCleanLevels( p, Gia_ManObjNum(p) );
    Gia_ManIncrementTravId( p );
    Gia_ManForEachCo( p, pObj, i )
    {
        Gia_ManChoiceLevel_rec( p, pObj );
        if ( LevelMax < Gia_ObjLevel(p, pObj) )
            LevelMax = Gia_ObjLevel(p, pObj);
    }
    // account for dangling boxes
    Gia_ManForEachCi( p, pObj, i )
    {
        Gia_ManChoiceLevel_rec( p, pObj );
        if ( LevelMax < Gia_ObjLevel(p, pObj) )
            LevelMax = Gia_ObjLevel(p, pObj);
//        Abc_Print( 1, "%d ", Gia_ObjLevel(p, pObj) );
    }
//    Abc_Print( 1, "\n" );
    Gia_ManForEachAnd( p, pObj, i )
        assert( Gia_ObjLevel(p, pObj) > 0 );
//    printf( "Max level %d\n", LevelMax );
    return LevelMax;
} 



/**Function*************************************************************

  Synopsis    [Converts GIA into IF manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
static inline If_Obj_t * If_ManFanin0Copy( If_Man_t * pIfMan, Gia_Obj_t * pObj ) { return If_NotCond( If_ManObj(pIfMan, Gia_ObjValue(Gia_ObjFanin0(pObj))), Gia_ObjFaninC0(pObj) ); }
static inline If_Obj_t * If_ManFanin1Copy( If_Man_t * pIfMan, Gia_Obj_t * pObj ) { return If_NotCond( If_ManObj(pIfMan, Gia_ObjValue(Gia_ObjFanin1(pObj))), Gia_ObjFaninC1(pObj) ); }
If_Man_t * Gia_ManToIf( Gia_Man_t * p, If_Par_t * pPars )
{
    If_Man_t * pIfMan;
    If_Obj_t * pIfObj;
    Gia_Obj_t * pObj;
    int i;
    // create levels with choices
    Gia_ManChoiceLevel( p );
    // mark representative nodes
    if ( p->pSibls )
    Gia_ManMarkFanoutDrivers( p );
    // start the mapping manager and set its parameters
    pIfMan = If_ManStart( pPars );
    pIfMan->pName = Abc_UtilStrsav( Gia_ManName(p) );
    // print warning about excessive memory usage
    if ( 1.0 * Gia_ManObjNum(p) * pIfMan->nObjBytes / (1<<30) > 1.0 )
        printf( "Warning: The mapper will allocate %.1f GB for to represent the subject graph with %d AIG nodes.\n", 
            1.0 * Gia_ManObjNum(p) * pIfMan->nObjBytes / (1<<30), Gia_ManObjNum(p) );
    // load the AIG into the mapper
    Gia_ManFillValue( p );
    Gia_ManConst0(p)->Value = If_ObjId( If_ManConst1(pIfMan) );
    Gia_ManForEachObj1( p, pObj, i )
    {
        if ( Gia_ObjIsAnd(pObj) )
            pIfObj = If_ManCreateAnd( pIfMan, If_ManFanin0Copy(pIfMan, pObj), If_ManFanin1Copy(pIfMan, pObj) );
        else if ( Gia_ObjIsCi(pObj) )
        {
            pIfObj = If_ManCreateCi( pIfMan );
            If_ObjSetLevel( pIfObj, Gia_ObjLevel(p, pObj) );
//            Abc_Print( 1, "pi%d=%d\n ", If_ObjId(pIfObj), If_ObjLevel(pIfObj) );
            if ( pIfMan->nLevelMax < (int)pIfObj->Level )
                pIfMan->nLevelMax = (int)pIfObj->Level;
        }
        else if ( Gia_ObjIsCo(pObj) )
        {
            pIfObj = If_ManCreateCo( pIfMan, If_NotCond( If_ManFanin0Copy(pIfMan, pObj), Gia_ObjIsConst0(Gia_ObjFanin0(pObj))) );
//            Abc_Print( 1, "po%d=%d\n ", If_ObjId(pIfObj), If_ObjLevel(pIfObj) );
        }
        else assert( 0 );
        assert( i == If_ObjId(pIfObj) );
        Gia_ObjSetValue( pObj, If_ObjId(pIfObj) );
        // set up the choice node
        if ( Gia_ObjSibl(p, i) && pObj->fMark0 )
        {
            Gia_Obj_t * pSibl, * pPrev;
            for ( pPrev = pObj, pSibl = Gia_ObjSiblObj(p, i); pSibl; pPrev = pSibl, pSibl = Gia_ObjSiblObj(p, Gia_ObjId(p, pSibl)) )
                If_ObjSetChoice( If_ManObj(pIfMan, Gia_ObjValue(pPrev)), If_ManObj(pIfMan, Gia_ObjValue(pSibl)) );
            If_ManCreateChoice( pIfMan, If_ManObj(pIfMan, Gia_ObjValue(pObj)) );
            pPars->fExpRed = 0;
        }
//        assert( If_ObjLevel(pIfObj) == Gia_ObjLevel(pNode) );
    }
    if ( p->pSibls )
    Gia_ManCleanMark0( p );
    return pIfMan;
}

/**Function*************************************************************

  Synopsis    [Rebuilds GIA from mini AIG.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManBuildFromMiniInt( Gia_Man_t * pNew, Vec_Int_t * vLeaves, Vec_Int_t * vAig, int fHash )
{
    assert( Vec_IntSize(vAig) > 0 );
    assert( Vec_IntEntryLast(vAig) < 2 );
    if ( Vec_IntSize(vAig) == 1 ) // const
        return Vec_IntEntry(vAig, 0);
    if ( Vec_IntSize(vAig) == 2 ) // variable
    {
        assert( Vec_IntEntry(vAig, 0) == 0 );
        assert( Vec_IntSize(vLeaves) == 1 );
        return Abc_LitNotCond( Vec_IntEntry(vLeaves, 0), Vec_IntEntry(vAig, 1) );
    }
    else
    {
        int nLeaves = Vec_IntSize(vLeaves);
        int i, iVar0, iVar1, iLit0, iLit1, iLit = 0;
        assert( Vec_IntSize(vAig) & 1 );
        Vec_IntForEachEntryDouble( vAig, iLit0, iLit1, i )
        {
            iVar0 = Abc_Lit2Var( iLit0 );
            iVar1 = Abc_Lit2Var( iLit1 );
            iLit0 = Abc_LitNotCond( iVar0 < nLeaves ? Vec_IntEntry(vLeaves, iVar0) : Vec_IntEntry(vAig, iVar0 - nLeaves), Abc_LitIsCompl(iLit0) );
            iLit1 = Abc_LitNotCond( iVar1 < nLeaves ? Vec_IntEntry(vLeaves, iVar1) : Vec_IntEntry(vAig, iVar1 - nLeaves), Abc_LitIsCompl(iLit1) );
            if ( fHash )
                iLit = Gia_ManHashAnd( pNew, iLit0, iLit1 );
            else if ( iLit0 == iLit1 )
                iLit = iLit0;
            else
                iLit = Gia_ManAppendAnd( pNew, iLit0, iLit1 );
            assert( (i & 1) == 0 );
            Vec_IntWriteEntry( vAig, Abc_Lit2Var(i), iLit );  // overwriting entries
        }
        assert( i == Vec_IntSize(vAig) - 1 );
        iLit = Abc_LitNotCond( iLit, Vec_IntEntry(vAig, i) );
        Vec_IntClear( vAig ); // useless
        return iLit;
    }
}
int Gia_ManBuildFromMini( Gia_Man_t * pNew, If_Man_t * pIfMan, If_Cut_t * pCut, Vec_Int_t * vLeaves, Vec_Int_t * vAig, int fHash, int fUseDsd )
{
    if ( fUseDsd )
        If_CutDsdBalanceEval( pIfMan, pCut, vAig );
    else
        If_CutSopBalanceEval( pIfMan, pCut, vAig );
    return Gia_ManBuildFromMiniInt( pNew, vLeaves, vAig, fHash );
}

/**Function*************************************************************

  Synopsis    [Converts IF into GIA manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Gia_Man_t * Gia_ManFromIfAig( If_Man_t * pIfMan )
{
    int fHash = 0;
    Gia_Man_t * pNew;
    If_Obj_t * pIfObj, * pIfLeaf;
    If_Cut_t * pCutBest;
    Vec_Int_t * vLeaves;
    Vec_Int_t * vAig;
    int i, k;
    assert( pIfMan->pPars->pLutStruct == NULL );
    assert( pIfMan->pPars->fDelayOpt || pIfMan->pPars->fDsdBalance || pIfMan->pPars->fUserRecLib );
    // create new manager
    pNew = Gia_ManStart( If_ManObjNum(pIfMan) );
    Gia_ManHashAlloc( pNew );
    // iterate through nodes used in the mapping
    vAig = Vec_IntAlloc( 1 << 16 );
    vLeaves = Vec_IntAlloc( 16 );
    If_ManCleanCutData( pIfMan );
    If_ManForEachObj( pIfMan, pIfObj, i )
    {
        if ( pIfObj->nRefs == 0 && !If_ObjIsTerm(pIfObj) )
            continue;
        if ( If_ObjIsAnd(pIfObj) )
        {
            pCutBest = If_ObjCutBest( pIfObj );
            // collect leaves of the best cut
            Vec_IntClear( vLeaves );
            If_CutForEachLeaf( pIfMan, pCutBest, pIfLeaf, k )
                Vec_IntPush( vLeaves, pIfLeaf->iCopy );
            // get the functionality
            if ( pIfMan->pPars->fDelayOpt )
                pIfObj->iCopy = Gia_ManBuildFromMini( pNew, pIfMan, pCutBest, vLeaves, vAig, fHash, 0 );
            else if ( pIfMan->pPars->fDsdBalance )
                pIfObj->iCopy = Gia_ManBuildFromMini( pNew, pIfMan, pCutBest, vLeaves, vAig, fHash, 1 );
            else if ( pIfMan->pPars->fUserRecLib )
                pIfObj->iCopy = Abc_RecToGia3( pNew, pIfMan, pCutBest, vLeaves, fHash );
            else assert( 0 );
        }
        else if ( If_ObjIsCi(pIfObj) )
            pIfObj->iCopy = Gia_ManAppendCi(pNew);
        else if ( If_ObjIsCo(pIfObj) )
            pIfObj->iCopy = Gia_ManAppendCo( pNew, Abc_LitNotCond(If_ObjFanin0(pIfObj)->iCopy, If_ObjFaninC0(pIfObj)) );
        else if ( If_ObjIsConst1(pIfObj) )
            pIfObj->iCopy = 1;
        else assert( 0 );
    }
    Vec_IntFree( vAig );
    Vec_IntFree( vLeaves );
    Gia_ManHashStop( pNew );
    return pNew;
}


/**Function*************************************************************

  Synopsis    [Write mapping for LUT with given fanins.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManFromIfLogicCreateLut( Gia_Man_t * pNew, word * pRes, Vec_Int_t * vLeaves, Vec_Int_t * vCover, Vec_Int_t * vMapping, Vec_Int_t * vMapping2 )
{
    int i, iLit, iObjLit1;
    iObjLit1 = Kit_TruthToGia( pNew, (unsigned *)pRes, Vec_IntSize(vLeaves), vCover, vLeaves, 0 );
    // do not create LUT in the simple case
    if ( Abc_Lit2Var(iObjLit1) == 0 )
        return iObjLit1;
    Vec_IntForEachEntry( vLeaves, iLit, i )
        if ( Abc_Lit2Var(iObjLit1) == Abc_Lit2Var(iLit) )
            return iObjLit1;
    // write mapping
    Vec_IntSetEntry( vMapping, Abc_Lit2Var(iObjLit1), Vec_IntSize(vMapping2) );
    Vec_IntPush( vMapping2, Vec_IntSize(vLeaves) );
//    Vec_IntForEachEntry( vLeaves, iLit, i )
//        assert( Abc_Lit2Var(iLit) < Abc_Lit2Var(iObjLit1) );
    Vec_IntForEachEntry( vLeaves, iLit, i )
        Vec_IntPush( vMapping2, Abc_Lit2Var(iLit)  );
    Vec_IntPush( vMapping2, Abc_Lit2Var(iObjLit1) );
    return iObjLit1;
}

/**Function*************************************************************

  Synopsis    [Write mapping for LUT with given fanins.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManFromIfLogicCreateLutSpecial( Gia_Man_t * pNew, word * pRes, Vec_Int_t * vLeaves, Vec_Int_t * vLeavesTemp, Vec_Int_t * vCover, Vec_Int_t * vMapping, Vec_Int_t * vMapping2, Vec_Int_t * vPacking )
{
    word z = If_CutPerformDerive07( NULL, (unsigned *)pRes, Vec_IntSize(vLeaves), Vec_IntSize(vLeaves), NULL );
    word Truth;
    int i, iObjLit1, iObjLit2;
    // create first LUT
    Vec_IntClear( vLeavesTemp );
    for ( i = 0; i < 4; i++ )
    {
        int v = (int)((z >> (16+(i<<2))) & 7);
        if ( v == 6 && Vec_IntSize(vLeaves) == 5 )
            continue;
        Vec_IntPush( vLeavesTemp, Vec_IntEntry(vLeaves, v) );
    }
    Truth = (z & 0xffff);
    Truth |= (Truth << 16);
    Truth |= (Truth << 32);
    iObjLit1 = Gia_ManFromIfLogicCreateLut( pNew, &Truth, vLeavesTemp, vCover, vMapping, vMapping2 );
    // create second LUT
    Vec_IntClear( vLeavesTemp );
    for ( i = 0; i < 4; i++ )
    {
        int v =  (int)((z >> (48+(i<<2))) & 7);
        if ( v == 6 && Vec_IntSize(vLeaves) == 5 )
            continue;
        if ( v == 7 )
            Vec_IntPush( vLeavesTemp, iObjLit1 );
        else
            Vec_IntPush( vLeavesTemp, Vec_IntEntry(vLeaves, v) );
    }
    Truth = ((z >> 32) & 0xffff);
    Truth |= (Truth << 16);
    Truth |= (Truth << 32);
    iObjLit2 = Gia_ManFromIfLogicCreateLut( pNew, &Truth, vLeavesTemp, vCover, vMapping, vMapping2 );
    // write packing
    Vec_IntPush( vPacking, 2 );
    Vec_IntPush( vPacking, Abc_Lit2Var(iObjLit1) );
    Vec_IntPush( vPacking, Abc_Lit2Var(iObjLit2) );
    Vec_IntAddToEntry( vPacking, 0, 1 );
    return iObjLit2;
}

/**Function*************************************************************

  Synopsis    [Write the node into a file.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManFromIfLogicNode( void * pIfMan, Gia_Man_t * pNew, int iObj, Vec_Int_t * vLeaves, Vec_Int_t * vLeavesTemp, 
    word * pRes, char * pStr, Vec_Int_t * vCover, Vec_Int_t * vMapping, Vec_Int_t * vMapping2, Vec_Int_t * vPacking, int fCheck75, int fCheck44e )
{
    int nLeaves = Vec_IntSize(vLeaves);
    int i, Length, nLutLeaf, nLutLeaf2, nLutRoot, iObjLit1, iObjLit2, iObjLit3;
    // workaround for the special case
    if ( fCheck75 )
        pStr = "54";
    // perform special case matching for 44
    if ( fCheck44e )
    {
        if ( Vec_IntSize(vLeaves) <= 4 )
        {
            // create mapping
            iObjLit1 = Gia_ManFromIfLogicCreateLut( pNew, pRes, vLeaves, vCover, vMapping, vMapping2 );
            // write packing
            if ( !Gia_ObjIsCi(Gia_ManObj(pNew, Abc_Lit2Var(iObjLit1))) && iObjLit1 > 1 )
            {
                Vec_IntPush( vPacking, 1 );
                Vec_IntPush( vPacking, Abc_Lit2Var(iObjLit1) );
                Vec_IntAddToEntry( vPacking, 0, 1 );
            }
            return iObjLit1;
        }
        return Gia_ManFromIfLogicCreateLutSpecial( pNew, pRes, vLeaves, vLeavesTemp, vCover, vMapping, vMapping2, vPacking );
    }
    // check if there is no LUT structures
    if ( pStr == NULL )
        return Gia_ManFromIfLogicCreateLut( pNew, pRes, vLeaves, vCover, vMapping, vMapping2 );
    // quit if parameters are wrong
    Length = strlen(pStr);
    if ( Length != 2 && Length != 3 )
    {
        printf( "Wrong LUT struct (%s)\n", pStr );
        return -1;
    }
    for ( i = 0; i < Length; i++ )
        if ( pStr[i] - '0' < 3 || pStr[i] - '0' > 6 )
        {
            printf( "The LUT size (%d) should belong to {3,4,5,6}.\n", pStr[i] - '0' );
            return -1;
        }

    nLutLeaf  =                   pStr[0] - '0';
    nLutLeaf2 = ( Length == 3 ) ? pStr[1] - '0' : 0;
    nLutRoot  =                   pStr[Length-1] - '0';
    if ( nLeaves > nLutLeaf - 1 + (nLutLeaf2 ? nLutLeaf2 - 1 : 0) + nLutRoot )
    {
        printf( "The node size (%d) is too large for the LUT structure %s.\n", nLeaves, pStr );
        return -1;
    }

    // consider easy case
    if ( nLeaves <= Abc_MaxInt( nLutLeaf2, Abc_MaxInt(nLutLeaf, nLutRoot) ) )
    {
        // create mapping
        iObjLit1 = Gia_ManFromIfLogicCreateLut( pNew, pRes, vLeaves, vCover, vMapping, vMapping2 );
        // write packing
        if ( !Gia_ObjIsCi(Gia_ManObj(pNew, Abc_Lit2Var(iObjLit1))) && iObjLit1 > 1 )
        {
            Vec_IntPush( vPacking, 1 );
            Vec_IntPush( vPacking, Abc_Lit2Var(iObjLit1) );
            Vec_IntAddToEntry( vPacking, 0, 1 );
        }
        return iObjLit1;
    }
    else
    {
        extern int If_CluMinimumBase( word * t, int * pSupp, int nVarsAll, int * pnVars );

        static word TruthStore[16][1<<10] = {{0}}, * pTruths[16];
        word Func0, Func1, Func2;
        char pLut0[32], pLut1[32], pLut2[32] = {0};

        if ( TruthStore[0][0] == 0 )
        {
            static word Truth6[6] = {
                0xAAAAAAAAAAAAAAAA,
                0xCCCCCCCCCCCCCCCC,
                0xF0F0F0F0F0F0F0F0,
                0xFF00FF00FF00FF00,
                0xFFFF0000FFFF0000,
                0xFFFFFFFF00000000
            };
            int nVarsMax = 16;
            int nWordsMax = (1 << 10);
            int i, k;
            assert( nVarsMax <= 16 );
            for ( i = 0; i < nVarsMax; i++ )
                pTruths[i] = TruthStore[i];
            for ( i = 0; i < 6; i++ )
                for ( k = 0; k < nWordsMax; k++ )
                    pTruths[i][k] = Truth6[i];
            for ( i = 6; i < nVarsMax; i++ )
                for ( k = 0; k < nWordsMax; k++ )
                    pTruths[i][k] = ((k >> (i-6)) & 1) ? ~(word)0 : 0;
        }
        // derive truth table
        if ( Kit_TruthIsConst0((unsigned *)pRes, nLeaves) || Kit_TruthIsConst1((unsigned *)pRes, nLeaves) )
        {
//            fprintf( pFile, ".names %s\n %d\n", Abc_ObjName(Abc_ObjFanout0(pObj)), Kit_TruthIsConst1((unsigned *)pRes, nLeaves) );
            iObjLit1 = Abc_LitNotCond( 0, Kit_TruthIsConst1((unsigned *)pRes, nLeaves) );
            // write mapping
            if ( Vec_IntEntry(vMapping, 0) == 0 )
            {
                Vec_IntSetEntry( vMapping, 0, Vec_IntSize(vMapping2) );
                Vec_IntPush( vMapping2, 0 );
                Vec_IntPush( vMapping2, 0 );
            }
            return iObjLit1;
        }
        // check for elementary truth table
        for ( i = 0; i < nLeaves; i++ )
        {
            if ( Kit_TruthIsEqual((unsigned *)pRes, (unsigned *)pTruths[i], nLeaves) )
                return Vec_IntEntry(vLeaves, i);
            if ( Kit_TruthIsOpposite((unsigned *)pRes, (unsigned *)pTruths[i], nLeaves) )
                return Abc_LitNot(Vec_IntEntry(vLeaves, i));
        }

        // perform decomposition
        if ( fCheck75 ) 
        {
//            if ( nLeaves < 8 && If_CutPerformCheck16( p, (unsigned *)pTruth, nVars, nLeaves, "44" ) )
            if ( nLeaves < 8 && If_CluCheckExt( NULL, pRes, nLeaves, 4, 4, pLut0, pLut1, &Func0, &Func1 ) )
            {
                nLutLeaf = 4;
                nLutRoot = 4;
            }
//            if ( If_CutPerformCheck45( p, (unsigned *)pTruth, nVars, nLeaves, pStr ) )
            else if ( If_CluCheckExt( NULL, pRes, nLeaves, 5, 4, pLut0, pLut1, &Func0, &Func1 ) )
            {
                nLutLeaf = 5;
                nLutRoot = 4;
            }
//            if ( If_CutPerformCheck54( p, (unsigned *)pTruth, nVars, nLeaves, pStr ) )
            else if ( If_CluCheckExt( NULL, pRes, nLeaves, 4, 5, pLut0, pLut1, &Func0, &Func1 ) )
            {
                nLutLeaf = 4;
                nLutRoot = 5;
            }
            else
            {
                Extra_PrintHex( stdout, (unsigned *)pRes, nLeaves );  printf( "    " );
                Kit_DsdPrintFromTruth( (unsigned*)pRes, nLeaves );  printf( "\n" );
                printf( "Node %d is not decomposable. Deriving LUT structures has failed.\n", iObj );
                return -1;
            }
        }
        else
        {
            if ( Length == 2 )
            {
                if ( !If_CluCheckExt( NULL, pRes, nLeaves, nLutLeaf, nLutRoot, pLut0, pLut1, &Func0, &Func1 ) )
                {
                    Extra_PrintHex( stdout, (unsigned *)pRes, nLeaves );  printf( "    " );
                    Kit_DsdPrintFromTruth( (unsigned*)pRes, nLeaves );  printf( "\n" );
                    printf( "Node %d is not decomposable. Deriving LUT structures has failed.\n", iObj );
                    return -1;
                }
            }
            else
            {
                if ( !If_CluCheckExt3( pIfMan, pRes, nLeaves, nLutLeaf, nLutLeaf2, nLutRoot, pLut0, pLut1, pLut2, &Func0, &Func1, &Func2 ) )
                {
                    Extra_PrintHex( stdout, (unsigned *)pRes, nLeaves );  printf( "    " );
                    Kit_DsdPrintFromTruth( (unsigned*)pRes, nLeaves );  printf( "\n" );
                    printf( "Node %d is not decomposable. Deriving LUT structures has failed.\n", iObj );
                    return -1;
                }
            }
        }

/*
        // write leaf node
        Id = Abc2_NtkAllocObj( pNew, pLut1[0], Abc2_ObjType(pObj) );
        iObjLit1 = Abc_Var2Lit( Id, 0 );
        pObjNew = Abc2_NtkObj( pNew, Id );
        for ( i = 0; i < pLut1[0]; i++ )
            Abc2_ObjSetFaninLit( pObjNew, i, Abc2_ObjFaninCopy(pObj, pLut1[2+i]) );
        Abc2_ObjSetTruth( pObjNew, Func1 );
*/
        // write leaf node
        Vec_IntClear( vLeavesTemp );
        for ( i = 0; i < pLut1[0]; i++ )
            Vec_IntPush( vLeavesTemp, Vec_IntEntry(vLeaves, pLut1[2+i]) );
        iObjLit1 = Gia_ManFromIfLogicCreateLut( pNew, &Func1, vLeavesTemp, vCover, vMapping, vMapping2 );

        if ( Length == 3 && pLut2[0] > 0 )
        {
        /*
            Id = Abc2_NtkAllocObj( pNew, pLut2[0], Abc2_ObjType(pObj) );
            iObjLit2 = Abc_Var2Lit( Id, 0 );
            pObjNew = Abc2_NtkObj( pNew, Id );
            for ( i = 0; i < pLut2[0]; i++ )
                if ( pLut2[2+i] == nLeaves )
                    Abc2_ObjSetFaninLit( pObjNew, i, iObjLit1 );
                else
                    Abc2_ObjSetFaninLit( pObjNew, i, Abc2_ObjFaninCopy(pObj, pLut2[2+i]) );
            Abc2_ObjSetTruth( pObjNew, Func2 );
        */

            // write leaf node
            Vec_IntClear( vLeavesTemp );
            for ( i = 0; i < pLut2[0]; i++ )
                if ( pLut2[2+i] == nLeaves )
                    Vec_IntPush( vLeavesTemp, iObjLit1 );
                else
                    Vec_IntPush( vLeavesTemp, Vec_IntEntry(vLeaves, pLut2[2+i]) );
            iObjLit2 = Gia_ManFromIfLogicCreateLut( pNew, &Func2, vLeavesTemp, vCover, vMapping, vMapping2 );

            // write packing
            Vec_IntPush( vPacking, 3 );
            Vec_IntPush( vPacking, Abc_Lit2Var(iObjLit1) );
            Vec_IntPush( vPacking, Abc_Lit2Var(iObjLit2) );
        }
        else
        {
            // write packing
            Vec_IntPush( vPacking, 2 );
            Vec_IntPush( vPacking, Abc_Lit2Var(iObjLit1) );
            iObjLit2 = -1;
        }
/*
        // write root node
        Id = Abc2_NtkAllocObj( pNew, pLut0[0], Abc2_ObjType(pObj) );
        iObjLit3 = Abc_Var2Lit( Id, 0 );
        pObjNew = Abc2_NtkObj( pNew, Id );
        for ( i = 0; i < pLut0[0]; i++ )
            if ( pLut0[2+i] == nLeaves )
                Abc2_ObjSetFaninLit( pObjNew, i, iObjLit1 );
            else if ( pLut0[2+i] == nLeaves+1 )
                Abc2_ObjSetFaninLit( pObjNew, i, iObjLit2 );
            else
                Abc2_ObjSetFaninLit( pObjNew, i, Abc2_ObjFaninCopy(pObj, pLut0[2+i]) );
        Abc2_ObjSetTruth( pObjNew, Func0 );
        Abc2_ObjSetCopy( pObj, iObjLit3 );
*/
        // write root node
        Vec_IntClear( vLeavesTemp );
        for ( i = 0; i < pLut0[0]; i++ )
            if ( pLut0[2+i] == nLeaves )
                Vec_IntPush( vLeavesTemp, iObjLit1 );
            else if ( pLut0[2+i] == nLeaves+1 )
                Vec_IntPush( vLeavesTemp, iObjLit2 );
            else
                Vec_IntPush( vLeavesTemp, Vec_IntEntry(vLeaves, pLut0[2+i]) );
        iObjLit3 = Gia_ManFromIfLogicCreateLut( pNew, &Func0, vLeavesTemp, vCover, vMapping, vMapping2 );

        // write packing
        Vec_IntPush( vPacking, Abc_Lit2Var(iObjLit3) );
        Vec_IntAddToEntry( vPacking, 0, 1 );
    }
    return iObjLit3;
}

/**Function*************************************************************

  Synopsis    [Recursively derives the local AIG for the cut.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManNodeIfToGia_rec( Gia_Man_t * pNew, If_Man_t * pIfMan, If_Obj_t * pIfObj, Vec_Ptr_t * vVisited, int fHash )
{
    If_Cut_t * pCut;
    If_Obj_t * pTemp;
    int iFunc, iFunc0, iFunc1;
    // get the best cut
    pCut = If_ObjCutBest(pIfObj);
    // if the cut is visited, return the result
    if ( If_CutDataInt(pCut) )
        return If_CutDataInt(pCut);
    // mark the node as visited
    Vec_PtrPush( vVisited, pCut );
    // insert the worst case
    If_CutSetDataInt( pCut, ~0 );
    // skip in case of primary input
    if ( If_ObjIsCi(pIfObj) )
        return If_CutDataInt(pCut);
    // compute the functions of the children
    for ( pTemp = pIfObj; pTemp; pTemp = pTemp->pEquiv )
    {
        iFunc0 = Gia_ManNodeIfToGia_rec( pNew, pIfMan, pTemp->pFanin0, vVisited, fHash );
        if ( iFunc0 == ~0 )
            continue;
        iFunc1 = Gia_ManNodeIfToGia_rec( pNew, pIfMan, pTemp->pFanin1, vVisited, fHash );
        if ( iFunc1 == ~0 )
            continue;
        // both branches are solved
        if ( fHash )
            iFunc = Gia_ManHashAnd( pNew, Abc_LitNotCond(iFunc0, pTemp->fCompl0), Abc_LitNotCond(iFunc1, pTemp->fCompl1) ); 
        else
            iFunc = Gia_ManAppendAnd( pNew, Abc_LitNotCond(iFunc0, pTemp->fCompl0), Abc_LitNotCond(iFunc1, pTemp->fCompl1) ); 
        if ( pTemp->fPhase != pIfObj->fPhase )
            iFunc = Abc_LitNot(iFunc);
        If_CutSetDataInt( pCut, iFunc );
        break;
    }
    return If_CutDataInt(pCut);
}
int Gia_ManNodeIfToGia( Gia_Man_t * pNew, If_Man_t * pIfMan, If_Obj_t * pIfObj, Vec_Int_t * vLeaves, int fHash )
{
    If_Cut_t * pCut;
    If_Obj_t * pLeaf;
    int i, iRes;
    // get the best cut
    pCut = If_ObjCutBest(pIfObj);
    assert( pCut->nLeaves > 1 );
    // set the leaf variables
    If_CutForEachLeaf( pIfMan, pCut, pLeaf, i )
        If_CutSetDataInt( If_ObjCutBest(pLeaf), Vec_IntEntry(vLeaves, i) );
    // recursively compute the function while collecting visited cuts
    Vec_PtrClear( pIfMan->vTemp );
    iRes = Gia_ManNodeIfToGia_rec( pNew, pIfMan, pIfObj, pIfMan->vTemp, fHash ); 
    if ( iRes == ~0 )
    {
        Abc_Print( -1, "Gia_ManNodeIfToGia(): Computing local AIG has failed.\n" );
        return ~0;
    }
    // clean the cuts
    If_CutForEachLeaf( pIfMan, pCut, pLeaf, i )
        If_CutSetDataInt( If_ObjCutBest(pLeaf), 0 );
    Vec_PtrForEachEntry( If_Cut_t *, pIfMan->vTemp, pCut, i )
        If_CutSetDataInt( pCut, 0 );
    return iRes;
}

/**Function*************************************************************

  Synopsis    [Converts IF into GIA manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManFromIfLogicFindLut( If_Man_t * pIfMan, Gia_Man_t * pNew, If_Cut_t * pCutBest, sat_solver * pSat, Vec_Int_t * vLeaves, Vec_Int_t * vLits, Vec_Int_t * vCover, Vec_Int_t * vMapping, Vec_Int_t * vMapping2, Vec_Int_t * vPacking )
{
    word uBound, uFree;
    int nLutSize = (int)(pIfMan->pPars->pLutStruct[0] - '0');
    int nVarsF = 0, pVarsF[IF_MAX_FUNC_LUTSIZE];
    int nVarsB = 0, pVarsB[IF_MAX_FUNC_LUTSIZE];
    int nVarsS = 0, pVarsS[IF_MAX_FUNC_LUTSIZE];
    unsigned uSetNew, uSetOld;
    int RetValue, RetValue2, k;
    char * pPerm;
    if ( Vec_IntSize(vLeaves) <= nLutSize )
    {
        RetValue = Gia_ManFromIfLogicCreateLut( pNew, If_CutTruthW(pIfMan, pCutBest), vLeaves, vCover, vMapping, vMapping2 );
        // write packing
        if ( !Gia_ObjIsCi(Gia_ManObj(pNew, Abc_Lit2Var(RetValue))) && RetValue > 1 )
        {
            Vec_IntPush( vPacking, 1 );
            Vec_IntPush( vPacking, Abc_Lit2Var(RetValue) );
            Vec_IntAddToEntry( vPacking, 0, 1 );
        }
        return RetValue;
    }
    assert( If_DsdManSuppSize(pIfMan->pIfDsdMan, If_CutDsdLit(pIfMan, pCutBest)) == (int)pCutBest->nLeaves );
    // find the bound set
    if ( pIfMan->pPars->fDelayOptLut )
        uSetOld = pCutBest->uMaskFunc;
    else
        uSetOld = If_DsdManCheckXY( pIfMan->pIfDsdMan, If_CutDsdLit(pIfMan, pCutBest), nLutSize, 1, 0, 1, 0 );
    // remap bound set
    uSetNew = 0;
    pPerm = If_CutDsdPerm( pIfMan, pCutBest );
    for ( k = 0; k < If_CutLeaveNum(pCutBest); k++ )
    {
        int iVar = Abc_Lit2Var((int)pPerm[k]);
        int Value = ((uSetOld >> (k << 1)) & 3);
        if ( Value == 1 )
            uSetNew |= (1 << (2*iVar));
        else if ( Value == 3 )
            uSetNew |= (3 << (2*iVar));
        else assert( Value == 0 );
    }
    RetValue = If_ManSatCheckXY( pSat, nLutSize, If_CutTruthW(pIfMan, pCutBest), pCutBest->nLeaves, uSetNew, &uBound, &uFree, vLits );
    assert( RetValue );
    // collect variables
    for ( k = 0; k < If_CutLeaveNum(pCutBest); k++ )
    {
        int Value = ((uSetNew >> (k << 1)) & 3);
        if ( Value == 0 )
            pVarsF[nVarsF++] = k;
        else if ( Value == 1 )
            pVarsB[nVarsB++] = k;
        else if ( Value == 3 )
            pVarsS[nVarsS++] = k;
        else assert( Value == 0 );
    }
    // collect bound set variables
    Vec_IntClear( vLits );
    for ( k = 0; k < nVarsS; k++ )
        Vec_IntPush( vLits, Vec_IntEntry(vLeaves, pVarsS[k]) );
    for ( k = 0; k < nVarsB; k++ )
        Vec_IntPush( vLits, Vec_IntEntry(vLeaves, pVarsB[k]) );
    RetValue = Gia_ManFromIfLogicCreateLut( pNew, &uBound, vLits, vCover, vMapping, vMapping2 );
    // collecct free set variables
    Vec_IntClear( vLits );
    Vec_IntPush( vLits, RetValue );
    for ( k = 0; k < nVarsS; k++ )
        Vec_IntPush( vLits, Vec_IntEntry(vLeaves, pVarsS[k]) );
    for ( k = 0; k < nVarsF; k++ )
        Vec_IntPush( vLits, Vec_IntEntry(vLeaves, pVarsF[k]) );
    // add packing
    RetValue2 = Gia_ManFromIfLogicCreateLut( pNew, &uFree, vLits, vCover, vMapping, vMapping2 );
    // write packing
    Vec_IntPush( vPacking, 2 );
    Vec_IntPush( vPacking, Abc_Lit2Var(RetValue) );
    Vec_IntPush( vPacking, Abc_Lit2Var(RetValue2) );
    Vec_IntAddToEntry( vPacking, 0, 1 );
    return RetValue2;
}

/**Function*************************************************************

  Synopsis    [Converts IF into GIA manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Gia_Man_t * Gia_ManFromIfLogic( If_Man_t * pIfMan )
{
    Gia_Man_t * pNew;
    If_Cut_t * pCutBest;
    If_Obj_t * pIfObj, * pIfLeaf;
    Vec_Int_t * vMapping, * vMapping2, * vPacking = NULL;
    Vec_Int_t * vLeaves, * vLeaves2, * vCover, * vLits;
    sat_solver * pSat = NULL;
    int i, k, Entry;
    assert( !pIfMan->pPars->fDeriveLuts || pIfMan->pPars->fTruth );
//    if ( pIfMan->pPars->fEnableCheck07 )
//        pIfMan->pPars->fDeriveLuts = 0;
    // start mapping and packing
    vMapping  = Vec_IntStart( If_ManObjNum(pIfMan) );
    vMapping2 = Vec_IntStart( 1 );
    if ( pIfMan->pPars->fDeriveLuts && (pIfMan->pPars->pLutStruct || pIfMan->pPars->fEnableCheck75 || pIfMan->pPars->fEnableCheck75u || pIfMan->pPars->fEnableCheck07) )
    {
        vPacking = Vec_IntAlloc( 1000 );
        Vec_IntPush( vPacking, 0 );
    }
    // create new manager
    pNew = Gia_ManStart( If_ManObjNum(pIfMan) );
    // iterate through nodes used in the mapping
    vLits    = Vec_IntAlloc( 1000 );
    vCover   = Vec_IntAlloc( 1 << 16 );
    vLeaves  = Vec_IntAlloc( 16 );
    vLeaves2 = Vec_IntAlloc( 16 );
    If_ManCleanCutData( pIfMan );
    If_ManForEachObj( pIfMan, pIfObj, i )
    {
        if ( pIfObj->nRefs == 0 && !If_ObjIsTerm(pIfObj) )
            continue;
        if ( If_ObjIsAnd(pIfObj) )
        {
            pCutBest = If_ObjCutBest( pIfObj );
            // perform sorting of cut leaves by delay, so that the slowest pin drives the fastest input of the LUT
            if ( !pIfMan->pPars->fUseTtPerm && !pIfMan->pPars->fDelayOpt && !pIfMan->pPars->fDelayOptLut && !pIfMan->pPars->fDsdBalance && !pIfMan->pPars->pLutStruct && !pIfMan->pPars->fUserRecLib && !pIfMan->pPars->nGateSize && !pIfMan->pPars->fEnableCheck75 && !pIfMan->pPars->fEnableCheck75u && !pIfMan->pPars->fEnableCheck07 )
                If_CutRotatePins( pIfMan, pCutBest );
            // collect leaves of the best cut
            Vec_IntClear( vLeaves );
            If_CutForEachLeaf( pIfMan, pCutBest, pIfLeaf, k )
                Vec_IntPush( vLeaves, pIfLeaf->iCopy );
            // perform one of the two types of mapping: with and without structures
            if ( pIfMan->pPars->fUseDsd && pIfMan->pPars->pLutStruct )
            {
                if ( pSat == NULL )
                    pSat = (sat_solver *)If_ManSatBuildXY( (int)(pIfMan->pPars->pLutStruct[0] - '0') );
                if ( pIfMan->pPars->pLutStruct && pIfMan->pPars->fDeriveLuts )
                    pIfObj->iCopy = Gia_ManFromIfLogicFindLut( pIfMan, pNew, pCutBest, pSat, vLeaves, vLits, vCover, vMapping, vMapping2, vPacking );
                else
                    pIfObj->iCopy = Gia_ManFromIfLogicCreateLut( pNew, If_CutTruthW(pIfMan, pCutBest), vLeaves, vCover, vMapping, vMapping2 );
                pIfObj->iCopy = Abc_LitNotCond( pIfObj->iCopy, pCutBest->fCompl );
            }
            else if ( (pIfMan->pPars->fDeriveLuts && pIfMan->pPars->fTruth) || pIfMan->pPars->fUseDsd || pIfMan->pPars->fUseTtPerm )
            {
                word * pTruth = If_CutTruthW(pIfMan, pCutBest);
                if ( pIfMan->pPars->fUseTtPerm )
                    for ( k = 0; k < (int)pCutBest->nLeaves; k++ )
                        if ( If_CutLeafBit(pCutBest, k) )
                            Abc_TtFlip( pTruth, Abc_TtWordNum(pCutBest->nLeaves), k );
                // perform decomposition of the cut
                pIfObj->iCopy = Gia_ManFromIfLogicNode( pIfMan, pNew, i, vLeaves, vLeaves2, pTruth, pIfMan->pPars->pLutStruct, vCover, vMapping, vMapping2, vPacking, (pIfMan->pPars->fEnableCheck75 || pIfMan->pPars->fEnableCheck75u), pIfMan->pPars->fEnableCheck07 );
                pIfObj->iCopy = Abc_LitNotCond( pIfObj->iCopy, pCutBest->fCompl );
            }
            else
            {
                pIfObj->iCopy = Gia_ManNodeIfToGia( pNew, pIfMan, pIfObj, vLeaves, 0 );
                // write mapping
                Vec_IntSetEntry( vMapping, Abc_Lit2Var(pIfObj->iCopy), Vec_IntSize(vMapping2) );
                Vec_IntPush( vMapping2, Vec_IntSize(vLeaves) );
                Vec_IntForEachEntry( vLeaves, Entry, k )
                    assert( Abc_Lit2Var(Entry) < Abc_Lit2Var(pIfObj->iCopy) );
                Vec_IntForEachEntry( vLeaves, Entry, k )
                    Vec_IntPush( vMapping2, Abc_Lit2Var(Entry)  );
                Vec_IntPush( vMapping2, Abc_Lit2Var(pIfObj->iCopy) );
            }
        }
        else if ( If_ObjIsCi(pIfObj) )
            pIfObj->iCopy = Gia_ManAppendCi(pNew);
        else if ( If_ObjIsCo(pIfObj) )
            pIfObj->iCopy = Gia_ManAppendCo( pNew, Abc_LitNotCond(If_ObjFanin0(pIfObj)->iCopy, If_ObjFaninC0(pIfObj)) );
        else if ( If_ObjIsConst1(pIfObj) )
        {
            pIfObj->iCopy = 1;
            // create const LUT
            Vec_IntWriteEntry( vMapping, 0, Vec_IntSize(vMapping2) );
            Vec_IntPush( vMapping2, 0 );
            Vec_IntPush( vMapping2, 0 );
        }
        else assert( 0 );
    }
    Vec_IntFree( vLits );
    Vec_IntFree( vCover );
    Vec_IntFree( vLeaves );
    Vec_IntFree( vLeaves2 );
    if ( pSat != NULL )
        sat_solver_delete(pSat);
//    printf( "Mapping array size:  IfMan = %d. Gia = %d. Increase = %.2f\n", 
//        If_ManObjNum(pIfMan), Gia_ManObjNum(pNew), 1.0 * Gia_ManObjNum(pNew) / If_ManObjNum(pIfMan) );
    // finish mapping 
    if ( Vec_IntSize(vMapping) > Gia_ManObjNum(pNew) )
        Vec_IntShrink( vMapping, Gia_ManObjNum(pNew) );
    else
        Vec_IntFillExtra( vMapping, Gia_ManObjNum(pNew), 0 );
    assert( Vec_IntSize(vMapping) == Gia_ManObjNum(pNew) );
    Vec_IntForEachEntry( vMapping, Entry, i )
        if ( Entry > 0 )
            Vec_IntAddToEntry( vMapping, i, Gia_ManObjNum(pNew) );
    Vec_IntAppend( vMapping, vMapping2 );
    Vec_IntFree( vMapping2 );
    // attach mapping and packing
    assert( pNew->vMapping == NULL );
    assert( pNew->vPacking == NULL );
    pNew->vMapping = vMapping;
    pNew->vPacking = vPacking;
    // verify that COs have mapping
    {
        Gia_Obj_t * pObj;
        Gia_ManForEachCo( pNew, pObj, i )
           assert( !Gia_ObjIsAnd(Gia_ObjFanin0(pObj)) || Gia_ObjIsLut(pNew, Gia_ObjFaninId0p(pNew, pObj)) );
    }
    return pNew;
}


/**Function*************************************************************

  Synopsis    [Verifies mapping.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Gia_ManMappingVerify_rec( Gia_Man_t * p, Gia_Obj_t * pObj )
{
    int Id, iFan, k, Result = 1;
    if ( Gia_ObjIsTravIdCurrent(p, pObj) )
        return 1;
    Gia_ObjSetTravIdCurrent(p, pObj);
    if ( !Gia_ObjIsAnd(pObj) )
        return 1;
    if ( !Gia_ObjIsLut(p, Gia_ObjId(p, pObj)) )
    {
        Abc_Print( -1, "Gia_ManMappingVerify: Internal node %d does not have mapping.\n", Gia_ObjId(p, pObj) );
        return 0;
    }
    Id = Gia_ObjId(p, pObj);
    Gia_LutForEachFanin( p, Id, iFan, k )
        if ( Result )
            Result &= Gia_ManMappingVerify_rec( p, Gia_ManObj(p, iFan) );
    return Result;
}
void Gia_ManMappingVerify( Gia_Man_t * p )
{
    Gia_Obj_t * pObj, * pFanin;
    int i, Result = 1;
    assert( Gia_ManHasMapping(p) );
    Gia_ManIncrementTravId( p );
    Gia_ManForEachCo( p, pObj, i )
    {
        pFanin = Gia_ObjFanin0(pObj);
        if ( !Gia_ObjIsAnd(pFanin) )
            continue;
        if ( !Gia_ObjIsLut(p, Gia_ObjId(p, pFanin)) )
        {
            Abc_Print( -1, "Gia_ManMappingVerify: CO driver %d does not have mapping.\n", Gia_ObjId(p, pFanin) );
            Result = 0;
            continue;
        }
        Result &= Gia_ManMappingVerify_rec( p, pFanin );
    }
//    if ( Result && Gia_NtkIsRoot(p) )
//        Abc_Print( 1, "Mapping verified correctly.\n" );
}


/**Function*************************************************************

  Synopsis    [Transfers mapping from hie GIA to normalized GIA.]

  Description [Hie GIA (pGia) points to normalized GIA (p).]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Gia_ManTransferMapping( Gia_Man_t * pGia, Gia_Man_t * p )
{
    Gia_Obj_t * pObj;
    int i, k, iFan;
    if ( !Gia_ManHasMapping(pGia) )
        return;
    Gia_ManMappingVerify( pGia );
    Vec_IntFreeP( &p->vMapping );
    p->vMapping = Vec_IntAlloc( 2 * Gia_ManObjNum(p) );
    Vec_IntFill( p->vMapping, Gia_ManObjNum(p), 0 );
    Gia_ManForEachLut( pGia, i )
    {
        assert( !Abc_LitIsCompl(Gia_ObjValue(Gia_ManObj(pGia, i))) );
        pObj = Gia_ManObj( p, Abc_Lit2Var(Gia_ObjValue(Gia_ManObj(pGia, i))) );
        Vec_IntWriteEntry( p->vMapping, Gia_ObjId(p, pObj), Vec_IntSize(p->vMapping) );
        Vec_IntPush( p->vMapping, Gia_ObjLutSize(pGia, i) );
        Gia_LutForEachFanin( pGia, i, iFan, k )
            Vec_IntPush( p->vMapping, Abc_Lit2Var(Gia_ObjValue(Gia_ManObj(pGia, iFan))) );
        Vec_IntPush( p->vMapping, Gia_ObjId(p, pObj) );
    }
    Gia_ManMappingVerify( p );
}

/**Function*************************************************************

  Synopsis    [Transfers packing from hie GIA to normalized GIA.]

  Description [Hie GIA (pGia) points to normalized GIA (p).]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Gia_ManTransferPacking( Gia_Man_t * pGia, Gia_Man_t * p )
{
    Vec_Int_t * vPackingNew;
    Gia_Obj_t * pObj, * pObjNew;
    int i, k, Entry, nEntries, nEntries2;
    if ( pGia->vPacking == NULL )
        return;
    nEntries = Vec_IntEntry( pGia->vPacking, 0 );
    nEntries2 = 0;
    // create new packing info
    vPackingNew = Vec_IntAlloc( Vec_IntSize(pGia->vPacking) );
    Vec_IntPush( vPackingNew, nEntries );
    Vec_IntForEachEntryStart( pGia->vPacking, Entry, i, 1 )
    {
        assert( Entry > 0 && Entry < 4 );
        Vec_IntPush( vPackingNew, Entry );
        i++;
        for ( k = 0; k < Entry; k++, i++ )
        {
            pObj = Gia_ManObj(pGia, Vec_IntEntry(pGia->vPacking, i));
            pObjNew = Gia_ManObj(p, Abc_Lit2Var(Gia_ObjValue(pObj)));
            assert( Gia_ObjIsLut(pGia, Gia_ObjId(pGia, pObj)) );
            assert( Gia_ObjIsLut(p, Gia_ObjId(p, pObjNew)) );
            Vec_IntPush( vPackingNew, Gia_ObjId(p, pObjNew) );
//            printf( "%d -> %d  ", Vec_IntEntry(pGia->vPacking, i), Gia_ObjId(p, pObjNew) );
        }
        i--;
        nEntries2++;
    }
    assert( nEntries == nEntries2 );
    // attach packing info
    assert( p->vPacking == NULL );
    p->vPacking = vPackingNew;
}


/**Function*************************************************************

  Synopsis    [Interface of LUT mapping package.]

  Description []
               
  SideEffects []

  SeeAlso     [] 

***********************************************************************/
Gia_Man_t * Gia_ManPerformMapping( Gia_Man_t * p, void * pp, int fNormalized )
{
    extern void Gia_ManIffTest( Gia_Man_t * pGia, If_LibLut_t * pLib, int fVerbose );
    Gia_Man_t * pNew;
    If_Man_t * pIfMan;
    If_Par_t * pPars = (If_Par_t *)pp;
    // disable cut minimization when GIA strucure is needed
    if ( !pPars->fDelayOpt && !pPars->fDelayOptLut && !pPars->fDsdBalance && !pPars->fUserRecLib && !pPars->fDeriveLuts && !pPars->fUseDsd && !pPars->fUseTtPerm )
        pPars->fCutMin = 0;

    // reconstruct GIA according to the hierarchy manager
    assert( pPars->pTimesArr == NULL );
    assert( pPars->pTimesReq == NULL );
    if ( p->pManTime )
    {
        if ( fNormalized )
        {
            pNew = Gia_ManDupUnnormalize( p );
            if ( pNew == NULL )
                return NULL;
            pNew->pManTime   = p->pManTime;   p->pManTime  = NULL;
            pNew->pAigExtra  = p->pAigExtra;  p->pAigExtra = NULL;
            pNew->nAnd2Delay = p->nAnd2Delay; p->nAnd2Delay = 0;
            p = pNew;
            // set arrival and required times
            pPars->pTimesArr = Tim_ManGetArrTimes( (Tim_Man_t *)p->pManTime );
            pPars->pTimesReq = Tim_ManGetReqTimes( (Tim_Man_t *)p->pManTime );
        }
    }
    else 
        p = Gia_ManDup( p );
    // translate into the mapper
    pIfMan = Gia_ManToIf( p, pPars );    
    if ( pIfMan == NULL )
    {
        Gia_ManStop( p );
        return NULL;
    }
    // create DSD manager
    if ( pPars->fUseDsd )
    {
        If_DsdMan_t * p = (If_DsdMan_t *)Abc_FrameReadManDsd();
        assert( pPars->nLutSize <= If_DsdManVarNum(p) );
        assert( (pPars->pLutStruct == NULL && If_DsdManLutSize(p) == 0) || (pPars->pLutStruct && pPars->pLutStruct[0] - '0' == If_DsdManLutSize(p)) );
        pIfMan->pIfDsdMan = (If_DsdMan_t *)Abc_FrameReadManDsd();
        if ( pPars->fDsdBalance )
            If_DsdManAllocIsops( pIfMan->pIfDsdMan, pPars->nLutSize );
    }
    // compute switching for the IF objects
    if ( pPars->fPower )
    {
        Gia_Obj_t * pObj; int i;
        assert( pIfMan->vSwitching == NULL );
        pIfMan->vSwitching = Vec_IntStart( If_ManObjNum(pIfMan) );
        Gia_ManForEachObj( p, pObj, i )
            if ( ~Gia_ObjValue(pObj) )
                Vec_IntWriteEntry( pIfMan->vSwitching, Gia_ObjValue(pObj), Vec_IntEntry(p->vSwitching, i) );
//        Vec_IntFreeP( &p->vSwitching );
    }
    if ( p->pManTime )
        pIfMan->pManTim = Tim_ManDup( (Tim_Man_t *)p->pManTime, 0 );
    if ( !If_ManPerformMapping( pIfMan ) )
    {
        If_ManStop( pIfMan );
        Gia_ManStop( p );
        return NULL;
    }
    // transform the result of mapping into the new network
    if ( pIfMan->pPars->fDelayOpt || pIfMan->pPars->fDsdBalance || pIfMan->pPars->fUserRecLib )
        pNew = Gia_ManFromIfAig( pIfMan );
    else
        pNew = Gia_ManFromIfLogic( pIfMan );
    If_ManStop( pIfMan );
    // transfer name
    assert( pNew->pName == NULL );
    pNew->pName = Abc_UtilStrsav( p->pName );
    pNew->pSpec = Abc_UtilStrsav( p->pSpec );
    Gia_ManSetRegNum( pNew, Gia_ManRegNum(p) );
    // return the original (unmodified by the mapper) timing manager
    pNew->pManTime   = p->pManTime;   p->pManTime   = NULL;
    pNew->pAigExtra  = p->pAigExtra;  p->pAigExtra  = NULL;
    pNew->nAnd2Delay = p->nAnd2Delay; p->nAnd2Delay = 0;
    Gia_ManStop( p );
    // normalize and transfer mapping
    pNew = Gia_ManDupNormalize( p = pNew );
    Gia_ManTransferMapping( p, pNew );
    Gia_ManTransferPacking( p, pNew );
    pNew->pManTime   = p->pManTime;   p->pManTime   = NULL;
    pNew->pAigExtra  = p->pAigExtra;  p->pAigExtra  = NULL;
    pNew->nAnd2Delay = p->nAnd2Delay; p->nAnd2Delay = 0;
    Gia_ManStop( p );
//    printf( "PERFORMING VERIFICATION:\n" );
//    Gia_ManVerifyWithBoxes( pNew, NULL );
//    if ( pPars->fRepack )
//        Gia_ManIffTest( pNew, pPars->pLutLib, pPars->fVerbose );
    return pNew;
}

/**Function*************************************************************

  Synopsis    [Tests decomposition structures.]

  Description []
               
  SideEffects []

  SeeAlso     [] 

***********************************************************************/
void Gia_ManTestStruct( Gia_Man_t * p )
{
    int nCutMax = 7;
    int LutCount[8] = {0}, LutNDecomp[8] = {0};
    int i, k, iFan, nFanins, Status;
    Vec_Int_t * vLeaves;
    word * pTruth;

    vLeaves = Vec_IntAlloc( 100 );
    Gia_ObjComputeTruthTableStart( p, nCutMax );
    Gia_ManForEachLut( p, i )
    {
        nFanins = Gia_ObjLutSize(p, i);
        assert( nFanins <= 7 );
        LutCount[Abc_MaxInt(nFanins, 5)]++;
        if ( nFanins <= 5 )
            continue;
        Vec_IntClear( vLeaves );
        Gia_LutForEachFanin( p, i, iFan, k )
            Vec_IntPush( vLeaves, iFan );
        pTruth = Gia_ObjComputeTruthTableCut( p, Gia_ManObj(p, i), vLeaves );
        // check if it is decomposable
        Status = If_CutPerformCheck07( NULL, (unsigned *)pTruth, 7, nFanins, NULL );
        if ( Status == 1 )
            continue;
        LutNDecomp[nFanins]++;
        if ( LutNDecomp[nFanins] > 10 )
            continue;
        Kit_DsdPrintFromTruth( (unsigned *)pTruth, nFanins ); printf( "\n" );
    }
    Gia_ObjComputeTruthTableStop( p );

    printf( "LUT5 = %d    ", LutCount[5] );
    printf( "LUT6 = %d  NonDec = %d (%.2f %%)    ", LutCount[6], LutNDecomp[6], 100.0 * LutNDecomp[6]/Abc_MaxInt(LutCount[6], 1) );
    printf( "LUT7 = %d  NonDec = %d (%.2f %%)    ", LutCount[7], LutNDecomp[7], 100.0 * LutNDecomp[7]/Abc_MaxInt(LutCount[7], 1) );
    printf( "\n" );
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

