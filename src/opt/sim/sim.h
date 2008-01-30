/**CFile****************************************************************

  FileName    [sim.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Simulation package.]

  Synopsis    [External declarations.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: sim.h,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/
 
#ifndef __SIM_H__
#define __SIM_H__

/*
    The ideas realized in this package are described in the paper:
    "Detecting Symmetries in Boolean Functions using Circuit Representation, 
    Simulation, and Satisfiability".
*/

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                         PARAMETERS                               ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                         BASIC TYPES                              ///
////////////////////////////////////////////////////////////////////////

typedef struct Sym_Man_t_ Sym_Man_t;
struct Sym_Man_t_
{
    // info about the network
    Abc_Ntk_t *       pNtk;          // the network 
    Vec_Ptr_t *       vNodes;        // internal nodes in topological order
    int               nInputs;
    int               nOutputs;
    // internal simulation information
    int               nSimWords;     // the number of bits in simulation info
    Vec_Ptr_t *       vSim;          // simulation info
    // support information
    Vec_Ptr_t *       vSuppFun;      // bit representation
    Vec_Vec_t *       vSupports;     // integer representation
    // symmetry info for each output
    Vec_Ptr_t *       vMatrSymms;    // symmetric pairs
    Vec_Ptr_t *       vMatrNonSymms; // non-symmetric pairs
    Vec_Int_t *       vPairsTotal;   // total pairs
    Vec_Int_t *       vPairsSym;     // symmetric pairs
    Vec_Int_t *       vPairsNonSym;  // non-symmetric pairs
    // temporary simulation info
    unsigned *        uPatRand;
    unsigned *        uPatCol;
    unsigned *        uPatRow;
    // temporary
    Vec_Int_t *       vVarsU;
    Vec_Int_t *       vVarsV;
    int               iOutput;
    int               iVar1;
    int               iVar2;
    int               iVar1Old;
    int               iVar2Old;
    // internal data structures
    int               nSatRuns;
    int               nSatRunsSat;
    int               nSatRunsUnsat;
    // pairs
    int               nPairsSymm;
    int               nPairsSymmStr;
    int               nPairsNonSymm;
    int               nPairsRem;
    int               nPairsTotal;
    // runtime statistics
    int               timeStruct;
    int               timeCount;
    int               timeMatr;
    int               timeSim;
    int               timeFraig;
    int               timeSat;
    int               timeTotal;
};

typedef struct Sim_Man_t_ Sim_Man_t;
struct Sim_Man_t_
{
    // info about the network
    Abc_Ntk_t *       pNtk;
    int               nInputs;
    int               nOutputs;
    // internal simulation information
    int               nSimBits;      // the number of bits in simulation info
    int               nSimWords;     // the number of words in simulation info
    Vec_Ptr_t *       vSim0;         // simulation info 1
    Vec_Ptr_t *       vSim1;         // simulation info 2
    // support information
    int               nSuppBits;     // the number of bits in support info
    int               nSuppWords;    // the number of words in support info
    Vec_Ptr_t *       vSuppStr;      // structural supports
    Vec_Ptr_t *       vSuppFun;      // functional supports
    // simulation targets
    Vec_Vec_t *       vSuppTargs;    // support targets
    int               iInput;        // the input current processed
    // internal data structures
    Extra_MmFixed_t * pMmPat;   
    Vec_Ptr_t *       vFifo;
    Vec_Int_t *       vDiffs;
    int               nSatRuns;
    int               nSatRunsSat;
    int               nSatRunsUnsat;
    // runtime statistics
    int               timeSim;
    int               timeTrav;
    int               timeFraig;
    int               timeSat;
    int               timeTotal;
};

typedef struct Sim_Pat_t_ Sim_Pat_t;
struct Sim_Pat_t_
{
    int              Input;         // the input which it has detected
    int              Output;        // the output for which it was collected
    unsigned *       pData;         // the simulation data
};

////////////////////////////////////////////////////////////////////////
///                      MACRO DEFITIONS                             ///
////////////////////////////////////////////////////////////////////////

#define SIM_NUM_WORDS(n)      ((n)/32 + (((n)%32) > 0))
#define SIM_LAST_BITS(n)      ((((n)%32) > 0)? (n)%32 : 32)

#define SIM_MASK_FULL         (0xFFFFFFFF)
#define SIM_MASK_BEG(n)       (SIM_MASK_FULL >> (32-n))
#define SIM_MASK_END(n)       (SIM_MASK_FULL << (n))
#define SIM_SET_0_FROM(m,n)   ((m) & ~SIM_MASK_BEG(n))
#define SIM_SET_1_FROM(m,n)   ((m) |  SIM_MASK_END(n))

// generating random unsigned (#define RAND_MAX 0x7fff)
#define SIM_RANDOM_UNSIGNED   ((((unsigned)rand()) << 24) ^ (((unsigned)rand()) << 12) ^ ((unsigned)rand()))

// macros to get hold of bits in a bit string
#define Sim_SetBit(p,i)       ((p)[(i)>>5] |= (1<<((i) & 31)))
#define Sim_XorBit(p,i)       ((p)[(i)>>5] ^= (1<<((i) & 31)))
#define Sim_HasBit(p,i)      (((p)[(i)>>5]  & (1<<((i) & 31))) > 0)

// macros to get hold of the support info
#define Sim_SuppStrSetVar(vSupps,pNode,v)     Sim_SetBit((unsigned*)(vSupps)->pArray[(pNode)->Id],(v))
#define Sim_SuppStrHasVar(vSupps,pNode,v)     Sim_HasBit((unsigned*)(vSupps)->pArray[(pNode)->Id],(v))
#define Sim_SuppFunSetVar(vSupps,Output,v)    Sim_SetBit((unsigned*)(vSupps)->pArray[Output],(v))
#define Sim_SuppFunHasVar(vSupps,Output,v)    Sim_HasBit((unsigned*)(vSupps)->pArray[Output],(v))
#define Sim_SimInfoSetVar(vSupps,pNode,v)     Sim_SetBit((unsigned*)(vSupps)->pArray[(pNode)->Id],(v))
#define Sim_SimInfoHasVar(vSupps,pNode,v)     Sim_HasBit((unsigned*)(vSupps)->pArray[(pNode)->Id],(v))

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DECLARATIONS                         ///
////////////////////////////////////////////////////////////////////////

/*=== simMan.c ==========================================================*/
extern Sym_Man_t *     Sym_ManStart( Abc_Ntk_t * pNtk, int fVerbose );
extern void            Sym_ManStop( Sym_Man_t * p );
extern void            Sym_ManPrintStats( Sym_Man_t * p );
extern Sim_Man_t *     Sim_ManStart( Abc_Ntk_t * pNtk );
extern void            Sim_ManStop( Sim_Man_t * p );
extern void            Sim_ManPrintStats( Sim_Man_t * p );
extern Sim_Pat_t *     Sim_ManPatAlloc( Sim_Man_t * p );
extern void            Sim_ManPatFree( Sim_Man_t * p, Sim_Pat_t * pPat );
/*=== simSupp.c ==========================================================*/
extern Vec_Ptr_t *     Sim_ComputeStrSupp( Abc_Ntk_t * pNtk );
extern Vec_Ptr_t *     Sim_ComputeFunSupp( Abc_Ntk_t * pNtk, int fVerbose );
/*=== simSym.c ==========================================================*/
extern int             Sim_ComputeTwoVarSymms( Abc_Ntk_t * pNtk, int fVerbose );
/*=== simSymSat.c ==========================================================*/
extern int             Sim_SymmsGetPatternUsingSat( Sym_Man_t * p, unsigned * pPattern );
/*=== simSymStr.c ==========================================================*/
extern void            Sim_SymmsStructCompute( Abc_Ntk_t * pNtk, Vec_Ptr_t * vMatrs, Vec_Ptr_t * vSuppFun );
/*=== simSymSim.c ==========================================================*/
extern void            Sim_SymmsSimulate( Sym_Man_t * p, unsigned * pPatRand, Vec_Ptr_t * vMatrsNonSym );
/*=== simUtil.c ==========================================================*/
extern Vec_Ptr_t *     Sim_UtilInfoAlloc( int nSize, int nWords, bool fClean );
extern void            Sim_UtilInfoFree( Vec_Ptr_t * p );
extern void            Sim_UtilInfoAdd( unsigned * pInfo1, unsigned * pInfo2, int nWords );
extern void            Sim_UtilInfoDetectDiffs( unsigned * pInfo1, unsigned * pInfo2, int nWords, Vec_Int_t * vDiffs );
extern void            Sim_UtilInfoDetectNews( unsigned * pInfo1, unsigned * pInfo2, int nWords, Vec_Int_t * vDiffs );
extern void            Sim_UtilInfoFlip( Sim_Man_t * p, Abc_Obj_t * pNode );
extern bool            Sim_UtilInfoCompare( Sim_Man_t * p, Abc_Obj_t * pNode );
extern void            Sim_UtilSimulate( Sim_Man_t * p, bool fFirst );
extern void            Sim_UtilSimulateNode( Sim_Man_t * p, Abc_Obj_t * pNode, bool fType, bool fType1, bool fType2 );
extern void            Sim_UtilSimulateNodeOne( Abc_Obj_t * pNode, Vec_Ptr_t * vSimInfo, int nSimWords );
extern int             Sim_UtilCountSuppSizes( Sim_Man_t * p, int fStruct );
extern int             Sim_UtilCountOnes( unsigned * pSimInfo, int nSimWords );
extern void            Sim_UtilGetRandom( unsigned * pPatRand, int nSimWords );
extern int             Sim_UtilCountAllPairs( Vec_Ptr_t * vSuppFun, int nSimWords, Vec_Int_t * vCounters );
extern void            Sim_UtilCountPairsAll( Sym_Man_t * p );
extern int             Sim_UtilMatrsAreDisjoint( Sym_Man_t * p );

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

#endif

