/**CFile****************************************************************

  FileName    [seqInt.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Construction and manipulation of sequential AIGs.]

  Synopsis    [Internal declarations.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: seqInt.h,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/
 
#ifndef __SEQ_INT_H__
#define __SEQ_INT_H__

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#include "abc.h"
#include "cut.h"
#include "seq.h"

////////////////////////////////////////////////////////////////////////
///                         PARAMETERS                               ///
////////////////////////////////////////////////////////////////////////

#define SEQ_FULL_MASK        0xFFFFFFFF   

// node status after updating its arrival time
enum { SEQ_UPDATE_FAIL, SEQ_UPDATE_NO, SEQ_UPDATE_YES };

////////////////////////////////////////////////////////////////////////
///                         BASIC TYPES                              ///
////////////////////////////////////////////////////////////////////////

// manager of sequential AIG
struct Abc_Seq_t_
{
    // sequential information
    Abc_Ntk_t *         pNtk;           // the network
    int                 nSize;          // the number of entries in all internal arrays
    Vec_Ptr_t *         vInits;         // the initial states for each edge in the AIG
    Extra_MmFixed_t *   pMmInits;       // memory manager for latch structures used to remember init states
    int                 fVerbose;       // the verbose flag
    // K-feasible cuts
    int                 nVarsMax;       // the max cut size
    Cut_Man_t *         pCutMan;        // cut manager
    // sequential arrival time computation
    Vec_Int_t *         vLValues;       // the arrival times (L-Values of nodes)
    Vec_Str_t *         vLags;          // the lags of the mapped nodes
    // representation of the mapping
    Vec_Ptr_t *         vMapAnds;       // nodes visible in the mapping 
    Vec_Vec_t *         vMapCuts;       // best cuts for each node 
    // runtime stats
    int                 timeCuts;       // runtime to compute the cuts
    int                 timeDelay;      // runtime to compute the L-values
    int                 timeRet;        // runtime to retime the resulting network
    int                 timeNtk;        // runtime to create the final network

};

// data structure to store initial state
typedef struct Seq_Lat_t_  Seq_Lat_t;
struct Seq_Lat_t_
{
    Seq_Lat_t *         pNext;          // the next Lat in the ring
    Seq_Lat_t *         pPrev;          // the prev Lat in the ring 
};

// representation of latch on the edge
typedef struct Seq_RetEdge_t_       Seq_RetEdge_t;   
struct Seq_RetEdge_t_ // 1 word
{
    unsigned            iNode    : 24;  // the ID of the node
    unsigned            iEdge    :  1;  // the edge of the node
    unsigned            iLatch   :  7;  // the latch number counting from the node
};

// representation of one retiming step
typedef struct Seq_RetStep_t_       Seq_RetStep_t;   
struct Seq_RetStep_t_ // 1 word
{
    unsigned            iNode    : 24;  // the ID of the node
    unsigned            nLatches :  8;  // the number of latches to retime
};

////////////////////////////////////////////////////////////////////////
///                      MACRO DEFINITIONS                           ///
////////////////////////////////////////////////////////////////////////

// transforming retedges into ints and back
static inline int            Seq_RetEdge2Int( Seq_RetEdge_t Val )  { return *((int *)&Val);           }
static inline Seq_RetEdge_t  Seq_Int2RetEdge( int Num )            { return *((Seq_RetEdge_t *)&Num); }
// transforming retsteps into ints and back
static inline int            Seq_RetStep2Int( Seq_RetStep_t Val )  { return *((int *)&Val);           }
static inline Seq_RetStep_t  Seq_Int2RetStep( int Num )            { return *((Seq_RetStep_t *)&Num); }

// reading l-values and lags
static inline Vec_Int_t *    Seq_NodeLValues( Abc_Obj_t * pNode )               { return ((Abc_Seq_t *)(pNode)->pNtk->pManFunc)->vLValues;           }
static inline int            Seq_NodeGetLValue( Abc_Obj_t * pNode )             { return Vec_IntEntry( Seq_NodeLValues(pNode), (pNode)->Id );        }
static inline void           Seq_NodeSetLValue( Abc_Obj_t * pNode, int Value )  { Vec_IntWriteEntry( Seq_NodeLValues(pNode), (pNode)->Id, Value );   }
static inline int            Seq_NodeComputeLag( int LValue, int Fi )           { return (LValue + 1024*Fi)/Fi - 1024 - (int)(LValue % Fi == 0);     }

// reading best cuts at each node
static inline Cut_Man_t *    Seq_NodeCutMan( Abc_Obj_t * pNode )                { return ((Abc_Seq_t *)(pNode)->pNtk->pManFunc)->pCutMan;                 }
//static inline Vec_Ptr_t *    Seq_NodeCutBests( Abc_Obj_t * pNode )              { return ((Abc_Seq_t *)(pNode)->pNtk->pManFunc)->vBestCuts;               }
//static inline Cut_Cut_t *    Seq_NodeGetCutBest( Abc_Obj_t * pNode )            { return Vec_PtrEntry( Seq_NodeCutBests(pNode), (pNode)->Id );            }
//static inline void           Seq_NodeSetCutBest( Abc_Obj_t * pNode, Cut_Cut_t * pCut ) { Vec_PtrWriteEntry( Seq_NodeCutBests(pNode), (pNode)->Id, pCut ); }

// reading the contents of the lat
static inline Abc_InitType_t Seq_LatInit( Seq_Lat_t * pLat )  { return ((unsigned)pLat->pPrev) & 3;                                 }
static inline Seq_Lat_t *    Seq_LatNext( Seq_Lat_t * pLat )  { return pLat->pNext;                                                 }
static inline Seq_Lat_t *    Seq_LatPrev( Seq_Lat_t * pLat )  { return (void *)(((unsigned)pLat->pPrev) & (SEQ_FULL_MASK << 2));    }

// setting the contents of the lat
static inline void           Seq_LatSetInit( Seq_Lat_t * pLat, Abc_InitType_t Init )  { pLat->pPrev = (void *)( (3 & Init) | (((unsigned)pLat->pPrev) & (SEQ_FULL_MASK << 2)) );    }
static inline void           Seq_LatSetNext( Seq_Lat_t * pLat, Seq_Lat_t * pNext )    { pLat->pNext = pNext;                                                                        }
static inline void           Seq_LatSetPrev( Seq_Lat_t * pLat, Seq_Lat_t * pPrev )    { Abc_InitType_t Init = Seq_LatInit(pLat);  pLat->pPrev = pPrev;  Seq_LatSetInit(pLat, Init); }

// accessing retiming lags
static inline Vec_Str_t *    Seq_NodeLags( Abc_Obj_t * pNode )                        { return ((Abc_Seq_t *)(pNode)->pNtk->pManFunc)->vLags;            }
static inline char           Seq_NodeGetLag( Abc_Obj_t * pNode )                      { return Vec_StrEntry( Seq_NodeLags(pNode), (pNode)->Id );         }
static inline void           Seq_NodeSetLag( Abc_Obj_t * pNode, char Value )          { Vec_StrWriteEntry( Seq_NodeLags(pNode), (pNode)->Id, (Value) );  }

// accessing initial states
static inline Vec_Ptr_t *    Seq_NodeLats( Abc_Obj_t * pObj )                                { return ((Abc_Seq_t*)pObj->pNtk->pManFunc)->vInits;                                           }
static inline Seq_Lat_t *    Seq_NodeGetRing( Abc_Obj_t * pObj, int Edge )                   { return Vec_PtrEntry( Seq_NodeLats(pObj), (pObj->Id<<1)+Edge );                               }
static inline void           Seq_NodeSetRing( Abc_Obj_t * pObj, int Edge, Seq_Lat_t * pLat ) { Vec_PtrWriteEntry( Seq_NodeLats(pObj), (pObj->Id<<1)+Edge, pLat );                           }
static inline Seq_Lat_t *    Seq_NodeCreateLat( Abc_Obj_t * pObj )                           { return (Seq_Lat_t *)Extra_MmFixedEntryFetch( ((Abc_Seq_t*)pObj->pNtk->pManFunc)->pMmInits ); }
static inline void           Seq_NodeRecycleLat( Abc_Obj_t * pObj, Seq_Lat_t * pLat )        { Extra_MmFixedEntryRecycle( ((Abc_Seq_t*)pObj->pNtk->pManFunc)->pMmInits, (char *)pLat );     }

// getting hold of the structure storing initial states of the latches
static inline Seq_Lat_t *    Seq_NodeGetLatFirst( Abc_Obj_t * pObj, int Edge )               { return Seq_NodeGetRing(pObj, Edge);                      }
static inline Seq_Lat_t *    Seq_NodeGetLatLast( Abc_Obj_t * pObj, int Edge )                { return Seq_LatPrev( Seq_NodeGetRing(pObj, Edge) );       }
static inline Seq_Lat_t *    Seq_NodeGetLat( Abc_Obj_t * pObj, int Edge, int iLat )          { int c; Seq_Lat_t * pLat = Seq_NodeGetRing(pObj, Edge); for ( c = 0; c != iLat; c++ ) pLat = pLat->pNext; return pLat; }
static inline int            Seq_NodeCountLats( Abc_Obj_t * pObj, int Edge )                 { int c; Seq_Lat_t * pLat, * pRing = Seq_NodeGetRing(pObj, Edge); if ( pRing == NULL ) return 0; for ( c = 0, pLat = pRing; !c || pLat != pRing; c++ )      pLat = pLat->pNext; return c; }

// getting/setting initial states of the latches
static inline Abc_InitType_t Seq_NodeGetInitOne( Abc_Obj_t * pObj, int Edge, int iLat )      { return Seq_LatInit( Seq_NodeGetLat(pObj, Edge, iLat) );  }
static inline Abc_InitType_t Seq_NodeGetInitFirst( Abc_Obj_t * pObj, int Edge )              { return Seq_LatInit( Seq_NodeGetLatFirst(pObj, Edge) );   }
static inline Abc_InitType_t Seq_NodeGetInitLast( Abc_Obj_t * pObj, int Edge )               { return Seq_LatInit( Seq_NodeGetLatLast(pObj, Edge) );    }
static inline void           Seq_NodeSetInitOne( Abc_Obj_t * pObj, int Edge, int iLat, Abc_InitType_t Init )  { Seq_LatSetInit( Seq_NodeGetLat(pObj, Edge, iLat), Init );  }

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DECLARATIONS                         ///
////////////////////////////////////////////////////////////////////////

/*=== seqLatch.c ===============================================================*/
extern void                  Seq_NodeInsertFirst( Abc_Obj_t * pObj, int Edge, Abc_InitType_t Init );  
extern void                  Seq_NodeInsertLast( Abc_Obj_t * pObj, int Edge, Abc_InitType_t Init );  
extern Abc_InitType_t        Seq_NodeDeleteFirst( Abc_Obj_t * pObj, int Edge );  
extern Abc_InitType_t        Seq_NodeDeleteLast( Abc_Obj_t * pObj, int Edge );  
/*=== seqFpgaIter.c ============================================================*/
extern void                  Seq_FpgaMappingDelays( Abc_Ntk_t * pNtk, int fVerbose );
extern int                   Seq_FpgaNodeUpdateLValue( Abc_Obj_t * pObj, int Fi );
/*=== seqRetIter.c =============================================================*/
extern void                  Seq_NtkRetimeDelayLags( Abc_Ntk_t * pNtk, int fVerbose );
extern int                   Seq_NtkImplementRetiming( Abc_Ntk_t * pNtk, Vec_Str_t * vLags, int fVerbose );
/*=== seqUtil.c ================================================================*/
extern int                   Seq_ObjFanoutLMax( Abc_Obj_t * pObj );
extern int                   Seq_ObjFanoutLMin( Abc_Obj_t * pObj );
extern int                   Seq_ObjFanoutLSum( Abc_Obj_t * pObj );
extern int                   Seq_ObjFaninLSum( Abc_Obj_t * pObj );

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

#endif

