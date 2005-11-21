/**CFile****************************************************************

  FileName    [seq.h]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Construction and manipulation of sequential AIGs.]

  Synopsis    [External declarations.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: seq.h,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#ifndef __SEQ_H__
#define __SEQ_H__

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                         PARAMETERS                               ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                         BASIC TYPES                              ///
////////////////////////////////////////////////////////////////////////

typedef struct Abc_Seq_t_  Abc_Seq_t;

////////////////////////////////////////////////////////////////////////
///                      MACRO DEFINITIONS                           ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                    FUNCTION DECLARATIONS                         ///
////////////////////////////////////////////////////////////////////////

/*=== seqFpgaCore.c ===============================================================*/
extern Abc_Ntk_t *     Seq_NtkFpgaMapRetime( Abc_Ntk_t * pNtk, int fVerbose );
/*=== seqLatch.c ===============================================================*/
extern void            Seq_NodeDupLats( Abc_Obj_t * pObjNew, Abc_Obj_t * pObj, int Edge );
extern int             Seq_NodeCompareLats( Abc_Obj_t * pObj1, int Edge1, Abc_Obj_t * pObj2, int Edge2 );
/*=== seqMan.c ===============================================================*/
extern Abc_Seq_t *     Seq_Create( Abc_Ntk_t * pNtk );
extern void            Seq_Resize( Abc_Seq_t * p, int nMaxId );
extern void            Seq_Delete( Abc_Seq_t * p );
/*=== abcSeq.c ===============================================================*/
extern Abc_Ntk_t *     Abc_NtkAigToSeq( Abc_Ntk_t * pNtk );
extern Abc_Ntk_t *     Abc_NtkSeqToLogicSop( Abc_Ntk_t * pNtk );
/*=== seqShare.c =============================================================*/
extern void            Seq_NtkShareFanouts( Abc_Ntk_t * pNtk );
/*=== seqRetCore.c ===========================================================*/
extern void            Seq_NtkSeqRetimeDelay( Abc_Ntk_t * pNtk, int fInitial, int fVerbose );
extern void            Seq_NtkSeqRetimeForward( Abc_Ntk_t * pNtk, int fInitial, int fVerbose );
extern void            Seq_NtkSeqRetimeBackward( Abc_Ntk_t * pNtk, int fInitial, int fVerbose );
/*=== seqUtil.c ==============================================================*/
extern char *          Seq_ObjFaninGetInitPrintable( Abc_Obj_t * pObj, int Edge );
extern void            Seq_NtkLatchSetValues( Abc_Ntk_t * pNtk, Abc_InitType_t Init );
extern int             Seq_NtkLatchNum( Abc_Ntk_t * pNtk );
extern int             Seq_NtkLatchNumMax( Abc_Ntk_t * pNtk );
extern int             Seq_NtkLatchNumShared( Abc_Ntk_t * pNtk );
extern void            Seq_NtkLatchGetInitNums( Abc_Ntk_t * pNtk, int * pInits );
extern int             Seq_NtkLatchGetEqualFaninNum( Abc_Ntk_t * pNtk );

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

#endif

