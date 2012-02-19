/**CFile****************************************************************

  FileName    [saigBmc.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Sequential AIG package.]

  Synopsis    [Simple BMC package.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: saigBmc.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "saig.h"
#include "src/sat/cnf/cnf.h"
#include "src/sat/bsat/satStore.h"
#include "src/proof/ssw/ssw.h"

ABC_NAMESPACE_IMPL_START


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

#define AIG_VISITED       ((Aig_Obj_t *)(ABC_PTRUINT_T)1)

typedef struct Saig_Bmc_t_ Saig_Bmc_t;
struct Saig_Bmc_t_
{
    // parameters
    int                   nFramesMax;     // the max number of timeframes to consider
    int                   nNodesMax;      // the max number of nodes to add
    int                   nConfMaxOne;    // the max number of conflicts at a target
    int                   nConfMaxAll;    // the max number of conflicts for all targets
    int                   fVerbose;       // enables verbose output
    // AIG managers
    Aig_Man_t *           pAig;           // the user's AIG manager
    Aig_Man_t *           pFrm;           // Frames manager
    Vec_Ptr_t *           vVisited;       // nodes visited in Frames
    // node mapping
    int                   nObjs;          // the largest number of an AIG object
    Vec_Ptr_t *           vAig2Frm;       // mapping of AIG nodees into Frames nodes
//    Vec_Str_t *           vAig2Frm2;      // mapping of AIG nodees into Frames nodes
    // SAT solver
    sat_solver *          pSat;           // SAT solver
    int                   nSatVars;       // the number of used SAT variables
    Vec_Int_t *           vObj2Var;       // mapping of frames objects in CNF variables
    int                   nStitchVars;
    // subproblems
    Vec_Ptr_t *           vTargets;       // targets to be solved in this interval
    int                   iFramePrev;     // previous frame  
    int                   iFrameLast;     // last frame  
    int                   iOutputLast;    // last output
    int                   iFrameFail;     // failed frame 
    int                   iOutputFail;    // failed output
};

static inline int         Saig_BmcSatNum( Saig_Bmc_t * p, Aig_Obj_t * pObj )                                { return Vec_IntGetEntry( p->vObj2Var, pObj->Id );  }
static inline void        Saig_BmcSetSatNum( Saig_Bmc_t * p, Aig_Obj_t * pObj, int Num )                    { Vec_IntSetEntry(p->vObj2Var, pObj->Id, Num);      }

static inline Aig_Obj_t * Saig_BmcObjFrame( Saig_Bmc_t * p, Aig_Obj_t * pObj, int i )                       { return (Aig_Obj_t *)Vec_PtrGetEntry( p->vAig2Frm, p->nObjs*i+pObj->Id );     }
static inline void        Saig_BmcObjSetFrame( Saig_Bmc_t * p, Aig_Obj_t * pObj, int i, Aig_Obj_t * pNode ) { Vec_PtrSetEntry( p->vAig2Frm, p->nObjs*i+pObj->Id, pNode );     }

static inline Aig_Obj_t * Saig_BmcObjChild0( Saig_Bmc_t * p, Aig_Obj_t * pObj, int i )                   { assert( !Aig_IsComplement(pObj) ); return Aig_NotCond(Saig_BmcObjFrame(p, Aig_ObjFanin0(pObj), i), Aig_ObjFaninC0(pObj));  }
static inline Aig_Obj_t * Saig_BmcObjChild1( Saig_Bmc_t * p, Aig_Obj_t * pObj, int i )                   { assert( !Aig_IsComplement(pObj) ); return Aig_NotCond(Saig_BmcObjFrame(p, Aig_ObjFanin1(pObj), i), Aig_ObjFaninC1(pObj));  }

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

#define ABS_ZER 1
#define ABS_ONE 2
#define ABS_UND 3

static inline int Abs_ManSimInfoNot( int Value )
{
    if ( Value == ABS_ZER )
        return ABS_ONE;
    if ( Value == ABS_ONE )
        return ABS_ZER;
    return ABS_UND;
}

static inline int Abs_ManSimInfoAnd( int Value0, int Value1 )
{
    if ( Value0 == ABS_ZER || Value1 == ABS_ZER )
        return ABS_ZER;
    if ( Value0 == ABS_ONE && Value1 == ABS_ONE )
        return ABS_ONE;
    return ABS_UND;
}

static inline int Abs_ManSimInfoGet( Vec_Ptr_t * vSimInfo, Aig_Obj_t * pObj, int iFrame )
{
    unsigned * pInfo = (unsigned *)Vec_PtrEntry( vSimInfo, iFrame );
    return 3 & (pInfo[Aig_ObjId(pObj) >> 4] >> ((Aig_ObjId(pObj) & 15) << 1));
}

static inline void Abs_ManSimInfoSet( Vec_Ptr_t * vSimInfo, Aig_Obj_t * pObj, int iFrame, int Value )
{
    unsigned * pInfo = (unsigned *)Vec_PtrEntry( vSimInfo, iFrame );
    assert( Value >= ABS_ZER && Value <= ABS_UND );
    Value ^= Abs_ManSimInfoGet( vSimInfo, pObj, iFrame );
    pInfo[Aig_ObjId(pObj) >> 4] ^= (Value << ((Aig_ObjId(pObj) & 15) << 1));
}

/**Function*************************************************************

  Synopsis    [Performs ternary simulation for one node.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abs_ManExtendOneEval_rec( Vec_Ptr_t * vSimInfo, Aig_Man_t * p, Aig_Obj_t * pObj, int iFrame )
{
    int Value0, Value1, Value;
    Value = Abs_ManSimInfoGet( vSimInfo, pObj, iFrame );
    if ( Value )
        return Value;
    if ( Aig_ObjIsPi(pObj) )
    {
        assert( Saig_ObjIsLo(p, pObj) );
        Value = Abs_ManExtendOneEval_rec( vSimInfo, p, Saig_ObjLoToLi(p, pObj), iFrame-1 );
        Abs_ManSimInfoSet( vSimInfo, pObj, iFrame, Value );
        return Value;
    }
    Value0 = Abs_ManExtendOneEval_rec( vSimInfo, p, Aig_ObjFanin0(pObj), iFrame );
    if ( Aig_ObjFaninC0(pObj) )
        Value0 = Abs_ManSimInfoNot( Value0 );
    if ( Aig_ObjIsPo(pObj) )
    {
        Abs_ManSimInfoSet( vSimInfo, pObj, iFrame, Value0 );
        return Value0;
    }
    assert( Aig_ObjIsNode(pObj) );
    if ( Value0 == ABS_ZER )
        Value = ABS_ZER;
    else
    {
        Value1 = Abs_ManExtendOneEval_rec( vSimInfo, p, Aig_ObjFanin1(pObj), iFrame );
        if ( Aig_ObjFaninC1(pObj) )
            Value1 = Abs_ManSimInfoNot( Value1 );
        Value = Abs_ManSimInfoAnd( Value0, Value1 );
    }
    Abs_ManSimInfoSet( vSimInfo, pObj, iFrame, Value );
    assert( Value );
    return Value;
}

/**Function*************************************************************

  Synopsis    [Performs ternary simulation for one design.]

  Description [The returned array contains the result of ternary 
  simulation for all the frames where the output could be proved 0.]
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Vec_Ptr_t * Abs_ManTernarySimulate( Aig_Man_t * p, int nFramesMax, int fVerbose )
{
    Vec_Ptr_t * vSimInfo;
    Aig_Obj_t * pObj;
    int i, f, nFramesLimit, nFrameWords;
    int clk = clock();
    assert( Aig_ManRegNum(p) > 0 );
    // the maximum number of frames will be determined to use at most 200Mb of RAM
    nFramesLimit = 1 + (200000000 * 4)/Aig_ManObjNum(p);
    nFramesLimit = Abc_MinInt( nFramesLimit, nFramesMax );
    nFrameWords  = Abc_BitWordNum( 2 * Aig_ManObjNum(p) );
    // allocate simulation info
    vSimInfo = Vec_PtrAlloc( nFramesLimit );
    for ( f = 0; f < nFramesLimit; f++ )
    {
        Vec_PtrPush( vSimInfo, ABC_CALLOC(unsigned, nFrameWords) );
        if ( f == 0 ) 
        {
            Saig_ManForEachLo( p, pObj, i )
                Abs_ManSimInfoSet( vSimInfo, pObj, 0, ABS_ZER );
        }
        Abs_ManSimInfoSet( vSimInfo, Aig_ManConst1(p), f, ABS_ONE );
        Saig_ManForEachPi( p, pObj, i )
            Abs_ManSimInfoSet( vSimInfo, pObj, f, ABS_UND );
        Saig_ManForEachPo( p, pObj, i )
            Abs_ManExtendOneEval_rec( vSimInfo, p, pObj, f );
        // check if simulation has derived at least one fail or unknown
        Saig_ManForEachPo( p, pObj, i )
            if ( Abs_ManSimInfoGet(vSimInfo, pObj, f) != ABS_ZER )
            {
                if ( fVerbose )
                {
                    printf( "Ternary sim found non-zero output in frame %d.  Used %5.2f Mb.  ", 
                        f, 0.25 * (f+1) * Aig_ManObjNum(p) / (1<<20) );
                    ABC_PRT( "Time", clock() - clk );
                }
                return vSimInfo;
            }
    }
    if ( fVerbose )
    {
        printf( "Ternary sim proved all outputs in the first %d frames.  Used %5.2f Mb.  ", 
            nFramesLimit, 0.25 * nFramesLimit * Aig_ManObjNum(p) / (1<<20) );
        ABC_PRT( "Time", clock() - clk );
    }
    return vSimInfo;
}

/**Function*************************************************************

  Synopsis    [Free the array of simulation info.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abs_ManFreeAray( Vec_Ptr_t * p )
{
    void * pTemp;
    int i;
    Vec_PtrForEachEntry( void *, p, pTemp, i )
        ABC_FREE( pTemp );
    Vec_PtrFree( p );
}


/**Function*************************************************************

  Synopsis    [Create manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Saig_Bmc_t * Saig_BmcManStart( Aig_Man_t * pAig, int nFramesMax, int nNodesMax, int nConfMaxOne, int nConfMaxAll, int fVerbose )
{
    Saig_Bmc_t * p;
    Aig_Obj_t * pObj;
    int i, Lit;
//    assert( Aig_ManRegNum(pAig) > 0 );
    p = (Saig_Bmc_t *)ABC_ALLOC( char, sizeof(Saig_Bmc_t) );
    memset( p, 0, sizeof(Saig_Bmc_t) );
    // set parameters
    p->nFramesMax   = nFramesMax;
    p->nNodesMax    = nNodesMax;
    p->nConfMaxOne  = nConfMaxOne;
    p->nConfMaxAll  = nConfMaxAll;
    p->fVerbose     = fVerbose;
    p->pAig         = pAig;
    p->nObjs        = Aig_ManObjNumMax(pAig);
    // create node and variable mappings
    p->vAig2Frm     = Vec_PtrAlloc( 0 );
    Vec_PtrFill( p->vAig2Frm, 5 * p->nObjs, NULL );
    p->vObj2Var     = Vec_IntAlloc( 0 );
    Vec_IntFill( p->vObj2Var, 5 * p->nObjs, 0 );
    // start the AIG manager and map primary inputs
    p->pFrm         = Aig_ManStart( 5 * p->nObjs );
    Saig_ManForEachLo( pAig, pObj, i )
        Saig_BmcObjSetFrame( p, pObj, 0, Aig_ManConst0(p->pFrm) ); 
    // create SAT solver
    p->nSatVars     = 1;
    p->pSat         = sat_solver_new();
    sat_solver_setnvars( p->pSat, 2000 );
    Lit = toLit( p->nSatVars );
    sat_solver_addclause( p->pSat, &Lit, &Lit + 1 );
    Saig_BmcSetSatNum( p, Aig_ManConst1(p->pFrm), p->nSatVars++ );
    // other data structures
    p->vTargets     = Vec_PtrAlloc( 0 );
    p->vVisited     = Vec_PtrAlloc( 0 );
    p->iOutputFail  = -1;
    p->iFrameFail   = -1;
    return p;
}

/**Function*************************************************************

  Synopsis    [Delete manager.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Saig_BmcManStop( Saig_Bmc_t * p )
{
//    Aig_Obj_t * pObj;
//    int i, f, Counter = 0;
//    int i, Counter = 0;
//    for ( i = 0; i < p->vAig2Frm2->nSize; i++ )
//        Counter += (p->vAig2Frm2->pArray[i] != 0);
//    for ( i = 0; i < p->vAig2Frm->nSize; i++ )
//        Counter += (p->vAig2Frm->pArray[i] != NULL);
//    printf( "Objs = %d.  Nodes = %d.  Frames = %d.  Used = %d.  Non-empty = %d.\n", 
//        p->nObjs, Aig_ManNodeNum(p->pAig), p->iFrameLast, p->vAig2Frm->nSize, Counter );

/*
    Aig_ManForEachObj( p->pAig, pObj, i )
    {
        int Last = -1;
        for ( f = 0; f <= p->iFrameLast; f++ )
            if ( Saig_BmcObjFrame( p, pObj, f ) )
                Last = f;
        if ( i % 50 == 0 )
            printf( "%d ", Last );
    }
    printf( "\n" );
*/

    Aig_ManStop( p->pFrm  );
    Vec_PtrFree( p->vAig2Frm );
//    Vec_StrFree( p->vAig2Frm2 );
    Vec_IntFree( p->vObj2Var );
    sat_solver_delete( p->pSat );
    Vec_PtrFree( p->vTargets );
    Vec_PtrFree( p->vVisited );
    ABC_FREE( p );
}

/**Function*************************************************************

  Synopsis    [Explores the possibility of constructing the output.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Saig_BmcIntervalExplore_rec( Saig_Bmc_t * p, Aig_Obj_t * pObj, int i )
{
    Aig_Obj_t * pRes, * p0, * p1, * pConst1 = Aig_ManConst1(p->pAig);
    pRes = Saig_BmcObjFrame( p, pObj, i );
    if ( pRes != NULL )
        return pRes;
    if ( Saig_ObjIsPi( p->pAig, pObj ) )
        pRes = AIG_VISITED;
    else if ( Saig_ObjIsLo( p->pAig, pObj ) )
        pRes = Saig_BmcIntervalExplore_rec( p, Saig_ObjLoToLi(p->pAig, pObj), i-1 );
    else if ( Aig_ObjIsPo( pObj ) )
    {
        pRes = Saig_BmcIntervalExplore_rec( p, Aig_ObjFanin0(pObj), i );
        if ( pRes != AIG_VISITED )
            pRes = Saig_BmcObjChild0( p, pObj, i );
    }
    else 
    {
        p0 = Saig_BmcIntervalExplore_rec( p, Aig_ObjFanin0(pObj), i );
        if ( p0 != AIG_VISITED )
            p0 = Saig_BmcObjChild0( p, pObj, i );
        p1 = Saig_BmcIntervalExplore_rec( p, Aig_ObjFanin1(pObj), i );
        if ( p1 != AIG_VISITED )
            p1 = Saig_BmcObjChild1( p, pObj, i );

        if ( p0 == Aig_Not(p1) )
            pRes = Aig_ManConst0(p->pFrm);
        else if ( Aig_Regular(p0) == pConst1 )
            pRes = (p0 == pConst1) ? p1 : Aig_ManConst0(p->pFrm);
        else if ( Aig_Regular(p1) == pConst1 )
            pRes = (p1 == pConst1) ? p0 : Aig_ManConst0(p->pFrm);
        else 
            pRes = AIG_VISITED;

        if ( pRes != AIG_VISITED && !Aig_ObjIsConst1(Aig_Regular(pRes)) )
            pRes = AIG_VISITED;
    }
    Saig_BmcObjSetFrame( p, pObj, i, pRes );
    return pRes;
}

/**Function*************************************************************

  Synopsis    [Performs the actual construction of the output.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Saig_BmcIntervalConstruct_rec( Saig_Bmc_t * p, Aig_Obj_t * pObj, int i, Vec_Ptr_t * vVisited )
{
    Aig_Obj_t * pRes;
    pRes = Saig_BmcObjFrame( p, pObj, i );
    if ( pRes != NULL )
        return pRes;
    if ( Saig_ObjIsPi( p->pAig, pObj ) )
        pRes = Aig_ObjCreatePi(p->pFrm);
    else if ( Saig_ObjIsLo( p->pAig, pObj ) )
        pRes = Saig_BmcIntervalConstruct_rec( p, Saig_ObjLoToLi(p->pAig, pObj), i-1, vVisited );
    else if ( Aig_ObjIsPo( pObj ) )
    {
        Saig_BmcIntervalConstruct_rec( p, Aig_ObjFanin0(pObj), i, vVisited );
        pRes = Saig_BmcObjChild0( p, pObj, i );
    }
    else 
    {
        Saig_BmcIntervalConstruct_rec( p, Aig_ObjFanin0(pObj), i, vVisited );
        if ( Saig_BmcObjChild0(p, pObj, i) == Aig_ManConst0(p->pFrm) )
            pRes = Aig_ManConst0(p->pFrm);
        else
        {
            Saig_BmcIntervalConstruct_rec( p, Aig_ObjFanin1(pObj), i, vVisited );
            pRes = Aig_And( p->pFrm, Saig_BmcObjChild0(p, pObj, i), Saig_BmcObjChild1(p, pObj, i) );
        }
    }
    assert( pRes != NULL );
    Saig_BmcObjSetFrame( p, pObj, i, pRes );
    Vec_PtrPush( vVisited, pObj );
    Vec_PtrPush( vVisited, (void *)(ABC_PTRINT_T)i );
    return pRes;
}

/**Function*************************************************************

  Synopsis    [Adds new AIG nodes to the frames.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Saig_BmcInterval( Saig_Bmc_t * p )
{
    Aig_Obj_t * pTarget;
    Aig_Obj_t * pObj, * pRes;
    int i, iFrame, Counter;
    int nNodes = Aig_ManObjNum( p->pFrm );
    Vec_PtrClear( p->vTargets );
    p->iFramePrev = p->iFrameLast;
    for ( ; p->iFrameLast < p->nFramesMax; p->iFrameLast++, p->iOutputLast = 0 )
    {
        if ( p->iOutputLast == 0 )
        {
            Saig_BmcObjSetFrame( p, Aig_ManConst1(p->pAig), p->iFrameLast, Aig_ManConst1(p->pFrm) );
        }
        for ( ; p->iOutputLast < Saig_ManPoNum(p->pAig); p->iOutputLast++ )
        {
            if ( Aig_ManObjNum(p->pFrm) >= nNodes + p->nNodesMax )
                return;
//            Saig_BmcIntervalExplore_rec( p, Aig_ManPo(p->pAig, p->iOutputLast), p->iFrameLast );
            Vec_PtrClear( p->vVisited );
            pTarget = Saig_BmcIntervalConstruct_rec( p, Aig_ManPo(p->pAig, p->iOutputLast), p->iFrameLast, p->vVisited );
            Vec_PtrPush( p->vTargets, pTarget );
            Aig_ObjCreatePo( p->pFrm, pTarget );
            Aig_ManCleanup( p->pFrm );
            // check if the node is gone
            Counter = 0;
            Vec_PtrForEachEntry( Aig_Obj_t *, p->vVisited, pObj, i )
            {
                iFrame = (int)(ABC_PTRINT_T)Vec_PtrEntry( p->vVisited, 1+i++ );
                pRes = Saig_BmcObjFrame( p, pObj, iFrame );
                if ( Aig_ObjIsNone( Aig_Regular(pRes) ) )
                {
                    Saig_BmcObjSetFrame( p, pObj, iFrame, NULL );
                    Counter++;
                }
            }
//            printf( "%d ", Counter );
        }
    }
}

/**Function*************************************************************

  Synopsis    [Performs the actual construction of the output.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Obj_t * Saig_BmcIntervalToAig_rec( Saig_Bmc_t * p, Aig_Man_t * pNew, Aig_Obj_t * pObj )
{
    if ( pObj->pData )
        return (Aig_Obj_t *)pObj->pData;
    Vec_PtrPush( p->vVisited, pObj );
    if ( Saig_BmcSatNum(p, pObj) || Aig_ObjIsPi(pObj) )
    {
        p->nStitchVars += !Aig_ObjIsPi(pObj);
        return (Aig_Obj_t *)(pObj->pData = Aig_ObjCreatePi(pNew));
    }
    Saig_BmcIntervalToAig_rec( p, pNew, Aig_ObjFanin0(pObj) );
    Saig_BmcIntervalToAig_rec( p, pNew, Aig_ObjFanin1(pObj) );
    assert( pObj->pData == NULL );
    return (Aig_Obj_t *)(pObj->pData = Aig_And( pNew, Aig_ObjChild0Copy(pObj), Aig_ObjChild1Copy(pObj) ));
}

/**Function*************************************************************

  Synopsis    [Creates AIG of the newly added nodes.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Aig_Man_t * Saig_BmcIntervalToAig( Saig_Bmc_t * p )
{
    Aig_Man_t * pNew;
    Aig_Obj_t * pObj, * pObjNew;
    int i;
    Aig_ManForEachObj( p->pFrm, pObj, i )
        assert( pObj->pData == NULL );

    pNew = Aig_ManStart( p->nNodesMax );
    Aig_ManConst1(p->pFrm)->pData = Aig_ManConst1(pNew);
    Vec_PtrClear( p->vVisited );
    Vec_PtrPush( p->vVisited, Aig_ManConst1(p->pFrm) );
    Vec_PtrForEachEntry( Aig_Obj_t *, p->vTargets, pObj, i )
    {
//        assert( !Aig_ObjIsConst1(Aig_Regular(pObj)) );
        pObjNew = Saig_BmcIntervalToAig_rec( p, pNew, Aig_Regular(pObj) );
        assert( !Aig_IsComplement(pObjNew) );
        Aig_ObjCreatePo( pNew, pObjNew );
    }
    return pNew;
}

/**Function*************************************************************

  Synopsis    [Derives CNF for the newly added nodes.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Saig_BmcLoadCnf( Saig_Bmc_t * p, Cnf_Dat_t * pCnf )
{
    Aig_Obj_t * pObj, * pObjNew;
    int i, Lits[2], VarNumOld, VarNumNew;
    Vec_PtrForEachEntry( Aig_Obj_t *, p->vVisited, pObj, i )
    {
        // get the new variable of this node
        pObjNew     = (Aig_Obj_t *)pObj->pData;
        pObj->pData = NULL;
        VarNumNew   = pCnf->pVarNums[ pObjNew->Id ];
        if ( VarNumNew == -1 )
            continue;
        // get the old variable of this node
        VarNumOld   = Saig_BmcSatNum( p, pObj );
        if ( VarNumOld == 0 )
        {
            Saig_BmcSetSatNum( p, pObj, VarNumNew );
            continue;
        }
        // add clauses connecting existing variables
        Lits[0] = toLitCond( VarNumOld, 0 );
        Lits[1] = toLitCond( VarNumNew, 1 );
        if ( !sat_solver_addclause( p->pSat, Lits, Lits+2 ) )
            assert( 0 );
        Lits[0] = toLitCond( VarNumOld, 1 );
        Lits[1] = toLitCond( VarNumNew, 0 );
        if ( !sat_solver_addclause( p->pSat, Lits, Lits+2 ) )
            assert( 0 );
    }
    // add CNF to the SAT solver
    for ( i = 0; i < pCnf->nClauses; i++ )
        if ( !sat_solver_addclause( p->pSat, pCnf->pClauses[i], pCnf->pClauses[i+1] ) )
            break;
    if ( i < pCnf->nClauses )
        printf( "SAT solver became UNSAT after adding clauses.\n" );
}

/**Function*************************************************************

  Synopsis    [Solves targets with the given resource limit.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Saig_BmcDeriveFailed( Saig_Bmc_t * p, int iTargetFail )
{
    int k;
    p->iOutputFail = p->iOutputLast;
    p->iFrameFail  = p->iFrameLast;
    for ( k = Vec_PtrSize(p->vTargets); k > iTargetFail; k-- )
    {
        if ( p->iOutputFail == 0 )
        {
            p->iOutputFail = Saig_ManPoNum(p->pAig);
            p->iFrameFail--;
        }
        p->iOutputFail--;
    }
}

/**Function*************************************************************

  Synopsis    [Solves targets with the given resource limit.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Abc_Cex_t * Saig_BmcGenerateCounterExample( Saig_Bmc_t * p )
{
    Abc_Cex_t * pCex;
    Aig_Obj_t * pObj, * pObjFrm;
    int i, f, iVarNum;
    // start the counter-example
    pCex = Abc_CexAlloc( Aig_ManRegNum(p->pAig), Saig_ManPiNum(p->pAig), p->iFrameFail+1 );
    pCex->iFrame = p->iFrameFail;
    pCex->iPo    = p->iOutputFail;
    // copy the bit data
    for ( f = 0; f <= p->iFrameFail; f++ )
    {
        Saig_ManForEachPi( p->pAig, pObj, i )
        {
            pObjFrm = Saig_BmcObjFrame( p, pObj, f );
            if ( pObjFrm == NULL )
                continue;
            iVarNum = Saig_BmcSatNum( p, pObjFrm );
            if ( iVarNum == 0 )
                continue;
            if ( sat_solver_var_value( p->pSat, iVarNum ) )
                Abc_InfoSetBit( pCex->pData, pCex->nRegs + Saig_ManPiNum(p->pAig) * f + i );
        }
    }
    // verify the counter example
    if ( !Saig_ManVerifyCex( p->pAig, pCex ) )
    {
        printf( "Saig_BmcGenerateCounterExample(): Counter-example is invalid.\n" );
        Abc_CexFree( pCex );
        pCex = NULL;
    }
    return pCex;
}

/**Function*************************************************************

  Synopsis    [Solves targets with the given resource limit.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Saig_BmcSolveTargets( Saig_Bmc_t * p, int nStart, int * pnOutsSolved )
{
    Aig_Obj_t * pObj;
    int i, VarNum, Lit, RetValue;
    assert( Vec_PtrSize(p->vTargets) > 0 );
    if ( p->pSat->qtail != p->pSat->qhead )
    {
        RetValue = sat_solver_simplify(p->pSat);
        assert( RetValue != 0 );
    }
    Vec_PtrForEachEntry( Aig_Obj_t *, p->vTargets, pObj, i )
    {
        if ( ((*pnOutsSolved)++ / Saig_ManPoNum(p->pAig)) < nStart )
            continue;
        if ( p->nConfMaxAll && p->pSat->stats.conflicts > p->nConfMaxAll )
            return l_Undef;
        VarNum = Saig_BmcSatNum( p, Aig_Regular(pObj) );
        Lit = toLitCond( VarNum, Aig_IsComplement(pObj) );
        RetValue = sat_solver_solve( p->pSat, &Lit, &Lit + 1, (ABC_INT64_T)p->nConfMaxOne, (ABC_INT64_T)0, (ABC_INT64_T)0, (ABC_INT64_T)0 );
        if ( RetValue == l_False ) // unsat
            continue;
        if ( RetValue == l_Undef ) // undecided
            return l_Undef;
        // generate counter-example
        Saig_BmcDeriveFailed( p, i );
        p->pAig->pSeqModel = Saig_BmcGenerateCounterExample( p );

        {
//            extern Vec_Int_t * Saig_ManExtendCounterExampleTest( Aig_Man_t * p, int iFirstPi, void * pCex );
//            Saig_ManExtendCounterExampleTest( p->pAig, 0, p->pAig->pSeqModel );
        }
        return l_True;
    }
    return l_False;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Saig_BmcAddTargetsAsPos( Saig_Bmc_t * p )
{
    Aig_Obj_t * pObj;
    int i;
    Vec_PtrForEachEntry( Aig_Obj_t *, p->vTargets, pObj, i )
        Aig_ObjCreatePo( p->pFrm, pObj );
    Aig_ManPrintStats( p->pFrm );
    Aig_ManCleanup( p->pFrm );
    Aig_ManPrintStats( p->pFrm );
} 

/**Function*************************************************************

  Synopsis    [Performs BMC with the given parameters.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Saig_BmcPerform( Aig_Man_t * pAig, int nStart, int nFramesMax, int nNodesMax, int nTimeOut, int nConfMaxOne, int nConfMaxAll, int fVerbose, int fVerbOverwrite, int * piFrames )
{
    Saig_Bmc_t * p;
    Aig_Man_t * pNew;
    Cnf_Dat_t * pCnf;
    int nTimeToStop = time(NULL) + nTimeOut;
    int nOutsSolved = 0;
    int Iter, RetValue = -1, clk = clock(), clk2, clkTotal = clock();
    int Status = -1;
/*
    Vec_Ptr_t * vSimInfo;
    vSimInfo = Abs_ManTernarySimulate( pAig, nFramesMax, fVerbose );
    Abs_ManFreeAray( vSimInfo );
*/
    if ( fVerbose )
    {
        printf( "Running \"bmc2\". AIG:  PI/PO/Reg = %d/%d/%d.  Node = %6d. Lev = %5d.\n", 
            Saig_ManPiNum(pAig), Saig_ManPoNum(pAig), Saig_ManRegNum(pAig),
            Aig_ManNodeNum(pAig), Aig_ManLevelNum(pAig) );
        printf( "Params: FramesMax = %d. NodesDelta = %d. ConfMaxOne = %d. ConfMaxAll = %d.\n", 
            nFramesMax, nNodesMax, nConfMaxOne, nConfMaxAll );
    } 
    nFramesMax = nFramesMax ? nFramesMax : ABC_INFINITY;
    p = Saig_BmcManStart( pAig, nFramesMax, nNodesMax, nConfMaxOne, nConfMaxAll, fVerbose );
    // set runtime limit
    if ( nTimeOut )
        sat_solver_set_runtime_limit( p->pSat, nTimeToStop );
    for ( Iter = 0; ; Iter++ )
    {
        clk2 = clock();
        // add new logic interval to frames
        Saig_BmcInterval( p );
//        Saig_BmcAddTargetsAsPos( p );
        if ( Vec_PtrSize(p->vTargets) == 0 )
            break;
        // convert logic slice into new AIG
        pNew = Saig_BmcIntervalToAig( p );
//printf( "StitchVars = %d.\n", p->nStitchVars );
        // derive CNF for the new AIG
        pCnf = Cnf_Derive( pNew, Aig_ManPoNum(pNew) );
        Cnf_DataLift( pCnf, p->nSatVars );
        p->nSatVars += pCnf->nVars;
        // add this CNF to the solver
        Saig_BmcLoadCnf( p, pCnf );
        Cnf_DataFree( pCnf );
        Aig_ManStop( pNew );
        // solve the targets
        RetValue = Saig_BmcSolveTargets( p, nStart, &nOutsSolved );
        if ( fVerbose )
        {
            printf( "%3d : F = %3d. O =%4d.  And = %7d. Var = %7d. Conf = %7d. ", 
                Iter, p->iFrameLast, p->iOutputLast, Aig_ManNodeNum(p->pFrm), p->nSatVars, (int)p->pSat->stats.conflicts );   
//            ABC_PRT( "Time", clock() - clk2 );
            printf( "%9.2f sec", (float)(clock() - clkTotal)/(float)(CLOCKS_PER_SEC) );
            printf( "\n" );
        }
        if ( RetValue != l_False )
            break;
        // check the timeout
        if ( nTimeOut && time(NULL) > nTimeToStop )
        {
            printf( "Reached timeout (%d seconds).\n",  nTimeOut );
            if ( piFrames )
                *piFrames = p->iFrameLast-1;
            Saig_BmcManStop( p );
            return Status;
        }
    }
    if ( RetValue == l_True )
    {
        assert( p->iFrameFail * Saig_ManPoNum(p->pAig) + p->iOutputFail + 1 == nOutsSolved );
        printf( "Output %d was asserted in frame %d (use \"write_counter\" to dump a witness). ", 
            p->iOutputFail, p->iFrameFail );
        Status = 0;
        if ( piFrames )
            *piFrames = p->iFrameFail;
    }
    else // if ( RetValue == l_False || RetValue == l_Undef )
    {
        printf( "No output was asserted in %d frames.  ", p->iFramePrev-1 );
        if ( piFrames )
        {
            if ( p->iOutputLast > 0 )
                *piFrames = p->iFramePrev - 1;
            else
                *piFrames = p->iFramePrev;
        }
    }
    if ( fVerbOverwrite )
    {
        ABC_PRTr( "Time", clock() - clk );
    }
    else
    {
        ABC_PRT( "Time", clock() - clk );
    }
    if ( RetValue != l_True )
    {
        if ( p->iFrameLast >= p->nFramesMax )
            printf( "Reached limit on the number of timeframes (%d).\n", p->nFramesMax );
        else if ( p->nConfMaxAll && p->pSat->stats.conflicts > p->nConfMaxAll )
            printf( "Reached global conflict limit (%d).\n", p->nConfMaxAll );
        else if ( nTimeOut && time(NULL) > nTimeToStop )
            printf( "Reached timeout (%d seconds).\n", nTimeOut );
        else
            printf( "Reached local conflict limit (%d).\n", p->nConfMaxOne );
    }
    Saig_BmcManStop( p );
    return Status;
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

