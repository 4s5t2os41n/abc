/**CFile****************************************************************
 
  FileName    [abc.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Network and node package.]
 
  Synopsis    [Command file.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: abc.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/
 
#include "abc.h"
#include "mainInt.h"
#include "fraig.h"
#include "fxu.h"
#include "cut.h"
#include "fpga.h"
#include "if.h"
#include "res.h"
#include "lpk.h"
#include "giaAig.h"
#include "dar.h"
#include "mfs.h"
#include "mfx.h"
#include "fra.h"
#include "saig.h"
#include "nwkMerge.h"
#include "int.h"
#include "dch.h"
#include "ssw.h"
#include "cgt.h"
#include "amap.h"
#include "cec.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static int Abc_CommandPrintStats     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintExdc      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintIo        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintLatch     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintFanio     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintMffc      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintFactor    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintLevel     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintSupport   ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintSymms     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintUnate     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintAuto      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintKMap      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintGates     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintSharing   ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintXCut      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintDsd       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintCone      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPrintMiter     ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandShow           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandShowBdd        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandShowCut        ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandCollapse       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandStrash         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandBalance        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandMuxStruct      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandMulti          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandRenode         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandCleanup        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSweep          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFastExtract    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandEliminate      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDisjoint       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandLutpack        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandLutmin         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandImfs           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandMfs            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandTrace          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSpeedup        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPowerdown      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandMerge          ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandRewrite        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandRefactor       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandRestructure    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandResubstitute   ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandRr             ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandCascade        ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandLogic          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandComb           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandMiter          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDemiter        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandOrPos          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAndPos         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAppend         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFrames         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDFrames        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSop            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandBdd            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAig            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandReorder        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandBidec          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandOrder          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandMuxes          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandExtSeqDcs      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandReach          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandCone           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandNode           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandTopmost        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandTopAnd         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandTrim           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandShortNames     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandExdcFree       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandExdcGet        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandExdcSet        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandCareSet        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandCut            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandEspresso       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandGen            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandCover          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDouble         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandInter          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandTest           ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandQuaVar         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandQuaRel         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandQuaReach       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSenseInput     ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandIStrash        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandICut           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandIRewrite       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDRewrite       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDRefactor      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDC2            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDChoice        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDch            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDrwsat         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandIRewriteSeq    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandIResyn         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandISat           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandIFraig         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDFraig         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandCSweep         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDProve         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbSec          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSimSec         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandMatch          ( Abc_Frame_t * pAbc, int argc, char ** argv );
//static int Abc_CommandHaig           ( Abc_Frame_t * pAbc, int argc, char ** argv );
//static int Abc_CommandMini           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandQbf            ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandFraig          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFraigTrust     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFraigStore     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFraigRestore   ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFraigClean     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFraigSweep     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFraigDress     ( Abc_Frame_t * pAbc, int argc, char ** argv );

//static int Abc_CommandHaigStart      ( Abc_Frame_t * pAbc, int argc, char ** argv );
//static int Abc_CommandHaigStop       ( Abc_Frame_t * pAbc, int argc, char ** argv );
//static int Abc_CommandHaigUse        ( Abc_Frame_t * pAbc, int argc, char ** argv );

//static int Abc_CommandRecStart       ( Abc_Frame_t * pAbc, int argc, char ** argv );
//static int Abc_CommandRecStop        ( Abc_Frame_t * pAbc, int argc, char ** argv );
//static int Abc_CommandRecAdd         ( Abc_Frame_t * pAbc, int argc, char ** argv );
//static int Abc_CommandRecPs          ( Abc_Frame_t * pAbc, int argc, char ** argv );
//static int Abc_CommandRecUse         ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandMap            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAmap           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandUnmap          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAttach         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSuperChoice    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSuperChoiceLut ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandFpga           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFpgaFast       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandIf             ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandScut           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandInit           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandZero           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandUndc           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandOneHot         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPipe           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSeq            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandUnseq          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandRetime         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDRetime        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandFlowRetime     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandCRetime        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSeqFpga        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSeqMap         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSeqSweep       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSeqSweep2      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSeqSweepTest   ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandLcorr          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSeqCleanup     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandCycle          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandXsim           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSim            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDarPhase       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSynch          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandClockGate      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandExtWin         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandInsWin         ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandCec            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDCec           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSec            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDSec           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandSat            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDSat           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPSat           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandProve          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandIProve         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandDebug          ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandBmc            ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandBmc2           ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandBmcInter       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandIndcut         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandEnlarge        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandInduction      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandPBAbstraction  ( Abc_Frame_t * pAbc, int argc, char ** argv );


static int Abc_CommandTraceStart     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandTraceCheck     ( Abc_Frame_t * pAbc, int argc, char ** argv );


static int Abc_CommandAbc8Read       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8ReadLogic  ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Write      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8WriteLogic ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8ReadLut    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8PrintLut   ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Check      ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandAbc8Ps         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Pfan       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8If         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8DChoice    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Dch        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8DC2        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Bidec      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Strash     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Mfs        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Lutpack    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Lutmin     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Balance    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Speedup    ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Merge      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Insert     ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandAbc8Fraig      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Scl        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Lcorr      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Ssw        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Scorr      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Sweep      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8Zero       ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandAbc8Cec        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc8DSec       ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandAbc9Get        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Put        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Read       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Write      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Ps         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9PFan       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9PSig       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Status     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Show       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Hash       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Topand     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Cof        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Trim       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Dfs        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Sim        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Resim      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Equiv      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Semi       ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Times      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Frames     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Retime     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Enable     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Miter      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Scl        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Lcorr      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Scorr      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Choice     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Sat        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Fraig      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Srm        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Reduce     ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Cec        ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Force      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Embed      ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9If         ( Abc_Frame_t * pAbc, int argc, char ** argv );
static int Abc_CommandAbc9Test       ( Abc_Frame_t * pAbc, int argc, char ** argv );

static int Abc_CommandAbcTestNew     ( Abc_Frame_t * pAbc, int argc, char ** argv );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_FrameClearDesign()
{
    extern Abc_Frame_t * Abc_FrameGetGlobalFrame();
    extern void Ntl_ManFree( void * p );
    extern void Nwk_ManFree( void * p );
    Abc_Frame_t * pAbc;

    pAbc = Abc_FrameGetGlobalFrame();
    if ( pAbc->pAbc8Ntl )
    {
        Ntl_ManFree( pAbc->pAbc8Ntl );
        pAbc->pAbc8Ntl = NULL;
    }
    if ( pAbc->pAbc8Aig )
    {
        Aig_ManStop( pAbc->pAbc8Aig );
        pAbc->pAbc8Aig = NULL;
    }
    if ( pAbc->pAbc8Nwk )
    {
        Nwk_ManFree( pAbc->pAbc8Nwk );
        pAbc->pAbc8Nwk = NULL;
    }
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_Init( Abc_Frame_t * pAbc )
{
//    Amap_LibParseTest( "at\\syn\\libraries\\LIBS\\BRDCM\\tsmc13_5.ff.genlib" );
//    Amap_LibParseTest( "at\\syn\\libraries\\LIBS\\GS60\\GS60_W_30_1.7_CORE.genlib" );
//    Amap_LibParseTest( "at\\syn\\libraries\\LIBS\\TYPICAL\\typical.genlib" );

    Cmd_CommandAdd( pAbc, "Printing",     "print_stats",   Abc_CommandPrintStats,       0 ); 
    Cmd_CommandAdd( pAbc, "Printing",     "print_exdc",    Abc_CommandPrintExdc,        0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_io",      Abc_CommandPrintIo,          0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_latch",   Abc_CommandPrintLatch,       0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_fanio",   Abc_CommandPrintFanio,       0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_mffc",    Abc_CommandPrintMffc,        0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_factor",  Abc_CommandPrintFactor,      0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_level",   Abc_CommandPrintLevel,       0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_supp",    Abc_CommandPrintSupport,     0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_symm",    Abc_CommandPrintSymms,       0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_unate",   Abc_CommandPrintUnate,       0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_auto",    Abc_CommandPrintAuto,        0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_kmap",    Abc_CommandPrintKMap,        0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_gates",   Abc_CommandPrintGates,       0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_sharing", Abc_CommandPrintSharing,     0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_xcut",    Abc_CommandPrintXCut,        0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_dsd",     Abc_CommandPrintDsd,         0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_cone",    Abc_CommandPrintCone,        0 );
    Cmd_CommandAdd( pAbc, "Printing",     "print_miter",   Abc_CommandPrintMiter,       0 );

    Cmd_CommandAdd( pAbc, "Printing",     "show",          Abc_CommandShow,             0 );
    Cmd_CommandAdd( pAbc, "Printing",     "show_bdd",      Abc_CommandShowBdd,          0 );
    Cmd_CommandAdd( pAbc, "Printing",     "show_cut",      Abc_CommandShowCut,          0 );

    Cmd_CommandAdd( pAbc, "Synthesis",    "collapse",      Abc_CommandCollapse,         1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "strash",        Abc_CommandStrash,           1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "balance",       Abc_CommandBalance,          1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "mux_struct",    Abc_CommandMuxStruct,        1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "multi",         Abc_CommandMulti,            1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "renode",        Abc_CommandRenode,           1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "cleanup",       Abc_CommandCleanup,          1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "sweep",         Abc_CommandSweep,            1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "fx",            Abc_CommandFastExtract,      1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "eliminate",     Abc_CommandEliminate,        1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "dsd",           Abc_CommandDisjoint,         1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "lutpack",       Abc_CommandLutpack,          1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "lutmin",        Abc_CommandLutmin,           1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "imfs",          Abc_CommandImfs,             1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "mfs",           Abc_CommandMfs,              1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "trace",         Abc_CommandTrace,            0 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "speedup",       Abc_CommandSpeedup,          1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "powerdown",     Abc_CommandPowerdown,        1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "merge",         Abc_CommandMerge,            1 );

    Cmd_CommandAdd( pAbc, "Synthesis",    "rewrite",       Abc_CommandRewrite,          1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "refactor",      Abc_CommandRefactor,         1 );
//    Cmd_CommandAdd( pAbc, "Synthesis",    "restructure",   Abc_CommandRestructure,      1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "resub",         Abc_CommandResubstitute,     1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "rr",            Abc_CommandRr,               1 );
    Cmd_CommandAdd( pAbc, "Synthesis",    "cascade",       Abc_CommandCascade,          1 );

    Cmd_CommandAdd( pAbc, "Various",      "logic",         Abc_CommandLogic,            1 );
    Cmd_CommandAdd( pAbc, "Various",      "comb",          Abc_CommandComb,             1 );
    Cmd_CommandAdd( pAbc, "Various",      "miter",         Abc_CommandMiter,            1 );
    Cmd_CommandAdd( pAbc, "Various",      "demiter",       Abc_CommandDemiter,          1 );
    Cmd_CommandAdd( pAbc, "Various",      "orpos",         Abc_CommandOrPos,            1 );
    Cmd_CommandAdd( pAbc, "Various",      "andpos",        Abc_CommandAndPos,           1 );
    Cmd_CommandAdd( pAbc, "Various",      "append",        Abc_CommandAppend,           1 );
    Cmd_CommandAdd( pAbc, "Various",      "frames",        Abc_CommandFrames,           1 );
    Cmd_CommandAdd( pAbc, "Various",      "dframes",       Abc_CommandDFrames,          1 );
    Cmd_CommandAdd( pAbc, "Various",      "sop",           Abc_CommandSop,              0 );
    Cmd_CommandAdd( pAbc, "Various",      "bdd",           Abc_CommandBdd,              0 );
    Cmd_CommandAdd( pAbc, "Various",      "aig",           Abc_CommandAig,              0 );
    Cmd_CommandAdd( pAbc, "Various",      "reorder",       Abc_CommandReorder,          0 );
    Cmd_CommandAdd( pAbc, "Various",      "bidec",         Abc_CommandBidec,            1 );
    Cmd_CommandAdd( pAbc, "Various",      "order",         Abc_CommandOrder,            0 );
    Cmd_CommandAdd( pAbc, "Various",      "muxes",         Abc_CommandMuxes,            1 );
    Cmd_CommandAdd( pAbc, "Various",      "ext_seq_dcs",   Abc_CommandExtSeqDcs,        0 );
    Cmd_CommandAdd( pAbc, "Various",      "reach",         Abc_CommandReach,            0 );
    Cmd_CommandAdd( pAbc, "Various",      "cone",          Abc_CommandCone,             1 );
    Cmd_CommandAdd( pAbc, "Various",      "node",          Abc_CommandNode,             1 );
    Cmd_CommandAdd( pAbc, "Various",      "topmost",       Abc_CommandTopmost,          1 );
    Cmd_CommandAdd( pAbc, "Various",      "topand",        Abc_CommandTopAnd,          1 );
    Cmd_CommandAdd( pAbc, "Various",      "trim",          Abc_CommandTrim,             1 );
    Cmd_CommandAdd( pAbc, "Various",      "short_names",   Abc_CommandShortNames,       0 );
    Cmd_CommandAdd( pAbc, "Various",      "exdc_free",     Abc_CommandExdcFree,         1 );
    Cmd_CommandAdd( pAbc, "Various",      "exdc_get",      Abc_CommandExdcGet,          1 );
    Cmd_CommandAdd( pAbc, "Various",      "exdc_set",      Abc_CommandExdcSet,          1 );
    Cmd_CommandAdd( pAbc, "Various",      "care_set",      Abc_CommandCareSet,          1 );
    Cmd_CommandAdd( pAbc, "Various",      "cut",           Abc_CommandCut,              0 );
    Cmd_CommandAdd( pAbc, "Various",      "espresso",      Abc_CommandEspresso,         1 );
    Cmd_CommandAdd( pAbc, "Various",      "gen",           Abc_CommandGen,              0 );
    Cmd_CommandAdd( pAbc, "Various",      "cover",         Abc_CommandCover,            1 );
    Cmd_CommandAdd( pAbc, "Various",      "double",        Abc_CommandDouble,           1 );
    Cmd_CommandAdd( pAbc, "Various",      "inter",         Abc_CommandInter,            1 );
    Cmd_CommandAdd( pAbc, "Various",      "test",          Abc_CommandTest,             0 );

    Cmd_CommandAdd( pAbc, "Various",      "qvar",          Abc_CommandQuaVar,           1 );
    Cmd_CommandAdd( pAbc, "Various",      "qrel",          Abc_CommandQuaRel,           1 );
    Cmd_CommandAdd( pAbc, "Various",      "qreach",        Abc_CommandQuaReach,         1 );
    Cmd_CommandAdd( pAbc, "Various",      "senseinput",    Abc_CommandSenseInput,       1 );

    Cmd_CommandAdd( pAbc, "New AIG",      "istrash",       Abc_CommandIStrash,          1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "icut",          Abc_CommandICut,             0 );
    Cmd_CommandAdd( pAbc, "New AIG",      "irw",           Abc_CommandIRewrite,         1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "drw",           Abc_CommandDRewrite,         1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "drf",           Abc_CommandDRefactor,        1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "dc2",           Abc_CommandDC2,              1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "dchoice",       Abc_CommandDChoice,          1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "dch",           Abc_CommandDch,              1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "drwsat",        Abc_CommandDrwsat,           1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "irws",          Abc_CommandIRewriteSeq,      1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "iresyn",        Abc_CommandIResyn,           1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "isat",          Abc_CommandISat,             1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "ifraig",        Abc_CommandIFraig,           1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "dfraig",        Abc_CommandDFraig,           1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "csweep",        Abc_CommandCSweep,           1 );
//    Cmd_CommandAdd( pAbc, "New AIG",      "haig",          Abc_CommandHaig,             1 );
//    Cmd_CommandAdd( pAbc, "New AIG",      "mini",          Abc_CommandMini,             1 );
    Cmd_CommandAdd( pAbc, "New AIG",      "qbf",           Abc_CommandQbf,              0 );

    Cmd_CommandAdd( pAbc, "Fraiging",     "fraig",         Abc_CommandFraig,            1 );
    Cmd_CommandAdd( pAbc, "Fraiging",     "fraig_trust",   Abc_CommandFraigTrust,       1 );
    Cmd_CommandAdd( pAbc, "Fraiging",     "fraig_store",   Abc_CommandFraigStore,       0 );
    Cmd_CommandAdd( pAbc, "Fraiging",     "fraig_restore", Abc_CommandFraigRestore,     1 );
    Cmd_CommandAdd( pAbc, "Fraiging",     "fraig_clean",   Abc_CommandFraigClean,       0 );
    Cmd_CommandAdd( pAbc, "Fraiging",     "fraig_sweep",   Abc_CommandFraigSweep,       1 );
    Cmd_CommandAdd( pAbc, "Fraiging",     "dress",         Abc_CommandFraigDress,       1 );

//    Cmd_CommandAdd( pAbc, "Choicing",     "haig_start",    Abc_CommandHaigStart,        0 );
//    Cmd_CommandAdd( pAbc, "Choicing",     "haig_stop",     Abc_CommandHaigStop,         0 );
//    Cmd_CommandAdd( pAbc, "Choicing",     "haig_use",      Abc_CommandHaigUse,          1 );

//    Cmd_CommandAdd( pAbc, "Choicing",     "rec_start",     Abc_CommandRecStart,         0 );
//    Cmd_CommandAdd( pAbc, "Choicing",     "rec_stop",      Abc_CommandRecStop,          0 );
//    Cmd_CommandAdd( pAbc, "Choicing",     "rec_add",       Abc_CommandRecAdd,           0 );
//    Cmd_CommandAdd( pAbc, "Choicing",     "rec_ps",        Abc_CommandRecPs,            0 );
//    Cmd_CommandAdd( pAbc, "Choicing",     "rec_use",       Abc_CommandRecUse,           1 );

    Cmd_CommandAdd( pAbc, "SC mapping",   "map",           Abc_CommandMap,              1 );
    Cmd_CommandAdd( pAbc, "SC mapping",   "amap",          Abc_CommandAmap,             1 );
    Cmd_CommandAdd( pAbc, "SC mapping",   "unmap",         Abc_CommandUnmap,            1 );
    Cmd_CommandAdd( pAbc, "SC mapping",   "attach",        Abc_CommandAttach,           1 );
    Cmd_CommandAdd( pAbc, "SC mapping",   "sc",            Abc_CommandSuperChoice,      1 );
    Cmd_CommandAdd( pAbc, "SC mapping",   "scl",           Abc_CommandSuperChoiceLut,   1 );

    Cmd_CommandAdd( pAbc, "FPGA mapping", "fpga",          Abc_CommandFpga,             1 );
    Cmd_CommandAdd( pAbc, "FPGA mapping", "ffpga",         Abc_CommandFpgaFast,         1 );
    Cmd_CommandAdd( pAbc, "FPGA mapping", "if",            Abc_CommandIf,               1 );

//    Cmd_CommandAdd( pAbc, "Sequential",   "scut",          Abc_CommandScut,             0 );
    Cmd_CommandAdd( pAbc, "Sequential",   "init",          Abc_CommandInit,             1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "zero",          Abc_CommandZero,             1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "undc",          Abc_CommandUndc,             1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "onehot",        Abc_CommandOneHot,           1 );
//    Cmd_CommandAdd( pAbc, "Sequential",   "pipe",          Abc_CommandPipe,             1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "retime",        Abc_CommandRetime,           1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "dretime",       Abc_CommandDRetime,          1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "fretime",       Abc_CommandFlowRetime,       1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "cretime",       Abc_CommandCRetime,          1 );
//    Cmd_CommandAdd( pAbc, "Sequential",   "sfpga",         Abc_CommandSeqFpga,          1 );
//    Cmd_CommandAdd( pAbc, "Sequential",   "smap",          Abc_CommandSeqMap,           1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "ssweep",        Abc_CommandSeqSweep,         1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "scorr",         Abc_CommandSeqSweep2,        1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "testssw",       Abc_CommandSeqSweepTest,     0 );
    Cmd_CommandAdd( pAbc, "Sequential",   "lcorr",         Abc_CommandLcorr,            1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "scleanup",      Abc_CommandSeqCleanup,       1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "cycle",         Abc_CommandCycle,            1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "xsim",          Abc_CommandXsim,             0 );
    Cmd_CommandAdd( pAbc, "Sequential",   "sim",           Abc_CommandSim,              0 );
    Cmd_CommandAdd( pAbc, "Sequential",   "phase",         Abc_CommandDarPhase,         1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "synch",         Abc_CommandSynch,            1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "clockgate",     Abc_CommandClockGate,        1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "extwin",        Abc_CommandExtWin,           1 );
    Cmd_CommandAdd( pAbc, "Sequential",   "inswin",        Abc_CommandInsWin,           1 );

    Cmd_CommandAdd( pAbc, "Verification", "cec",           Abc_CommandCec,              0 );
    Cmd_CommandAdd( pAbc, "Verification", "dcec",          Abc_CommandDCec,             0 );
    Cmd_CommandAdd( pAbc, "Verification", "sec",           Abc_CommandSec,              0 );
    Cmd_CommandAdd( pAbc, "Verification", "dsec",          Abc_CommandDSec,             0 );
    Cmd_CommandAdd( pAbc, "Verification", "dprove",        Abc_CommandDProve,           0 );
    Cmd_CommandAdd( pAbc, "Verification", "absec",         Abc_CommandAbSec,            0 );
    Cmd_CommandAdd( pAbc, "Verification", "simsec",        Abc_CommandSimSec,           0 );
    Cmd_CommandAdd( pAbc, "Verification", "match",         Abc_CommandMatch,            0 );
    Cmd_CommandAdd( pAbc, "Verification", "sat",           Abc_CommandSat,              0 );
    Cmd_CommandAdd( pAbc, "Verification", "dsat",          Abc_CommandDSat,             0 );
    Cmd_CommandAdd( pAbc, "Verification", "psat",          Abc_CommandPSat,             0 );
    Cmd_CommandAdd( pAbc, "Verification", "prove",         Abc_CommandProve,            1 );
    Cmd_CommandAdd( pAbc, "Verification", "iprove",        Abc_CommandIProve,           1 );
    Cmd_CommandAdd( pAbc, "Verification", "debug",         Abc_CommandDebug,            0 );
    Cmd_CommandAdd( pAbc, "Verification", "bmc",           Abc_CommandBmc,              0 );
    Cmd_CommandAdd( pAbc, "Verification", "bmc2",          Abc_CommandBmc2,             0 );
    Cmd_CommandAdd( pAbc, "Verification", "int",           Abc_CommandBmcInter,         0 );
    Cmd_CommandAdd( pAbc, "Verification", "indcut",        Abc_CommandIndcut,           0 );
    Cmd_CommandAdd( pAbc, "Verification", "enlarge",       Abc_CommandEnlarge,          1 );
    Cmd_CommandAdd( pAbc, "Verification", "ind",           Abc_CommandInduction,        0 );
    Cmd_CommandAdd( pAbc, "Verification", "abs",           Abc_CommandPBAbstraction,    0 );


    Cmd_CommandAdd( pAbc, "ABC8",         "*r",            Abc_CommandAbc8Read,         0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*rlogic",       Abc_CommandAbc8ReadLogic,    0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*w",            Abc_CommandAbc8Write,        0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*wlogic",       Abc_CommandAbc8WriteLogic,   0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*rlut",         Abc_CommandAbc8ReadLut,      0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*plut",         Abc_CommandAbc8PrintLut,     0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*check",        Abc_CommandAbc8Check,        0 );

    Cmd_CommandAdd( pAbc, "ABC8",         "*ps",           Abc_CommandAbc8Ps,           0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*pfan",         Abc_CommandAbc8Pfan,         0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*if",           Abc_CommandAbc8If,           0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*dchoice",      Abc_CommandAbc8DChoice,      0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*dch",          Abc_CommandAbc8Dch,          0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*dc2",          Abc_CommandAbc8DC2,          0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*bidec",        Abc_CommandAbc8Bidec,        0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*st",           Abc_CommandAbc8Strash,       0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*mfs",          Abc_CommandAbc8Mfs,          0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*lp",           Abc_CommandAbc8Lutpack,      0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*b",            Abc_CommandAbc8Balance,      0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*speedup",      Abc_CommandAbc8Speedup,      0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*merge",        Abc_CommandAbc8Merge,        0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*insert",       Abc_CommandAbc8Insert,       0 );

    Cmd_CommandAdd( pAbc, "ABC8",         "*fraig",        Abc_CommandAbc8Fraig,        0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*scl",          Abc_CommandAbc8Scl,          0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*lcorr",        Abc_CommandAbc8Lcorr,        0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*ssw",          Abc_CommandAbc8Ssw,          0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*scorr",        Abc_CommandAbc8Scorr,        0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*sw",           Abc_CommandAbc8Sweep,        0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*zero",         Abc_CommandAbc8Zero,         0 );

    Cmd_CommandAdd( pAbc, "ABC8",         "*cec",          Abc_CommandAbc8Cec,          0 );
    Cmd_CommandAdd( pAbc, "ABC8",         "*dsec",         Abc_CommandAbc8DSec,         0 );

    Cmd_CommandAdd( pAbc, "AIG",          "&get",          Abc_CommandAbc9Get,          0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&put",          Abc_CommandAbc9Put,          0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&r",            Abc_CommandAbc9Read,         0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&w",            Abc_CommandAbc9Write,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&ps",           Abc_CommandAbc9Ps,           0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&pfan",         Abc_CommandAbc9PFan,         0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&psig",         Abc_CommandAbc9PSig,         0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&status",       Abc_CommandAbc9Status,       0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&show",         Abc_CommandAbc9Show,         0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&st",           Abc_CommandAbc9Hash,         0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&topand",       Abc_CommandAbc9Topand,       0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&cof",          Abc_CommandAbc9Cof,          0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&trim",         Abc_CommandAbc9Trim,         0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&dfs",          Abc_CommandAbc9Dfs,          0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&sim",          Abc_CommandAbc9Sim,          0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&resim",        Abc_CommandAbc9Resim,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&equiv",        Abc_CommandAbc9Equiv,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&semi",         Abc_CommandAbc9Semi,         0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&times",        Abc_CommandAbc9Times,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&frames",       Abc_CommandAbc9Frames,       0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&retime",       Abc_CommandAbc9Retime,       0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&enable",       Abc_CommandAbc9Enable,       0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&miter",        Abc_CommandAbc9Miter,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&scl",          Abc_CommandAbc9Scl,          0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&lcorr",        Abc_CommandAbc9Lcorr,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&scorr",        Abc_CommandAbc9Scorr,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&choice",       Abc_CommandAbc9Choice,       0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&sat",          Abc_CommandAbc9Sat,          0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&fraig",        Abc_CommandAbc9Fraig,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&srm",          Abc_CommandAbc9Srm,          0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&reduce",       Abc_CommandAbc9Reduce,       0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&cec",          Abc_CommandAbc9Cec,          0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&force",        Abc_CommandAbc9Force,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&embed",        Abc_CommandAbc9Embed,        0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&if",           Abc_CommandAbc9If,           0 );
    Cmd_CommandAdd( pAbc, "AIG",          "&test",         Abc_CommandAbc9Test,         0 );

    Cmd_CommandAdd( pAbc, "Various",      "testnew",         Abc_CommandAbcTestNew,     0 );

//    Cmd_CommandAdd( pAbc, "Verification", "trace_start",   Abc_CommandTraceStart,       0 );
//    Cmd_CommandAdd( pAbc, "Verification", "trace_check",   Abc_CommandTraceCheck,       0 );

//    Rwt_Man4ExploreStart();
//    Map_Var3Print();
//    Map_Var4Test();

//    Abc_NtkPrint256();
//    Kit_TruthCountMintermsPrecomp();
//    Kit_DsdPrecompute4Vars();

    {
        extern void Dar_LibStart();
        Dar_LibStart();
    }
    {
        extern void Bdc_ManDecomposeTest( unsigned uTruth, int nVars );
//        Bdc_ManDecomposeTest( 0x0f0f0f0f, 3 );
    }

    {
//        extern void Aig_ManRandomTest1();
//        Aig_ManRandomTest1();
    }
    {
        extern void Extra_MemTest();
//       Extra_MemTest();
    }

    {
        extern void Gia_SortTest();
//        Gia_SortTest();
    }
    {
        void For_ManFileExperiment();
//        For_ManFileExperiment();
    }

} 

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Abc_End()
{
    Abc_FrameClearDesign(); 
    {
        extern void If_LutLibFree( If_Lib_t * pLutLib );
        if ( Abc_FrameGetGlobalFrame()->pAbc8Lib )
            If_LutLibFree( Abc_FrameGetGlobalFrame()->pAbc8Lib );
    }

//    Dar_LibDumpPriorities();
    {
        extern int Abc_NtkCompareAndSaveBest( Abc_Ntk_t * pNtk );
        Abc_NtkCompareAndSaveBest( NULL );
    }

    {
        extern void Cnf_ClearMemory();
        Cnf_ClearMemory();
    }
    {
        extern void Dar_LibStop();
        Dar_LibStop();
    }
    {
        extern void Aig_RManQuit();
        Aig_RManQuit();
    }

    Abc_NtkFraigStoreClean();
//    Rwt_Man4ExplorePrint();

    if ( Abc_FrameGetGlobalFrame()->pAig )
        Gia_ManStop( Abc_FrameGetGlobalFrame()->pAig );
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintStats( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int fFactor;
    int fSaveBest;
    int fDumpResult;
    int fUseLutLib;
    int fPrintTime;
    int fPrintMuxes;
    int fPower;
    int fGlitch;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set the defaults
    fFactor   = 0;
    fSaveBest = 0;
    fDumpResult = 0;
    fUseLutLib = 0;
    fPrintTime = 0;
    fPrintMuxes = 0;
    fPower = 0;
    fGlitch = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "fbdltmpgh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'f':
            fFactor ^= 1;
            break;
        case 'b':
            fSaveBest ^= 1;
            break;
        case 'd':
            fDumpResult ^= 1;
            break;
        case 'l':
            fUseLutLib ^= 1;
            break;
        case 't':
            fPrintTime ^= 1;
            break;
        case 'm':
            fPrintMuxes ^= 1;
            break;
        case 'p':
            fPower ^= 1;
            break;
        case 'g':
            fGlitch ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( Abc_FrameReadErr(pAbc), "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) && fUseLutLib )
    {
        fprintf( Abc_FrameReadErr(pAbc), "Cannot print LUT delay for a non-logic network.\n" );
        return 1;
    }
    Abc_NtkPrintStats( pOut, pNtk, fFactor, fSaveBest, fDumpResult, fUseLutLib, fPrintMuxes, fPower, fGlitch );
    if ( fPrintTime )
    {
        pAbc->TimeTotal += pAbc->TimeCommand;
        fprintf( pAbc->Out, "elapse: %3.2f seconds, total: %3.2f seconds\n", 
            pAbc->TimeCommand, pAbc->TimeTotal );
        pAbc->TimeCommand = 0.0;
    }
    return 0;

usage:
    fprintf( pErr, "usage: print_stats [-fbdltmpgh]\n" );
    fprintf( pErr, "\t        prints the network statistics\n" );
    fprintf( pErr, "\t-f    : toggles printing the literal count in the factored forms [default = %s]\n", fFactor? "yes": "no" );
    fprintf( pErr, "\t-b    : toggles saving the best logic network in \"best.blif\" [default = %s]\n", fSaveBest? "yes": "no" );
    fprintf( pErr, "\t-d    : toggles dumping network into file \"<input_file_name>_dump.blif\" [default = %s]\n", fDumpResult? "yes": "no" );
    fprintf( pErr, "\t-l    : toggles printing delay of LUT mapping using LUT library [default = %s]\n", fSaveBest? "yes": "no" );
    fprintf( pErr, "\t-t    : toggles printing runtime statistics [default = %s]\n", fPrintTime? "yes": "no" );
    fprintf( pErr, "\t-m    : toggles printing MUX statistics [default = %s]\n", fPrintMuxes? "yes": "no" );
    fprintf( pErr, "\t-p    : toggles printing power dissipation due to switching [default = %s]\n", fPower? "yes": "no" );
    fprintf( pErr, "\t-q    : toggles printing percentage of increased power due to glitching [default = %s]\n", fGlitch? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintExdc( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkTemp;
    double Percentage;
    bool fShort;
    int c;
    int fPrintDc;

    extern double Abc_NtkSpacePercentage( Abc_Obj_t * pNode );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set the defaults
    fShort  = 1;
    fPrintDc = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "sdh" ) ) != EOF )
    {
        switch ( c )
        {
        case 's':
            fShort ^= 1;
            break;
        case 'd':
            fPrintDc ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( Abc_FrameReadErr(pAbc), "Empty network.\n" );
        return 1;
    }
    if ( pNtk->pExdc == NULL )
    {
        fprintf( Abc_FrameReadErr(pAbc), "Network has no EXDC.\n" );
        return 1;
    }

    if ( fPrintDc )
    {
        if ( !Abc_NtkIsStrash(pNtk->pExdc) )
        {
            pNtkTemp = Abc_NtkStrash(pNtk->pExdc, 0, 0, 0);
            Percentage = Abc_NtkSpacePercentage( Abc_ObjChild0( Abc_NtkPo(pNtkTemp, 0) ) );
            Abc_NtkDelete( pNtkTemp );
        }
        else
            Percentage = Abc_NtkSpacePercentage( Abc_ObjChild0( Abc_NtkPo(pNtk->pExdc, 0) ) );

        printf( "EXDC network statistics: " );
        printf( "(" );
        if ( Percentage > 0.05 && Percentage < 99.95 )
            printf( "%.2f", Percentage );
        else if ( Percentage > 0.000005 && Percentage < 99.999995 )
            printf( "%.6f", Percentage );
        else
            printf( "%f", Percentage );
        printf( " %% don't-cares)\n" );
    }
    else
        printf( "EXDC network statistics: \n" );
    Abc_NtkPrintStats( pOut, pNtk->pExdc, 0, 0, 0, 0, 0, 0, 0 );
    return 0;

usage:
    fprintf( pErr, "usage: print_exdc [-dh]\n" );
    fprintf( pErr, "\t        prints the EXDC network statistics\n" );
    fprintf( pErr, "\t-d    : toggles printing don't-care percentage [default = %s]\n", fPrintDc? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintIo( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Abc_Obj_t * pNode;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( argc > globalUtilOptind + 1 )
    {
        fprintf( pErr, "Wrong number of auguments.\n" );
        goto usage;
    }

    if ( argc == globalUtilOptind + 1 )
    {
        pNode = Abc_NtkFindNode( pNtk, argv[globalUtilOptind] );
        if ( pNode == NULL )
        {
            fprintf( pErr, "Cannot find node \"%s\".\n", argv[globalUtilOptind] );
            return 1;
        }
        Abc_NodePrintFanio( pOut, pNode );
        return 0;
    }
    // print the nodes
    Abc_NtkPrintIo( pOut, pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: print_io [-h] <node>\n" );
    fprintf( pErr, "\t        prints the PIs/POs or fanins/fanouts of a node\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    fprintf( pErr, "\tnode  : the node to print fanins/fanouts\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintLatch( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fPrintSccs;
    extern void Abc_NtkPrintSccs( Abc_Ntk_t * pNtk, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fPrintSccs = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "sh" ) ) != EOF )
    {
        switch ( c )
        {
        case 's':
            fPrintSccs ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    // print the nodes
    Abc_NtkPrintLatch( pOut, pNtk );
    if ( fPrintSccs )
        Abc_NtkPrintSccs( pNtk, 0 );
    return 0;

usage:
    fprintf( pErr, "usage: print_latch [-sh]\n" );
    fprintf( pErr, "\t        prints information about latches\n" );
    fprintf( pErr, "\t-s    : toggles printing SCCs of registers [default = %s]\n", fPrintSccs? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintFanio( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fVerbose;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // print the nodes
    if ( fVerbose )
        Abc_NtkPrintFanio( pOut, pNtk );
    else
        Abc_NtkPrintFanioNew( pOut, pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: print_fanio [-vh]\n" );
    fprintf( pErr, "\t        prints the statistics about fanins/fanouts of all nodes\n" );
    fprintf( pErr, "\t-v    : toggles verbose way of printing the stats [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintMffc( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    extern void Abc_NtkPrintMffc( FILE * pFile, Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // print the nodes
    Abc_NtkPrintMffc( pOut, pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: print_mffc [-h]\n" );
    fprintf( pErr, "\t        prints the MFFC of each node in the network\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintFactor( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Abc_Obj_t * pNode;
    int c;
    int fUseRealNames;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseRealNames = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "nh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'n':
            fUseRealNames ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsSopLogic(pNtk) )
    {
        fprintf( pErr, "Printing factored forms can be done for SOP networks.\n" );
        return 1;
    }

    if ( argc > globalUtilOptind + 1 )
    {
        fprintf( pErr, "Wrong number of auguments.\n" );
        goto usage;
    }

    if ( argc == globalUtilOptind + 1 )
    {
        pNode = Abc_NtkFindNode( pNtk, argv[globalUtilOptind] );
        if ( pNode == NULL )
        {
            fprintf( pErr, "Cannot find node \"%s\".\n", argv[globalUtilOptind] );
            return 1;
        }
        Abc_NodePrintFactor( pOut, pNode, fUseRealNames );
        return 0;
    }
    // print the nodes
    Abc_NtkPrintFactor( pOut, pNtk, fUseRealNames );
    return 0;

usage:
    fprintf( pErr, "usage: print_factor [-nh] <node>\n" );
    fprintf( pErr, "\t        prints the factored forms of nodes\n" );
    fprintf( pErr, "\t-n    : toggles real/dummy fanin names [default = %s]\n", fUseRealNames? "real": "dummy" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    fprintf( pErr, "\tnode  : (optional) one node to consider\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintLevel( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Abc_Obj_t * pNode;
    int c;
    int fListNodes;
    int fProfile;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fListNodes = 0;
    fProfile   = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "nph" ) ) != EOF )
    {
        switch ( c )
        {
        case 'n':
            fListNodes ^= 1;
            break;
        case 'p':
            fProfile ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !fProfile && !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for AIGs (run \"strash\").\n" );
        return 1;
    }

    if ( argc > globalUtilOptind + 1 )
    {
        fprintf( pErr, "Wrong number of auguments.\n" );
        goto usage;
    }

    if ( argc == globalUtilOptind + 1 )
    {
        pNode = Abc_NtkFindNode( pNtk, argv[globalUtilOptind] );
        if ( pNode == NULL )
        {
            fprintf( pErr, "Cannot find node \"%s\".\n", argv[globalUtilOptind] );
            return 1;
        }
        Abc_NodePrintLevel( pOut, pNode );
        return 0;
    }
    // process all COs
    Abc_NtkPrintLevel( pOut, pNtk, fProfile, fListNodes );
    return 0;

usage:
    fprintf( pErr, "usage: print_level [-nph] <node>\n" );
    fprintf( pErr, "\t        prints information about node level and cone size\n" );
    fprintf( pErr, "\t-n    : toggles printing nodes by levels [default = %s]\n", fListNodes? "yes": "no" );
    fprintf( pErr, "\t-p    : toggles printing level profile [default = %s]\n", fProfile? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    fprintf( pErr, "\tnode  : (optional) one node to consider\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintSupport( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Vec_Ptr_t * vSuppFun;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fStruct;
    int fVerbose;
    extern Vec_Ptr_t * Sim_ComputeFunSupp( Abc_Ntk_t * pNtk, int fVerbose );
    extern void Abc_NtkPrintStrSupports( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fStruct = 1;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "svh" ) ) != EOF )
    {
        switch ( c )
        {
        case 's':
            fStruct ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // print support information
    if ( fStruct )
    {
        Abc_NtkPrintStrSupports( pNtk );
        return 0;
    }

    if ( !Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "This command works only for combinational networks (run \"comb\").\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for AIGs (run \"strash\").\n" );
        return 1;
    }
    vSuppFun = Sim_ComputeFunSupp( pNtk, fVerbose );
    ABC_FREE( vSuppFun->pArray[0] );
    Vec_PtrFree( vSuppFun );
    return 0;

usage:
    fprintf( pErr, "usage: print_supp [-svh]\n" );
    fprintf( pErr, "\t        prints the supports of the CO nodes\n" );
    fprintf( pErr, "\t-s    : toggle printing structural support only [default = %s].\n", fStruct? "yes": "no" );  
    fprintf( pErr, "\t-v    : enable verbose output [default = %s].\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintSymms( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fUseBdds;
    int fNaive;
    int fReorder;
    int fVerbose;
    extern void Abc_NtkSymmetries( Abc_Ntk_t * pNtk, int fUseBdds, int fNaive, int fReorder, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseBdds = 0;
    fNaive   = 0;
    fReorder = 1;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "bnrvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'b':
            fUseBdds ^= 1;
            break;
        case 'n':
            fNaive ^= 1;
            break;
        case 'r':
            fReorder ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "This command works only for combinational networks (run \"comb\").\n" );
        return 1;
    }
    if ( Abc_NtkIsStrash(pNtk) )
        Abc_NtkSymmetries( pNtk, fUseBdds, fNaive, fReorder, fVerbose );
    else
    {
        pNtk = Abc_NtkStrash( pNtk, 0, 0, 0 );
        Abc_NtkSymmetries( pNtk, fUseBdds, fNaive, fReorder, fVerbose );
        Abc_NtkDelete( pNtk );
    }
    return 0;

usage:
    fprintf( pErr, "usage: print_symm [-bnrvh]\n" );
    fprintf( pErr, "\t         computes symmetries of the PO functions\n" );
    fprintf( pErr, "\t-b     : toggle BDD-based or SAT-based computations [default = %s].\n", fUseBdds? "BDD": "SAT" );  
    fprintf( pErr, "\t-n     : enable naive BDD-based computation [default = %s].\n", fNaive? "yes": "no" );  
    fprintf( pErr, "\t-r     : enable dynamic BDD variable reordering [default = %s].\n", fReorder? "yes": "no" );  
    fprintf( pErr, "\t-v     : enable verbose output [default = %s].\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintUnate( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fUseBdds;
    int fUseNaive;
    int fVerbose;
    extern void Abc_NtkPrintUnate( Abc_Ntk_t * pNtk, int fUseBdds, int fUseNaive, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseBdds  = 1;
    fUseNaive = 0;
    fVerbose  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "bnvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'b':
            fUseBdds ^= 1;
            break;
        case 'n':
            fUseNaive ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for AIGs (run \"strash\").\n" );
        return 1;
    }
    Abc_NtkPrintUnate( pNtk, fUseBdds, fUseNaive, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: print_unate [-bnvh]\n" );
    fprintf( pErr, "\t         computes unate variables of the PO functions\n" );
    fprintf( pErr, "\t-b     : toggle BDD-based or SAT-based computations [default = %s].\n", fUseBdds? "BDD": "SAT" );  
    fprintf( pErr, "\t-n     : toggle naive BDD-based computation [default = %s].\n", fUseNaive? "yes": "no" );  
    fprintf( pErr, "\t-v     : enable verbose output [default = %s].\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintAuto( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int Output;
    int fNaive;
    int fVerbose;
    extern void Abc_NtkAutoPrint( Abc_Ntk_t * pNtk, int Output, int fNaive, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Output   = -1;
    fNaive   = 0;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Onvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'O':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-O\" should be followed by an integer.\n" );
                goto usage;
            }
            Output = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Output < 0 ) 
                goto usage;
            break;
        case 'n':
            fNaive ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for AIGs (run \"strash\").\n" );
        return 1;
    }


    Abc_NtkAutoPrint( pNtk, Output, fNaive, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: print_auto [-O num] [-nvh]\n" );
    fprintf( pErr, "\t         computes autosymmetries of the PO functions\n" );
    fprintf( pErr, "\t-O num : (optional) the 0-based number of the output [default = all]\n");
    fprintf( pErr, "\t-n     : enable naive BDD-based computation [default = %s].\n", fNaive? "yes": "no" );  
    fprintf( pErr, "\t-v     : enable verbose output [default = %s].\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintKMap( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Abc_Obj_t * pNode;
    int c;
    int fUseRealNames;

    extern void Abc_NodePrintKMap( Abc_Obj_t * pNode, int fUseRealNames );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseRealNames = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "nh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'n':
            fUseRealNames ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "Visualization of Karnaugh maps works for logic networks.\n" );
        return 1;
    }
    if ( argc > globalUtilOptind + 1 )
    {
        fprintf( pErr, "Wrong number of auguments.\n" );
        goto usage;
    }
    if ( argc == globalUtilOptind )
    {
        pNode = Abc_ObjFanin0( Abc_NtkPo(pNtk, 0) );
        if ( !Abc_ObjIsNode(pNode) )
        {
            fprintf( pErr, "The driver \"%s\" of the first PO is not an internal node.\n", Abc_ObjName(pNode) );
            return 1;
        }
    }
    else
    {
        pNode = Abc_NtkFindNode( pNtk, argv[globalUtilOptind] );
        if ( pNode == NULL )
        {
            fprintf( pErr, "Cannot find node \"%s\".\n", argv[globalUtilOptind] );
            return 1;
        }
    }
    Abc_NtkToBdd(pNtk);
    Abc_NodePrintKMap( pNode, fUseRealNames );
    return 0;

usage:
    fprintf( pErr, "usage: print_kmap [-nh] <node>\n" );
    fprintf( pErr, "          shows the truth table of the node\n" );
    fprintf( pErr, "\t-n    : toggles real/dummy fanin names [default = %s]\n", fUseRealNames? "real": "dummy" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    fprintf( pErr, "\tnode  : the node to consider (default = the driver of the first PO)\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintGates( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fUseLibrary;

    extern void Abc_NtkPrintGates( Abc_Ntk_t * pNtk, int fUseLibrary );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseLibrary = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUseLibrary ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkHasAig(pNtk) )
    {
        fprintf( pErr, "Printing gates does not work for AIGs and sequential AIGs.\n" );
        return 1;
    }

    Abc_NtkPrintGates( pNtk, fUseLibrary );
    return 0;

usage:
    fprintf( pErr, "usage: print_gates [-lh]\n" );
    fprintf( pErr, "\t        prints statistics about gates used in the network\n" );
    fprintf( pErr, "\t-l    : used library gate names (if mapped) [default = %s]\n", fUseLibrary? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintSharing( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fUseLibrary;

    extern void Abc_NtkPrintSharing( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseLibrary = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUseLibrary ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    Abc_NtkPrintSharing( pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: print_sharing [-h]\n" );
    fprintf( pErr, "\t        prints the number of shared nodes in the TFI cones of the COs\n" );
//    fprintf( pErr, "\t-l    : used library gate names (if mapped) [default = %s]\n", fUseLibrary? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintXCut( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fUseLibrary;

    extern int Abc_NtkCrossCut( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseLibrary = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUseLibrary ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    Abc_NtkCrossCut( pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: print_xcut [-h]\n" );
    fprintf( pErr, "\t        prints the size of the cross cut of the current network\n" );
//    fprintf( pErr, "\t-l    : used library gate names (if mapped) [default = %s]\n", fUseLibrary? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintDsd( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fCofactor;
    int nCofLevel;

    extern void Kit_DsdTest( unsigned * pTruth, int nVars );
    extern void Kit_DsdPrintCofactors( unsigned * pTruth, int nVars, int nCofLevel, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nCofLevel = 1;
    fCofactor = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Nch" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nCofLevel = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCofLevel < 0 ) 
                goto usage;
            break;
        case 'c':
            fCofactor ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    // get the truth table of the first output
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "Currently works only for logic networks.\n" );
        return 1;
    }
    Abc_NtkToAig( pNtk );
    // convert it to truth table
    {
        Abc_Obj_t * pObj = Abc_ObjFanin0( Abc_NtkPo(pNtk, 0) );
        Vec_Int_t * vMemory = Vec_IntAlloc(0);
        unsigned * pTruth;
        if ( !Abc_ObjIsNode(pObj) )
        {
            fprintf( pErr, "The fanin of the first PO node does not have a logic function.\n" );
            return 1;
        }
        if ( Abc_ObjFaninNum(pObj) > 16 )
        {
            fprintf( pErr, "Currently works only for up to 16 inputs.\n" );
            return 1;
        }
        pTruth = Hop_ManConvertAigToTruth( pNtk->pManFunc, Hop_Regular(pObj->pData), Abc_ObjFaninNum(pObj), vMemory, 0 );
        if ( Hop_IsComplement(pObj->pData) )
            Extra_TruthNot( pTruth, pTruth, Abc_ObjFaninNum(pObj) );
//        Extra_PrintBinary( stdout, pTruth, 1 << Abc_ObjFaninNum(pObj) );
//        printf( "\n" );
        if ( fCofactor )
            Kit_DsdPrintCofactors( pTruth, Abc_ObjFaninNum(pObj), nCofLevel, 1 );
        else
            Kit_DsdTest( pTruth, Abc_ObjFaninNum(pObj) );
        Vec_IntFree( vMemory );
    }
    return 0;

usage:
    fprintf( pErr, "usage: print_dsd [-ch] [-N num]\n" );
    fprintf( pErr, "\t         print DSD formula for a single-output function with less than 16 variables\n" );
    fprintf( pErr, "\t-c     : toggle recursive cofactoring [default = %s]\n", fCofactor? "yes": "no" );
    fprintf( pErr, "\t-N num : the number of levels to cofactor [default = %d]\n", nCofLevel );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintCone( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fUseLibrary;

    extern int Abc_NtkDarPrintCone( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseLibrary = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUseLibrary ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkLatchNum(pNtk) == 0 )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 1;
    }
    Abc_NtkDarPrintCone( pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: print_cone [-h]\n" );
    fprintf( pErr, "\t        prints cones of influence info for each primary output\n" );
//    fprintf( pErr, "\t-l    : used library gate names (if mapped) [default = %s]\n", fUseLibrary? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPrintMiter( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fUseLibrary;

    extern void Abc_NtkPrintMiter( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseLibrary = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUseLibrary ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "The network is should be structurally hashed.\n" );
        return 1;
    }
    Abc_NtkPrintMiter( pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: print_miter [-h]\n" );
    fprintf( pErr, "\t        prints the status of the miter\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandShow( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fSeq;
    int fGateNames;
    int fUseReverse;
    extern void Abc_NtkShow( Abc_Ntk_t * pNtk, int fGateNames, int fSeq, int fUseReverse );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fSeq        = 0;
    fGateNames  = 0;
    fUseReverse = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "rsgh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'r':
            fUseReverse ^= 1;
            break;
        case 's':
            fSeq ^= 1;
            break;
        case 'g':
            fGateNames ^= 1;
            break;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    Abc_NtkShow( pNtk, fGateNames, fSeq, fUseReverse );
    return 0;

usage:
    fprintf( pErr, "usage: show [-srgh]\n" );
    fprintf( pErr, "       visualizes the network structure using DOT and GSVIEW\n" );
#ifdef WIN32
    fprintf( pErr, "       \"dot.exe\" and \"gsview32.exe\" should be set in the paths\n" );
    fprintf( pErr, "       (\"gsview32.exe\" may be in \"C:\\Program Files\\Ghostgum\\gsview\\\")\n" );
#endif
    fprintf( pErr, "\t-s    : toggles visualization of sequential networks [default = %s].\n", fSeq? "yes": "no" );  
    fprintf( pErr, "\t-r    : toggles ordering nodes in reverse order [default = %s].\n", fUseReverse? "yes": "no" );  
    fprintf( pErr, "\t-g    : toggles printing gate names for mapped network [default = %s].\n", fGateNames? "yes": "no" );  
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandShowBdd( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Abc_Obj_t * pNode;
    int c;
    extern void Abc_NodeShowBdd( Abc_Obj_t * pNode );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsBddLogic(pNtk) )
    {
        fprintf( pErr, "Visualizing BDDs can only be done for logic BDD networks (run \"bdd\").\n" );
        return 1;
    }

    if ( argc > globalUtilOptind + 1 )
    {
        fprintf( pErr, "Wrong number of auguments.\n" );
        goto usage;
    }
    if ( argc == globalUtilOptind )
    {
        pNode = Abc_ObjFanin0( Abc_NtkPo(pNtk, 0) );
        if ( !Abc_ObjIsNode(pNode) )
        {
            fprintf( pErr, "The driver \"%s\" of the first PO is not an internal node.\n", Abc_ObjName(pNode) );
            return 1;
        }
    }
    else
    {
        pNode = Abc_NtkFindNode( pNtk, argv[globalUtilOptind] );
        if ( pNode == NULL )
        {
            fprintf( pErr, "Cannot find node \"%s\".\n", argv[globalUtilOptind] );
            return 1;
        }
    }
    Abc_NodeShowBdd( pNode );
    return 0;

usage:
    fprintf( pErr, "usage: show_bdd [-h] <node>\n" );
    fprintf( pErr, "       visualizes the BDD of a node using DOT and GSVIEW\n" );
#ifdef WIN32
    fprintf( pErr, "       \"dot.exe\" and \"gsview32.exe\" should be set in the paths\n" );
    fprintf( pErr, "       (\"gsview32.exe\" may be in \"C:\\Program Files\\Ghostgum\\gsview\\\")\n" );
#endif
    fprintf( pErr, "\tnode  : the node to consider [default = the driver of the first PO]\n");
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandShowCut( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Abc_Obj_t * pNode;
    int c;
    int nNodeSizeMax;
    int nConeSizeMax;
    extern void Abc_NodeShowCut( Abc_Obj_t * pNode, int nNodeSizeMax, int nConeSizeMax );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nNodeSizeMax = 10;
    nConeSizeMax = ABC_INFINITY;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NCh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nNodeSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nNodeSizeMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConeSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConeSizeMax < 0 ) 
                goto usage;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Visualizing cuts only works for AIGs (run \"strash\").\n" );
        return 1;
    }
    if ( argc != globalUtilOptind + 1 )
    {
        fprintf( pErr, "Wrong number of auguments.\n" );
        goto usage;
    }

    pNode = Abc_NtkFindNode( pNtk, argv[globalUtilOptind] );
    if ( pNode == NULL )
    {
        fprintf( pErr, "Cannot find node \"%s\".\n", argv[globalUtilOptind] );
        return 1;
    }
    Abc_NodeShowCut( pNode, nNodeSizeMax, nConeSizeMax );
    return 0;

usage:
    fprintf( pErr, "usage: show_cut [-N num] [-C num] [-h] <node>\n" );
    fprintf( pErr, "       visualizes the cut of a node using DOT and GSVIEW\n" );
#ifdef WIN32
    fprintf( pErr, "       \"dot.exe\" and \"gsview32.exe\" should be set in the paths\n" );
    fprintf( pErr, "       (\"gsview32.exe\" may be in \"C:\\Program Files\\Ghostgum\\gsview\\\")\n" );
#endif
    fprintf( pErr, "\t-N num : the max size of the cut to be computed [default = %d]\n", nNodeSizeMax );  
    fprintf( pErr, "\t-C num : the max support of the containing cone [default = %d]\n", nConeSizeMax );  
    fprintf( pErr, "\tnode   : the node to consider\n");
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCollapse( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int fVerbose;
    int fBddSizeMax;
    int fDualRail;
    int fReorder;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose = 1;
    fReorder = 1;
    fDualRail = 0;
    fBddSizeMax = 50000000;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Brdvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'B':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-B\" should be followed by an integer.\n" );
                goto usage;
            }
            fBddSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( fBddSizeMax < 0 ) 
                goto usage;
            break;
        case 'd':
            fDualRail ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'r':
            fReorder ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsLogic(pNtk) && !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Can only collapse a logic network or an AIG.\n" );
        return 1;
    }

    // get the new network
    if ( Abc_NtkIsStrash(pNtk) )
        pNtkRes = Abc_NtkCollapse( pNtk, fBddSizeMax, fDualRail, fReorder, fVerbose );
    else
    {
        pNtk = Abc_NtkStrash( pNtk, 0, 0, 0 );
        pNtkRes = Abc_NtkCollapse( pNtk, fBddSizeMax, fDualRail, fReorder, fVerbose );
        Abc_NtkDelete( pNtk );
    }
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Collapsing has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: collapse [-B num] [-rdvh]\n" );
    fprintf( pErr, "\t          collapses the network by constructing global BDDs\n" );
    fprintf( pErr, "\t-B num  : limit on live BDD nodes during collapsing [default = %d]\n", fBddSizeMax );
    fprintf( pErr, "\t-r      : toggles dynamic variable reordering [default = %s]\n", fReorder? "yes": "no" );
    fprintf( pErr, "\t-d      : toggles dual-rail collapsing mode [default = %s]\n", fDualRail? "yes": "no" );
    fprintf( pErr, "\t-v      : print verbose information [default = %s]\n", fVerbose? "yes": "no" ); 
    fprintf( pErr, "\t-h      : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandStrash( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    Abc_Obj_t * pObj;
    int c;
    int fAllNodes;
    int fRecord;
    int fCleanup;
    int fComplOuts;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fAllNodes = 0;
    fCleanup  = 1;
    fRecord   = 0;
    fComplOuts= 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "acrih" ) ) != EOF )
    {
        switch ( c )
        {
        case 'a':
            fAllNodes ^= 1;
            break;
        case 'c':
            fCleanup ^= 1;
            break;
        case 'r':
            fRecord ^= 1;
            break;
        case 'i':
            fComplOuts ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // get the new network
    pNtkRes = Abc_NtkStrash( pNtk, fAllNodes, fCleanup, fRecord );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Strashing has failed.\n" );
        return 1;
    }
    if ( fComplOuts )
    Abc_NtkForEachPo( pNtkRes, pObj, c )
        Abc_ObjXorFaninC( pObj, 0 );
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: strash [-acrih]\n" );
    fprintf( pErr, "\t        transforms combinational logic into an AIG\n" );
    fprintf( pErr, "\t-a    : toggles between using all nodes and DFS nodes [default = %s]\n", fAllNodes? "all": "DFS" );
    fprintf( pErr, "\t-c    : toggles cleanup to remove the dagling AIG nodes [default = %s]\n", fCleanup? "all": "DFS" );
    fprintf( pErr, "\t-r    : toggles using the record of AIG subgraphs [default = %s]\n", fRecord? "yes": "no" );
    fprintf( pErr, "\t-i    : toggles complementing the COs of the AIG [default = %s]\n", fComplOuts? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandBalance( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes, * pNtkTemp;
    int c;
    bool fDuplicate;
    bool fSelective;
    bool fUpdateLevel;
    int fExor;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkBalanceExor( Abc_Ntk_t * pNtk, int fUpdateLevel, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fDuplicate   = 0;
    fSelective   = 0;
    fUpdateLevel = 1;
    fExor        = 0;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ldsxvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'd':
            fDuplicate ^= 1;
            break;
        case 's':
            fSelective ^= 1;
            break;
        case 'x':
            fExor ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    // get the new network
    if ( Abc_NtkIsStrash(pNtk) )
    {
        if ( fExor ) 
            pNtkRes = Abc_NtkBalanceExor( pNtk, fUpdateLevel, fVerbose );
        else
            pNtkRes = Abc_NtkBalance( pNtk, fDuplicate, fSelective, fUpdateLevel );
    }
    else
    {
        pNtkTemp = Abc_NtkStrash( pNtk, 0, 0, 0 );
        if ( pNtkTemp == NULL )
        {
            fprintf( pErr, "Strashing before balancing has failed.\n" );
            return 1;
        }
        if ( fExor ) 
            pNtkRes = Abc_NtkBalanceExor( pNtkTemp, fUpdateLevel, fVerbose );
        else
            pNtkRes = Abc_NtkBalance( pNtkTemp, fDuplicate, fSelective, fUpdateLevel );
        Abc_NtkDelete( pNtkTemp );
    }

    // check if balancing worked
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Balancing has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: balance [-ldsxvh]\n" );
    fprintf( pErr, "\t        transforms the current network into a well-balanced AIG\n" );
    fprintf( pErr, "\t-l    : toggle minimizing the number of levels [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-d    : toggle duplication of logic [default = %s]\n", fDuplicate? "yes": "no" );
    fprintf( pErr, "\t-s    : toggle duplication on the critical paths [default = %s]\n", fSelective? "yes": "no" );
    fprintf( pErr, "\t-x    : toggle balancing multi-input EXORs [default = %s]\n", fExor? "yes": "no" );
    fprintf( pErr, "\t-v    : print verbose information [default = %s]\n", fVerbose? "yes": "no" ); 
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandMuxStruct( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fVerbose;

    extern Abc_Ntk_t * Abc_NtkMuxRestructure( Abc_Ntk_t * pNtk, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    // get the new network
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Does not work for a logic network.\n" );
        return 1;
    }
    // check if balancing worked
//    pNtkRes = Abc_NtkMuxRestructure( pNtk, fVerbose );
    pNtkRes = NULL;
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "MUX restructuring has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: mux_struct [-vh]\n" );
    fprintf( pErr, "\t        performs MUX restructuring of the current network\n" );
    fprintf( pErr, "\t-v    : print verbose information [default = %s]\n", fVerbose? "yes": "no" ); 
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandMulti( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int nThresh, nFaninMax, c;
    int fCnf;
    int fMulti;
    int fSimple;
    int fFactor;
    extern Abc_Ntk_t * Abc_NtkMulti( Abc_Ntk_t * pNtk, int nThresh, int nFaninMax, int fCnf, int fMulti, int fSimple, int fFactor );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nThresh   =  1;
    nFaninMax = 20;
    fCnf      =  0;
    fMulti    =  1;
    fSimple   =  0;
    fFactor   =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "TFmcsfh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            nThresh = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nThresh < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFaninMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFaninMax < 0 ) 
                goto usage;
            break;
        case 'c':
            fCnf ^= 1;
            break;
        case 'm':
            fMulti ^= 1;
            break;
        case 's':
            fSimple ^= 1;
            break;
        case 'f':
            fFactor ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Cannot renode a network that is not an AIG (run \"strash\").\n" );
        return 1;
    }

    // get the new network
    pNtkRes = Abc_NtkMulti( pNtk, nThresh, nFaninMax, fCnf, fMulti, fSimple, fFactor );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Renoding has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: multi [-T num] [-F num] [-msfch]\n" );
    fprintf( pErr, "\t          transforms an AIG into a logic network by creating larger nodes\n" );
    fprintf( pErr, "\t-F num  : the maximum fanin size after renoding [default = %d]\n", nFaninMax );
    fprintf( pErr, "\t-T num  : the threshold for AIG node duplication [default = %d]\n", nThresh );
    fprintf( pErr, "\t          (an AIG node is the root of a new node after renoding\n" );
    fprintf( pErr, "\t          if this leads to duplication of no more than %d AIG nodes,\n", nThresh );
    fprintf( pErr, "\t          that is, if [(numFanouts(Node)-1) * size(MFFC(Node))] <= %d)\n", nThresh );
    fprintf( pErr, "\t-m      : creates multi-input AND graph [default = %s]\n", fMulti? "yes": "no" );
    fprintf( pErr, "\t-s      : creates a simple AIG (no renoding) [default = %s]\n", fSimple? "yes": "no" );
    fprintf( pErr, "\t-f      : creates a factor-cut network [default = %s]\n", fFactor? "yes": "no" );
    fprintf( pErr, "\t-c      : performs renoding to derive the CNF [default = %s]\n", fCnf? "yes": "no" );
    fprintf( pErr, "\t-h      : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRenode( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int nLutSize, nCutsMax, c;
    int nFlowIters, nAreaIters;
    int fArea;
    int fUseBdds;
    int fUseSops;
    int fUseCnfs;
    int fUseMv;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkRenode( Abc_Ntk_t * pNtk, int nLutSize, int nCutsMax, int nFlowIters, int nAreaIters, int fArea, int fUseBdds, int fUseSops, int fUseCnfs, int fUseMv, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nLutSize   =  8;
    nCutsMax   =  4;
    nFlowIters =  1;
    nAreaIters =  1;
    fArea      =  0;
    fUseBdds   =  0;
    fUseSops   =  0;
    fUseCnfs   =  0;
    fUseMv     =  0;
    fVerbose   =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "KCFAabscivh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLutSize < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCutsMax < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nFlowIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFlowIters < 0 ) 
                goto usage;
            break;
        case 'A':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-A\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nAreaIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nAreaIters < 0 ) 
                goto usage;
            break;
        case 'a':
            fArea ^= 1;
            break;
        case 'b':
            fUseBdds ^= 1;
            break;
        case 's':
            fUseSops ^= 1;
            break;
        case 'c':
            fUseCnfs ^= 1;
            break;
        case 'i':
            fUseMv ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( fUseBdds + fUseSops + fUseCnfs + fUseMv > 1 )
    {
        fprintf( pErr, "Cannot optimize two parameters at the same time.\n" );
        return 1;
    }

    if ( nLutSize < 3 || nLutSize > IF_MAX_FUNC_LUTSIZE )
    {
        fprintf( pErr, "Incorrect LUT size (%d).\n", nLutSize );
        return 1;
    }

    if ( nCutsMax < 1 || nCutsMax >= (1<<12) )
    {
        fprintf( pErr, "Incorrect number of cuts.\n" );
        return 1;
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Cannot renode a network that is not an AIG (run \"strash\").\n" );
        return 1;
    }

    // get the new network
    pNtkRes = Abc_NtkRenode( pNtk, nLutSize, nCutsMax, nFlowIters, nAreaIters, fArea, fUseBdds, fUseSops, fUseCnfs, fUseMv, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Renoding has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: renode [-K num] [-C num] [-F num] [-A num] [-sbciav]\n" );
    fprintf( pErr, "\t          transforms the AIG into a logic network with larger nodes\n" );
    fprintf( pErr, "\t          while minimizing the number of FF literals of the node SOPs\n" );
    fprintf( pErr, "\t-K num  : the max cut size for renoding (2 < num < %d) [default = %d]\n", IF_MAX_FUNC_LUTSIZE+1, nLutSize );
    fprintf( pErr, "\t-C num  : the max number of cuts used at a node (0 < num < 2^12) [default = %d]\n", nCutsMax );
    fprintf( pErr, "\t-F num  : the number of area flow recovery iterations (num >= 0) [default = %d]\n", nFlowIters );
    fprintf( pErr, "\t-A num  : the number of exact area recovery iterations (num >= 0) [default = %d]\n", nAreaIters );
    fprintf( pErr, "\t-s      : toggles minimizing SOP cubes instead of FF lits [default = %s]\n", fUseSops? "yes": "no" );
    fprintf( pErr, "\t-b      : toggles minimizing BDD nodes instead of FF lits [default = %s]\n", fUseBdds? "yes": "no" );
    fprintf( pErr, "\t-c      : toggles minimizing CNF clauses instead of FF lits [default = %s]\n", fUseCnfs? "yes": "no" );
    fprintf( pErr, "\t-i      : toggles minimizing MV-SOP instead of FF lits [default = %s]\n", fUseMv? "yes": "no" );
    fprintf( pErr, "\t-a      : toggles area-oriented mapping [default = %s]\n", fArea? "yes": "no" );
    fprintf( pErr, "\t-v      : print verbose information [default = %s]\n", fVerbose? "yes": "no" ); 
    fprintf( pErr, "\t-h      : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCleanup( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fCleanupPis;
    int fCleanupPos;
    int fVerbose;

    extern Abc_Ntk_t * Abc_NtkDarCleanupAig( Abc_Ntk_t * pNtk, int fCleanupPis, int fCleanupPos, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fCleanupPis = 1;
    fCleanupPos = 1;
    fVerbose    = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "iovh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'i':
            fCleanupPis ^= 1;
            break;
        case 'o':
            fCleanupPos ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsStrash(pNtk) )
    {
        if ( !fCleanupPos && !fCleanupPos )
        {
            printf( "Cleanup for PIs and POs is not enabled.\n" );
            pNtkRes = Abc_NtkDup( pNtk );
        }
        else
            pNtkRes = Abc_NtkDarCleanupAig( pNtk, fCleanupPis, fCleanupPos, fVerbose );
    }
    else
    {
        Abc_NtkCleanup( pNtk, fVerbose );
        pNtkRes = Abc_NtkDup( pNtk );
    }
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Cleanup has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: cleanup [-iovh]\n" );
    fprintf( pErr, "\t        for logic networks, removes dangling combinatinal logic\n" );
    fprintf( pErr, "\t        for AIGs, removes PIs w/o fanout and POs driven by const-0\n" );
    fprintf( pErr, "\t-i    : toggles removing PIs without fanout [default = %s]\n", fCleanupPis? "yes": "no" );
    fprintf( pErr, "\t-o    : toggles removing POs with const-0 drivers [default = %s]\n", fCleanupPos? "yes": "no" );
    fprintf( pErr, "\t-v    : print verbose information [default = %s]\n", fVerbose? "yes": "no" ); 
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSweep( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "The classical (SIS-like) sweep can only be performed on a logic network.\n" );
        return 1;
    }
    // modify the current network
    Abc_NtkSweep( pNtk, 0 );
    return 0;

usage:
    fprintf( pErr, "usage: sweep [-h]\n" );
    fprintf( pErr, "\t        removes dangling nodes; propagates constant, buffers, inverters\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFastExtract( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    FILE * pOut, * pErr;
    Fxu_Data_t * p = NULL;
    int c;
    extern bool Abc_NtkFastExtract( Abc_Ntk_t * pNtk, Fxu_Data_t * p );
    extern void Abc_NtkFxuFreeInfo( Fxu_Data_t * p );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // allocate the structure
    p = ABC_ALLOC( Fxu_Data_t, 1 );
    memset( p, 0, sizeof(Fxu_Data_t) );
    // set the defaults
    p->nSingleMax = 20000;
    p->nPairsMax  = 30000;
    p->nNodesExt  = 10000;
    p->fOnlyS     = 0;
    p->fOnlyD     = 0;
    p->fUse0      = 0;
    p->fUseCompl  = 1;
    p->fVerbose   = 0;
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "SDNsdzcvh")) != EOF ) 
    {
        switch (c) 
        {
            case 'S':
                if ( globalUtilOptind >= argc )
                {
                    fprintf( pErr, "Command line switch \"-S\" should be followed by an integer.\n" );
                    goto usage;
                }
                p->nSingleMax = atoi(argv[globalUtilOptind]);
                globalUtilOptind++;
                if ( p->nSingleMax < 0 ) 
                    goto usage;
                break;
            case 'D':
                if ( globalUtilOptind >= argc )
                {
                    fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                    goto usage;
                }
                p->nPairsMax = atoi(argv[globalUtilOptind]);
                globalUtilOptind++;
                if ( p->nPairsMax < 0 ) 
                    goto usage;
                break;
            case 'N':
                if ( globalUtilOptind >= argc )
                {
                    fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                    goto usage;
                }
                p->nNodesExt = atoi(argv[globalUtilOptind]);
                globalUtilOptind++;
                if ( p->nNodesExt < 0 ) 
                    goto usage;
                break;
            case 's':
                p->fOnlyS ^= 1;
                break;
            case 'd':
                p->fOnlyD ^= 1;
                break;
            case 'z':
                p->fUse0 ^= 1;
                break;
            case 'c':
                p->fUseCompl ^= 1;
                break;
            case 'v':
                p->fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    } 

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        Abc_NtkFxuFreeInfo( p );
        return 1;
    }

    if ( Abc_NtkNodeNum(pNtk) == 0 )
    {
        fprintf( pErr, "The network does not have internal nodes.\n" );
        Abc_NtkFxuFreeInfo( p );
        return 1;
    }

    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "Fast extract can only be applied to a logic network (run \"renode\").\n" );
        Abc_NtkFxuFreeInfo( p );
        return 1;
    }


    // the nodes to be merged are linked into the special linked list
    Abc_NtkFastExtract( pNtk, p );
    Abc_NtkFxuFreeInfo( p );
    return 0;

usage:
    fprintf( pErr, "usage: fx [-SDN num] [-sdzcvh]\n");
    fprintf( pErr, "\t         performs unate fast extract on the current network\n");
    fprintf( pErr, "\t-S num : max number of single-cube divisors to consider [default = %d]\n", p->nSingleMax );  
    fprintf( pErr, "\t-D num : max number of double-cube divisors to consider [default = %d]\n", p->nPairsMax );  
    fprintf( pErr, "\t-N num : the maximum number of divisors to extract [default = %d]\n", p->nNodesExt );  
    fprintf( pErr, "\t-s     : use only single-cube divisors [default = %s]\n", p->fOnlyS? "yes": "no" );  
    fprintf( pErr, "\t-d     : use only double-cube divisors [default = %s]\n", p->fOnlyD? "yes": "no" );  
    fprintf( pErr, "\t-z     : use zero-weight divisors [default = %s]\n", p->fUse0? "yes": "no" );  
    fprintf( pErr, "\t-c     : use complement in the binary case [default = %s]\n", p->fUseCompl? "yes": "no" );  
    fprintf( pErr, "\t-v     : print verbose information [default = %s]\n", p->fVerbose? "yes": "no" ); 
    fprintf( pErr, "\t-h     : print the command usage\n");
    Abc_NtkFxuFreeInfo( p );
    return 1;       
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandEliminate( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    FILE * pOut, * pErr;
    int nMaxSize;
    int fReverse;
    int fVerbose;
    int c;
    extern int Abc_NtkEliminate( Abc_Ntk_t * pNtk, int nMaxSize, int fReverse, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set the defaults
    nMaxSize  = 8;
    fReverse  = 0;
    fVerbose  = 0;
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "Nrvh")) != EOF ) 
    {
        switch (c) 
        {
            case 'N':
                if ( globalUtilOptind >= argc )
                {
                    fprintf( pErr, "Command line switch \"-N\" should be followed by a positive integer.\n" );
                    goto usage;
                }
                nMaxSize = atoi(argv[globalUtilOptind]);
                globalUtilOptind++;
                if ( nMaxSize <= 0 ) 
                    goto usage;
                break;
            case 'r':
                fReverse ^= 1;
                break;
            case 'v':
                fVerbose ^= 1;
                break;
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    } 

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkNodeNum(pNtk) == 0 )
    {
        fprintf( pErr, "The network does not have internal nodes.\n" );
        return 1;
    }

    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to a logic network (run \"renode\" or \"if\").\n" );
        return 1;
    }

    // the nodes to be merged are linked into the special linked list
    Abc_NtkEliminate( pNtk, nMaxSize, fReverse, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: eliminate [-N num] [-rvh]\n");
    fprintf( pErr, "\t         greedily eliminates nodes by collapsing them into fanouts\n");
    fprintf( pErr, "\t-N num : the maximum support size after collapsing [default = %d]\n", nMaxSize );  
    fprintf( pErr, "\t-r     : use the reverse topological order [default = %s]\n", fReverse? "yes": "no" );  
    fprintf( pErr, "\t-v     : print verbose information [default = %s]\n", fVerbose? "yes": "no" ); 
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;       
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDisjoint( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes, * pNtkNew;
    int fGlobal, fRecursive, fVerbose, fPrint, fShort, c;

    extern Abc_Ntk_t * Abc_NtkDsdGlobal( Abc_Ntk_t * pNtk, bool fVerbose, bool fPrint, bool fShort );
    extern int         Abc_NtkDsdLocal( Abc_Ntk_t * pNtk, bool fVerbose, bool fRecursive );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fGlobal    = 1;
    fRecursive = 0;
    fVerbose   = 0;
    fPrint     = 0;
    fShort     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "grvpsh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'g':
                fGlobal ^= 1;
                break;
            case 'r':
                fRecursive ^= 1;
                break;
            case 'v':
                fVerbose ^= 1;
                break;
            case 'p':
                fPrint ^= 1;
                break;
            case 's':
                fShort ^= 1;
                break;
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( fGlobal )
    {
//        fprintf( stdout, "Performing DSD of global functions of the network.\n" );
        // get the new network
        if ( !Abc_NtkIsStrash(pNtk) )
        {
            pNtkNew = Abc_NtkStrash( pNtk, 0, 0, 0 );
            pNtkRes = Abc_NtkDsdGlobal( pNtkNew, fVerbose, fPrint, fShort );
            Abc_NtkDelete( pNtkNew );
        }
        else
        {
            pNtkRes = Abc_NtkDsdGlobal( pNtk, fVerbose, fPrint, fShort );
        }
        if ( pNtkRes == NULL )
        {
            fprintf( pErr, "Global DSD has failed.\n" );
            return 1;
        }
        // replace the current network
        Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    }
    else if ( fRecursive )
    {
        if ( !Abc_NtkIsBddLogic( pNtk ) )
        {
            fprintf( pErr, "This command is only applicable to logic BDD networks.\n" );
            return 1;
        }
        fprintf( stdout, "Performing recursive DSD and MUX decomposition of local functions.\n" );
        if ( !Abc_NtkDsdLocal( pNtk, fVerbose, fRecursive ) )
            fprintf( pErr, "Recursive DSD has failed.\n" );
    }
    else 
    {
        if ( !Abc_NtkIsBddLogic( pNtk ) )
        {
            fprintf( pErr, "This command is only applicable to logic BDD networks (run \"bdd\").\n" );
            return 1;
        }
        fprintf( stdout, "Performing simple non-recursive DSD of local functions.\n" );
        if ( !Abc_NtkDsdLocal( pNtk, fVerbose, fRecursive ) )
            fprintf( pErr, "Simple DSD of local functions has failed.\n" );
    }
    return 0;

usage:
    fprintf( pErr, "usage: dsd [-grvpsh]\n" );
    fprintf( pErr, "\t     decomposes the network using disjoint-support decomposition\n" );
    fprintf( pErr, "\t-g     : toggle DSD of global and local functions [default = %s]\n", fGlobal? "global": "local" );  
    fprintf( pErr, "\t-r     : toggle recursive DSD/MUX and simple DSD [default = %s]\n", fRecursive? "recursive DSD/MUX": "simple DSD" );  
    fprintf( pErr, "\t-v     : prints DSD statistics and runtime [default = %s]\n", fVerbose? "yes": "no" ); 
    fprintf( pErr, "\t-p     : prints DSD structure to the standard output [default = %s]\n", fPrint? "yes": "no" ); 
    fprintf( pErr, "\t-s     : use short PI names when printing DSD structure [default = %s]\n", fShort? "yes": "no" ); 
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandLutpack( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Lpk_Par_t Pars, * pPars = &Pars;
    int c;
 
    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    memset( pPars, 0, sizeof(Lpk_Par_t) );
    pPars->nLutsMax     =  4; // (N) the maximum number of LUTs in the structure
    pPars->nLutsOver    =  3; // (Q) the maximum number of LUTs not in the MFFC
    pPars->nVarsShared  =  0; // (S) the maximum number of shared variables (crossbars)
    pPars->nGrowthLevel =  0; // (L) the maximum number of increased levels
    pPars->fSatur       =  1;
    pPars->fZeroCost    =  0; 
    pPars->fFirst       =  0;
    pPars->fOldAlgo     =  0;
    pPars->fVerbose     =  0;
    pPars->fVeryVerbose =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NQSLszfovwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nLutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nLutsMax < 2 || pPars->nLutsMax > 8 ) 
                goto usage;
            break;
        case 'Q':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-Q\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nLutsOver = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nLutsOver < 0 || pPars->nLutsOver > 8 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nVarsShared = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nVarsShared < 0 || pPars->nVarsShared > 4 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nGrowthLevel = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nGrowthLevel < 0 || pPars->nGrowthLevel > ABC_INFINITY ) 
                goto usage;
            break;
        case 's':
            pPars->fSatur ^= 1;
            break;
        case 'z':
            pPars->fZeroCost ^= 1;
            break;
        case 'f':
            pPars->fFirst ^= 1;
            break;
        case 'o':
            pPars->fOldAlgo ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to a logic network.\n" );
        return 1;
    }
    if ( pPars->nVarsShared < 0 || pPars->nVarsShared > 3 )
    {
        fprintf( pErr, "The number of shared variables (%d) is not in the range 0 <= S <= 3.\n", pPars->nVarsShared );
        return 1;
    }

    // modify the current network
    if ( !Lpk_Resynthesize( pNtk, pPars ) )
    {
        fprintf( pErr, "Resynthesis has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: lutpack [-N <num>] [-Q <num>] [-S <num>] [-L <num>] [-szfovwh]\n" );
    fprintf( pErr, "\t           performs \"rewriting\" for LUT network;\n" );
    fprintf( pErr, "\t           determines LUT size as the max fanin count of a node;\n" );
    fprintf( pErr, "\t           if the network is not LUT-mapped, packs it into 6-LUTs\n" );
    fprintf( pErr, "\t           (there is another command for resynthesis after LUT mapping, \"imfs\")\n" );
    fprintf( pErr, "\t-N <num> : the max number of LUTs in the structure (2 <= num) [default = %d]\n", pPars->nLutsMax );
    fprintf( pErr, "\t-Q <num> : the max number of LUTs not in MFFC (0 <= num) [default = %d]\n", pPars->nLutsOver );
    fprintf( pErr, "\t-S <num> : the max number of LUT inputs shared (0 <= num <= 3) [default = %d]\n", pPars->nVarsShared );
    fprintf( pErr, "\t-L <num> : max level increase after resynthesis (0 <= num) [default = %d]\n", pPars->nGrowthLevel );
    fprintf( pErr, "\t-s       : toggle iteration till saturation [default = %s]\n", pPars->fSatur? "yes": "no" );
    fprintf( pErr, "\t-z       : toggle zero-cost replacements [default = %s]\n", pPars->fZeroCost? "yes": "no" );
    fprintf( pErr, "\t-f       : toggle using only first node and first cut [default = %s]\n", pPars->fFirst? "yes": "no" );
    fprintf( pErr, "\t-o       : toggle using old implementation [default = %s]\n", pPars->fOldAlgo? "yes": "no" );
    fprintf( pErr, "\t-v       : toggle verbose printout [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w       : toggle detailed printout of decomposed functions [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : print the command usage\n");
    return 1;
} 

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandLutmin( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int nLutSize;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkLutmin( Abc_Ntk_t * pNtk, int nLutSize, int fVerbose );
 
    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nLutSize = 4;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Kvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    // modify the current network
    pNtkRes = Abc_NtkLutmin( pNtk, nLutSize, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "The command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: lutmin [-K <num>] [-vh]\n" );
    fprintf( pErr, "\t           perform FPGA mapping while minimizing the LUT count\n" );
    fprintf( pErr, "\t           as described in the paper T. Sasao and A. Mishchenko:\n" );
    fprintf( pErr, "\t           \"On the number of LUTs to implement logic functions\".\n" );
    fprintf( pErr, "\t-K <num> : the LUT size to use for the mapping (2 <= num) [default = %d]\n", nLutSize );
    fprintf( pErr, "\t-v       : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : print the command usage\n");
    return 1;
} 

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandImfs( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Res_Par_t Pars, * pPars = &Pars;
    int c;

//    printf( "Implementation of this command is not finished.\n" );
//    return 1;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    pPars->nWindow      = 62;
    pPars->nCands       =  5;
    pPars->nSimWords    =  4;
    pPars->nGrowthLevel =  0;
    pPars->fArea        =  0;
    pPars->fVerbose     =  0;
    pPars->fVeryVerbose =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "WSCLavwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWindow = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWindow < 1 || pPars->nWindow > 99 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nSimWords = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nSimWords < 1 || pPars->nSimWords > 256 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nCands = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nCands < 0 || pPars->nCands > ABC_INFINITY ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nGrowthLevel = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nGrowthLevel < 0 || pPars->nGrowthLevel > ABC_INFINITY ) 
                goto usage;
            break;
        case 'a':
            pPars->fArea ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to a logic network.\n" );
        return 1;
    }

    // modify the current network
    if ( !Abc_NtkResynthesize( pNtk, pPars ) )
    {
        fprintf( pErr, "Resynthesis has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: imfs [-W <NM>] [-L <num>] [-C <num>] [-S <num>] [-avwh]\n" );
    fprintf( pErr, "\t           performs resubstitution-based resynthesis with interpolation\n" );
    fprintf( pErr, "\t           (there is another command for resynthesis after LUT mapping, \"lutpack\")\n" );
    fprintf( pErr, "\t-W <NM>  : fanin/fanout levels (NxM) of the window (00 <= NM <= 99) [default = %d%d]\n", pPars->nWindow/10, pPars->nWindow%10 );
    fprintf( pErr, "\t-C <num> : the max number of resub candidates (1 <= n) [default = %d]\n", pPars->nCands );
    fprintf( pErr, "\t-S <num> : the number of simulation words (1 <= n <= 256) [default = %d]\n", pPars->nSimWords );
    fprintf( pErr, "\t-L <num> : the max increase in node level after resynthesis (0 <= num) [default = %d]\n", pPars->nGrowthLevel );
    fprintf( pErr, "\t-a       : toggle optimization for area only [default = %s]\n", pPars->fArea? "yes": "no" );
    fprintf( pErr, "\t-v       : toggle verbose printout [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w       : toggle printout subgraph statistics [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : print the command usage\n");
    return 1;
} 

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandMfs( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Mfs_Par_t Pars, * pPars = &Pars;
    int c;

//    printf( "Implementation of this command is not finished.\n" );
//    return 1;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Abc_NtkMfsParsDefault( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "WFDMLCraestpvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWinTfoLevs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWinTfoLevs < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFanoutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFanoutsMax < 1 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nDepthMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nDepthMax < 0 ) 
                goto usage;
            break;
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-M\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWinSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWinSizeMax < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nGrowthLevel = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nGrowthLevel < 0 || pPars->nGrowthLevel > ABC_INFINITY ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'r':
            pPars->fResub ^= 1;
            break;
        case 'a':
            pPars->fArea ^= 1;
            break;
        case 'e':
            pPars->fMoreEffort ^= 1;
            break;
        case 's':
            pPars->fSwapEdge ^= 1;
            break;
        case 't':
            pPars->fOneHotness ^= 1;
            break;
        case 'p':
            pPars->fPower ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to a logic network.\n" );
        return 1;
    }

    // modify the current network
    if ( !Abc_NtkMfs( pNtk, pPars ) )
    {
        fprintf( pErr, "Resynthesis has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: mfs [-WFDMLC <num>] [-raestpvh]\n" );
    fprintf( pErr, "\t           performs don't-care-based optimization of logic networks\n" );
    fprintf( pErr, "\t-W <num> : the number of levels in the TFO cone (0 <= num) [default = %d]\n", pPars->nWinTfoLevs );
    fprintf( pErr, "\t-F <num> : the max number of fanouts to skip (1 <= num) [default = %d]\n", pPars->nFanoutsMax );
    fprintf( pErr, "\t-D <num> : the max depth nodes to try (0 = no limit) [default = %d]\n", pPars->nDepthMax );
    fprintf( pErr, "\t-M <num> : the max node count of windows to consider (0 = no limit) [default = %d]\n", pPars->nWinSizeMax );
    fprintf( pErr, "\t-L <num> : the max increase in node level after resynthesis (0 <= num) [default = %d]\n", pPars->nGrowthLevel );
    fprintf( pErr, "\t-C <num> : the max number of conflicts in one SAT run (0 = no limit) [default = %d]\n", pPars->nBTLimit );
    fprintf( pErr, "\t-r       : toggle resubstitution and dc-minimization [default = %s]\n", pPars->fResub? "resub": "dc-min" );
    fprintf( pErr, "\t-a       : toggle minimizing area or area+edges [default = %s]\n", pPars->fArea? "area": "area+edges" );
    fprintf( pErr, "\t-e       : toggle high-effort resubstitution [default = %s]\n", pPars->fMoreEffort? "yes": "no" );
    fprintf( pErr, "\t-s       : toggle evaluation of edge swapping [default = %s]\n", pPars->fSwapEdge? "yes": "no" );
    fprintf( pErr, "\t-t       : toggle using artificial one-hotness conditions [default = %s]\n", pPars->fOneHotness? "yes": "no" );
    fprintf( pErr, "\t-p       : toggle power-aware optimization [default = %s]\n", pPars->fPower? "yes": "no" );
    fprintf( pErr, "\t-v       : toggle printing optimization summary [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w       : toggle printing detailed stats for each node [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : print the command usage\n");
    return 1;
} 


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandTrace( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fUseLutLib;
    int fVerbose;
    extern void Abc_NtkDelayTracePrint( Abc_Ntk_t * pNtk, int fUseLutLib, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseLutLib = 0;
    fVerbose   = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUseLutLib ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to a logic network.\n" );
        return 1;
    }

    // modify the current network
    Abc_NtkDelayTracePrint( pNtk, fUseLutLib, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: trace [-lvh]\n" );
    fprintf( pErr, "\t           performs delay trace of LUT-mapped network\n" );
    fprintf( pErr, "\t-l       : toggle using unit- or LUT-library-delay model [default = %s]\n", fUseLutLib? "lib": "unit" );
    fprintf( pErr, "\t-v       : toggle printing optimization summary [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : print the command usage\n");
    return 1;
} 

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSpeedup( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fUseLutLib;
    int Percentage;
    int Degree;
    int fVerbose;
    int fVeryVerbose;
    extern Abc_Ntk_t * Abc_NtkSpeedup( Abc_Ntk_t * pNtk, int fUseLutLib, int Percentage, int Degree, int fVerbose, int fVeryVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseLutLib = 0;
    Percentage = 5;
    Degree     = 2;
    fVerbose   = 0;
    fVeryVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PNlvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            Percentage = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Percentage < 1 || Percentage > 100 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            Degree = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Degree < 1 || Degree > 5 ) 
                goto usage;
            break;
        case 'l':
            fUseLutLib ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'w':
            fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to a logic network.\n" );
        return 1;
    }

    // modify the current network
    pNtkRes = Abc_NtkSpeedup( pNtk, fUseLutLib, Percentage, Degree, fVerbose, fVeryVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "The command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: speedup [-P num] [-N num] [-lvwh]\n" );
    fprintf( pErr, "\t           transforms LUT-mapped network into an AIG with choices;\n" );
    fprintf( pErr, "\t           the choices are added to speedup the next round of mapping\n" );
    fprintf( pErr, "\t-P <num> : delay delta defining critical path for library model [default = %d%%]\n", Percentage );
    fprintf( pErr, "\t-N <num> : the max critical path degree for resynthesis (0 < num < 6) [default = %d]\n", Degree );
    fprintf( pErr, "\t-l       : toggle using unit- or LUT-library-delay model [default = %s]\n", fUseLutLib? "lib" : "unit" );
    fprintf( pErr, "\t-v       : toggle printing optimization summary [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w       : toggle printing detailed stats for each node [default = %s]\n", fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : print the command usage\n");
    return 1;
} 

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPowerdown( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fUseLutLib;
    int Percentage;
    int Degree;
    int fVerbose;
    int fVeryVerbose;
    extern Abc_Ntk_t * Abc_NtkPowerdown( Abc_Ntk_t * pNtk, int fUseLutLib, int Percentage, int Degree, int fVerbose, int fVeryVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseLutLib = 0;
    Percentage =10;
    Degree     = 2;
    fVerbose   = 0;
    fVeryVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PNlvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            Percentage = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Percentage < 1 || Percentage > 100 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            Degree = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Degree < 1 || Degree > 5 ) 
                goto usage;
            break;
        case 'l':
            fUseLutLib ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'w':
            fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to a logic network.\n" );
        return 1;
    }

    // modify the current network
    pNtkRes = Abc_NtkPowerdown( pNtk, fUseLutLib, Percentage, Degree, fVerbose, fVeryVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "The command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: powerdown [-P num] [-N num] [-vwh]\n" );
    fprintf( pErr, "\t           transforms LUT-mapped network into an AIG with choices;\n" );
    fprintf( pErr, "\t           the choices are added to power down the next round of mapping\n" );
    fprintf( pErr, "\t-P <num> : switching propability delta defining power critical edges [default = %d%%]\n", Percentage );
    fprintf( pErr, "\t           (e.g. 5% means hot wires switch with probability: 0.45 <= p <= 0.50 (max)\n" );
    fprintf( pErr, "\t-N <num> : the max critical path degree for resynthesis (0 < num < 6) [default = %d]\n", Degree );
//    fprintf( pErr, "\t-l       : toggle using unit- or LUT-library-delay model [default = %s]\n", fUseLutLib? "lib" : "unit" );
    fprintf( pErr, "\t-v       : toggle printing optimization summary [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w       : toggle printing detailed stats for each node [default = %s]\n", fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : print the command usage\n");
    return 1;
} 

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandMerge( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    Nwk_LMPars_t Pars, * pPars = &Pars;
    Vec_Int_t * vResult;
    int c;
    extern Vec_Int_t * Abc_NtkLutMerge( Abc_Ntk_t * pNtk, Nwk_LMPars_t * pPars );
    pNtk = Abc_FrameReadNtk(pAbc);

    // set defaults
    memset( pPars, 0, sizeof(Nwk_LMPars_t) );
    pPars->nMaxLutSize    = 5;   // the max LUT size for merging (N=5)
    pPars->nMaxSuppSize   = 5;   // the max total support size after merging (S=5)
    pPars->nMaxDistance   = 3;   // the max number of nodes separating LUTs
    pPars->nMaxLevelDiff  = 2;   // the max difference in levels
    pPars->nMaxFanout     = 100; // the max number of fanouts to traverse
    pPars->fUseDiffSupp   = 0;   // enables the use of nodes with different support
    pPars->fUseTfiTfo     = 0;   // enables the use of TFO/TFO nodes as candidates
    pPars->fVeryVerbose   = 0;   // enables additional verbose output
    pPars->fVerbose       = 1;   // enables verbose output
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NSDLFscvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxLutSize < 2 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxSuppSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxSuppSize < 2 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxDistance = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxDistance < 2 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxLevelDiff = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxLevelDiff < 2 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxFanout = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxFanout < 2 ) 
                goto usage;
            break;
        case 's':
            pPars->fUseDiffSupp ^= 1;
            break;
        case 'c':
            pPars->fUseTfiTfo ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL || !Abc_NtkIsLogic(pNtk) )
    {
        printf( "Abc_CommandSpeedup(): There is no mapped network to merge LUTs.\n" );
        return 1;
    }
       
    vResult = Abc_NtkLutMerge( pNtk, pPars );
    Vec_IntFree( vResult );
    return 0;

usage:
    fprintf( stdout, "usage: merge [-NSDLF num] [-scwvh]\n" );
    fprintf( stdout, "\t           creates pairs of topologically-related LUTs\n" );
    fprintf( stdout, "\t-N <num> : the max LUT size for merging (1 < num) [default = %d]\n", pPars->nMaxLutSize );
    fprintf( stdout, "\t-S <num> : the max total support size after merging (1 < num) [default = %d]\n", pPars->nMaxSuppSize );
    fprintf( stdout, "\t-D <num> : the max distance in terms of LUTs (0 < num) [default = %d]\n", pPars->nMaxDistance );
    fprintf( stdout, "\t-L <num> : the max difference in levels (0 <= num) [default = %d]\n", pPars->nMaxLevelDiff );
    fprintf( stdout, "\t-F <num> : the max number of fanouts to stop traversal (0 < num) [default = %d]\n", pPars->nMaxFanout );
    fprintf( stdout, "\t-s       : toggle the use of nodes without support overlap [default = %s]\n", pPars->fUseDiffSupp? "yes" : "no" );
    fprintf( stdout, "\t-c       : toggle the use of TFI/TFO nodes as candidates [default = %s]\n", pPars->fUseTfiTfo? "yes" : "no" );
    fprintf( stdout, "\t-w       : toggle printing detailed stats for each node [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( stdout, "\t-v       : toggle printing optimization summary [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h       : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRewrite( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    bool fUpdateLevel;
    bool fPrecompute;
    bool fUseZeros;
    bool fVerbose;
    bool fVeryVerbose;
    bool fPlaceEnable;
    // external functions
    extern void Rwr_Precompute();

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUpdateLevel = 1;
    fPrecompute  = 0;
    fUseZeros    = 0;
    fVerbose     = 0;
    fVeryVerbose = 0;
    fPlaceEnable = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lxzvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'x':
            fPrecompute ^= 1;
            break;
        case 'z':
            fUseZeros ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'w':
            fVeryVerbose ^= 1;
            break;
        case 'p':
            fPlaceEnable ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( fPrecompute )
    {
        Rwr_Precompute();
        return 0;
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to an AIG (run \"strash\").\n" );
        return 1;
    }
    if ( Abc_NtkGetChoiceNum(pNtk) )
    {
        fprintf( pErr, "AIG resynthesis cannot be applied to AIGs with choice nodes.\n" );
        return 1;
    }

    // modify the current network
    if ( !Abc_NtkRewrite( pNtk, fUpdateLevel, fUseZeros, fVerbose, fVeryVerbose, fPlaceEnable ) )
    {
        fprintf( pErr, "Rewriting has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: rewrite [-lzvwh]\n" );
    fprintf( pErr, "\t         performs technology-independent rewriting of the AIG\n" );
    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-z     : toggle using zero-cost replacements [default = %s]\n", fUseZeros? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w     : toggle printout subgraph statistics [default = %s]\n", fVeryVerbose? "yes": "no" );
//    fprintf( pErr, "\t-p     : toggle placement-aware rewriting [default = %s]\n", fPlaceEnable? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
} 

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRefactor( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nNodeSizeMax;
    int nConeSizeMax;
    bool fUpdateLevel;
    bool fUseZeros;
    bool fUseDcs;
    bool fVerbose;
    extern int Abc_NtkRefactor( Abc_Ntk_t * pNtk, int nNodeSizeMax, int nConeSizeMax, bool fUpdateLevel, bool fUseZeros, bool fUseDcs, bool fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nNodeSizeMax = 10;
    nConeSizeMax = 16;
    fUpdateLevel =  1;
    fUseZeros    =  0;
    fUseDcs      =  0;
    fVerbose     =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NClzdvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nNodeSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nNodeSizeMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConeSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConeSizeMax < 0 ) 
                goto usage;
            break;
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'z':
            fUseZeros ^= 1;
            break;
        case 'd':
            fUseDcs ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to an AIG (run \"strash\").\n" );
        return 1;
    }
    if ( Abc_NtkGetChoiceNum(pNtk) )
    {
        fprintf( pErr, "AIG resynthesis cannot be applied to AIGs with choice nodes.\n" );
        return 1;
    }

    if ( fUseDcs && nNodeSizeMax >= nConeSizeMax )
    {
        fprintf( pErr, "For don't-care to work, containing cone should be larger than collapsed node.\n" );
        return 1;
    }

    // modify the current network
    if ( !Abc_NtkRefactor( pNtk, nNodeSizeMax, nConeSizeMax, fUpdateLevel, fUseZeros, fUseDcs, fVerbose ) )
    {
        fprintf( pErr, "Refactoring has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: refactor [-N num] [-C num] [-lzdvh]\n" );
    fprintf( pErr, "\t         performs technology-independent refactoring of the AIG\n" );
    fprintf( pErr, "\t-N num : the max support of the collapsed node [default = %d]\n", nNodeSizeMax );  
    fprintf( pErr, "\t-C num : the max support of the containing cone [default = %d]\n", nConeSizeMax );  
    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-z     : toggle using zero-cost replacements [default = %s]\n", fUseZeros? "yes": "no" );
    fprintf( pErr, "\t-d     : toggle using don't-cares [default = %s]\n", fUseDcs? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRestructure( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nCutsMax;
    bool fUpdateLevel;
    bool fUseZeros;
    bool fVerbose;
    extern int Abc_NtkRestructure( Abc_Ntk_t * pNtk, int nCutsMax, bool fUpdateLevel, bool fUseZeros, bool fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nCutsMax      =  5;
    fUpdateLevel =  0;
    fUseZeros    =  0;
    fVerbose     =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Klzvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCutsMax < 0 ) 
                goto usage;
            break;
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'z':
            fUseZeros ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( nCutsMax < 4 || nCutsMax > CUT_SIZE_MAX )
    {
        fprintf( pErr, "Can only compute the cuts for %d <= K <= %d.\n", 4, CUT_SIZE_MAX );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to an AIG (run \"strash\").\n" );
        return 1;
    }
    if ( Abc_NtkGetChoiceNum(pNtk) )
    {
        fprintf( pErr, "AIG resynthesis cannot be applied to AIGs with choice nodes.\n" );
        return 1;
    }

    // modify the current network
    if ( !Abc_NtkRestructure( pNtk, nCutsMax, fUpdateLevel, fUseZeros, fVerbose ) )
    {
        fprintf( pErr, "Refactoring has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: restructure [-K num] [-lzvh]\n" );
    fprintf( pErr, "\t         performs technology-independent restructuring of the AIG\n" );
    fprintf( pErr, "\t-K num : the max cut size (%d <= num <= %d) [default = %d]\n",   CUT_SIZE_MIN, CUT_SIZE_MAX, nCutsMax );  
    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-z     : toggle using zero-cost replacements [default = %s]\n", fUseZeros? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandResubstitute( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int RS_CUT_MIN =  4;
    int RS_CUT_MAX = 16;
    int c;
    int nCutsMax;
    int nNodesMax;
    int nLevelsOdc;
    bool fUpdateLevel;
    bool fUseZeros;
    bool fVerbose;
    bool fVeryVerbose;
    extern int Abc_NtkResubstitute( Abc_Ntk_t * pNtk, int nCutsMax, int nNodesMax, int nLevelsOdc, bool fUpdateLevel, bool fVerbose, bool fVeryVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nCutsMax     =  8;
    nNodesMax    =  1;
    nLevelsOdc   =  0;
    fUpdateLevel =  1;
    fUseZeros    =  0;
    fVerbose     =  0;
    fVeryVerbose =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "KNFlzvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCutsMax < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nNodesMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nNodesMax < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nLevelsOdc = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLevelsOdc < 0 ) 
                goto usage;
            break;
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'z':
            fUseZeros ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'w':
            fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( nCutsMax < RS_CUT_MIN || nCutsMax > RS_CUT_MAX )
    {
        fprintf( pErr, "Can only compute cuts for %d <= K <= %d.\n", RS_CUT_MIN, RS_CUT_MAX );
        return 1;
    }
    if ( nNodesMax < 0 || nNodesMax > 3 )
    {
        fprintf( pErr, "Can only resubstitute at most 3 nodes.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to an AIG (run \"strash\").\n" );
        return 1;
    }
    if ( Abc_NtkGetChoiceNum(pNtk) )
    {
        fprintf( pErr, "AIG resynthesis cannot be applied to AIGs with choice nodes.\n" );
        return 1;
    }

    // modify the current network
    if ( !Abc_NtkResubstitute( pNtk, nCutsMax, nNodesMax, nLevelsOdc, fUpdateLevel, fVerbose, fVeryVerbose ) )
    {
        fprintf( pErr, "Refactoring has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: resub [-K num] [-N num] [-F num] [-lzvwh]\n" );
    fprintf( pErr, "\t         performs technology-independent restructuring of the AIG\n" );
    fprintf( pErr, "\t-K num : the max cut size (%d <= num <= %d) [default = %d]\n", RS_CUT_MIN, RS_CUT_MAX, nCutsMax );  
    fprintf( pErr, "\t-N num : the max number of nodes to add (0 <= num <= 3) [default = %d]\n", nNodesMax );  
    fprintf( pErr, "\t-F num : the number of fanout levels for ODC computation [default = %d]\n", nLevelsOdc );  
    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-z     : toggle using zero-cost replacements [default = %s]\n", fUseZeros? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w     : toggle verbose printout of ODC computation [default = %s]\n", fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRr( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c, Window;
    int nFaninLevels;
    int nFanoutLevels;
    int fUseFanouts;
    int fVerbose;
    extern int Abc_NtkRR( Abc_Ntk_t * pNtk, int nFaninLevels, int nFanoutLevels, int fUseFanouts, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFaninLevels  = 3;
    nFanoutLevels = 3;
    fUseFanouts   = 0;
    fVerbose      = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Wfvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            Window = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Window < 0 ) 
                goto usage;
            nFaninLevels  = Window / 10;
            nFanoutLevels = Window % 10;
            break;
        case 'f':
            fUseFanouts ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command can only be applied to an AIG (run \"strash\").\n" );
        return 1;
    }
    if ( Abc_NtkGetChoiceNum(pNtk) )
    {
        fprintf( pErr, "AIG resynthesis cannot be applied to AIGs with choice nodes.\n" );
        return 1;
    }

    // modify the current network
    if ( !Abc_NtkRR( pNtk, nFaninLevels, nFanoutLevels, fUseFanouts, fVerbose ) )
    {
        fprintf( pErr, "Redundancy removal has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: rr [-W NM] [-fvh]\n" );
    fprintf( pErr, "\t         removes combinational redundancies in the current network\n" );
    fprintf( pErr, "\t-W NM  : window size: TFI (N) and TFO (M) logic levels [default = %d%d]\n", nFaninLevels, nFanoutLevels );
    fprintf( pErr, "\t-f     : toggle RR w.r.t. fanouts [default = %s]\n", fUseFanouts? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCascade( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, nLutSize;
    int fCheck;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkCascade( Abc_Ntk_t * pNtk, int nLutSize, int fCheck, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nLutSize = 12;
    fCheck   = 0;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Kcvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLutSize < 0 ) 
                goto usage;
            break;
        case 'c':
            fCheck ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsLogic(pNtk) && !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Can only collapse a logic network or an AIG.\n" );
        return 1;
    }

    // get the new network
    if ( Abc_NtkIsStrash(pNtk) )
        pNtkRes = Abc_NtkCascade( pNtk, nLutSize, fCheck, fVerbose );
    else
    {
        pNtk = Abc_NtkStrash( pNtk, 0, 0, 0 );
        pNtkRes = Abc_NtkCascade( pNtk, nLutSize, fCheck, fVerbose );
        Abc_NtkDelete( pNtk );
    }
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Cascade synthesis has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: cascade [-K <num>] [-cvh]\n" );
    fprintf( pErr, "\t         performs LUT cascade synthesis for the current network\n" );
    fprintf( pErr, "\t-K num : the number of LUT inputs [default = %d]\n", nLutSize );
    fprintf( pErr, "\t-c     : check equivalence after synthesis [default = %s]\n", fCheck? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\t         \n");
    fprintf( pErr, "  A lookup-table cascade is a programmable architecture developed by\n");
    fprintf( pErr, "  Professor Tsutomu Sasao (sasao@cse.kyutech.ac.jp) at Kyushu Institute\n");
    fprintf( pErr, "  of Technology. This work received Takeda Techno-Entrepreneurship Award:\n");
    fprintf( pErr, "  http://www.lsi-cad.com/sasao/photo/takeda.html\n");
    fprintf( pErr, "\t         \n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandLogic( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash( pNtk ) )
    {
        fprintf( pErr, "This command is only applicable to strashed networks.\n" );
        return 1;
    }

    // get the new network
    pNtkRes = Abc_NtkToLogic( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Converting to a logic network has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: logic [-h]\n" );
    fprintf( pErr, "\t        transforms an AIG into a logic network with SOPs\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandComb( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fRemoveLatches;
    int nLatchesToAdd;
    extern void Abc_NtkMakeSeq( Abc_Ntk_t * pNtk, int nLatchesToAdd );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fRemoveLatches = 0;
    nLatchesToAdd = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Llh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            } 
            nLatchesToAdd = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLatchesToAdd < 0 ) 
                goto usage;
            break;
        case 'l':
            fRemoveLatches ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsComb(pNtk) && nLatchesToAdd == 0 )
    {
        fprintf( pErr, "The network is already combinational.\n" );
        return 0;
    }
    if ( !Abc_NtkIsComb(pNtk) && nLatchesToAdd != 0 )
    {
        fprintf( pErr, "The network is already combinational.\n" );
        return 0;
    }

    // get the new network
    pNtkRes = Abc_NtkDup( pNtk );
    if ( nLatchesToAdd ) 
        Abc_NtkMakeSeq( pNtkRes, nLatchesToAdd );
    else
        Abc_NtkMakeComb( pNtkRes, fRemoveLatches );
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: comb [-L num] [-lh]\n" );
    fprintf( pErr, "\t        converts comb network into seq, and vice versa\n" );
    fprintf( pErr, "\t-L    : number of latches to add to comb network (0 = do not add) [default = %d]\n", nLatchesToAdd );
    fprintf( pErr, "\t-l    : toggle converting latches to PIs/POs or removing them [default = %s]\n", fRemoveLatches? "remove": "convert" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandMiter( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char Buffer[32];
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk1, * pNtk2, * pNtkRes;
    int fDelete1, fDelete2;
    char ** pArgvNew;
    int nArgcNew;
    int c;
    int fCheck;
    int fComb;
    int fImplic;
    int fMulti;
    int nPartSize;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fComb  = 1;
    fCheck = 1;
    fImplic = 0;
    fMulti = 0;
    nPartSize = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Pcmih" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPartSize < 0 ) 
                goto usage;
            break;
        case 'c':
            fComb ^= 1;
            break;
        case 'm':
            fMulti ^= 1;
            break;
        case 'i':
            fImplic ^= 1;
            break;
        default:
            goto usage;
        }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
        return 1;
    // compute the miter
    pNtkRes = Abc_NtkMiter( pNtk1, pNtk2, fComb, nPartSize, fImplic, fMulti );
    if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
    if ( fDelete2 ) Abc_NtkDelete( pNtk2 );

    // get the new network
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Miter computation has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    if ( nPartSize == 0 )
        strcpy( Buffer, "unused" );
    else
        sprintf( Buffer, "%d", nPartSize );
    fprintf( pErr, "usage: miter [-P num] [-cimh] <file1> <file2>\n" );
    fprintf( pErr, "\t         computes the miter of the two circuits\n" );
    fprintf( pErr, "\t-P num : output partition size [default = %s]\n", Buffer );
    fprintf( pErr, "\t-c     : toggles deriving combinational miter (latches as POs) [default = %s]\n", fComb? "yes": "no" );
    fprintf( pErr, "\t-i     : toggles deriving implication miter (file1 => file2) [default = %s]\n", fImplic? "yes": "no" );
    fprintf( pErr, "\t-m     : toggles creating multi-output miter [default = %s]\n", fMulti? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tfile1  : (optional) the file with the first network\n");
    fprintf( pErr, "\tfile2  : (optional) the file with the second network\n");
    fprintf( pErr, "\t         if no files are given, uses the current network and its spec\n");
    fprintf( pErr, "\t         if one file is given, uses the current network and the file\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDemiter( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;//, * pNtkRes;
    int fSeq;
    int c;
    extern int Abc_NtkDemiter( Abc_Ntk_t * pNtk );
    extern int Abc_NtkDarDemiter( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fSeq = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "sh" ) ) != EOF )
    {
        switch ( c )
        {
        case 's':
            fSeq ^= 1;
            break;
        default:
            goto usage;
        }
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "The network is not strashed.\n" );
        return 1;
    }

    // get the new network
    if ( fSeq )
    {
        if ( !Abc_NtkDarDemiter( pNtk ) )
        {
            fprintf( pErr, "Demitering has failed.\n" );
            return 1;
        }
    }
    else
    {
        if ( Abc_NtkPoNum(pNtk) != 1 )
        {
            fprintf( pErr, "The network is not a single-output miter.\n" );
            return 1;
        }
        if ( !Abc_NodeIsExorType(Abc_ObjFanin0(Abc_NtkPo(pNtk,0))) )
        {
            fprintf( pErr, "The miter's PO is not an EXOR.\n" );
            return 1;
        }
        if ( !Abc_NtkDemiter( pNtk ) )
        {
            fprintf( pErr, "Demitering has failed.\n" );
            return 1;
        }
    }
    // replace the current network
//    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: demiter [-sh]\n" );
    fprintf( pErr, "\t        removes topmost EXOR from the miter to create two POs\n" );
    fprintf( pErr, "\t-s    : applied multi-output algorithm [default = %s]\n", fSeq? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandOrPos( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;//, * pNtkRes;
    int fComb;
    int c;
    extern int Abc_NtkCombinePos( Abc_Ntk_t * pNtk, int fAnd );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
        case 'c':
            fComb ^= 1;
            break;
        default:
            goto usage;
        }
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "The network is not strashed.\n" );
        return 1;
    }
/*
    if ( Abc_NtkPoNum(pNtk) == 1 )
    {
        fprintf( pErr, "The network already has one PO.\n" );
        return 1;
    }
*/
/*
    if ( Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The miter has latches. ORing is not performed.\n" );
        return 1;
    }
*/
    // get the new network
    if ( !Abc_NtkCombinePos( pNtk, 0 ) )
    {
        fprintf( pErr, "ORing the POs has failed.\n" );
        return 1;
    }
    // replace the current network
//    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: orpos [-h]\n" );
    fprintf( pErr, "\t        creates single-output miter by ORing the POs of the current network\n" );
//    fprintf( pErr, "\t-c    : computes combinational miter (latches as POs) [default = %s]\n", fComb? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAndPos( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;//, * pNtkRes;
    int fComb;
    int c;
    extern int Abc_NtkCombinePos( Abc_Ntk_t * pNtk, int fAnd );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
        case 'c':
            fComb ^= 1;
            break;
        default:
            goto usage;
        }
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "The network is not strashed.\n" );
        return 1;
    }

    if ( Abc_NtkPoNum(pNtk) == 1 )
    {
        fprintf( pErr, "The network already has one PO.\n" );
        return 1;
    }

    if ( Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The miter has latches. ORing is not performed.\n" );
        return 1;
    }

    // get the new network
    if ( !Abc_NtkCombinePos( pNtk, 1 ) )
    {
        fprintf( pErr, "ANDing the POs has failed.\n" );
        return 1;
    }
    // replace the current network
//    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: andpos [-h]\n" );
    fprintf( pErr, "\t        creates single-output miter by ANDing the POs of the current network\n" );
//    fprintf( pErr, "\t-c    : computes combinational miter (latches as POs) [default = %s]\n", fComb? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAppend( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk2;
    char * FileName;
    int fComb;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
        case 'c':
            fComb ^= 1;
            break;
        default:
            goto usage;
        }
    }

    // get the second network
    if ( argc != globalUtilOptind + 1 )
    {
        fprintf( pErr, "The network to append is not given.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "The base network should be strashed for the appending to work.\n" );
        return 1;
    }

    // read the second network
    FileName = argv[globalUtilOptind];
    pNtk2 = Io_Read( FileName, Io_ReadFileType(FileName), 1 );
    if ( pNtk2 == NULL )
        return 1;

    // check if the second network is combinational
    if ( Abc_NtkLatchNum(pNtk2) )
    {
        fprintf( pErr, "The second network has latches. Appending does not work for such networks.\n" );
        return 0;
    }

    // get the new network
    if ( !Abc_NtkAppend( pNtk, pNtk2, 1 ) )
    {
        Abc_NtkDelete( pNtk2 );
        fprintf( pErr, "Appending the networks failed.\n" );
        return 1;
    }
    Abc_NtkDelete( pNtk2 );
    // sweep dangling logic
    Abc_AigCleanup( pNtk->pManFunc );
    // replace the current network
//    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: append [-h] <file>\n" );
    fprintf( pErr, "\t         appends a combinational network on top of the current network\n" );
//    fprintf( pErr, "\t-c     : computes combinational miter (latches as POs) [default = %s]\n", fComb? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\t<file> : file name with the second network\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFrames( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkTemp, * pNtkRes;
    int nFrames;
    int fInitial;
    int fVerbose;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFrames  = 5;
    fInitial = 0;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Fivh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames <= 0 ) 
                goto usage;
            break;
        case 'i':
            fInitial ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // get the new network
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        pNtkTemp = Abc_NtkStrash( pNtk, 0, 0, 0 );
        pNtkRes  = Abc_NtkFrames( pNtkTemp, nFrames, fInitial, fVerbose );
        Abc_NtkDelete( pNtkTemp );
    }
    else
        pNtkRes  = Abc_NtkFrames( pNtk, nFrames, fInitial, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Unrolling the network has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: frames [-F num] [-ivh]\n" );
    fprintf( pErr, "\t         unrolls the network for a number of time frames\n" );
    fprintf( pErr, "\t-F num : the number of frames to unroll [default = %d]\n", nFrames );
    fprintf( pErr, "\t-i     : toggles initializing the first frame [default = %s]\n", fInitial? "yes": "no" );  
    fprintf( pErr, "\t-v     : toggles outputting verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDFrames( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkTemp, * pNtkRes;
    int nPrefix;
    int nFrames;
    int fInitial;
    int fVerbose;
    int c;

    extern Abc_Ntk_t * Abc_NtkDarFrames( Abc_Ntk_t * pNtk, int nPrefix, int nFrames, int fInitial, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nPrefix  = 5;
    nFrames  = 5;
    fInitial = 0;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NFivh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nPrefix = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPrefix <= 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames <= 0 ) 
                goto usage;
            break;
        case 'i':
            fInitial ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( nPrefix > nFrames )
    {
        fprintf( pErr, "Prefix (%d) cannot be more than the number of frames (%d).\n", nPrefix, nFrames );
        return 1;
    }

    // get the new network
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        pNtkTemp = Abc_NtkStrash( pNtk, 0, 0, 0 );
        pNtkRes  = Abc_NtkDarFrames( pNtkTemp, nPrefix, nFrames, fInitial, fVerbose );
        Abc_NtkDelete( pNtkTemp );
    }
    else
        pNtkRes  = Abc_NtkDarFrames( pNtk, nPrefix, nFrames, fInitial, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Unrolling the network has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: dframes [-NF num] [-ivh]\n" );
    fprintf( pErr, "\t         unrolls the network with simplification\n" );
    fprintf( pErr, "\t-N num : the number of frames to use as prefix [default = %d]\n", nPrefix );
    fprintf( pErr, "\t-F num : the number of frames to unroll [default = %d]\n", nFrames );
    fprintf( pErr, "\t-i     : toggles initializing the first frame [default = %s]\n", fInitial? "yes": "no" );  
    fprintf( pErr, "\t-v     : toggles outputting verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}



/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSop( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int fDirect;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fDirect = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'd':
            fDirect ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "Converting to SOP is possible only for logic networks.\n" );
        return 1;
    }
    if ( !Abc_NtkToSop(pNtk, fDirect) )
    {
        fprintf( pErr, "Converting to SOP has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: sop [-dh]\n" );
    fprintf( pErr, "\t         converts node functions to SOP\n" );
    fprintf( pErr, "\t-d     : toggles using both phases or only positive [default = %s]\n", fDirect? "direct": "both" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandBdd( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "Converting to BDD is possible only for logic networks.\n" );
        return 1;
    }
    if ( Abc_NtkIsBddLogic(pNtk) )
    {
        fprintf( pOut, "The logic network is already in the BDD form.\n" );
        return 0;
    }
    if ( !Abc_NtkToBdd(pNtk) )
    {
        fprintf( pErr, "Converting to BDD has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: bdd [-h]\n" );
    fprintf( pErr, "\t         converts node functions to BDD\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAig( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "Converting to AIG is possible only for logic networks.\n" );
        return 1;
    }
    if ( Abc_NtkIsAigLogic(pNtk) )
    {
        fprintf( pOut, "The logic network is already in the AIG form.\n" );
        return 0;
    }
    if ( !Abc_NtkToAig(pNtk) )
    {
        fprintf( pErr, "Converting to AIG has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: aig [-h]\n" );
    fprintf( pErr, "\t         converts node functions to AIG\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandReorder( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fVerbose;
    extern void Abc_NtkBddReorder( Abc_Ntk_t * pNtk, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // get the new network
    if ( !Abc_NtkIsBddLogic(pNtk) )
    {
        fprintf( pErr, "Variable reordering is possible when node functions are BDDs (run \"bdd\").\n" );
        return 1;
    }
    Abc_NtkBddReorder( pNtk, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: reorder [-vh]\n" );
    fprintf( pErr, "\t         reorders local functions of the nodes using sifting\n" );
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandBidec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fVerbose;
    extern void Abc_NtkBidecResyn( Abc_Ntk_t * pNtk, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // get the new network
    if ( !Abc_NtkIsAigLogic(pNtk) )
    {
        fprintf( pErr, "Bi-decomposition only works when node functions are AIGs (run \"aig\").\n" );
        return 1;
    }
    Abc_NtkBidecResyn( pNtk, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: bidec [-vh]\n" );
    fprintf( pErr, "\t         applies bi-decomposition to local functions of the nodes\n" );
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandOrder( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr, * pFile;
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int c;
    int fReverse;
    int fVerbose;
    extern void Abc_NtkImplementCiOrder( Abc_Ntk_t * pNtk, char * pFileName, int fReverse, int fVerbose );
    extern void Abc_NtkFindCiOrder( Abc_Ntk_t * pNtk, int fReverse, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fReverse = 0;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "rvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'r':
            fReverse ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
//    if ( Abc_NtkLatchNum(pNtk) > 0 )
//    {
//        printf( "Currently this procedure does not work for sequential networks.\n" );
//        return 1;
//    }

    // if the var order file is given, implement this order
    pFileName = NULL;
    if ( argc == globalUtilOptind + 1 )
    {
        pFileName = argv[globalUtilOptind];
        pFile = fopen( pFileName, "r" );
        if ( pFile == NULL )
        {
            fprintf( pErr, "Cannot open file \"%s\" with the BDD variable order.\n", pFileName );
            return 1;
        }
        fclose( pFile );
    }
    if ( pFileName )
        Abc_NtkImplementCiOrder( pNtk, pFileName, fReverse, fVerbose );
    else
        Abc_NtkFindCiOrder( pNtk, fReverse, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: order [-rvh] <file>\n" );
    fprintf( pErr, "\t         computes a good static CI variable order\n" );
    fprintf( pErr, "\t-r     : toggle reverse ordering [default = %s]\n", fReverse? "yes": "no" );  
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\t<file> : (optional) file with the given variable order\n" );  
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandMuxes( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsBddLogic(pNtk) )
    {
        fprintf( pErr, "Only a BDD logic network can be converted to MUXes.\n" );
        return 1;
    }

    // get the new network
    pNtkRes = Abc_NtkBddToMuxes( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Converting to MUXes has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: muxes [-h]\n" );
    fprintf( pErr, "\t        converts the current network into a network derived by\n" );
    fprintf( pErr, "\t        replacing all nodes by DAGs isomorphic to the local BDDs\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandExtSeqDcs( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fVerbose;
    extern int Abc_NtkExtractSequentialDcs( Abc_Ntk_t * pNet, bool fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkLatchNum(pNtk) == 0 )
    {
        fprintf( stdout, "The current network has no latches.\n" );
        return 0;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Extracting sequential don't-cares works only for AIGs (run \"strash\").\n" );
        return 0;
    }
    if ( !Abc_NtkExtractSequentialDcs( pNtk, fVerbose ) )
    {
        fprintf( stdout, "Extracting sequential don't-cares has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: ext_seq_dcs [-vh]\n" );
    fprintf( pErr, "\t         create EXDC network using unreachable states\n" );
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandReach( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int nBddMax;
    int nIterMax;
    int fPartition;
    int fReorder;
    int fVerbose;
    int c;
    extern void Abc_NtkVerifyUsingBdds( Abc_Ntk_t * pNtk, int nBddMax, int nIterMax, int fPartition, int fReorder, int fVerbose );
    extern void Abc_NtkDarReach( Abc_Ntk_t * pNtk, int nBddMax, int nIterMax, int fPartition, int fReorder, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nBddMax    = 50000;
    nIterMax   = 1000;
    fPartition = 1;
    fReorder   = 1;
    fVerbose   = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "BFprvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'B':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-B\" should be followed by an integer.\n" );
                goto usage;
            }
            nBddMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nBddMax < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nIterMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nIterMax < 0 ) 
                goto usage;
            break;
        case 'p':
            fPartition ^= 1;
            break;
        case 'r':
            fReorder ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkLatchNum(pNtk) == 0 )
    {
        fprintf( stdout, "The current network has no latches.\n" );
        return 0;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Reachability analysis works only for AIGs (run \"strash\").\n" );
        return 1;
    }
/*
    if ( Abc_NtkLatchNum(pNtk) > 60 || Abc_NtkNodeNum(pNtk) > 3000 )
    {
        fprintf( stdout, "The number of latches %d and nodes %d. Skippping...\n", Abc_NtkLatchNum(pNtk), Abc_NtkNodeNum(pNtk) );
        return 0;
    }
*/
/*
    if ( Abc_NtkPoNum(pNtk) != 1 )
    {
        fprintf( stdout, "The sequential miter has more than one output (run \"orpos\").\n" );
        return 1;
    }
*/
//    Abc_NtkVerifyUsingBdds( pNtk, nBddMax, nIterMax, fPartition, fReorder, fVerbose );
    Abc_NtkDarReach( pNtk, nBddMax, nIterMax, fPartition, fReorder, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: reach [-BF num] [-prvh]\n" );
    fprintf( pErr, "\t         verifies sequential miter using BDD-based reachability\n" );
    fprintf( pErr, "\t-B num : max number of nodes in the intermediate BDDs [default = %d]\n", nBddMax );
    fprintf( pErr, "\t-F num : max number of reachability iterations [default = %d]\n", nIterMax );
    fprintf( pErr, "\t-p     : enable partitioned image computation [default = %s]\n", fPartition? "yes": "no" );  
    fprintf( pErr, "\t-r     : enable dynamic BDD variable reordering [default = %s]\n", fReorder? "yes": "no" );  
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCone( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    Abc_Obj_t * pNode, * pNodeCo;
    int c;
    int fUseAllCis;
    int fUseMffc;
    int fSeq;
    int Output;

    extern void Abc_NtkMakeOnePo( Abc_Ntk_t * pNtk, Abc_Obj_t * pNodePo );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseAllCis = 0;
    fUseMffc = 0;
    fSeq = 0;
    Output = -1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Omash" ) ) != EOF )
    {
        switch ( c )
        {
        case 'O':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-O\" should be followed by an integer.\n" );
                goto usage;
            }
            Output = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Output < 0 ) 
                goto usage;
            break;
        case 'm':
            fUseMffc ^= 1;
            break;
        case 'a':
            fUseAllCis ^= 1;
            break;
        case 's':
            fSeq ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsLogic(pNtk) && !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Currently can only be applied to the logic network or an AIG.\n" );
        return 1;
    }

    if ( argc > globalUtilOptind + 1 )
    {
        fprintf( pErr, "Wrong number of auguments.\n" );
        goto usage;
    }

    pNodeCo = NULL;
    if ( argc == globalUtilOptind + 1 )
    {
        pNode = Abc_NtkFindNode( pNtk, argv[globalUtilOptind] );
        if ( pNode == NULL )
        {
            fprintf( pErr, "Cannot find node \"%s\".\n", argv[globalUtilOptind] );
            return 1;
        }
        if ( fUseMffc )
            pNtkRes = Abc_NtkCreateMffc( pNtk, pNode, argv[globalUtilOptind] );
        else
            pNtkRes = Abc_NtkCreateCone( pNtk, pNode, argv[globalUtilOptind], fUseAllCis );
    }
    else
    {
        if ( Output == -1 )
        {
            fprintf( pErr, "The node is not specified.\n" );
            return 1;
        }
        if ( Output >= Abc_NtkCoNum(pNtk) )
        {
            fprintf( pErr, "The 0-based output number (%d) is larger than the number of outputs (%d).\n", Output, Abc_NtkCoNum(pNtk) );
            return 1;
        }
        pNodeCo = Abc_NtkCo( pNtk, Output );
        if ( fSeq )
        {
            pNtkRes = Abc_NtkDup( pNtk );
            pNodeCo = Abc_NtkPo( pNtkRes, Output );
            Abc_NtkMakeOnePo( pNtkRes, pNodeCo );
        }
        else if ( fUseMffc )
            pNtkRes = Abc_NtkCreateMffc( pNtk, Abc_ObjFanin0(pNodeCo), Abc_ObjName(pNodeCo) );
        else 
            pNtkRes = Abc_NtkCreateCone( pNtk, Abc_ObjFanin0(pNodeCo), Abc_ObjName(pNodeCo), fUseAllCis );
    }
    if ( pNodeCo && Abc_ObjFaninC0(pNodeCo) && !fSeq )
        printf( "The extracted cone represents the complement function of the CO.\n" );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Writing the logic cone of one node has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: cone [-O num] [-amsh] <name>\n" );
    fprintf( pErr, "\t         replaces the current network by one logic cone\n" );
    fprintf( pErr, "\t-a     : toggle keeping all CIs or structral support only [default = %s]\n", fUseAllCis? "all": "structural" );
    fprintf( pErr, "\t-m     : toggle keeping only MFFC or complete TFI cone [default = %s]\n", fUseMffc? "MFFC": "TFI cone" );
    fprintf( pErr, "\t-s     : toggle comb or sequential cone (works with \"-O num\") [default = %s]\n", fSeq? "seq": "comb" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\t-O num : (optional) the 0-based number of the CO to extract\n");
    fprintf( pErr, "\tname   : (optional) the name of the node to extract\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandNode( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    Abc_Obj_t * pNode;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
       case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "Currently can only be applied to a logic network.\n" );
        return 1;
    }

    if ( argc != globalUtilOptind + 1 )
    {
        fprintf( pErr, "Wrong number of auguments.\n" );
        goto usage;
    }

    pNode = Abc_NtkFindNode( pNtk, argv[globalUtilOptind] );
    if ( pNode == NULL )
    {
        fprintf( pErr, "Cannot find node \"%s\".\n", argv[globalUtilOptind] );
        return 1;
    }

    pNtkRes = Abc_NtkCreateFromNode( pNtk, pNode );
//    pNtkRes = Abc_NtkDeriveFromBdd( pNtk->pManFunc, pNode->pData, NULL, NULL );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Splitting one node has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: node [-h] <name>\n" );
    fprintf( pErr, "\t         replaces the current network by the network composed of one node\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tname   : the node name\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandTopmost( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, nLevels;
    extern Abc_Ntk_t * Abc_NtkTopmost( Abc_Ntk_t * pNtk, int nLevels );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nLevels = 10;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Nh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nLevels = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLevels < 0 ) 
                goto usage;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }

    if ( Abc_NtkLatchNum(pNtk) > 0 )
    {
        fprintf( stdout, "Currently can only works for combinational circuits.\n" );
        return 0;
    } 
    if ( Abc_NtkPoNum(pNtk) != 1 )
    {
        fprintf( stdout, "Currently expects a single-output miter.\n" );
        return 0;
    } 

    pNtkRes = Abc_NtkTopmost( pNtk, nLevels );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "The command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: topmost [-N num] [-h]\n" );
    fprintf( pErr, "\t         replaces the current network by several of its topmost levels\n" );
    fprintf( pErr, "\t-N num : max number of levels [default = %d]\n", nLevels );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tname   : the node name\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandTopAnd( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    extern Abc_Ntk_t * Abc_NtkTopAnd( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }

    if ( Abc_NtkLatchNum(pNtk) > 0 )
    {
        fprintf( stdout, "Currently can only works for combinational circuits.\n" );
        return 0;
    } 
    if ( Abc_NtkPoNum(pNtk) != 1 )
    {
        fprintf( stdout, "Currently expects a single-output miter.\n" );
        return 0;
    } 
    if ( Abc_ObjFaninC0(Abc_NtkPo(pNtk, 0)) )
    {
        fprintf( stdout, "The PO driver is complemented. AND-decomposition is impossible.\n" );
        return 0;
    } 
    if ( !Abc_ObjIsNode(Abc_ObjChild0(Abc_NtkPo(pNtk, 0))) )
    {
        fprintf( stdout, "The PO driver is not a node. AND-decomposition is impossible.\n" );
        return 0;
    } 
    pNtkRes = Abc_NtkTopAnd( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "The command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: topand [-h]\n" );
    fprintf( pErr, "\t         performs AND-decomposition of single-output combinational miter\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tname   : the node name\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []
 
  SeeAlso     []

***********************************************************************/
int Abc_CommandTrim( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, nLevels;
    extern Abc_Ntk_t * Abc_NtkTrim( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nLevels = 10;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Nh" ) ) != EOF )
    {
        switch ( c )
        {
/* 
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nLevels = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLevels < 0 ) 
                goto usage;
            break;
*/
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for logic circuits.\n" );
        return 0;
    }

    pNtkRes = Abc_NtkTrim( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "The command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: trim [-h]\n" );
    fprintf( pErr, "\t         removes POs fed by PIs and constants, and PIs w/o fanout\n" );
//    fprintf( pErr, "\t-N num : max number of levels [default = %d]\n", nLevels );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandShortNames( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    Abc_NtkShortNames( pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: short_names [-h]\n" );
    fprintf( pErr, "\t         replaces PI/PO/latch names by short char strings\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandExdcFree( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( pNtk->pExdc == NULL )
    {
        fprintf( pErr, "The network has no EXDC.\n" );
        return 1;
    }

    Abc_NtkDelete( pNtk->pExdc );
    pNtk->pExdc = NULL;

    // replace the current network
    pNtkRes = Abc_NtkDup( pNtk );
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: exdc_free [-h]\n" );
    fprintf( pErr, "\t         frees the EXDC network of the current network\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandExdcGet( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( pNtk->pExdc == NULL )
    {
        fprintf( pErr, "The network has no EXDC.\n" );
        return 1;
    }

    // replace the current network
    pNtkRes = Abc_NtkDup( pNtk->pExdc );
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: exdc_get [-h]\n" );
    fprintf( pErr, "\t         replaces the current network by the EXDC of the current network\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandExdcSet( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr, * pFile;
    Abc_Ntk_t * pNtk, * pNtkNew, * pNtkRes;
    char * FileName;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[globalUtilOptind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( (FileName = Extra_FileGetSimilarName( FileName, ".mv", ".blif", ".pla", ".eqn", ".bench" )) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );

    // set the new network
    pNtkNew = Io_Read( FileName, Io_ReadFileType(FileName), 1 );
    if ( pNtkNew == NULL )
    {
        fprintf( pAbc->Err, "Reading network from file has failed.\n" );
        return 1;
    }

    // replace the EXDC
    if ( pNtk->pExdc )
    {
        Abc_NtkDelete( pNtk->pExdc );
        pNtk->pExdc = NULL;
    }
    pNtkRes = Abc_NtkDup( pNtk );
    pNtkRes->pExdc = pNtkNew;

    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: exdc_set [-h] <file>\n" );
    fprintf( pErr, "\t         sets the network from file as EXDC for the current network\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\t<file> : file with the new EXDC network\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCareSet( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr, * pFile;
    Abc_Ntk_t * pNtk, * pNtkNew, * pNtkRes;
    char * FileName;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[globalUtilOptind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( (FileName = Extra_FileGetSimilarName( FileName, ".mv", ".blif", ".pla", ".eqn", ".bench" )) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );

    // set the new network
    pNtkNew = Io_Read( FileName, Io_ReadFileType(FileName), 1 );
    if ( pNtkNew == NULL )
    {
        fprintf( pAbc->Err, "Reading network from file has failed.\n" );
        return 1;
    }

    // replace the EXDC
    if ( pNtk->pExcare )
    {
        Abc_NtkDelete( pNtk->pExcare );
        pNtk->pExcare = NULL;
    }
    pNtkRes = Abc_NtkDup( pNtk );
    pNtkRes->pExcare = pNtkNew;

    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: care_set [-h] <file>\n" );
    fprintf( pErr, "\t         sets the network from file as a care for the current network\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\t<file> : file with the new care network\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCut( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cut_Params_t Params, * pParams = &Params;
    Cut_Man_t * pCutMan;
    Cut_Oracle_t * pCutOracle = NULL;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fOracle;
    extern Cut_Man_t * Abc_NtkCuts( Abc_Ntk_t * pNtk, Cut_Params_t * pParams );
    extern void Abc_NtkCutsOracle( Abc_Ntk_t * pNtk, Cut_Oracle_t * pCutOracle );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fOracle = 0;
    memset( pParams, 0, sizeof(Cut_Params_t) );
    pParams->nVarsMax  = 5;     // the max cut size ("k" of the k-feasible cuts)
    pParams->nKeepMax  = 1000;  // the max number of cuts kept at a node
    pParams->fTruth    = 1;     // compute truth tables
    pParams->fFilter   = 1;     // filter dominated cuts
    pParams->fDrop     = 0;     // drop cuts on the fly
    pParams->fDag      = 1;     // compute DAG cuts
    pParams->fTree     = 0;     // compute tree cuts
    pParams->fGlobal   = 0;     // compute global cuts
    pParams->fLocal    = 0;     // compute local cuts
    pParams->fFancy    = 0;     // compute something fancy
    pParams->fRecordAig= 1;     // compute something fancy
    pParams->fMap      = 0;     // compute mapping delay
    pParams->fAdjust   = 1;     // removes useless fanouts
    pParams->fVerbose  = 0;     // the verbosiness flag
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "KMtfdxyglzamjvoh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nVarsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nVarsMax < 0 ) 
                goto usage;
            break;
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-M\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nKeepMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nKeepMax < 0 ) 
                goto usage;
            break;
        case 't':
            pParams->fTruth ^= 1;
            break;
        case 'f':
            pParams->fFilter ^= 1;
            break;
        case 'd':
            pParams->fDrop ^= 1;
            break;
        case 'x':
            pParams->fDag ^= 1;
            break;
        case 'y':
            pParams->fTree ^= 1;
            break;
        case 'g':
            pParams->fGlobal ^= 1;
            break;
        case 'l':
            pParams->fLocal ^= 1;
            break;
        case 'z':
            pParams->fFancy ^= 1;
            break;
        case 'a':
            pParams->fRecordAig ^= 1;
            break;
        case 'm':
            pParams->fMap ^= 1;
            break;
        case 'j':
            pParams->fAdjust ^= 1;
            break;
        case 'v':
            pParams->fVerbose ^= 1;
            break;
        case 'o':
            fOracle ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Cut computation is available only for AIGs (run \"strash\").\n" );
        return 1;
    }
    if ( pParams->nVarsMax < CUT_SIZE_MIN || pParams->nVarsMax > CUT_SIZE_MAX )
    {
        fprintf( pErr, "Can only compute the cuts for %d <= K <= %d.\n", CUT_SIZE_MIN, CUT_SIZE_MAX );
        return 1;
    }
    if ( pParams->fDag && pParams->fTree )
    {
        fprintf( pErr, "Cannot compute both DAG cuts and tree cuts at the same time.\n" );
        return 1;
    }

    if ( fOracle )
        pParams->fRecord = 1;
    pCutMan = Abc_NtkCuts( pNtk, pParams );
    if ( fOracle )
        pCutOracle = Cut_OracleStart( pCutMan );
    Cut_ManStop( pCutMan );
    if ( fOracle )
    {
        assert(pCutOracle);
        Abc_NtkCutsOracle( pNtk, pCutOracle );
        Cut_OracleStop( pCutOracle );
    }
    return 0;

usage:
    fprintf( pErr, "usage: cut [-K num] [-M num] [-tfdcovamjvh]\n" );
    fprintf( pErr, "\t         computes k-feasible cuts for the AIG\n" );
    fprintf( pErr, "\t-K num : max number of leaves (%d <= num <= %d) [default = %d]\n",   CUT_SIZE_MIN, CUT_SIZE_MAX, pParams->nVarsMax );
    fprintf( pErr, "\t-M num : max number of cuts stored at a node [default = %d]\n",      pParams->nKeepMax );
    fprintf( pErr, "\t-t     : toggle truth table computation [default = %s]\n",           pParams->fTruth?   "yes": "no" );
    fprintf( pErr, "\t-f     : toggle filtering of duplicated/dominated [default = %s]\n", pParams->fFilter?  "yes": "no" );
    fprintf( pErr, "\t-d     : toggle dropping when fanouts are done [default = %s]\n",    pParams->fDrop?    "yes": "no" );
    fprintf( pErr, "\t-x     : toggle computing only DAG cuts [default = %s]\n",           pParams->fDag?     "yes": "no" );
    fprintf( pErr, "\t-y     : toggle computing only tree cuts [default = %s]\n",          pParams->fTree?    "yes": "no" );
    fprintf( pErr, "\t-g     : toggle computing only global cuts [default = %s]\n",        pParams->fGlobal?  "yes": "no" );
    fprintf( pErr, "\t-l     : toggle computing only local cuts [default = %s]\n",         pParams->fLocal?   "yes": "no" );
    fprintf( pErr, "\t-z     : toggle fancy computations [default = %s]\n",                pParams->fFancy?   "yes": "no" );
    fprintf( pErr, "\t-a     : toggle recording cut functions [default = %s]\n",           pParams->fRecordAig?"yes": "no" );
    fprintf( pErr, "\t-m     : toggle delay-oriented FPGA mapping [default = %s]\n",       pParams->fMap?     "yes": "no" );
    fprintf( pErr, "\t-j     : toggle removing fanouts due to XOR/MUX [default = %s]\n",   pParams->fAdjust?  "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n",      pParams->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandScut( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cut_Params_t Params, * pParams = &Params;
    Cut_Man_t * pCutMan;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    extern Cut_Man_t * Abc_NtkSeqCuts( Abc_Ntk_t * pNtk, Cut_Params_t * pParams );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    memset( pParams, 0, sizeof(Cut_Params_t) );
    pParams->nVarsMax  = 5;     // the max cut size ("k" of the k-feasible cuts)
    pParams->nKeepMax  = 1000;  // the max number of cuts kept at a node
    pParams->fTruth    = 0;     // compute truth tables
    pParams->fFilter   = 1;     // filter dominated cuts
    pParams->fSeq      = 1;     // compute sequential cuts
    pParams->fVerbose  = 0;     // the verbosiness flag
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "KMtvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nVarsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nVarsMax < 0 ) 
                goto usage;
            break;
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-M\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nKeepMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nKeepMax < 0 ) 
                goto usage;
            break;
        case 't':
            pParams->fTruth ^= 1;
            break;
        case 'v':
            pParams->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
/*
    if ( !Abc_NtkIsSeq(pNtk) )
    {
        fprintf( pErr, "Sequential cuts can be computed for sequential AIGs (run \"seq\").\n" );
        return 1;
    }
*/
    if ( pParams->nVarsMax < CUT_SIZE_MIN || pParams->nVarsMax > CUT_SIZE_MAX )
    {
        fprintf( pErr, "Can only compute the cuts for %d <= K <= %d.\n", CUT_SIZE_MIN, CUT_SIZE_MAX );
        return 1;
    }

    pCutMan = Abc_NtkSeqCuts( pNtk, pParams );
    Cut_ManStop( pCutMan );
    return 0;

usage:
    fprintf( pErr, "usage: scut [-K num] [-M num] [-tvh]\n" );
    fprintf( pErr, "\t         computes k-feasible cuts for the sequential AIG\n" );
    fprintf( pErr, "\t-K num : max number of leaves (%d <= num <= %d) [default = %d]\n",   CUT_SIZE_MIN, CUT_SIZE_MAX, pParams->nVarsMax );
    fprintf( pErr, "\t-M num : max number of cuts stored at a node [default = %d]\n",      pParams->nKeepMax );
    fprintf( pErr, "\t-t     : toggle truth table computation [default = %s]\n",           pParams->fTruth?   "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n",      pParams->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandEspresso( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fVerbose;
    extern void Abc_NtkEspresso( Abc_Ntk_t * pNtk, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    printf( "This command is currently disabled.\n" );
    return 0;

    // set defaults
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "SOP minimization is possible for logic networks (run \"renode\").\n" );
        return 1;
    }
//    Abc_NtkEspresso( pNtk, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: espresso [-vh]\n" );
    fprintf( pErr, "\t         minimizes SOPs of the local functions using Espresso\n" );
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandGen( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nVars;
    int fAdder;
    int fSorter;
    int fMesh;
    int fFpga;
    int fOneHot;
    int fRandom;
    int fVerbose;
    char * FileName;
    extern void Abc_GenAdder( char * pFileName, int nVars );
    extern void Abc_GenSorter( char * pFileName, int nVars );
    extern void Abc_GenMesh( char * pFileName, int nVars );
    extern void Abc_GenFpga( char * pFileName, int nLutSize, int nLuts, int nVars );
    extern void Abc_GenOneHot( char * pFileName, int nVars );
    extern void Abc_GenRandom( char * pFileName, int nPis );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nVars = 8;
    fAdder = 0;
    fSorter = 0;
    fMesh = 0;
    fFpga = 0;
    fOneHot = 0;
    fRandom = 0;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Nasmftrvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nVars = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nVars < 0 ) 
                goto usage;
            break;
        case 'a':
            fAdder ^= 1;
            break;
        case 's':
            fSorter ^= 1;
            break;
        case 'm':
            fMesh ^= 1;
            break;
        case 'f':
            fFpga ^= 1;
            break;
        case 't':
            fOneHot ^= 1;
            break;
        case 'r':
            fRandom ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[globalUtilOptind];
    if ( fAdder )
        Abc_GenAdder( FileName, nVars );
    else if ( fSorter )
        Abc_GenSorter( FileName, nVars );
    else if ( fMesh )
        Abc_GenMesh( FileName, nVars );
    else if ( fFpga )
        Abc_GenFpga( FileName, 4, 3, 10 );
//        Abc_GenFpga( FileName, 2, 2, 3 );
//        Abc_GenFpga( FileName, 3, 2, 5 );
    else if ( fOneHot )
        Abc_GenOneHot( FileName, nVars );
    else if ( fRandom )
        Abc_GenRandom( FileName, nVars );
    else
        printf( "Type of circuit is not specified.\n" );
    return 0;

usage:
    fprintf( pErr, "usage: gen [-N num] [-asmftrvh] <file>\n" );
    fprintf( pErr, "\t         generates simple circuits\n" );
    fprintf( pErr, "\t-N num : the number of variables [default = %d]\n", nVars );  
    fprintf( pErr, "\t-a     : generate ripple-carry adder [default = %s]\n", fAdder? "yes": "no" );  
    fprintf( pErr, "\t-s     : generate a sorter [default = %s]\n", fSorter? "yes": "no" );  
    fprintf( pErr, "\t-m     : generate a mesh [default = %s]\n", fMesh? "yes": "no" );  
    fprintf( pErr, "\t-f     : generate a LUT FPGA structure [default = %s]\n", fFpga? "yes": "no" );  
    fprintf( pErr, "\t-t     : generate one-hotness conditions [default = %s]\n", fOneHot? "yes": "no" );  
    fprintf( pErr, "\t-r     : generate random single-output function [default = %s]\n", fRandom? "yes": "no" );  
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\t<file> : output file name\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCover( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fVerbose;
    int fUseSop;
    int fUseEsop;
    int fUseInvs;
    int nFaninMax;
    extern Abc_Ntk_t * Abc_NtkSopEsopCover( Abc_Ntk_t * pNtk, int nFaninMax, bool fUseEsop, bool fUseSop, bool fUseInvs, bool fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseSop   =  1;
    fUseEsop  =  0;
    fVerbose  =  0;
    fUseInvs  =  1;
    nFaninMax =  8;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Nsxivh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nFaninMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFaninMax < 0 ) 
                goto usage;
            break;
        case 's':
            fUseSop ^= 1;
            break;
        case 'x':
            fUseEsop ^= 1;
            break;
        case 'i':
            fUseInvs ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Only works for strashed networks.\n" );
        return 1;
    }

    // run the command
    pNtkRes = Abc_NtkSopEsopCover( pNtk, nFaninMax, fUseEsop, fUseSop, fUseInvs, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: cover [-N num] [-sxvh]\n" );
    fprintf( pErr, "\t         decomposition into a network of SOP/ESOP PLAs\n" );
    fprintf( pErr, "\t-N num : maximum number of inputs [default = %d]\n", nFaninMax );
    fprintf( pErr, "\t-s     : toggle the use of SOPs [default = %s]\n", fUseSop? "yes": "no" );
    fprintf( pErr, "\t-x     : toggle the use of ESOPs [default = %s]\n", fUseEsop? "yes": "no" );
//    fprintf( pErr, "\t-i     : toggle the use of interters [default = %s]\n", fUseInvs? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandInter( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk1, * pNtk2, * pNtkRes = NULL;
    char ** pArgvNew;
    int nArgcNew;
    int c, fDelete1, fDelete2;
    int fRelation;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkInter( Abc_Ntk_t * pNtkOn, Abc_Ntk_t * pNtkOff, int fRelation, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fRelation = 0;
    fVerbose  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "rvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'r':
            fRelation ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
        return 1;
    if ( nArgcNew == 0 )
    {
        Abc_Obj_t * pObj;
        int i;
        printf( "Deriving new circuit structure for the current network.\n" );
        Abc_NtkForEachPo( pNtk2, pObj, i )
            Abc_ObjXorFaninC( pObj, 0 );
    }
    if ( fRelation && Abc_NtkCoNum(pNtk1) != 1 )
    {
        printf( "Computation of interplants as a relation only works for single-output functions.\n" );
        printf( "Use command \"cone\" to extract one output cone from the multi-output network.\n" );
    }
    else
        pNtkRes = Abc_NtkInter( pNtk1, pNtk2, fRelation, fVerbose );
    if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
    if ( fDelete2 ) Abc_NtkDelete( pNtk2 );

    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: inter [-rvh] <onset.blif> <offset.blif>\n" );
    fprintf( pErr, "\t         derives interpolant of two networks representing onset and offset;\n" );
    fprintf( pErr, "\t-r     : toggle computing interpolant as a relation [default = %s]\n", fRelation? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\t         \n" );
    fprintf( pErr, "\t         Comments:\n" );
    fprintf( pErr, "\t         \n" );
    fprintf( pErr, "\t         The networks given on the command line should have the same CIs/COs.\n" );
    fprintf( pErr, "\t         If only one network is given on the command line, this network\n" );
    fprintf( pErr, "\t         is assumed to be the offset, while the current network is the onset.\n" );
    fprintf( pErr, "\t         If no network is given on the command line, the current network is\n" );
    fprintf( pErr, "\t         assumed to be the onset and its complement is taken to be the offset.\n" );
    fprintf( pErr, "\t         The resulting interpolant is stored as the current network.\n" );
    fprintf( pErr, "\t         To verify that the interpolant agrees with the onset and the offset,\n" );
    fprintf( pErr, "\t         save it in file \"inter.blif\" and run the following:\n" );
    fprintf( pErr, "\t         (a) \"miter -i <onset.blif> <inter.blif>; iprove\"\n" );
    fprintf( pErr, "\t         (b) \"miter -i <inter.blif> <offset_inv.blif>; iprove\"\n" );
    fprintf( pErr, "\t         where <offset_inv.blif> is the network derived by complementing the\n" );
    fprintf( pErr, "\t         outputs of <offset.blif>: \"r <onset.blif>; st -i; w <offset_inv.blif>\"\n" ); 
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDouble( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int nFrames;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkDouble( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFrames    = 50;
    fVerbose   =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsSopLogic(pNtk) )
    {
        fprintf( pErr, "Only works for logic SOP networks.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkDouble( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: double [-vh]\n" );
    fprintf( pErr, "\t         puts together two parallel copies of the current network\n" );
//    fprintf( pErr, "\t-F num : the number of frames to simulate [default = %d]\n", nFrames );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandTest( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Abc_Ntk_t * pNtkRes = NULL;
    int c;
    int fBmc;
    int nFrames;
    int nLevels;
    int fVerbose;
    int fVeryVerbose;
//    char * pFileName;

//    extern Abc_Ntk_t * Abc_NtkNewAig( Abc_Ntk_t * pNtk );
//    extern Abc_Ntk_t * Abc_NtkIvy( Abc_Ntk_t * pNtk );
//    extern void Abc_NtkMaxFlowTest( Abc_Ntk_t * pNtk );
//    extern int Pr_ManProofTest( char * pFileName );
    extern void Abc_NtkCompareSupports( Abc_Ntk_t * pNtk );
    extern void Abc_NtkCompareCones( Abc_Ntk_t * pNtk );
    extern Abc_Ntk_t * Abc_NtkDar( Abc_Ntk_t * pNtk );
    extern Abc_Ntk_t * Abc_NtkDarToCnf( Abc_Ntk_t * pNtk, char * pFileName );
    extern Abc_Ntk_t * Abc_NtkFilter( Abc_Ntk_t * pNtk );
//    extern Abc_Ntk_t * Abc_NtkDarRetime( Abc_Ntk_t * pNtk, int nStepsMax, int fVerbose );
//    extern Abc_Ntk_t * Abc_NtkPcmTest( Abc_Ntk_t * pNtk, int fVerbose );
    extern Abc_Ntk_t * Abc_NtkDarHaigRecord( Abc_Ntk_t * pNtk, int nIters, int nSteps, int fRetimingOnly, int fAddBugs, int fUseCnf, int fVerbose );
//    extern void Abc_NtkDarTestBlif( char * pFileName );
//    extern Abc_Ntk_t * Abc_NtkDarPartition( Abc_Ntk_t * pNtk );
//    extern Abc_Ntk_t * Abc_NtkTestExor( Abc_Ntk_t * pNtk, int fVerbose );
//    extern Abc_Ntk_t * Abc_NtkNtkTest( Abc_Ntk_t * pNtk, If_Lib_t * pLutLib );
    extern Abc_Ntk_t * Abc_NtkDarRetimeStep( Abc_Ntk_t * pNtk, int fVerbose );
    extern void Abc_NtkDarTest( Abc_Ntk_t * pNtk );
//    extern void Aig_ProcedureTest();
    extern void Abc_NtkDarTest( Abc_Ntk_t * pNtk );
    extern Abc_Ntk_t * Abc_NtkDarTestNtk( Abc_Ntk_t * pNtk );
    extern void Amap_LibertyTest( char * pFileName );
    extern void Bbl_ManTest( Abc_Ntk_t * pNtk );
    extern void Bbl_ManSimpleDemo();
    extern Abc_Ntk_t * Abc_NtkCRetime( Abc_Ntk_t * pNtk, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

//    printf( "This command is temporarily disabled.\n" );
//    return 0;

    // set defaults
    fVeryVerbose = 0;
    fVerbose = 1;
    fBmc     = 0;
    nFrames  = 1;
    nLevels  = 200;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FNbvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nLevels = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLevels < 0 ) 
                goto usage;
            break;
        case 'b':
            fBmc ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'w':
            fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
/*
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsSeq(pNtk) )
    {
        fprintf( pErr, "Only works for non-sequential networks.\n" );
        return 1;
    }
*/

//    Abc_NtkTestEsop( pNtk );
//    Abc_NtkTestSop( pNtk );
//    printf( "This command is currently not used.\n" );
    // run the command
//    pNtkRes = Abc_NtkMiterForCofactors( pNtk, 0, 0, -1 );
//    pNtkRes = Abc_NtkNewAig( pNtk );

/*
    pNtkRes = NULL;
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
*/

//    if ( Cut_CellIsRunning() )
//        Cut_CellDumpToFile();
//    else
//        Cut_CellPrecompute();
//        Cut_CellLoad();
/*
        {
            Abc_Ntk_t * pNtkRes;
            extern Abc_Ntk_t * Abc_NtkTopmost( Abc_Ntk_t * pNtk, int nLevels );
            pNtkRes = Abc_NtkTopmost( pNtk, nLevels );
            Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
        }
*/
//    Abc_NtkSimulteBuggyMiter( pNtk );

//    Rwr_Temp();
//    Abc_MvExperiment();
//    Ivy_TruthTest();


//    Ivy_TruthEstimateNodesTest();
/*
    pNtkRes = Abc_NtkIvy( pNtk );
//    pNtkRes = Abc_NtkPlayer( pNtk, nLevels, 0 );
//    pNtkRes = NULL;
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
*/
//    Abc_NtkMaxFlowTest( pNtk );
//    Pr_ManProofTest( "trace.cnf" );

//    Abc_NtkCompareSupports( pNtk );
//    Abc_NtkCompareCones( pNtk );
/*
    {
        extern Vec_Vec_t * Abc_NtkPartitionSmart( Abc_Ntk_t * pNtk, int fVerbose );
        Vec_Vec_t * vParts;
        vParts = Abc_NtkPartitionSmart( pNtk, 1 );
        Vec_VecFree( vParts );
    }
*/
//    Abc_Ntk4VarTable( pNtk );
//    Dar_NtkGenerateArrays( pNtk );
//    Dar_ManDeriveCnfTest2();
/*
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Network should be strashed. Command has failed.\n" );
        return 1;
    }
*/
/*
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }
*/
/*
    if ( Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for logic circuits.\n" );
        return 0;
    }
*/


/*
//    pNtkRes = Abc_NtkDar( pNtk );
//    pNtkRes = Abc_NtkDarRetime( pNtk, nLevels, 1 ); 
//    pNtkRes = Abc_NtkPcmTestAig( pNtk, fVerbose );
    pNtkRes = Abc_NtkPcmTest( pNtk, fVerbose );
//    pNtkRes = NULL;
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;
*/


//    Abc_NtkDarClau( pNtk, nFrames, nLevels, fBmc, fVerbose, fVeryVerbose );
/*
    if ( globalUtilOptind != 1 )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 1;
    }
    Abc_NtkDarTestBlif( argv[globalUtilOptind] );
*/

//    Abc_NtkDarPartition( pNtk );
//Abc_NtkDarTest( pNtk );

/*
//    pNtkRes = Abc_NtkDarRetimeStep( pNtk, 0 );
    pNtkRes = Abc_NtkDarHaigRecord( pNtk, 3, 3000, 0, 0, 0, 0 );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
*/

/*
    pNtkRes = Abc_NtkDarTestNtk( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
*/

//Amap_LibertyTest( "bwrc.lib" );

//    Abc_NtkDarTest( pNtk );

//    Bbl_ManTest( pNtk );
/*
    {
    extern Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );
    extern void Aig_ManFactorAlgebraicTest( Aig_Man_t * p );
    Aig_Man_t * pAig;
    pAig = Abc_NtkToDar( pNtk, 0, 0 );
    Aig_ManFactorAlgebraicTest( pAig );
    Aig_ManStop( pAig );
    }
*/
//    Bbl_ManSimpleDemo();
//    pNtkRes = Abc_NtkCRetime( pNtk );
    pNtkRes = NULL;
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;
usage:
    fprintf( pErr, "usage: test [-h] <file_name>\n" );
    fprintf( pErr, "\t         testbench for new procedures\n" );
//    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
//    fprintf( pErr, "\t-w     : toggle printing very verbose information [default = %s]\n", fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandQuaVar( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, iVar, fUniv, fVerbose, RetValue;
    extern int Abc_NtkQuantify( Abc_Ntk_t * pNtk, int fUniv, int iVar, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    iVar = 0;
    fUniv = 0;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Iuvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            iVar = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( iVar < 0 ) 
                goto usage;
            break;
        case 'u':
            fUniv ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkGetChoiceNum( pNtk ) )
    {
        fprintf( pErr, "This command cannot be applied to an AIG with choice nodes.\n" );
        return 1;
    }

    // get the strashed network
    pNtkRes = Abc_NtkStrash( pNtk, 0, 1, 0 );
    RetValue = Abc_NtkQuantify( pNtkRes, fUniv, iVar, fVerbose );
    // clean temporary storage for the cofactors
    Abc_NtkCleanData( pNtkRes );
    Abc_AigCleanup( pNtkRes->pManFunc );
    // check the result 
    if ( !RetValue )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: qvar [-I num] [-uvh]\n" );
    fprintf( pErr, "\t         quantifies one variable using the AIG\n" );
    fprintf( pErr, "\t-I num : the zero-based index of a variable to quantify [default = %d]\n", iVar );
    fprintf( pErr, "\t-u     : toggle universal quantification [default = %s]\n", fUniv? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandQuaRel( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, iVar, fInputs, fVerbose;
    extern Abc_Ntk_t * Abc_NtkTransRel( Abc_Ntk_t * pNtk, int fInputs, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    iVar = 0;
    fInputs = 1;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Iqvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            iVar = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( iVar < 0 ) 
                goto usage;
            break;
        case 'q':
            fInputs ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkGetChoiceNum( pNtk ) )
    {
        fprintf( pErr, "This command cannot be applied to an AIG with choice nodes.\n" );
        return 1;
    }
    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "This command works only for sequential circuits.\n" );
        return 1;
    }

    // get the strashed network
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        pNtk = Abc_NtkStrash( pNtk, 0, 1, 0 );
        pNtkRes = Abc_NtkTransRel( pNtk, fInputs, fVerbose );
        Abc_NtkDelete( pNtk );
    }
    else
        pNtkRes = Abc_NtkTransRel( pNtk, fInputs, fVerbose );
    // check if the result is available
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: qrel [-qvh]\n" );
    fprintf( pErr, "\t         computes transition relation of the sequential network\n" );
//    fprintf( pErr, "\t-I num : the zero-based index of a variable to quantify [default = %d]\n", iVar );
    fprintf( pErr, "\t-q     : perform quantification of inputs [default = %s]\n", fInputs? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandQuaReach( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, nIters, fVerbose;
    extern Abc_Ntk_t * Abc_NtkReachability( Abc_Ntk_t * pNtk, int nIters, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nIters   = 256;
    fVerbose =   0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Ivh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            nIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nIters < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkGetChoiceNum( pNtk ) )
    {
        fprintf( pErr, "This command cannot be applied to an AIG with choice nodes.\n" );
        return 1;
    }
    if ( !Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "This command works only for combinational transition relations.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }
    if ( Abc_NtkPoNum(pNtk) > 1 )
    {
        fprintf( pErr, "The transition relation should have one output.\n" );
        return 1;
    }
    if ( Abc_NtkPiNum(pNtk) % 2 != 0 )
    {
        fprintf( pErr, "The transition relation should have an even number of inputs.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkReachability( pNtk, nIters, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: qreach [-I num] [-vh]\n" );
    fprintf( pErr, "\t         computes unreachable states using AIG-based quantification\n" );
    fprintf( pErr, "\t         assumes that the current network is a transition relation\n" );
    fprintf( pErr, "\t         assumes that the initial state is composed of all zeros\n" );
    fprintf( pErr, "\t-I num : the number of image computations to perform [default = %d]\n", nIters );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSenseInput( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Vec_Int_t * vResult;
    int c, nConfLim, fVerbose;

    extern Vec_Int_t * Abc_NtkSensitivity( Abc_Ntk_t * pNtk, int nConfLim, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nConfLim   = 1000;
    fVerbose   =    1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Cvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLim = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLim < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkGetChoiceNum( pNtk ) )
    {
        fprintf( pErr, "This command cannot be applied to an AIG with choice nodes.\n" );
        return 1;
    }
    if ( !Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "This command works only for combinational transition relations.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }
    if ( Abc_NtkPoNum(pNtk) < 2 )
    {
        fprintf( pErr, "The network should have at least two outputs.\n" );
        return 1;
    }

    vResult = Abc_NtkSensitivity( pNtk, nConfLim, fVerbose );
    Vec_IntFree( vResult );
    return 0;

usage:
    fprintf( pErr, "usage: senseinput [-C num] [-vh]\n" );
    fprintf( pErr, "\t         computes sensitivity of POs to PIs under constraint\n" );
    fprintf( pErr, "\t         constraint should be represented as the last PO" );
    fprintf( pErr, "\t-C num : the max number of conflicts at a node [default = %d]\n", nConfLim );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandIStrash( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes, * pNtkTemp;
    int c;
    extern Abc_Ntk_t * Abc_NtkIvyStrash( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        pNtkTemp = Abc_NtkStrash( pNtk, 0, 1, 0 );
        pNtkRes = Abc_NtkIvyStrash( pNtkTemp );
        Abc_NtkDelete( pNtkTemp );
    }
    else
        pNtkRes = Abc_NtkIvyStrash( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: istrash [-h]\n" );
    fprintf( pErr, "\t         perform sequential structural hashing\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandICut( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c, nInputs;
    extern void Abc_NtkIvyCuts( Abc_Ntk_t * pNtk, int nInputs );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nInputs = 5;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Kh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            nInputs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nInputs < 0 ) 
                goto usage;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }

    Abc_NtkIvyCuts( pNtk, nInputs );
    return 0;

usage:
    fprintf( pErr, "usage: icut [-K num] [-h]\n" );
    fprintf( pErr, "\t         computes sequential cuts of the given size\n" );
    fprintf( pErr, "\t-K num : the number of cut inputs (2 <= num <= 6) [default = %d]\n", nInputs );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandIRewrite( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, fUpdateLevel, fUseZeroCost, fVerbose;
    extern Abc_Ntk_t * Abc_NtkIvyRewrite( Abc_Ntk_t * pNtk, int fUpdateLevel, int fUseZeroCost, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUpdateLevel = 1;
    fUseZeroCost = 0;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lzvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'z':
            fUseZeroCost ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkIvyRewrite( pNtk, fUpdateLevel, fUseZeroCost, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: irw [-lzvh]\n" );
    fprintf( pErr, "\t         perform combinational AIG rewriting\n" );
    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-z     : toggle using zero-cost replacements [default = %s]\n", fUseZeroCost? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDRewrite( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    Dar_RwrPar_t Pars, * pPars = &Pars;
    int c;

    extern Abc_Ntk_t * Abc_NtkDRewrite( Abc_Ntk_t * pNtk, Dar_RwrPar_t * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Dar_ManDefaultRwrParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CNflzrvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nCutsMax < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nSubgMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nSubgMax < 0 ) 
                goto usage;
            break;
        case 'f':
            pPars->fFanout ^= 1;
            break;
        case 'l':
            pPars->fUpdateLevel ^= 1;
            break;
        case 'z':
            pPars->fUseZeros ^= 1;
            break;
        case 'r':
            pPars->fRecycle ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }
    pNtkRes = Abc_NtkDRewrite( pNtk, pPars );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: drw [-C num] [-N num] [-lfzrvwh]\n" );
    fprintf( pErr, "\t         performs combinational AIG rewriting\n" );
    fprintf( pErr, "\t-C num : the max number of cuts at a node [default = %d]\n", pPars->nCutsMax );
    fprintf( pErr, "\t-N num : the max number of subgraphs tried [default = %d]\n", pPars->nSubgMax );
    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", pPars->fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-f     : toggle representing fanouts [default = %s]\n", pPars->fFanout? "yes": "no" );
    fprintf( pErr, "\t-z     : toggle using zero-cost replacements [default = %s]\n", pPars->fUseZeros? "yes": "no" );
    fprintf( pErr, "\t-r     : toggle using cut recycling [default = %s]\n", pPars->fRecycle? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w     : toggle very verbose printout [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDRefactor( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    Dar_RefPar_t Pars, * pPars = &Pars;
    int c;

    extern Abc_Ntk_t * Abc_NtkDRefactor( Abc_Ntk_t * pNtk, Dar_RefPar_t * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Dar_ManDefaultRefParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "MKCelzvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMffcMin = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMffcMin < 0 ) 
                goto usage;
            break;
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nLeafMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nLeafMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nCutsMax < 0 ) 
                goto usage;
            break;
        case 'e':
            pPars->fExtend ^= 1;
            break;
        case 'l':
            pPars->fUpdateLevel ^= 1;
            break;
        case 'z':
            pPars->fUseZeros ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }
    if ( pPars->nLeafMax < 4 || pPars->nLeafMax > 15 )
    {
        fprintf( pErr, "This command only works for cut sizes 4 <= K <= 15.\n" );
        return 1;
    }
    pNtkRes = Abc_NtkDRefactor( pNtk, pPars );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: drf [-M num] [-K num] [-C num] [-elzvwh]\n" );
    fprintf( pErr, "\t         performs combinational AIG refactoring\n" );
    fprintf( pErr, "\t-M num : the min MFFC size to attempt refactoring [default = %d]\n", pPars->nMffcMin );
    fprintf( pErr, "\t-K num : the max number of cuts leaves [default = %d]\n", pPars->nLeafMax );
    fprintf( pErr, "\t-C num : the max number of cuts to try at a node [default = %d]\n", pPars->nCutsMax );
    fprintf( pErr, "\t-e     : toggle extending tbe cut below MFFC [default = %s]\n", pPars->fExtend? "yes": "no" );
    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", pPars->fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-z     : toggle using zero-cost replacements [default = %s]\n", pPars->fUseZeros? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w     : toggle very verbose printout [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDC2( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int fBalance, fVerbose, fUpdateLevel, fFanout, fPower, c;

    extern Abc_Ntk_t * Abc_NtkDC2( Abc_Ntk_t * pNtk, int fBalance, int fUpdateLevel, int fFanout, int fPower, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fBalance     = 0;
    fVerbose     = 0;
    fUpdateLevel = 0;
    fFanout      = 1;
    fPower       = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "blfpvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'b':
            fBalance ^= 1;
            break;
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'f':
            fFanout ^= 1;
            break;
        case 'p':
            fPower ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }
    pNtkRes = Abc_NtkDC2( pNtk, fBalance, fUpdateLevel, fFanout, fPower, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: dc2 [-blfpvh]\n" );
    fprintf( pErr, "\t         performs combinational AIG optimization\n" );
    fprintf( pErr, "\t-b     : toggle internal balancing [default = %s]\n", fBalance? "yes": "no" );
    fprintf( pErr, "\t-l     : toggle updating level [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-f     : toggle representing fanouts [default = %s]\n", fFanout? "yes": "no" );
    fprintf( pErr, "\t-p     : toggle power-aware rewriting [default = %s]\n", fPower? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDChoice( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int fBalance, fVerbose, fUpdateLevel, fConstruct, c;
    int nConfMax, nLevelMax;

    extern Abc_Ntk_t * Abc_NtkDChoice( Abc_Ntk_t * pNtk, int fBalance, int fUpdateLevel, int fConstruct, int nConfMax, int nLevelMax, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fBalance     = 1;
    fUpdateLevel = 1;
    fConstruct   = 0;
    nConfMax     = 1000;
    nLevelMax    = 0;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CLblcvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfMax < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            nLevelMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLevelMax < 0 ) 
                goto usage;
            break;
        case 'b':
            fBalance ^= 1;
            break;
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'c':
            fConstruct ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }
    pNtkRes = Abc_NtkDChoice( pNtk, fBalance, fUpdateLevel, fConstruct, nConfMax, nLevelMax, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: dchoice [-C num] [-L num] [-blcvh]\n" );
    fprintf( pErr, "\t         performs partitioned choicing using new AIG package\n" );
    fprintf( pErr, "\t-C num : the max number of conflicts at a node [default = %d]\n", nConfMax );
    fprintf( pErr, "\t-L num : the max level of nodes to consider (0 = not used) [default = %d]\n", nLevelMax );
    fprintf( pErr, "\t-b     : toggle internal balancing [default = %s]\n", fBalance? "yes": "no" );
    fprintf( pErr, "\t-l     : toggle updating level [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-c     : toggle constructive computation of choices [default = %s]\n", fConstruct? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDch( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Dch_Pars_t Pars, * pPars = &Pars;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;

    extern Abc_Ntk_t * Abc_NtkDch( Abc_Ntk_t * pNtk, Dch_Pars_t * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Dch_ManSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "WCSsptgcvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWords = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWords < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nSatVarMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nSatVarMax < 0 ) 
                goto usage;
            break;
        case 's':
            pPars->fSynthesis ^= 1;
            break;
        case 'p':
            pPars->fPower ^= 1;
            break;
        case 't':
            pPars->fSimulateTfo ^= 1;
            break;
        case 'g':
            pPars->fUseGia ^= 1;
            break;
        case 'c':
            pPars->fUseCSat ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL ) 
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }
    pNtkRes = Abc_NtkDch( pNtk, pPars );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: dch [-WCS num] [-sptgcvh]\n" );
    fprintf( pErr, "\t         computes structural choices using a new approach\n" );
    fprintf( pErr, "\t-W num : the max number of simulation words [default = %d]\n", pPars->nWords );
    fprintf( pErr, "\t-C num : the max number of conflicts at a node [default = %d]\n", pPars->nBTLimit );
    fprintf( pErr, "\t-S num : the max number of SAT variables [default = %d]\n", pPars->nSatVarMax );
    fprintf( pErr, "\t-s     : toggle synthesizing three snapshots [default = %s]\n", pPars->fSynthesis? "yes": "no" );
    fprintf( pErr, "\t-p     : toggle power-aware rewriting [default = %s]\n", pPars->fPower? "yes": "no" );
    fprintf( pErr, "\t-t     : toggle simulation of the TFO classes [default = %s]\n", pPars->fSimulateTfo? "yes": "no" );
    fprintf( pErr, "\t-g     : toggle using GIA to prove equivalences [default = %s]\n", pPars->fUseGia? "yes": "no" );
    fprintf( pErr, "\t-c     : toggle using circuit-based SAT vs. MiniSat [default = %s]\n", pPars->fUseCSat? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDrwsat( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int fBalance, fVerbose, c;

    extern Abc_Ntk_t * Abc_NtkDrwsat( Abc_Ntk_t * pNtk, int fBalance, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fBalance = 0;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "bvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'b':
            fBalance ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }
    pNtkRes = Abc_NtkDrwsat( pNtk, fBalance, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: drwsat [-bvh]\n" );
    fprintf( pErr, "\t         performs combinational AIG optimization for SAT\n" );
    fprintf( pErr, "\t-b     : toggle internal balancing [default = %s]\n", fBalance? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandIRewriteSeq( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, fUpdateLevel, fUseZeroCost, fVerbose;
    extern Abc_Ntk_t * Abc_NtkIvyRewriteSeq( Abc_Ntk_t * pNtk, int fUseZeroCost, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUpdateLevel = 0;
    fUseZeroCost = 0;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lzvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'z':
            fUseZeroCost ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkIvyRewriteSeq( pNtk, fUseZeroCost, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: irws [-zvh]\n" );
    fprintf( pErr, "\t         perform sequential AIG rewriting\n" );
//    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-z     : toggle using zero-cost replacements [default = %s]\n", fUseZeroCost? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandIResyn( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, fUpdateLevel, fVerbose;
    extern Abc_Ntk_t * Abc_NtkIvyResyn( Abc_Ntk_t * pNtk, int fUpdateLevel, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUpdateLevel = 1;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "lzvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkIvyResyn( pNtk, fUpdateLevel, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: iresyn [-lvh]\n" );
    fprintf( pErr, "\t         performs combinational resynthesis\n" );
    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandISat( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, fUpdateLevel, fVerbose;
    int nConfLimit;

    extern Abc_Ntk_t * Abc_NtkIvySat( Abc_Ntk_t * pNtk, int nConfLimit, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nConfLimit   = 100000;   
    fUpdateLevel = 1;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Clzvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkIvySat( pNtk, nConfLimit, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: isat [-C num] [-vh]\n" );
    fprintf( pErr, "\t         tries to prove the miter constant 0\n" );
    fprintf( pErr, "\t-C num : limit on the number of conflicts [default = %d]\n",    nConfLimit );
//    fprintf( pErr, "\t-l     : toggle preserving the number of levels [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandIFraig( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, fProve, fVerbose, fDoSparse;
    int nConfLimit;
    int nPartSize;
    int nLevelMax;

    extern Abc_Ntk_t * Abc_NtkIvyFraig( Abc_Ntk_t * pNtk, int nConfLimit, int fDoSparse, int fProve, int fTransfer, int fVerbose );
    extern Abc_Ntk_t * Abc_NtkDarFraigPart( Abc_Ntk_t * pNtk, int nPartSize, int nConfLimit, int nLevelMax, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nPartSize    = 0;
    nLevelMax    = 0;
    nConfLimit   = 100;   
    fDoSparse    = 0;
    fProve       = 0;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PCLspvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPartSize < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
         case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            nLevelMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLevelMax < 0 ) 
                goto usage;
            break;
        case 's':
            fDoSparse ^= 1;
            break;
        case 'p':
            fProve ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }

    if ( nPartSize > 0 )
        pNtkRes = Abc_NtkDarFraigPart( pNtk, nPartSize, nConfLimit, nLevelMax, fVerbose );
    else
        pNtkRes = Abc_NtkIvyFraig( pNtk, nConfLimit, fDoSparse, fProve, 0, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: ifraig [-P num] [-C num] [-L num] [-spvh]\n" );
    fprintf( pErr, "\t         performs fraiging using a new method\n" );
    fprintf( pErr, "\t-P num : partition size (0 = partitioning is not used) [default = %d]\n", nPartSize );
    fprintf( pErr, "\t-C num : limit on the number of conflicts [default = %d]\n", nConfLimit );
    fprintf( pErr, "\t-L num : limit on node level to fraig (0 = fraig all nodes) [default = %d]\n", nLevelMax );
    fprintf( pErr, "\t-s     : toggle considering sparse functions [default = %s]\n", fDoSparse? "yes": "no" );
    fprintf( pErr, "\t-p     : toggle proving the miter outputs [default = %s]\n", fProve? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDFraig( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, nConfLimit, fDoSparse, fProve, fSpeculate, fChoicing, fVerbose;

    extern Abc_Ntk_t * Abc_NtkDarFraig( Abc_Ntk_t * pNtk, int nConfLimit, int fDoSparse, int fProve, int fTransfer, int fSpeculate, int fChoicing, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nConfLimit   = 100;  
    fDoSparse    = 1;
    fProve       = 0;
    fSpeculate   = 0;
    fChoicing    = 0;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Csprcvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
        case 's':
            fDoSparse ^= 1;
            break;
        case 'p':
            fProve ^= 1;
            break;
        case 'r':
            fSpeculate ^= 1;
            break;
        case 'c':
            fChoicing ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkDarFraig( pNtk, nConfLimit, fDoSparse, fProve, 0, fSpeculate, fChoicing, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: dfraig [-C num] [-sprcvh]\n" );
    fprintf( pErr, "\t         performs fraiging using a new method\n" );
    fprintf( pErr, "\t-C num : limit on the number of conflicts [default = %d]\n", nConfLimit );
    fprintf( pErr, "\t-s     : toggle considering sparse functions [default = %s]\n", fDoSparse? "yes": "no" );
    fprintf( pErr, "\t-p     : toggle proving the miter outputs [default = %s]\n", fProve? "yes": "no" );
    fprintf( pErr, "\t-r     : toggle speculative reduction [default = %s]\n", fSpeculate? "yes": "no" );
    fprintf( pErr, "\t-c     : toggle accumulation of choices [default = %s]\n", fChoicing? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCSweep( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, nCutsMax, nLeafMax, fVerbose;

    extern Abc_Ntk_t * Abc_NtkCSweep( Abc_Ntk_t * pNtk, int nCutsMax, int nLeafMax, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nCutsMax  =  8;  
    nLeafMax  =  6;
    fVerbose  =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CKvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCutsMax < 0 ) 
                goto usage;
            break;
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            nLeafMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLeafMax < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( nCutsMax < 2 )
    {
        fprintf( pErr, "The number of cuts cannot be less than 2.\n" );
        return 1;
    }

    if ( nLeafMax < 3 || nLeafMax > 16 )
    {
        fprintf( pErr, "The number of leaves is infeasible.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkCSweep( pNtk, nCutsMax, nLeafMax, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: csweep [-C num] [-K num] [-vh]\n" );
    fprintf( pErr, "\t         performs cut sweeping using a new method\n" );
    fprintf( pErr, "\t-C num : limit on the number of cuts (C >= 2) [default = %d]\n", nCutsMax );
    fprintf( pErr, "\t-K num : limit on the cut size (3 <= K <= 16) [default = %d]\n", nLeafMax );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandIProve( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Prove_Params_t Params, * pParams = &Params;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkTemp;
    int c, clk, RetValue, iOut = -1;

    extern int Abc_NtkIvyProve( Abc_Ntk_t ** ppNtk, void * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Prove_ParamsSetDefault( pParams );
    pParams->fUseRewriting = 1;
    pParams->fVerbose      = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "rvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'r':
            pParams->fUseRewriting ^= 1;
            break;
        case 'v':
            pParams->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkLatchNum(pNtk) > 0 )
    {
        fprintf( pErr, "The network has registers. Use \"dprove\".\n" );
        return 1;
    }

    clk = clock();

    if ( Abc_NtkIsStrash(pNtk) )
        pNtkTemp = Abc_NtkDup( pNtk );
    else
        pNtkTemp = Abc_NtkStrash( pNtk, 0, 0, 0 );

    RetValue = Abc_NtkIvyProve( &pNtkTemp, pParams );

    // verify that the pattern is correct
    if ( RetValue == 0 )
    {
        Abc_Obj_t * pObj;
        int i;
        int * pSimInfo = Abc_NtkVerifySimulatePattern( pNtk, pNtkTemp->pModel );
        Abc_NtkForEachCo( pNtk, pObj, i )
            if ( pSimInfo[i] == 1 )
            {
                iOut = i;
                break;
            }
        if ( i == Abc_NtkCoNum(pNtk) )
            printf( "ERROR in Abc_NtkMiterProve(): Generated counter-example is invalid.\n" );
        ABC_FREE( pSimInfo );
    }
 
    if ( RetValue == -1 ) 
        printf( "UNDECIDED      " );
    else if ( RetValue == 0 )
        printf( "SATISFIABLE (output = %d) ", iOut );
    else
        printf( "UNSATISFIABLE  " );
    //printf( "\n" );

    ABC_PRT( "Time", clock() - clk );
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkTemp );
    return 0;

usage:
    fprintf( pErr, "usage: iprove [-rvh]\n" );
    fprintf( pErr, "\t         performs CEC using a new method\n" );
    fprintf( pErr, "\t-r     : toggle AIG rewriting [default = %s]\n", pParams->fUseRewriting? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", pParams->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
/*
int Abc_CommandHaig( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int nIters;
    int nSteps;
    int fRetimingOnly;
    int fAddBugs;
    int fUseCnf;
    int fVerbose;

    extern Abc_Ntk_t * Abc_NtkDarHaigRecord( Abc_Ntk_t * pNtk, int nIters, int nSteps, int fRetimingOnly, int fAddBugs, int fUseCnf, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nIters        = 3;
    nSteps        = 3000;
    fRetimingOnly = 0;
    fAddBugs      = 0;
    fUseCnf       = 0;
    fVerbose      = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ISrbcvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nIters < 0 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-S\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nSteps = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nSteps < 0 ) 
                goto usage;
            break;
        case 'r':
            fRetimingOnly ^= 1;
            break;
        case 'b':
            fAddBugs ^= 1;
            break;
        case 'c':
            fUseCnf ^= 1;
            break;
        case 'v':
            fUseCnf ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for strashed networks.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkDarHaigRecord( pNtk, nIters, nSteps, fRetimingOnly, fAddBugs, fUseCnf, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: haig [-IS num] [-rbcvh]\n" );
    fprintf( pErr, "\t         run a few rounds of comb+seq synthesis to test HAIG recording\n" );
    fprintf( pErr, "\t         the current network is set to be the result of synthesis performed\n" );
    fprintf( pErr, "\t         (this network can be verified using command \"dsec\")\n" );
    fprintf( pErr, "\t         HAIG is written out into the file \"haig.blif\"\n" );
    fprintf( pErr, "\t         (this HAIG can be proved using \"r haig.blif; st; dprove -abc -F 16\")\n" );
    fprintf( pErr, "\t-I num : the number of rounds of comb+seq synthesis [default = %d]\n", nIters );
    fprintf( pErr, "\t-S num : the number of forward retiming moves performed [default = %d]\n", nSteps );
    fprintf( pErr, "\t-r     : toggle the use of retiming only [default = %s]\n", fRetimingOnly? "yes": "no" );
    fprintf( pErr, "\t-b     : toggle bug insertion [default = %s]\n", fAddBugs? "yes": "no" );
    fprintf( pErr, "\t-c     : enable CNF-based proof (no speculative reduction) [default = %s]\n", fUseCnf? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}
*/

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandMini( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    extern Abc_Ntk_t * Abc_NtkMiniBalance( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Only works for combinatinally strashed AIG networks.\n" );
        return 1;
    }

    pNtkRes = Abc_NtkMiniBalance( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Command has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: mini [-h]\n" );
    fprintf( pErr, "\t         perform balancing using new package\n" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandQbf( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nPars;
    int fVerbose;

    extern void Abc_NtkQbf( Abc_Ntk_t * pNtk, int nPars, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nPars    = -1;
    fVerbose =  1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Pvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nPars = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPars < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "Works only for combinational networks.\n" );
        return 1;
    }
    if ( Abc_NtkPoNum(pNtk) != 1 )
    {
        fprintf( pErr, "The miter should have one primary output.\n" );
        return 1;
    }
    if ( !(nPars > 0 && nPars < Abc_NtkPiNum(pNtk)) )
    {
        fprintf( pErr, "The number of paramter variables is invalid (should be > 0 and < PI num).\n" );
        return 1;
    }
    if ( Abc_NtkIsStrash(pNtk) )
        Abc_NtkQbf( pNtk, nPars, fVerbose );
    else
    {
        pNtk = Abc_NtkStrash( pNtk, 0, 1, 0 );
        Abc_NtkQbf( pNtk, nPars, fVerbose );
        Abc_NtkDelete( pNtk );
    }
    return 0;

usage:
    fprintf( pErr, "usage: qbf [-P num] [-vh]\n" );
    fprintf( pErr, "\t         solves a quantified boolean formula problem EpVxM(p,x)\n" );
    fprintf( pErr, "\t-P num : number of paramters (should be the first PIs) [default = %d]\n", nPars );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFraig( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char Buffer[100];
    Fraig_Params_t Params, * pParams = &Params;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int fAllNodes;
    int fExdc;
    int c;
    int fPartition = 0;
    extern void Abc_NtkFraigPartitionedTime( Abc_Ntk_t * pNtk, void * pParams );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fExdc     = 0;
    fAllNodes = 0;
    memset( pParams, 0, sizeof(Fraig_Params_t) );
    pParams->nPatsRand  = 2048; // the number of words of random simulation info
    pParams->nPatsDyna  = 2048; // the number of words of dynamic simulation info
    pParams->nBTLimit   =  100; // the max number of backtracks to perform
    pParams->fFuncRed   =    1; // performs only one level hashing
    pParams->fFeedBack  =    1; // enables solver feedback
    pParams->fDist1Pats =    1; // enables distance-1 patterns
    pParams->fDoSparse  =    1; // performs equiv tests for sparse functions 
    pParams->fChoicing  =    0; // enables recording structural choices
    pParams->fTryProve  =    0; // tries to solve the final miter
    pParams->fVerbose   =    0; // the verbosiness flag
    pParams->fVerboseP  =    0; // the verbosiness flag
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "RDCrscptvaeh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'R':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-R\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nPatsRand = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nPatsRand < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nPatsDyna = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nPatsDyna < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nBTLimit < 0 ) 
                goto usage;
            break;

        case 'r':
            pParams->fFuncRed ^= 1;
            break;
        case 's':
            pParams->fDoSparse ^= 1;
            break;
        case 'c':
            pParams->fChoicing ^= 1;
            break;
        case 'p':
            pParams->fTryProve ^= 1;
            break;
        case 'v':
            pParams->fVerbose ^= 1;
            break;
        case 't':
            fPartition ^= 1;
            break;
        case 'a':
            fAllNodes ^= 1;
            break;
        case 'e':
            fExdc ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    } 

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) && !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Can only fraig a logic network or an AIG.\n" );
        return 1;
    }

    // report the proof
    pParams->fVerboseP = pParams->fTryProve;

    // get the new network
    if ( fPartition )
    {
        pNtkRes = Abc_NtkDup( pNtk );
        if ( Abc_NtkIsStrash(pNtk) )
            Abc_NtkFraigPartitionedTime( pNtk, &Params );
        else
        {
            pNtk = Abc_NtkStrash( pNtk, fAllNodes, !fAllNodes, 0 );
            Abc_NtkFraigPartitionedTime( pNtk, &Params );
            Abc_NtkDelete( pNtk );
        }
    }
    else
    {
        if ( Abc_NtkIsStrash(pNtk) )
            pNtkRes = Abc_NtkFraig( pNtk, &Params, fAllNodes, fExdc );
        else
        {
            pNtk = Abc_NtkStrash( pNtk, fAllNodes, !fAllNodes, 0 );
            pNtkRes = Abc_NtkFraig( pNtk, &Params, fAllNodes, fExdc );
            Abc_NtkDelete( pNtk );
        }
    }
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Fraiging has failed.\n" );
        return 1;
    }

    if ( pParams->fTryProve ) // report the result
        Abc_NtkMiterReport( pNtkRes );

    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    sprintf( Buffer, "%d", pParams->nBTLimit );
    fprintf( pErr, "usage: fraig [-R num] [-D num] [-C num] [-rscpvtah]\n" );
    fprintf( pErr, "\t         transforms a logic network into a functionally reduced AIG\n" );
    fprintf( pErr, "\t         (there are also newer fraiging commands, \"ifraig\" and \"dfraig\")\n" );
    fprintf( pErr, "\t-R num : number of random patterns (127 < num < 32769) [default = %d]\n",     pParams->nPatsRand );
    fprintf( pErr, "\t-D num : number of systematic patterns (127 < num < 32769) [default = %d]\n", pParams->nPatsDyna );
    fprintf( pErr, "\t-C num : number of backtracks for one SAT problem [default = %s]\n",    pParams->nBTLimit==-1? "infinity" : Buffer );
    fprintf( pErr, "\t-r     : toggle functional reduction [default = %s]\n",                 pParams->fFuncRed? "yes": "no" );
    fprintf( pErr, "\t-s     : toggle considering sparse functions [default = %s]\n",         pParams->fDoSparse? "yes": "no" );
    fprintf( pErr, "\t-c     : toggle accumulation of choices [default = %s]\n",              pParams->fChoicing? "yes": "no" );
    fprintf( pErr, "\t-p     : toggle proving the miter outputs [default = %s]\n",              pParams->fTryProve? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n",                       pParams->fVerbose?  "yes": "no" );
    fprintf( pErr, "\t-e     : toggle functional sweeping using EXDC [default = %s]\n",       fExdc? "yes": "no" );
    fprintf( pErr, "\t-a     : toggle between all nodes and DFS nodes [default = %s]\n",      fAllNodes? "all": "dfs" );
    fprintf( pErr, "\t-t     : toggle using partitioned representation [default = %s]\n",     fPartition? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFraigTrust( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fDuplicate;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fDuplicate = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'd':
            fDuplicate ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // get the new network
    pNtkRes = Abc_NtkFraigTrust( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Fraiging in the trust mode has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: fraig_trust [-h]\n" );
    fprintf( pErr, "\t        transforms the current network into an AIG assuming it is FRAIG with choices\n" );
//    fprintf( pErr, "\t-d    : toggle duplication of logic [default = %s]\n", fDuplicate? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFraigStore( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fDuplicate;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fDuplicate = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'd':
            fDuplicate ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // get the new network
    if ( !Abc_NtkFraigStore( pNtk ) )
    {
        fprintf( pErr, "Fraig storing has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: fraig_store [-h]\n" );
    fprintf( pErr, "\t        saves the current network in the AIG database\n" );
//    fprintf( pErr, "\t-d    : toggle duplication of logic [default = %s]\n", fDuplicate? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFraigRestore( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fDuplicate;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fDuplicate = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'd':
            fDuplicate ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // get the new network
    pNtkRes = Abc_NtkFraigRestore();
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Fraig restoring has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: fraig_restore [-h]\n" );
    fprintf( pErr, "\t        makes the current network by fraiging the AIG database\n" );
//    fprintf( pErr, "\t-d    : toggle duplication of logic [default = %s]\n", fDuplicate? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFraigClean( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fDuplicate;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fDuplicate = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'd':
            fDuplicate ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    Abc_NtkFraigStoreClean();
    return 0;

usage:
    fprintf( pErr, "usage: fraig_clean [-h]\n" );
    fprintf( pErr, "\t        cleans the internal FRAIG storage\n" );
//    fprintf( pErr, "\t-d    : toggle duplication of logic [default = %s]\n", fDuplicate? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFraigSweep( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fUseInv;
    int fExdc;
    int fVerbose;
    int fVeryVerbose;
    extern bool Abc_NtkFraigSweep( Abc_Ntk_t * pNtk, int fUseInv, int fExdc, int fVerbose, int fVeryVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fUseInv   = 1;
    fExdc     = 0;
    fVerbose  = 0;
    fVeryVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ievwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'i':
            fUseInv ^= 1;
            break;
        case 'e':
            fExdc ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'w':
            fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Cannot sweep AIGs (use \"fraig\").\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "Transform the current network into a logic network.\n" );
        return 1;
    }
    // modify the current network
    if ( !Abc_NtkFraigSweep( pNtk, fUseInv, fExdc, fVerbose, fVeryVerbose ) )
    {
        fprintf( pErr, "Sweeping has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: fraig_sweep [-evwh]\n" );
    fprintf( pErr, "\t        performs technology-dependent sweep\n" );
    fprintf( pErr, "\t-e    : toggle functional sweeping using EXDC [default = %s]\n", fExdc? "yes": "no" );
    fprintf( pErr, "\t-v    : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-w    : prints equivalence class information [default = %s]\n", fVeryVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFraigDress( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    char * pFileName;
    int c;
    int fVerbose;
    extern void Abc_NtkDress( Abc_Ntk_t * pNtk, char * pFileName, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for logic networks.\n" );
        return 1;
    }
    if ( argc != globalUtilOptind && argc != globalUtilOptind + 1 )
        goto usage;
    if ( argc == globalUtilOptind && Abc_NtkSpec(pNtk) == NULL )
    {
        fprintf( pErr, "The current network has no spec.\n" );
        return 1;
    }
    // get the input file name
    pFileName = (argc == globalUtilOptind + 1) ? argv[globalUtilOptind] : Abc_NtkSpec(pNtk);
    // modify the current network
    Abc_NtkDress( pNtk, pFileName, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: dress [-vh] <file>\n" );
    fprintf( pErr, "\t         transfers internal node names from file to the current network\n" );
    fprintf( pErr, "\t<file> : network with names (if not given, the current network spec is used)\n" );  
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

#if 0

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandHaigStart( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for AIGs; run strashing (\"st\").\n" );
        return 0;
    }
    Abc_NtkHaigStart( pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: haig_start [-h]\n" );
    fprintf( pErr, "\t        starts constructive accumulation of combinational choices\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandHaigStop( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for AIGs; run strashing (\"st\").\n" );
        return 0;
    }
    Abc_NtkHaigStop( pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: haig_stop [-h]\n" );
    fprintf( pErr, "\t        cleans the internal storage for combinational choices\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandHaigUse( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for AIGs; run strashing (\"st\").\n" );
        return 0;
    }
    // get the new network
    pNtkRes = Abc_NtkHaigUse( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Transforming internal storage into AIG with choices has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: haig_use [-h]\n" );
    fprintf( pErr, "\t        transforms internal storage into an AIG with choices\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}



/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRecStart( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nVars;
    int nCuts;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nVars = 4;
    nCuts = 8;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "KCh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            nVars = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nVars < 1 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nCuts = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCuts < 1 ) 
                goto usage;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( !(nVars >= 3 && nVars <= 16) )
    {
        fprintf( pErr, "The range of allowed values is 3 <= K <= 16.\n" );
        return 0;
    }
    if ( Abc_NtkRecIsRunning() )
    {
        fprintf( pErr, "The AIG subgraph recording is already started.\n" );
        return 0;
    }
    if ( pNtk && !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for AIGs; run strashing (\"st\").\n" );
        return 0;
    }
    Abc_NtkRecStart( pNtk, nVars, nCuts );
    return 0;

usage:
    fprintf( pErr, "usage: rec_start [-K num] [-C num] [-h]\n" );
    fprintf( pErr, "\t         starts recording AIG subgraphs (should be called for\n" );
    fprintf( pErr, "\t         an empty network or after reading in a previous record)\n" );
    fprintf( pErr, "\t-K num : the largest number of inputs [default = %d]\n", nVars );
    fprintf( pErr, "\t-C num : the max number of cuts used at a node (0 < num < 2^12) [default = %d]\n", nCuts );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRecStop( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( !Abc_NtkRecIsRunning() )
    {
        fprintf( pErr, "This command works only after calling \"rec_start\".\n" );
        return 0;
    }
    Abc_NtkRecStop();
    return 0;

usage:
    fprintf( pErr, "usage: rec_stop [-h]\n" );
    fprintf( pErr, "\t        cleans the internal storage for AIG subgraphs\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRecAdd( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works for AIGs.\n" );
        return 0;
    }
    if ( !Abc_NtkRecIsRunning() )
    {
        fprintf( pErr, "This command works for AIGs after calling \"rec_start\".\n" );
        return 0;
    }
    Abc_NtkRecAdd( pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: rec_add [-h]\n" );
    fprintf( pErr, "\t        adds subgraphs from the current network to the set\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRecPs( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( !Abc_NtkRecIsRunning() )
    {
        fprintf( pErr, "This command works for AIGs only after calling \"rec_start\".\n" );
        return 0;
    }
    Abc_NtkRecPs();
    return 0;

usage:
    fprintf( pErr, "usage: rec_ps [-h]\n" );
    fprintf( pErr, "\t        prints statistics about the recorded AIG subgraphs\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRecUse( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( !Abc_NtkRecIsRunning() )
    {
        fprintf( pErr, "This command works for AIGs only after calling \"rec_start\".\n" );
        return 0;
    }
    // get the new network
    pNtkRes = Abc_NtkRecUse();
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Transforming internal AIG subgraphs into an AIG with choices has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: rec_use [-h]\n" );
    fprintf( pErr, "\t        transforms internal storage into an AIG with choices\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

#endif

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandMap( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    char Buffer[100];
    double DelayTarget;
    int fAreaOnly;
    int fRecovery;
    int fSweep;
    int fSwitching;
    int fVerbose;
    int c;
    extern Abc_Ntk_t * Abc_NtkMap( Abc_Ntk_t * pNtk, double DelayTarget, int fRecovery, int fSwitching, int fVerbose );
    extern bool Abc_NtkFraigSweep( Abc_Ntk_t * pNtk, int fUseInv, int fExdc, int fVerbose, int fVeryVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    DelayTarget =-1;
    fAreaOnly   = 0;
    fRecovery   = 1;
    fSweep      = 1;
    fSwitching  = 0;
    fVerbose    = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Darspvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by a floating point number.\n" );
                goto usage;
            }
            DelayTarget = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( DelayTarget <= 0.0 ) 
                goto usage;
            break;
        case 'a':
            fAreaOnly ^= 1;
            break;
        case 'r':
            fRecovery ^= 1;
            break;
        case 's':
            fSweep ^= 1;
            break;
        case 'p':
            fSwitching ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( fAreaOnly )
        DelayTarget = 100000.0;

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        pNtk = Abc_NtkStrash( pNtk, 0, 0, 0 );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Strashing before mapping has failed.\n" );
            return 1;
        }
        pNtk = Abc_NtkBalance( pNtkRes = pNtk, 0, 0, 1 );
        Abc_NtkDelete( pNtkRes );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Balancing before mapping has failed.\n" );
            return 1;
        }
        fprintf( pOut, "The network was strashed and balanced before mapping.\n" );
        // get the new network
        pNtkRes = Abc_NtkMap( pNtk, DelayTarget, fRecovery, fSwitching, fVerbose );
        if ( pNtkRes == NULL )
        {
            Abc_NtkDelete( pNtk );
            fprintf( pErr, "Mapping has failed.\n" );
            return 1;
        }
        Abc_NtkDelete( pNtk );
    }
    else
    {
        // get the new network
        pNtkRes = Abc_NtkMap( pNtk, DelayTarget, fRecovery, fSwitching, fVerbose );
        if ( pNtkRes == NULL )
        {
            fprintf( pErr, "Mapping has failed.\n" );
            return 1;
        }
    }

    if ( fSweep )
        Abc_NtkFraigSweep( pNtkRes, 0, 0, 0, 0 );

    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    if ( DelayTarget == -1 ) 
        sprintf( Buffer, "not used" );
    else
        sprintf( Buffer, "%.3f", DelayTarget );
    fprintf( pErr, "usage: map [-D float] [-arspvh]\n" );
    fprintf( pErr, "\t           performs standard cell mapping of the current network\n" );
    fprintf( pErr, "\t-D float : sets the global required times [default = %s]\n", Buffer );  
    fprintf( pErr, "\t-a       : toggles area-only mapping [default = %s]\n", fAreaOnly? "yes": "no" );
    fprintf( pErr, "\t-r       : toggles area recovery [default = %s]\n", fRecovery? "yes": "no" );
    fprintf( pErr, "\t-s       : toggles sweep after mapping [default = %s]\n", fSweep? "yes": "no" );
    fprintf( pErr, "\t-p       : optimizes power by minimizing switching [default = %s]\n", fSwitching? "yes": "no" );
    fprintf( pErr, "\t-v       : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAmap( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Amap_Par_t Pars, * pPars = &Pars;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int fSweep;
    int c;
    extern Abc_Ntk_t * Abc_NtkDarAmap( Abc_Ntk_t * pNtk, Amap_Par_t * pPars );
    extern bool Abc_NtkFraigSweep( Abc_Ntk_t * pNtk, int fUseInv, int fExdc, int fVerbose, int fVeryVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fSweep = 0;
    Amap_ManSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FAEmxisvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by a floating point number.\n" );
                goto usage;
            }
            pPars->nIterFlow = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nIterFlow < 0 ) 
                goto usage;
            break;
        case 'A':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-A\" should be followed by a floating point number.\n" );
                goto usage;
            }
            pPars->nIterArea = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nIterArea < 0 ) 
                goto usage;
            break;
        case 'E':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-E\" should be followed by a floating point number.\n" );
                goto usage;
            }
            pPars->fEpsilon = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->fEpsilon < 0.0 || pPars->fEpsilon > 1.0 ) 
                goto usage;
            break;
        case 'm':
            pPars->fUseMuxes ^= 1;
            break;
        case 'x':
            pPars->fUseXors ^= 1;
            break;
        case 'i':
            pPars->fFreeInvs ^= 1;
            break;
        case 's':
            fSweep ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        pNtk = Abc_NtkStrash( pNtk, 0, 0, 0 );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Strashing before mapping has failed.\n" );
            return 1;
        }
        pNtk = Abc_NtkBalance( pNtkRes = pNtk, 0, 0, 1 );
        Abc_NtkDelete( pNtkRes );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Balancing before mapping has failed.\n" );
            return 1;
        }
        fprintf( pOut, "The network was strashed and balanced before mapping.\n" );
        // get the new network
        pNtkRes = Abc_NtkDarAmap( pNtk, pPars );
        if ( pNtkRes == NULL )
        {
            Abc_NtkDelete( pNtk );
            fprintf( pErr, "Mapping has failed.\n" );
            return 1;
        }
        Abc_NtkDelete( pNtk );
    }
    else
    {
        // get the new network
        pNtkRes = Abc_NtkDarAmap( pNtk, pPars );
        if ( pNtkRes == NULL )
        {
            fprintf( pErr, "Mapping has failed.\n" );
            return 1;
        }
    }

    if ( fSweep )
        Abc_NtkFraigSweep( pNtkRes, 0, 0, 0, 0 );

    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: amap [-FA <num>] [-E <float>] [-mxisvh]\n" );
    fprintf( pErr, "\t           performs standard cell mapping of the current network\n" );
    fprintf( pErr, "\t-F num   : the number of iterations of area flow [default = %d]\n", pPars->nIterFlow );
    fprintf( pErr, "\t-A num   : the number of iterations of exact area [default = %d]\n", pPars->nIterArea );
    fprintf( pErr, "\t-E float : sets epsilon used for tie-breaking [default = %f]\n", pPars->fEpsilon );  
    fprintf( pErr, "\t-m       : toggles using MUX matching [default = %s]\n", pPars->fUseMuxes? "yes": "no" );
    fprintf( pErr, "\t-x       : toggles using XOR matching [default = %s]\n", pPars->fUseXors? "yes": "no" );
    fprintf( pErr, "\t-i       : toggles assuming inverters are free [default = %s]\n", pPars->fFreeInvs? "yes": "no" );
    fprintf( pErr, "\t-s       : toggles sweep after mapping [default = %s]\n", fSweep? "yes": "no" );
    fprintf( pErr, "\t-v       : toggles verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandUnmap( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkHasMapping(pNtk) )
    {
        fprintf( pErr, "Cannot unmap the network that is not mapped.\n" );
        return 1;
    }

    // get the new network
    if ( !Abc_NtkMapToSop( pNtk ) )
    {
        fprintf( pErr, "Unmapping has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: unmap [-h]\n" );
    fprintf( pErr, "\t        replaces the library gates by the logic nodes represented using SOPs\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAttach( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsSopLogic(pNtk) )
    {
        fprintf( pErr, "Can only attach gates if the nodes have SOP representations.\n" );
        return 1;
    }

    // get the new network
    if ( !Abc_NtkAttach( pNtk ) )
    {
        fprintf( pErr, "Attaching gates has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pErr, "usage: attach [-h]\n" );
    fprintf( pErr, "\t        replaces the SOP functions by the gates from the library\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSuperChoice( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    extern Abc_Ntk_t * Abc_NtkSuperChoice( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Superchoicing works only for the AIG representation (run \"strash\").\n" );
        return 1;
    }

    // get the new network
    pNtkRes = Abc_NtkSuperChoice( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Superchoicing has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: sc [-h]\n" );
    fprintf( pErr, "\t      performs superchoicing\n" );
    fprintf( pErr, "\t      (accumulate: \"r file.blif; rsup; b; sc; f -ac; wb file_sc.blif\")\n" );
    fprintf( pErr, "\t      (map without supergate library: \"r file_sc.blif; ft; map\")\n" );
    fprintf( pErr, "\t-h  : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSuperChoiceLut( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int nLutSize;
    int nCutSizeMax;
    int fVerbose;
    extern int Abc_NtkSuperChoiceLut( Abc_Ntk_t * pNtk, int nLutSize, int nCutSizeMax, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose = 1;
    nLutSize = 4;
    nCutSizeMax = 10;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "KNh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLutSize < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nCutSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCutSizeMax < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Superchoicing works only for the AIG representation (run \"strash\").\n" );
        return 1;
    }

    // convert the network into the SOP network
    pNtkRes = Abc_NtkToLogic( pNtk );

    // get the new network
    if ( !Abc_NtkSuperChoiceLut( pNtkRes, nLutSize, nCutSizeMax, fVerbose ) )
    {
        Abc_NtkDelete( pNtkRes );
        fprintf( pErr, "Superchoicing has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: scl [-K num] [-N num] [-vh]\n" );
    fprintf( pErr, "\t        performs superchoicing for K-LUTs\n" );
    fprintf( pErr, "\t        (accumulate: \"r file.blif; b; scl; f -ac; wb file_sc.blif\")\n" );
    fprintf( pErr, "\t        (FPGA map: \"r file_sc.blif; ft; read_lut lutlibK; fpga\")\n" );
    fprintf( pErr, "\t-K num : the number of LUT inputs [default = %d]\n", nLutSize );
    fprintf( pErr, "\t-N num : the max size of the cut [default = %d]\n", nCutSizeMax );
    fprintf( pErr, "\t-v     : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFpga( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char Buffer[100];
    char LutSize[100];
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fRecovery;
    int fSwitching;
    int fLatchPaths;
    int fVerbose;
    int nLutSize;
    float DelayTarget;

    extern Abc_Ntk_t * Abc_NtkFpga( Abc_Ntk_t * pNtk, float DelayTarget, int fRecovery, int fSwitching, int fLatchPaths, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fRecovery   = 1;
    fSwitching  = 0;
    fLatchPaths = 0;
    fVerbose    = 0;
    DelayTarget =-1;
    nLutSize    =-1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "aplvhDK" ) ) != EOF )
    {
        switch ( c )
        {
        case 'a':
            fRecovery ^= 1;
            break;
        case 'p':
            fSwitching ^= 1;
            break;
        case 'l':
            fLatchPaths ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by a floating point number.\n" );
                goto usage;
            }
            DelayTarget = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( DelayTarget <= 0.0 ) 
                goto usage;
            break;
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLutSize < 0 ) 
                goto usage;
            break;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    // create the new LUT library
    if ( nLutSize >= 3 && nLutSize <= 10 )
        Fpga_SetSimpleLutLib( nLutSize );
/*
    else
    {
        fprintf( pErr, "Cannot perform FPGA mapping with LUT size %d.\n", nLutSize );
        return 1;
    }
*/
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        // strash and balance the network
        pNtk = Abc_NtkStrash( pNtk, 0, 0, 0 );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Strashing before FPGA mapping has failed.\n" );
            return 1;
        }
        pNtk = Abc_NtkBalance( pNtkRes = pNtk, 0, 0, 1 );
        Abc_NtkDelete( pNtkRes );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Balancing before FPGA mapping has failed.\n" );
            return 1;
        }
        fprintf( pOut, "The network was strashed and balanced before FPGA mapping.\n" );
        // get the new network
        pNtkRes = Abc_NtkFpga( pNtk, DelayTarget, fRecovery, fSwitching, fLatchPaths, fVerbose );
        if ( pNtkRes == NULL )
        {
            Abc_NtkDelete( pNtk );
            fprintf( pErr, "FPGA mapping has failed.\n" );
            return 1;
        }
        Abc_NtkDelete( pNtk );
    }
    else
    {
        // get the new network
        pNtkRes = Abc_NtkFpga( pNtk, DelayTarget, fRecovery, fSwitching, fLatchPaths, fVerbose );
        if ( pNtkRes == NULL )
        {
            fprintf( pErr, "FPGA mapping has failed.\n" );
            return 1;
        }
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    if ( DelayTarget == -1 ) 
        sprintf( Buffer, "best possible" );
    else
        sprintf( Buffer, "%.2f", DelayTarget );
    if ( nLutSize == -1 ) 
        sprintf( LutSize, "library" );
    else
        sprintf( LutSize, "%d", nLutSize );
    fprintf( pErr, "usage: fpga [-D float] [-K num] [-aplvh]\n" );
    fprintf( pErr, "\t           performs FPGA mapping of the current network\n" );
    fprintf( pErr, "\t-a       : toggles area recovery [default = %s]\n", fRecovery? "yes": "no" );
    fprintf( pErr, "\t-p       : optimizes power by minimizing switching activity [default = %s]\n", fSwitching? "yes": "no" );
    fprintf( pErr, "\t-l       : optimizes latch paths for delay, other paths for area [default = %s]\n", fLatchPaths? "yes": "no" );
    fprintf( pErr, "\t-D float : sets the required time for the mapping [default = %s]\n", Buffer );  
    fprintf( pErr, "\t-K num   : the number of LUT inputs (2 < num < 11) [default = %s]%s\n", LutSize, (nLutSize == -1 ? " (type \"print_lut\")" : "") );
    fprintf( pErr, "\t-v       : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : prints the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFpgaFast( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char Buffer[100];
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fRecovery;
    int fVerbose;
    int nLutSize;
    float DelayTarget;

    extern Abc_Ntk_t * Abc_NtkFpgaFast( Abc_Ntk_t * pNtk, int nLutSize, int fRecovery, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fRecovery   = 1;
    fVerbose    = 0;
    DelayTarget =-1;
    nLutSize    = 5;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "avhDK" ) ) != EOF )
    {
        switch ( c )
        {
        case 'a':
            fRecovery ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by a floating point number.\n" );
                goto usage;
            }
            DelayTarget = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( DelayTarget <= 0.0 ) 
                goto usage;
            break;
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLutSize < 0 ) 
                goto usage;
            break;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        // strash and balance the network
        pNtk = Abc_NtkStrash( pNtk, 0, 0, 0 );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Strashing before FPGA mapping has failed.\n" );
            return 1;
        }
        pNtk = Abc_NtkBalance( pNtkRes = pNtk, 0, 0, 1 );
        Abc_NtkDelete( pNtkRes );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Balancing before FPGA mapping has failed.\n" );
            return 1;
        }
        fprintf( pOut, "The network was strashed and balanced before FPGA mapping.\n" );
        // get the new network
        pNtkRes = Abc_NtkFpgaFast( pNtk, nLutSize, fRecovery, fVerbose );
        if ( pNtkRes == NULL )
        {
            Abc_NtkDelete( pNtk );
            fprintf( pErr, "FPGA mapping has failed.\n" );
            return 1;
        }
        Abc_NtkDelete( pNtk );
    }
    else
    {
        // get the new network
        pNtkRes = Abc_NtkFpgaFast( pNtk, nLutSize, fRecovery, fVerbose );
        if ( pNtkRes == NULL )
        {
            fprintf( pErr, "FPGA mapping has failed.\n" );
            return 1;
        }
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    if ( DelayTarget == -1 ) 
        sprintf( Buffer, "not used" );
    else
        sprintf( Buffer, "%.2f", DelayTarget );
    fprintf( pErr, "usage: ffpga [-K num] [-avh]\n" );
    fprintf( pErr, "\t           performs fast FPGA mapping of the current network\n" );
    fprintf( pErr, "\t-a       : toggles area recovery [default = %s]\n", fRecovery? "yes": "no" );
//    fprintf( pErr, "\t-D float : sets the required time for the mapping [default = %s]\n", Buffer );  
    fprintf( pErr, "\t-K num   : the number of LUT inputs (2 < num < 32) [default = %d]\n", nLutSize );
    fprintf( pErr, "\t-v       : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : prints the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandIf( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char Buffer[100];
    char LutSize[100];
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    If_Par_t Pars, * pPars = &Pars;
    int c;
    extern Abc_Ntk_t * Abc_NtkIf( Abc_Ntk_t * pNtk, If_Par_t * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    memset( pPars, 0, sizeof(If_Par_t) );
    // user-controlable paramters
    pPars->nLutSize    = -1;
    pPars->nCutsMax    =  8;
    pPars->nFlowIters  =  1;
    pPars->nAreaIters  =  2;
    pPars->DelayTarget = -1;
    pPars->Epsilon     =  (float)0.005;
    pPars->fPreprocess =  1; 
    pPars->fArea       =  0;
    pPars->fFancy      =  0;
    pPars->fExpRed     =  1;
    pPars->fLatchPaths =  0;
    pPars->fEdge       =  1;
    pPars->fPower      =  0;
    pPars->fCutMin     =  0;
    pPars->fSeqMap     =  0;
    pPars->fBidec      =  0;
    pPars->fVerbose    =  0;
    // internal parameters
    pPars->fTruth      =  0;
    pPars->nLatches    =  pNtk? Abc_NtkLatchNum(pNtk) : 0;
    pPars->fLiftLeaves =  0;
    pPars->pLutLib     =  Abc_FrameReadLibLut();
    pPars->pTimesArr   =  NULL; 
    pPars->pTimesArr   =  NULL;   
    pPars->pFuncCost   =  NULL;   

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "KCFADEqaflepmrstbvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nLutSize < 0 ) 
                goto usage;
            // if the LUT size is specified, disable library
            pPars->pLutLib = NULL; 
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nCutsMax < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nFlowIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFlowIters < 0 ) 
                goto usage;
            break;
        case 'A':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-A\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nAreaIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nAreaIters < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by a floating point number.\n" );
                goto usage;
            }
            pPars->DelayTarget = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->DelayTarget <= 0.0 ) 
                goto usage;
            break;
        case 'E':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-E\" should be followed by a floating point number.\n" );
                goto usage;
            }
            pPars->Epsilon = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->Epsilon < 0.0 || pPars->Epsilon > 1.0 ) 
                goto usage;
            break;
        case 'q':
            pPars->fPreprocess ^= 1;
            break;
        case 'a':
            pPars->fArea ^= 1;
            break;
        case 'r':
            pPars->fExpRed ^= 1;
            break;
        case 'f':
            pPars->fFancy ^= 1;
            break;
        case 'l':
            pPars->fLatchPaths ^= 1;
            break;
        case 'e':
            pPars->fEdge ^= 1;
            break;
        case 'p':
            pPars->fPower ^= 1;
            break;
        case 'm':
            pPars->fCutMin ^= 1;
            break;
        case 's':
            pPars->fSeqMap ^= 1;
            break;
        case 't':
            pPars->fLiftLeaves ^= 1;
            break;
        case 'b':
            pPars->fBidec ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
/*
    if ( pPars->fSeqMap )
    {
        fprintf( pErr, "Sequential mapping is currently disabled.\n" );
        return 1;
    }
*/

    if ( pPars->fSeqMap && pPars->nLatches == 0 )
    {
        fprintf( pErr, "The network has no latches. Use combinational mapping instead of sequential.\n" );
        return 1;
    }

    if ( pPars->nLutSize == -1 )
    {
        if ( pPars->pLutLib == NULL )
        {
            fprintf( pErr, "The LUT library is not given.\n" );
            return 1;
        }
        // get LUT size from the library
        pPars->nLutSize = pPars->pLutLib->LutMax;
        // if variable pin delay, force truth table computation
//        if ( pPars->pLutLib->fVarPinDelays )
//            pPars->fTruth = 1;
    }

    if ( pPars->nLutSize < 3 || pPars->nLutSize > IF_MAX_LUTSIZE )
    {
        fprintf( pErr, "Incorrect LUT size (%d).\n", pPars->nLutSize );
        return 1;
    }

    if ( pPars->nCutsMax < 1 || pPars->nCutsMax >= (1<<12) )
    {
        fprintf( pErr, "Incorrect number of cuts.\n" );
        return 1;
    }

    // enable truth table computation if choices are selected
    if ( (c = Abc_NtkGetChoiceNum( pNtk )) )
    {
        printf( "Performing LUT mapping with %d choices.\n", c );
//        pPars->fTruth = 1;
        pPars->fExpRed = 0;
    }
    // enable truth table computation if cut minimization is selected
    if ( pPars->fCutMin )
    {
        pPars->fTruth = 1;
        pPars->fExpRed = 0;
    }

    // complain if truth tables are requested but the cut size is too large
    if ( pPars->fTruth && pPars->nLutSize > IF_MAX_FUNC_LUTSIZE )
    {
        fprintf( pErr, "Truth tables cannot be computed for LUT larger than %d inputs.\n", IF_MAX_FUNC_LUTSIZE );
        return 1;
    }

    // disable cut-expansion if edge-based heuristics are selected
//    if ( pPars->fEdge )
//        pPars->fExpRed = 0;

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        // strash and balance the network
        pNtk = Abc_NtkStrash( pNtk, 0, 0, 0 );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Strashing before FPGA mapping has failed.\n" );
            return 1;
        }
        pNtk = Abc_NtkBalance( pNtkRes = pNtk, 0, 0, 1 );
        Abc_NtkDelete( pNtkRes );
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Balancing before FPGA mapping has failed.\n" );
            return 1;
        }
        fprintf( pOut, "The network was strashed and balanced before FPGA mapping.\n" );
        // get the new network
        pNtkRes = Abc_NtkIf( pNtk, pPars );
        if ( pNtkRes == NULL )
        {
            Abc_NtkDelete( pNtk );
            fprintf( pErr, "FPGA mapping has failed.\n" );
            return 0;
        }
        Abc_NtkDelete( pNtk );
    }
    else
    {
        // get the new network
        pNtkRes = Abc_NtkIf( pNtk, pPars );
        if ( pNtkRes == NULL )
        {
            fprintf( pErr, "FPGA mapping has failed.\n" );
            return 0;
        }
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    if ( pPars->DelayTarget == -1 ) 
        sprintf( Buffer, "best possible" );
    else
        sprintf( Buffer, "%.2f", pPars->DelayTarget );
    if ( pPars->nLutSize == -1 ) 
        sprintf( LutSize, "library" );
    else
        sprintf( LutSize, "%d", pPars->nLutSize );
    fprintf( pErr, "usage: if [-KCFA num] [-DE float] [-qarlepmsbvh]\n" );
    fprintf( pErr, "\t           performs FPGA technology mapping of the network\n" );
    fprintf( pErr, "\t-K num   : the number of LUT inputs (2 < num < %d) [default = %s]\n", IF_MAX_LUTSIZE+1, LutSize );
    fprintf( pErr, "\t-C num   : the max number of priority cuts (0 < num < 2^12) [default = %d]\n", pPars->nCutsMax );
    fprintf( pErr, "\t-F num   : the number of area flow recovery iterations (num >= 0) [default = %d]\n", pPars->nFlowIters );
    fprintf( pErr, "\t-A num   : the number of exact area recovery iterations (num >= 0) [default = %d]\n", pPars->nAreaIters );
    fprintf( pErr, "\t-D float : sets the delay constraint for the mapping [default = %s]\n", Buffer );  
    fprintf( pErr, "\t-E float : sets epsilon used for tie-breaking [default = %f]\n", pPars->Epsilon );  
    fprintf( pErr, "\t-q       : toggles preprocessing using several starting points [default = %s]\n", pPars->fPreprocess? "yes": "no" );
    fprintf( pErr, "\t-a       : toggles area-oriented mapping [default = %s]\n", pPars->fArea? "yes": "no" );
//    fprintf( pErr, "\t-f       : toggles one fancy feature [default = %s]\n", pPars->fFancy? "yes": "no" );
    fprintf( pErr, "\t-r       : enables expansion/reduction of the best cuts [default = %s]\n", pPars->fExpRed? "yes": "no" );
    fprintf( pErr, "\t-l       : optimizes latch paths for delay, other paths for area [default = %s]\n", pPars->fLatchPaths? "yes": "no" );
    fprintf( pErr, "\t-e       : uses edge-based cut selection heuristics [default = %s]\n", pPars->fEdge? "yes": "no" );
    fprintf( pErr, "\t-p       : uses power-aware cut selection heuristics [default = %s]\n", pPars->fPower? "yes": "no" );
    fprintf( pErr, "\t-m       : enables cut minimization by removing vacuous variables [default = %s]\n", pPars->fCutMin? "yes": "no" );
    fprintf( pErr, "\t-s       : toggles sequential mapping [default = %s]\n", pPars->fSeqMap? "yes": "no" );
//    fprintf( pErr, "\t-t       : toggles the use of true sequential cuts [default = %s]\n", pPars->fLiftLeaves? "yes": "no" );
    fprintf( pErr, "\t-b       : toggles deriving local AIGs using bi-decomposition [default = %s]\n", pPars->fBidec? "yes": "no" );
    fprintf( pErr, "\t-v       : toggles verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h       : prints the command usage\n");
    return 1;
}



/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandInit( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    Abc_Obj_t * pObj;
    int c, i;
    int fZeros;
    int fOnes;
    int fRandom;
    int fDontCare;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fZeros    = 0;
    fOnes     = 0;
    fRandom   = 0;
    fDontCare = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "zordh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'z':
            fZeros ^= 1;
            break;
        case 'o':
            fOnes ^= 1;
            break;
        case 'r':
            fRandom ^= 1;
            break;
        case 'd':
            fDontCare ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The current network is combinational.\n" );
        return 0;
    }

    if ( fZeros )
    {
        Abc_NtkForEachLatch( pNtk, pObj, i )
            Abc_LatchSetInit0( pObj );
    }
    else if ( fOnes )
    {
        Abc_NtkForEachLatch( pNtk, pObj, i )
            Abc_LatchSetInit1( pObj );
    }
    else if ( fRandom )
    {
        srand( time(NULL) );
        Abc_NtkForEachLatch( pNtk, pObj, i )
            if ( rand() & 1 )
                Abc_LatchSetInit1( pObj );
            else
                Abc_LatchSetInit0( pObj );
    }
    else if ( fDontCare )
    {
        Abc_NtkForEachLatch( pNtk, pObj, i )
            Abc_LatchSetInitDc( pObj );
    }
    else
        printf( "The initial states remain unchanged.\n" );
    return 0;

usage:
    fprintf( pErr, "usage: init [-zordh]\n" );
    fprintf( pErr, "\t        resets initial states of all latches\n" );
    fprintf( pErr, "\t-z    : set zeros initial states [default = %s]\n", fZeros? "yes": "no" );
    fprintf( pErr, "\t-o    : set ones initial states [default = %s]\n", fOnes? "yes": "no" );
    fprintf( pErr, "\t-d    : set don't-care initial states [default = %s]\n", fDontCare? "yes": "no" );
    fprintf( pErr, "\t-r    : set random initial states [default = %s]\n", fRandom? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandZero( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;

    extern Abc_Ntk_t * Abc_NtkRestrashZero( Abc_Ntk_t * pNtk, bool fCleanup );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The current network is combinational.\n" );
        return 0;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command works only for AIGs.\n" );
        return 0;
    }

    // get the new network
    pNtkRes = Abc_NtkRestrashZero( pNtk, 0 );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Converting to sequential AIG has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: zero [-h]\n" );
    fprintf( pErr, "\t        converts latches to have const-0 initial value\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandUndc( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The current network is combinational.\n" );
        return 0;
    }

    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command works only for logic networks.\n" );
        return 0;
    }

    // get the new network
    Abc_NtkConvertDcLatches( pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: undc [-h]\n" );
    fprintf( pErr, "\t        converts latches with DC init values into free PIs\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandOneHot( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    extern Abc_Ntk_t * Abc_NtkConvertOnehot( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The current network is combinational.\n" );
        return 0;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command works only for logic networks.\n" );
        return 0;
    }
    // get the new network
    pNtkRes = Abc_NtkConvertOnehot( pNtk );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Converting to one-hot encoding has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: onehot [-h]\n" );
    fprintf( pErr, "\t        converts natural encoding into one-hot encoding\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPipe( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nLatches;
    extern void Abc_NtkLatchPipe( Abc_Ntk_t * pNtk, int nLatches );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nLatches = 5;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Lh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nLatches = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLatches < 0 ) 
                goto usage;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The current network is combinational.\n" );
        return 0;
    }

    // update the network
    Abc_NtkLatchPipe( pNtk, nLatches );
    return 0;

usage:
    fprintf( pErr, "usage: pipe [-L num] [-h]\n" );
    fprintf( pErr, "\t         inserts the given number of latches at each PI for pipelining\n" );
    fprintf( pErr, "\t-L num : the number of latches to insert [default = %d]\n", nLatches );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSeq( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkLatchNum(pNtk) == 0 )
    {
        fprintf( pErr, "The network has no latches.\n" );
        return 0;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Conversion to sequential AIG works only for combinational AIGs (run \"strash\").\n" );
        return 1;
    }

    // get the new network
//    pNtkRes = Abc_NtkAigToSeq( pNtk );
    pNtkRes = NULL;
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Converting to sequential AIG has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: seq [-h]\n" );
    fprintf( pErr, "\t        converts AIG into sequential AIG\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandUnseq( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fShare;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fShare = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "sh" ) ) != EOF )
    {
        switch ( c )
        {
        case 's':
            fShare ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
/*
    if ( !Abc_NtkIsSeq(pNtk) )
    {
        fprintf( pErr, "Conversion to combinational AIG works only for sequential AIG (run \"seq\").\n" );
        return 1;
    }
*/
    // share the latches on the fanout edges
//    if ( fShare )
//        Seq_NtkShareFanouts(pNtk);

    // get the new network
//    pNtkRes = Abc_NtkSeqToLogicSop( pNtk );
    pNtkRes = NULL;
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Converting sequential AIG into an SOP logic network has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: unseq [-sh]\n" );
    fprintf( pErr, "\t        converts sequential AIG into an SOP logic network\n" );
    fprintf( pErr, "\t-s    : toggle sharing latches [default = %s]\n", fShare? "yes": "no" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandRetime( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, nMaxIters;
    int fForward;
    int fBackward;
    int fOneStep;
    int fVerbose;
    int Mode;
    extern int Abc_NtkRetime( Abc_Ntk_t * pNtk, int Mode, int fForwardOnly, int fBackwardOnly, int fOneStep, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Mode      =  5;
    fForward  =  0;
    fBackward =  0;
    fOneStep  =  0;
    fVerbose  =  0;
    nMaxIters = 15;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Mfbsvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-M\" should be followed by a positive integer.\n" );
                goto usage;
            }
            Mode = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Mode < 0 ) 
                goto usage;
            break;
        case 'f':
            fForward ^= 1;
            break;
        case 'b':
            fBackward ^= 1;
            break;
        case 's':
            fOneStep ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( fForward && fBackward )
    {
        fprintf( pErr, "Only one switch \"-f\" or \"-b\" can be selected at a time.\n" );
        return 1;
    }

    if ( !Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The network has no latches. Retiming is not performed.\n" );
        return 0;
    }

    if ( Mode < 0 || Mode > 6 )
    {
        fprintf( pErr, "The mode (%d) is incorrect. Retiming is not performed.\n", Mode );
        return 0;
    }

    if ( Abc_NtkIsStrash(pNtk) )
    {
        if ( Abc_NtkGetChoiceNum(pNtk) )
        {
            fprintf( pErr, "Retiming with choice nodes is not implemented.\n" );
            return 0;
        }
        // convert the network into an SOP network
        pNtkRes = Abc_NtkToLogic( pNtk );
        // perform the retiming
        Abc_NtkRetime( pNtkRes, Mode, fForward, fBackward, fOneStep, fVerbose );
        // replace the current network
        Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
        return 0;
    }

    // get the network in the SOP form
    if ( !Abc_NtkToSop(pNtk, 0) )
    {
        printf( "Converting to SOPs has failed.\n" );
        return 0;
    }

    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "The network is not a logic network. Retiming is not performed.\n" );
        return 0;
    }

    // perform the retiming
    Abc_NtkRetime( pNtk, Mode, fForward, fBackward, fOneStep, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: retime [-M num] [-fbvh]\n" );
    fprintf( pErr, "\t         retimes the current network using one of the algorithms:\n" );
    fprintf( pErr, "\t             1: most forward retiming\n" );
    fprintf( pErr, "\t             2: most backward retiming\n" );
    fprintf( pErr, "\t             3: forward and backward min-area retiming\n" );
    fprintf( pErr, "\t             4: forward and backward min-delay retiming\n" );
    fprintf( pErr, "\t             5: mode 3 followed by mode 4\n" );
    fprintf( pErr, "\t             6: Pan's optimum-delay retiming using binary search\n" );
    fprintf( pErr, "\t-M num : the retiming algorithm to use [default = %d]\n", Mode );
    fprintf( pErr, "\t-f     : enables forward-only retiming in modes 3,4,5 [default = %s]\n", fForward? "yes": "no" );
    fprintf( pErr, "\t-b     : enables backward-only retiming in modes 3,4,5 [default = %s]\n", fBackward? "yes": "no" );
    fprintf( pErr, "\t-s     : enables retiming one step only in mode 4 [default = %s]\n", fOneStep? "yes": "no" );
    fprintf( pErr, "\t-v     : enables verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
//    fprintf( pErr, "\t-I num : max number of iterations of l-value computation [default = %d]\n", nMaxIters );
//    fprintf( pErr, "\t-f     : toggle forward retiming (for AIGs) [default = %s]\n", fForward? "yes": "no" );
//    fprintf( pErr, "\t-b     : toggle backward retiming (for AIGs) [default = %s]\n", fBackward? "yes": "no" );
//    fprintf( pErr, "\t-i     : toggle computation of initial state [default = %s]\n", fInitial? "yes": "no" );
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDRetime( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int fMinArea;
    int fForwardOnly;
    int fBackwardOnly;
    int fInitial;
    int nStepsMax;
    int fFastAlgo;
    int fVerbose;
    int c, nMaxIters;
    extern Abc_Ntk_t * Abc_NtkDarRetime( Abc_Ntk_t * pNtk, int nStepsMax, int fVerbose );
    extern Abc_Ntk_t * Abc_NtkDarRetimeF( Abc_Ntk_t * pNtk, int nStepsMax, int fVerbose );
    extern Abc_Ntk_t * Abc_NtkDarRetimeMinArea( Abc_Ntk_t * pNtk, int nMaxIters, int fForwardOnly, int fBackwardOnly, int fInitial, int fVerbose );
    extern Abc_Ntk_t * Abc_NtkDarRetimeMostFwd( Abc_Ntk_t * pNtk, int nMaxIters, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fMinArea  = 1;
    fForwardOnly = 0;
    fBackwardOnly = 0;
    fInitial = 1;
    nStepsMax = 100000;
    fFastAlgo = 0;
    nMaxIters = 20;
    fVerbose  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NSmfbiavh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nMaxIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nMaxIters < 0 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-S\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nStepsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nStepsMax < 0 ) 
                goto usage;
            break;
        case 'm':
            fMinArea ^= 1;
            break;
        case 'f':
            fForwardOnly ^= 1;
            break;
        case 'b':
            fBackwardOnly ^= 1;
            break;
        case 'i':
            fInitial ^= 1;
            break;
        case 'a':
            fFastAlgo ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The network has no latches. Retiming is not performed.\n" );
        return 0;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        printf( "This command works only for structrally hashed networks. Run \"st\".\n" );
        return 0;
    }

    // perform the retiming
    if ( fMinArea )
        pNtkRes = Abc_NtkDarRetimeMinArea( pNtk, nMaxIters, fForwardOnly, fBackwardOnly, fInitial, fVerbose );
    else if ( fFastAlgo )
        pNtkRes = Abc_NtkDarRetime( pNtk, nStepsMax, fVerbose );
    else
//        pNtkRes = Abc_NtkDarRetimeF( pNtk, nStepsMax, fVerbose );
        pNtkRes = Abc_NtkDarRetimeMostFwd( pNtk, nMaxIters, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Retiming has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: dretime [-NS num] [-mfbiavh]\n" );
    fprintf( pErr, "\t         new implementation of min-area (or most-forward) retiming\n" );
    fprintf( pErr, "\t-m     : toggle min-area retiming and most-forward retiming [default = %s]\n", fMinArea? "min-area": "most-fwd" );
    fprintf( pErr, "\t-f     : enables forward-only retiming [default = %s]\n", fForwardOnly? "yes": "no" );
    fprintf( pErr, "\t-b     : enables backward-only retiming [default = %s]\n", fBackwardOnly? "yes": "no" );
    fprintf( pErr, "\t-i     : enables init state computation [default = %s]\n", fInitial? "yes": "no" );
    fprintf( pErr, "\t-N num : the max number of one-frame iterations to perform [default = %d]\n", nMaxIters );
    fprintf( pErr, "\t-S num : the max number of forward retiming steps to perform [default = %d]\n", nStepsMax );
    fprintf( pErr, "\t-a     : enables a fast most-forward algorithm [default = %s]\n", fFastAlgo? "yes": "no" );
    fprintf( pErr, "\t-v     : enables verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandFlowRetime( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c, nMaxIters;
    int fForward;
    int fBackward;
    int fVerbose;
    int fComputeInit, fGuaranteeInit, fBlockConst;
    int fFastButConservative;
    int maxDelay;

    printf( "This command is temporarily disabled.\n" );
    return 0;
//    extern Abc_Ntk_t* Abc_FlowRetime_MinReg( Abc_Ntk_t * pNtk, int fVerbose, 
//                                             int fComputeInit, int fGuaranteeInit, int fBlockConst,
//                                             int fForward, int fBackward, int nMaxIters,
//                                             int maxDelay, int fFastButConservative);

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fForward  =  0;
    fFastButConservative = 0;
    fBackward =  0;
    fComputeInit =  1;
    fGuaranteeInit =  0;
    fVerbose  =  0;
    fBlockConst  =  0;
    nMaxIters = 999;
    maxDelay  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "MDfcgbkivh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-M\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nMaxIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nMaxIters < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by a positive integer.\n" );
                goto usage;
            }
            maxDelay = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( maxDelay < 0 ) 
                goto usage;
           break;
        case 'f':
            fForward ^= 1;
            break;
        case 'c':
            fFastButConservative ^= 1;
            break;
        case 'i':
            fComputeInit ^= 1;
            break;
        case 'b':
            fBackward ^= 1;
            break;
        case 'g':
            fGuaranteeInit ^= 1;
            break;
        case 'k':
            fBlockConst ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( fForward && fBackward )
    {
        fprintf( pErr, "Only one switch \"-f\" or \"-b\" can be selected at a time.\n" );
        return 1;
    }
 
    if ( fGuaranteeInit && !fComputeInit )
    {
      fprintf( pErr, "Initial state guarantee (-g) requires initial state computation (-i).\n" );
      return 1;
    }

    if ( !Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The network has no latches. Retiming is not performed.\n" );
        return 0;
    }

    if ( Abc_NtkGetChoiceNum(pNtk) )
      {
        fprintf( pErr, "Retiming with choice nodes is not implemented.\n" );
        return 0;
      }

    // perform the retiming
//    pNtkRes = Abc_FlowRetime_MinReg( pNtk, fVerbose, fComputeInit,
//                                     fGuaranteeInit, fBlockConst, 
//                                     fForward, fBackward, 
//                                     nMaxIters, maxDelay, fFastButConservative );

    if (pNtkRes != pNtk)
      Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );

    return 0;

usage:
    fprintf( pErr, "usage: fretime [-M num] [-D num] [-fbvih]\n" );
    fprintf( pErr, "\t         retimes the current network using flow-based algorithm\n" );
    fprintf( pErr, "\t-M num : the maximum number of iterations [default = %d]\n", nMaxIters );
    fprintf( pErr, "\t-D num : the maximum delay [default = none]\n" );
    fprintf( pErr, "\t-i     : enables init state computation [default = %s]\n", fComputeInit? "yes": "no" );
    fprintf( pErr, "\t-k     : blocks retiming over const nodes [default = %s]\n", fBlockConst? "yes": "no" );
    fprintf( pErr, "\t-g     : guarantees init state computation [default = %s]\n", fGuaranteeInit? "yes": "no" );
    fprintf( pErr, "\t-c     : very fast (but conserv.) delay constraints [default = %s]\n", fFastButConservative? "yes": "no" );
    fprintf( pErr, "\t-f     : enables forward-only retiming  [default = %s]\n", fForward? "yes": "no" );
    fprintf( pErr, "\t-b     : enables backward-only retiming [default = %s]\n", fBackward? "yes": "no" );
    fprintf( pErr, "\t-v     : enables verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCRetime( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkCRetime( Abc_Ntk_t * pNtk, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose    = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Only works for logic networks.\n" );
        return 1;
    }
    if ( !Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 0;
    }
    // modify the current network
    pNtkRes = Abc_NtkCRetime( pNtk, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Sequential cleanup has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: cretime [-vh]\n" );
    fprintf( pErr, "\t         performs most-forward retiming with equiv classes\n" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSeqFpga( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkNew, * pNtkRes;
    int c, nMaxIters;
    int fVerbose;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nMaxIters = 15;
    fVerbose  =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Ivh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nMaxIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nMaxIters < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkHasAig(pNtk) )
    {
/*
        // quit if there are choice nodes
        if ( Abc_NtkGetChoiceNum(pNtk) )
        {
            fprintf( pErr, "Currently cannot map/retime networks with choice nodes.\n" );
            return 0;
        }
*/
//        if ( Abc_NtkIsStrash(pNtk) )
//            pNtkNew = Abc_NtkAigToSeq(pNtk);
//        else
//            pNtkNew = Abc_NtkDup(pNtk);
        pNtkNew = NULL;
    }
    else
    {
        // strash and balance the network
        pNtkNew = Abc_NtkStrash( pNtk, 0, 0, 0 );
        if ( pNtkNew == NULL )
        {
            fprintf( pErr, "Strashing before FPGA mapping/retiming has failed.\n" );
            return 1;
        }

        pNtkNew = Abc_NtkBalance( pNtkRes = pNtkNew, 0, 0, 1 );
        Abc_NtkDelete( pNtkRes );
        if ( pNtkNew == NULL )
        {
            fprintf( pErr, "Balancing before FPGA mapping has failed.\n" );
            return 1;
        }

        // convert into a sequential AIG
//        pNtkNew = Abc_NtkAigToSeq( pNtkRes = pNtkNew );
        pNtkNew = NULL;
        Abc_NtkDelete( pNtkRes );
        if ( pNtkNew == NULL )
        {
            fprintf( pErr, "Converting into a seq AIG before FPGA mapping/retiming has failed.\n" );
            return 1;
        }

        fprintf( pOut, "The network was strashed and balanced before FPGA mapping/retiming.\n" );
    }

    // get the new network
//    pNtkRes = Seq_NtkFpgaMapRetime( pNtkNew, nMaxIters, fVerbose );
    pNtkRes = NULL;
    if ( pNtkRes == NULL )
    {
//        fprintf( pErr, "Sequential FPGA mapping has failed.\n" );
        Abc_NtkDelete( pNtkNew );
        return 0;
    }
    Abc_NtkDelete( pNtkNew );
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: sfpga [-I num] [-vh]\n" );
    fprintf( pErr, "\t         performs integrated sequential FPGA mapping/retiming\n" );
    fprintf( pErr, "\t-I num : max number of iterations of l-value computation [default = %d]\n", nMaxIters );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSeqMap( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkNew, * pNtkRes;
    int c, nMaxIters;
    int fVerbose;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nMaxIters = 15;
    fVerbose  =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Ivh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by a positive integer.\n" );
                goto usage;
            }
            nMaxIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nMaxIters < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkHasAig(pNtk) )
    {
/*
        // quit if there are choice nodes
        if ( Abc_NtkGetChoiceNum(pNtk) )
        {
            fprintf( pErr, "Currently cannot map/retime networks with choice nodes.\n" );
            return 0;
        }
*/
//        if ( Abc_NtkIsStrash(pNtk) )
//            pNtkNew = Abc_NtkAigToSeq(pNtk);
//        else
//            pNtkNew = Abc_NtkDup(pNtk);
        pNtkNew = NULL;
    }
    else
    {
        // strash and balance the network
        pNtkNew = Abc_NtkStrash( pNtk, 0, 0, 0 );
        if ( pNtkNew == NULL )
        {
            fprintf( pErr, "Strashing before SC mapping/retiming has failed.\n" );
            return 1;
        }

        pNtkNew = Abc_NtkBalance( pNtkRes = pNtkNew, 0, 0, 1 );
        Abc_NtkDelete( pNtkRes );
        if ( pNtkNew == NULL )
        {
            fprintf( pErr, "Balancing before SC mapping/retiming has failed.\n" );
            return 1;
        }

        // convert into a sequential AIG
//        pNtkNew = Abc_NtkAigToSeq( pNtkRes = pNtkNew );
        pNtkNew = NULL;
        Abc_NtkDelete( pNtkRes );
        if ( pNtkNew == NULL )
        {
            fprintf( pErr, "Converting into a seq AIG before SC mapping/retiming has failed.\n" );
            return 1;
        }

        fprintf( pOut, "The network was strashed and balanced before SC mapping/retiming.\n" );
    }

    // get the new network
//    pNtkRes = Seq_MapRetime( pNtkNew, nMaxIters, fVerbose );
    pNtkRes = NULL;
    if ( pNtkRes == NULL )
    {
//        fprintf( pErr, "Sequential FPGA mapping has failed.\n" );
        Abc_NtkDelete( pNtkNew );
        return 0;
    }
    Abc_NtkDelete( pNtkNew );

    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: smap [-I num] [-vh]\n" );
    fprintf( pErr, "\t         performs integrated sequential standard-cell mapping/retiming\n" );
    fprintf( pErr, "\t-I num : max number of iterations of l-value computation [default = %d]\n", nMaxIters );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSeqSweep( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    Fra_Ssw_t Pars, * pPars = &Pars;
    int c;
    extern Abc_Ntk_t * Abc_NtkDarSeqSweep( Abc_Ntk_t * pNtk, Fra_Ssw_t * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    pPars->nPartSize  = 0;
    pPars->nOverSize  = 0;
    pPars->nFramesP   = 0;
    pPars->nFramesK   = 1;
    pPars->nMaxImps   = 5000;
    pPars->nMaxLevs   = 0;
    pPars->fUseImps   = 0;
    pPars->fRewrite   = 0;
    pPars->fFraiging  = 0;
    pPars->fLatchCorr = 0;
    pPars->fWriteImps = 0;
    pPars->fUse1Hot   = 0;
    pPars->fVerbose   = 0;
    pPars->TimeLimit  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PQNFILirfletvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nPartSize < 2 ) 
                goto usage;
            break;
        case 'Q':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-Q\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nOverSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nOverSize < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesP = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesP < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesK = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesK <= 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxImps = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxImps <= 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxLevs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxLevs <= 0 ) 
                goto usage;
            break;
        case 'i':
            pPars->fUseImps ^= 1;
            break;
        case 'r':
            pPars->fRewrite ^= 1;
            break;
        case 'f':
            pPars->fFraiging ^= 1;
            break;
        case 'l':
            pPars->fLatchCorr ^= 1;
            break;
        case 'e':
            pPars->fWriteImps ^= 1;
            break;
        case 't':
            pPars->fUse1Hot ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The network is combinational (run \"fraig\" or \"fraig_sweep\").\n" );
        return 0;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        printf( "This command works only for structrally hashed networks. Run \"st\".\n" );
        return 0;
    }

    if ( pPars->nFramesK > 1 && pPars->fUse1Hot )
    {
        printf( "Currrently can only use one-hotness for simple induction (K=1).\n" );
        return 0;
    }

    if ( pPars->nFramesP && pPars->fUse1Hot )
    {
        printf( "Currrently can only use one-hotness without prefix.\n" );
        return 0;
    }

    // get the new network
    pNtkRes = Abc_NtkDarSeqSweep( pNtk, pPars );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Sequential sweeping has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: ssweep [-PQNFL <num>] [-lrfetvh]\n" );
    fprintf( pErr, "\t         performs sequential sweep using K-step induction\n" );
    fprintf( pErr, "\t-P num : max partition size (0 = no partitioning) [default = %d]\n", pPars->nPartSize );
    fprintf( pErr, "\t-Q num : partition overlap (0 = no overlap) [default = %d]\n", pPars->nOverSize );
    fprintf( pErr, "\t-N num : number of time frames to use as the prefix [default = %d]\n", pPars->nFramesP );
    fprintf( pErr, "\t-F num : number of time frames for induction (1=simple) [default = %d]\n", pPars->nFramesK );
    fprintf( pErr, "\t-L num : max number of levels to consider (0=all) [default = %d]\n", pPars->nMaxLevs );
//    fprintf( pErr, "\t-I num : max number of implications to consider [default = %d]\n", pPars->nMaxImps );
//    fprintf( pErr, "\t-i     : toggle using implications [default = %s]\n", pPars->fUseImps? "yes": "no" );
    fprintf( pErr, "\t-l     : toggle latch correspondence only [default = %s]\n", pPars->fLatchCorr? "yes": "no" );
    fprintf( pErr, "\t-r     : toggle AIG rewriting [default = %s]\n", pPars->fRewrite? "yes": "no" );
    fprintf( pErr, "\t-f     : toggle fraiging (combinational SAT sweeping) [default = %s]\n", pPars->fFraiging? "yes": "no" );
    fprintf( pErr, "\t-e     : toggle writing implications as assertions [default = %s]\n", pPars->fWriteImps? "yes": "no" );
    fprintf( pErr, "\t-t     : toggle using one-hotness conditions [default = %s]\n", pPars->fUse1Hot? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSeqSweep2( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    Ssw_Pars_t Pars, * pPars = &Pars;
    int c;
    extern Abc_Ntk_t * Abc_NtkDarSeqSweep2( Abc_Ntk_t * pNtk, Ssw_Pars_t * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Ssw_ManSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PQFCLNSIVMplfudsvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nPartSize < 2 ) 
                goto usage;
            break;
        case 'Q':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-Q\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nOverSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nOverSize < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesK = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesK <= 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit <= 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxLevs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxLevs <= 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nConstrs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nConstrs < 0 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesAddSim = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesAddSim < 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nItersStop = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nItersStop < 0 ) 
                goto usage;
            break;
        case 'V':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-V\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nSatVarMax2 = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nSatVarMax2 < 0 ) 
                goto usage;
            break;
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-M\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nRecycleCalls2 = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nRecycleCalls2 < 0 ) 
                goto usage;
            break;
        case 'p':
            pPars->fPolarFlip ^= 1;
            break;
        case 'l':
            pPars->fLatchCorr ^= 1;
            break;
        case 'f':
            pPars->fSemiFormal ^= 1;
            break;
        case 'u':
            pPars->fUniqueness ^= 1;
            break;
        case 'd':
            pPars->fDynamic ^= 1;
            break;
        case 's':
            pPars->fLocalSim ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'w':
            pPars->fFlopVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The network is combinational (run \"fraig\" or \"fraig_sweep\").\n" );
        return 0;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        printf( "This command works only for structrally hashed networks. Run \"st\".\n" );
        return 0;
    }

    // get the new network
    pNtkRes = Abc_NtkDarSeqSweep2( pNtk, pPars );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Sequential sweeping has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: scorr [-PQFCLNSIVM <num>] [-pludsvwh]\n" );
    fprintf( pErr, "\t         performs sequential sweep using K-step induction\n" );
    fprintf( pErr, "\t-P num : max partition size (0 = no partitioning) [default = %d]\n", pPars->nPartSize );
    fprintf( pErr, "\t-Q num : partition overlap (0 = no overlap) [default = %d]\n", pPars->nOverSize );
    fprintf( pErr, "\t-F num : number of time frames for induction (1=simple) [default = %d]\n", pPars->nFramesK );
    fprintf( pErr, "\t-C num : max number of conflicts at a node (0=inifinite) [default = %d]\n", pPars->nBTLimit );
    fprintf( pErr, "\t-L num : max number of levels to consider (0=all) [default = %d]\n", pPars->nMaxLevs );
    fprintf( pErr, "\t-N num : number of last POs treated as constraints (0=none) [default = %d]\n", pPars->nConstrs );
    fprintf( pErr, "\t-S num : additional simulation frames for c-examples (0=none) [default = %d]\n", pPars->nFramesAddSim );
    fprintf( pErr, "\t-I num : iteration number to stop and output SR-model (-1=none) [default = %d]\n", pPars->nItersStop );
    fprintf( pErr, "\t-V num : min var num needed to recycle the SAT solver [default = %d]\n", pPars->nSatVarMax2 );
    fprintf( pErr, "\t-M num : min call num needed to recycle the SAT solver [default = %d]\n", pPars->nRecycleCalls2 );
    fprintf( pErr, "\t-p     : toggle alighning polarity of SAT variables [default = %s]\n", pPars->fPolarFlip? "yes": "no" );
    fprintf( pErr, "\t-l     : toggle latch correspondence only [default = %s]\n", pPars->fLatchCorr? "yes": "no" );
//    fprintf( pErr, "\t-f     : toggle filtering using iterative BMC [default = %s]\n", pPars->fSemiFormal? "yes": "no" );
    fprintf( pErr, "\t-u     : toggle using uniqueness constraints [default = %s]\n", pPars->fUniqueness? "yes": "no" );
    fprintf( pErr, "\t-d     : toggle dynamic addition of constraints [default = %s]\n", pPars->fDynamic? "yes": "no" );
    fprintf( pErr, "\t-s     : toggle local simulation in the cone of influence [default = %s]\n", pPars->fLocalSim? "yes": "no" );
    fprintf( pErr, "\t-w     : toggle printout of flop equivalences [default = %s]\n", pPars->fFlopVerbose? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSeqSweepTest( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char * pFileName;
    Fra_Ssw_t Pars, * pPars = &Pars;
    int c;
//    extern Abc_Ntk_t * Abc_NtkDarSeqSweep( Abc_Ntk_t * pNtk, Fra_Ssw_t * pPars );
    extern int Fra_FraigInductionTest( char * pFileName, Fra_Ssw_t * pParams );

    // set defaults
    pPars->nPartSize  = 0;
    pPars->nOverSize  = 0;
    pPars->nFramesP   = 0;
    pPars->nFramesK   = 1;
    pPars->nMaxImps   = 5000;
    pPars->nMaxLevs   = 0;
    pPars->fUseImps   = 0;
    pPars->fRewrite   = 0;
    pPars->fFraiging  = 0;
    pPars->fLatchCorr = 0;
    pPars->fWriteImps = 0;
    pPars->fUse1Hot   = 0;
    pPars->fVerbose   = 0;
    pPars->TimeLimit  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PQNFILirfletvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nPartSize < 2 ) 
                goto usage;
            break;
        case 'Q':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-Q\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nOverSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nOverSize < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesP = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesP < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesK = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesK <= 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxImps = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxImps <= 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxLevs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxLevs <= 0 ) 
                goto usage;
            break;
        case 'i':
            pPars->fUseImps ^= 1;
            break;
        case 'r':
            pPars->fRewrite ^= 1;
            break;
        case 'f':
            pPars->fFraiging ^= 1;
            break;
        case 'l':
            pPars->fLatchCorr ^= 1;
            break;
        case 'e':
            pPars->fWriteImps ^= 1;
            break;
        case 't':
            pPars->fUse1Hot ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    // get the input file name
    if ( argc == globalUtilOptind + 1 )
        pFileName = argv[globalUtilOptind];
    else
    {
        printf( "File name should be given on the command line.\n" );
        return 1;
    }
    Fra_FraigInductionTest( pFileName, pPars );
    return 0;

usage:
    fprintf( stdout, "usage: testssw [-PQNFL num] [-lrfetvh] <file>\n" );
    fprintf( stdout, "\t         performs sequential sweep using K-step induction\n" );
    fprintf( stdout, "\t         (outputs a file with a set of pairs of equivalent nodes)\n" );
    fprintf( stdout, "\t-P num : max partition size (0 = no partitioning) [default = %d]\n", pPars->nPartSize );
    fprintf( stdout, "\t-Q num : partition overlap (0 = no overlap) [default = %d]\n", pPars->nOverSize );
    fprintf( stdout, "\t-N num : number of time frames to use as the prefix [default = %d]\n", pPars->nFramesP );
    fprintf( stdout, "\t-F num : number of time frames for induction (1=simple) [default = %d]\n", pPars->nFramesK );
    fprintf( stdout, "\t-L num : max number of levels to consider (0=all) [default = %d]\n", pPars->nMaxLevs );
//    fprintf( stdout, "\t-I num : max number of implications to consider [default = %d]\n", pPars->nMaxImps );
//    fprintf( stdout, "\t-i     : toggle using implications [default = %s]\n", pPars->fUseImps? "yes": "no" );
    fprintf( stdout, "\t-l     : toggle latch correspondence only [default = %s]\n", pPars->fLatchCorr? "yes": "no" );
    fprintf( stdout, "\t-r     : toggle AIG rewriting [default = %s]\n", pPars->fRewrite? "yes": "no" );
    fprintf( stdout, "\t-f     : toggle fraiging (combinational SAT sweeping) [default = %s]\n", pPars->fFraiging? "yes": "no" );
    fprintf( stdout, "\t-e     : toggle writing implications as assertions [default = %s]\n", pPars->fWriteImps? "yes": "no" );
    fprintf( stdout, "\t-t     : toggle using one-hotness conditions [default = %s]\n", pPars->fUse1Hot? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandLcorr( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int nFramesP;
    int nConfMax;
    int nVarsMax;
    int fNewAlgor;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkDarLcorr( Abc_Ntk_t * pNtk, int nFramesP, int nConfMax, int fVerbose );
    extern Abc_Ntk_t * Abc_NtkDarLcorrNew( Abc_Ntk_t * pNtk, int nVarsMax, int nConfMax, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFramesP   =     0;
    nConfMax   =  1000;
    nVarsMax   =  1000;
    fNewAlgor  =     1;
    fVerbose   =     0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PCSnvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nFramesP = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFramesP < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfMax < 0 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            nVarsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nVarsMax < 0 ) 
                goto usage;
            break;
        case 'n':
            fNewAlgor ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The network is combinational (run \"fraig\" or \"fraig_sweep\").\n" );
        return 0;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        printf( "This command works only for structrally hashed networks. Run \"st\".\n" );
        return 0;
    }

    // get the new network
    if ( fNewAlgor )
        pNtkRes = Abc_NtkDarLcorrNew( pNtk, nVarsMax, nConfMax, fVerbose );
    else
        pNtkRes = Abc_NtkDarLcorr( pNtk, nFramesP, nConfMax, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Sequential sweeping has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: lcorr [-PCS num] [-nvh]\n" );
    fprintf( pErr, "\t         computes latch correspondence using 1-step induction\n" );
    fprintf( pErr, "\t-P num : number of time frames to use as the prefix [default = %d]\n", nFramesP );
    fprintf( pErr, "\t-C num : max conflict number when proving latch equivalence [default = %d]\n", nConfMax );
    fprintf( pErr, "\t-S num : the max number of SAT variables [default = %d]\n", nVarsMax );
    fprintf( pErr, "\t-n     : toggle using new algorithm [default = %s]\n", fNewAlgor? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSeqCleanup( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int fLatchConst;
    int fLatchEqual;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkDarLatchSweep( Abc_Ntk_t * pNtk, int fLatchConst, int fLatchEqual, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fLatchConst = 1;
    fLatchEqual = 1;
    fVerbose    = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "cevh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'c':
            fLatchConst ^= 1;
            break;
        case 'e':
            fLatchEqual ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Only works for structrally hashed networks.\n" );
        return 1;
    }
    if ( !Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 0;
    }
    // modify the current network
    pNtkRes = Abc_NtkDarLatchSweep( pNtk, fLatchConst, fLatchEqual, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Sequential cleanup has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: scleanup [-cevh]\n" );
    fprintf( pErr, "\t         performs sequential cleanup of the current network\n" );
    fprintf( pErr, "\t         by removing nodes and latches that do not feed into POs\n" );
    fprintf( pErr, "\t-c     : sweep stuck-at latches detected by ternary simulation [default = %s]\n", fLatchConst? "yes": "no" );
    fprintf( pErr, "\t-e     : merge equal latches (same data inputs and init states) [default = %s]\n", fLatchEqual? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCycle( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nFrames;
    int fUseXval;
    int fVerbose;
    extern void Abc_NtkCycleInitState( Abc_Ntk_t * pNtk, int nFrames, int fUseXval, int fVerbose );
    extern void Abc_NtkCycleInitStateSop( Abc_Ntk_t * pNtk, int nFrames, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFrames    = 100;
    fUseXval   =   0;
    fVerbose   =   0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Fxvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'x':
            fUseXval ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) && !Abc_NtkIsSopLogic(pNtk) )
    {
        fprintf( pErr, "Only works for strashed networks or logic SOP networks.\n" );
        return 1;
    }
    if ( !Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 0;
    }
    if ( fUseXval && !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "X-valued simulation only works for AIGs. Run \"strash\".\n" );
        return 0;
    }
    if ( fUseXval )
        Abc_NtkCycleInitState( pNtk, nFrames, 1, fVerbose );
    else if ( Abc_NtkIsStrash(pNtk) )
        Abc_NtkCycleInitState( pNtk, nFrames, 0, fVerbose );
    else
        Abc_NtkCycleInitStateSop( pNtk, nFrames, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: cycle [-F num] [-xvh]\n" );
    fprintf( pErr, "\t         cycles sequential circuit for the given number of timeframes\n" );
    fprintf( pErr, "\t         to derive a new initial state (which may be on the envelope)\n" );
    fprintf( pErr, "\t-F num : the number of frames to simulate [default = %d]\n", nFrames );
    fprintf( pErr, "\t-x     : use x-valued primary inputs [default = %s]\n", fUseXval? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandXsim( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nFrames;
    int fXInputs;
    int fXState;
    int fVerbose;
    extern void Abc_NtkXValueSimulate( Abc_Ntk_t * pNtk, int nFrames, int fXInputs, int fXState, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFrames    = 10;
    fXInputs   =  0;
    fXState    =  0;
    fVerbose   =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Fisvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'i':
            fXInputs ^= 1;
            break;
        case 's':
            fXState ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Only works for strashed networks.\n" );
        return 1;
    }
    if ( !Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 0;
    }
    Abc_NtkXValueSimulate( pNtk, nFrames, fXInputs, fXState, fVerbose );
    return 0;

usage:
    fprintf( pErr, "usage: xsim [-F num] [-isvh]\n" );
    fprintf( pErr, "\t         performs X-valued simulation of the AIG\n" );
    fprintf( pErr, "\t-F num : the number of frames to simulate [default = %d]\n", nFrames );
    fprintf( pErr, "\t-i     : toggle X-valued representation of inputs [default = %s]\n", fXInputs? "yes": "no" );
    fprintf( pErr, "\t-s     : toggle X-valued representation of state [default = %s]\n", fXState? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSim( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fNew;
    int fComb;
    int nFrames;
    int nWords;
    int TimeOut;
    int fMiter;
    int fVerbose;
    extern int Abc_NtkDarSeqSim( Abc_Ntk_t * pNtk, int nFrames, int nWords, int TimeOut, int fNew, int fComb, int fMiter, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fNew       =  0;
    fComb      =  0;
    nFrames    = 32;
    nWords     =  8;
    TimeOut    = 30;
    fMiter     =  0;
    fVerbose   =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FWTncmvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            nWords = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nWords < 0 ) 
                goto usage;
            break;
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            TimeOut = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( TimeOut < 0 ) 
                goto usage;
            break;
        case 'n':
            fNew ^= 1;
            break;
        case 'c':
            fComb ^= 1;
            break;
        case 'm':
            fMiter ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Only works for strashed networks.\n" );
        return 1;
    }
    ABC_FREE( pNtk->pSeqModel );
    Abc_NtkDarSeqSim( pNtk, nFrames, nWords, TimeOut, fNew, fComb, fMiter, fVerbose );        
    return 0;

usage:
    fprintf( pErr, "usage: sim [-FWT num] [-ncmvh]\n" );
    fprintf( pErr, "\t         performs random simulation of the sequential miter\n" );
    fprintf( pErr, "\t-F num : the number of frames to simulate [default = %d]\n", nFrames );
    fprintf( pErr, "\t-W num : the number of words to simulate [default = %d]\n", nWords );
    fprintf( pErr, "\t-T num : approximate runtime limit in seconds [default = %d]\n", TimeOut );
    fprintf( pErr, "\t-n     : toggle new vs. old implementation [default = %s]\n", fNew? "new": "old" );
    fprintf( pErr, "\t-c     : toggle comb vs. seq simulaton [default = %s]\n", fComb? "comb": "seq" );
    fprintf( pErr, "\t-m     : toggle miter vs. any circuit [default = %s]\n", fMiter? "miter": "not miter" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDarPhase( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int c;
    int nFrames, nPref;
    int fIgnore;
    int fPrint;
    int fVerbose;
    extern Abc_Ntk_t * Abc_NtkPhaseAbstract( Abc_Ntk_t * pNtk, int nFrames, int nPref, int fIgnore, int fPrint, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFrames     = 0;
    nPref       = 0;
    fIgnore     = 0;
    fPrint      = 0;
    fVerbose    = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FPipvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nPref = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPref < 0 ) 
                goto usage;
            break;
        case 'i':
            fIgnore ^= 1;
            break;
        case 'p':
            fPrint ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Only works for structrally hashed networks.\n" );
        return 1;
    }
    if ( !Abc_NtkLatchNum(pNtk) )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 0;
    }
    if ( fPrint )
    {
        Abc_NtkPhaseAbstract( pNtk, 0, nPref, fIgnore, 1, fVerbose );
        return 0;
    }
    // modify the current network
    pNtkRes = Abc_NtkPhaseAbstract( pNtk, nFrames, nPref, fIgnore, 0, fVerbose );
    if ( pNtkRes == NULL )
    {
//        fprintf( pErr, "Phase abstraction has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: phase [-FP <num>] [-ipvh]\n" );
    fprintf( pErr, "\t         performs sequential cleanup of the current network\n" );
    fprintf( pErr, "\t         by removing nodes and latches that do not feed into POs\n" );
    fprintf( pErr, "\t-F num : the number of frames to abstract [default = %d]\n", nFrames );
    fprintf( pErr, "\t-P num : the number of prefix frames to skip [default = %d]\n", nPref );
    fprintf( pErr, "\t-i     : toggle ignoring the initial state [default = %s]\n", fIgnore? "yes": "no" );
    fprintf( pErr, "\t-p     : toggle printing statistics about generators [default = %s]\n", fPrint? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSynch( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtkRes, * pNtk1, * pNtk2, * pNtk;
    char ** pArgvNew;
    int nArgcNew;
    int fDelete1, fDelete2;
    int c;
    int nWords;
    int fVerbose;

    extern Abc_Ntk_t * Abc_NtkDarSynch( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nWords, int fVerbose );
    extern Abc_Ntk_t * Abc_NtkDarSynchOne( Abc_Ntk_t * pNtk, int nWords, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nWords   =  32;
    fVerbose =   1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Wvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            nWords = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nWords <= 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( nArgcNew == 0 )
    {
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Empty network.\n" );
            return 1;
        }
        pNtkRes = Abc_NtkDarSynchOne( pNtk, nWords, fVerbose );
    }
    else
    {
        if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
            return 1;
        if ( Abc_NtkLatchNum(pNtk1) == 0 || Abc_NtkLatchNum(pNtk2) == 0 )
        {
            if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
            if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
            printf( "The network has no latches..\n" );
            return 0;
        }

        // modify the current network
        pNtkRes = Abc_NtkDarSynch( pNtk1, pNtk2, nWords, fVerbose );
        if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
        if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
    }
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Synchronization has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: synch [-W <num>] [-vh] <file1> <file2>\n" );
    fprintf( pErr, "\t         derives and applies synchronization sequence\n" );
    fprintf( pErr, "\t-W num : the number of simulation words [default = %d]\n", nWords );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tfile1  : (optional) the file with the first design\n");
    fprintf( pErr, "\tfile2  : (optional) the file with the second design\n\n");
    fprintf( pErr, "\t         If no designs are given on the command line,\n" );
    fprintf( pErr, "\t         assumes the current network has no initial state,\n" );
    fprintf( pErr, "\t         derives synchronization sequence and applies it.\n\n" );
    fprintf( pErr, "\t         If two designs are given on the command line\n" );
    fprintf( pErr, "\t         assumes both of them have no initial state,\n" );
    fprintf( pErr, "\t         derives sequences for both designs, synchorinizes\n" );
    fprintf( pErr, "\t         them, and creates SEC miter comparing two designs.\n\n" );
    fprintf( pErr, "\t         If only one design is given on the command line,\n" );
    fprintf( pErr, "\t         considers the second design to be the current network,\n" );
    fprintf( pErr, "\t         and derives SEC miter for them, as described above.\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandClockGate( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Cgt_Par_t Pars, * pPars = &Pars;
    Abc_Ntk_t * pNtkRes, * pNtk, * pNtkCare;
    int c;

    extern Abc_Ntk_t * Abc_NtkDarClockGate( Abc_Ntk_t * pNtk, Abc_Ntk_t * pCare, Cgt_Par_t * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Cgt_SetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "LNDCVKavwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nLevelMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nLevelMax <= 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nCandMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nCandMax <= 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nOdcMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nOdcMax <= 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nConfMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nConfMax <= 0 ) 
                goto usage;
            break;
        case 'V':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-V\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nVarsMin = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nVarsMin <= 0 ) 
                goto usage;
            break;
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFlopsMin = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFlopsMin <= 0 ) 
                goto usage;
            break;
        case 'a':
            pPars->fAreaOnly ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( argc == globalUtilOptind + 1 )
    {
        pNtkCare = Io_Read( argv[globalUtilOptind], Io_ReadFileType(argv[globalUtilOptind]), 1 );
        if ( pNtkCare == NULL )
        {
            printf( "Reading care network has failed.\n" );
            return 1;
        }
        // modify the current network
        pNtkRes = Abc_NtkDarClockGate( pNtk, pNtkCare, pPars );
        Abc_NtkDelete( pNtkCare );
    }
    else if ( argc == globalUtilOptind )
    {
        pNtkRes = Abc_NtkDarClockGate( pNtk, NULL, pPars );
    }
    else
    {
        fprintf( pErr, "Wrong number of arguments.\n" );
        return 0;
    }
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Clock gating has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: clockgate [-LNDCVK <num>] [-avwh] <file>\n" );
    fprintf( pErr, "\t         sequential clock gating with observability don't-cares\n" );
    fprintf( pErr, "\t-L num : max level number of a clock gate [default = %d]\n", pPars->nLevelMax );
    fprintf( pErr, "\t-N num : max number of candidates for a flop [default = %d]\n", pPars->nCandMax );
    fprintf( pErr, "\t-D num : max number of ODC levels to consider [default = %d]\n", pPars->nOdcMax );
    fprintf( pErr, "\t-C num : max number of conflicts at a node [default = %d]\n", pPars->nConfMax );
    fprintf( pErr, "\t-V num : min number of vars to recycle SAT solver [default = %d]\n", pPars->nVarsMin );
    fprintf( pErr, "\t-K num : min number of flops to recycle SAT solver [default = %d]\n", pPars->nFlopsMin );
    fprintf( pErr, "\t-a     : toggle minimizing area-only [default = %s]\n", pPars->fAreaOnly? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w     : toggle even more detailed output [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tfile   : (optional) constraints for primary inputs and register outputs\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandExtWin( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtkRes, * pNtk;
    int c;
    int nObjId;
    int nDist;
    int fVerbose;

    extern Abc_Ntk_t * Abc_NtkDarExtWin( Abc_Ntk_t * pNtk, int nObjId, int nDist, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nObjId   = -1;
    nDist    =  5;
    fVerbose =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NDvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nObjId = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nObjId <= 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            nDist = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nDist <= 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Only works for structrally hashed networks.\n" );
        return 1;
    }

    if ( argc != globalUtilOptind )
    {
        fprintf( pErr, "Not enough command-line arguments.\n" );
        return 1;
    }
    // modify the current network
    pNtkRes = Abc_NtkDarExtWin( pNtk, nObjId, nDist, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Extracting sequential window has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: extwin [-ND <num>] [-vh]\n" );
    fprintf( pErr, "\t         extracts sequential window from the AIG\n" );
    fprintf( pErr, "\t-N num : the ID of the object to use as the center [default = %d]\n", nObjId );
    fprintf( pErr, "\t-D num : the \"radius\" of the window [default = %d]\n", nDist );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandInsWin( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtkRes, * pNtk, * pNtkCare;
    int c;
    int nObjId;
    int nDist;
    int fVerbose;

    extern Abc_Ntk_t * Abc_NtkDarInsWin( Abc_Ntk_t * pNtk, Abc_Ntk_t * pWnd, int nObjId, int nDist, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nObjId   = -1;
    nDist    =  5;
    fVerbose =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NDvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nObjId = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nObjId <= 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            nDist = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nDist <= 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "Only works for structrally hashed networks.\n" );
        return 1;
    }

    if ( argc != globalUtilOptind + 1 )
    {
        fprintf( pErr, "Not enough command-line arguments.\n" );
        return 1;
    }
    pNtkCare = Io_Read( argv[globalUtilOptind], Io_ReadFileType(argv[globalUtilOptind]), 1 );
    if ( pNtkCare == NULL )
    {
        printf( "Reading care network has failed.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtkCare) )
    {
        Abc_Ntk_t * pNtkTemp;
        pNtkCare = Abc_NtkStrash( pNtkTemp = pNtkCare, 0, 1, 0 );
        Abc_NtkDelete( pNtkTemp );
    }
    // modify the current network
    pNtkRes = Abc_NtkDarInsWin( pNtk, pNtkCare, nObjId, nDist, fVerbose );
    Abc_NtkDelete( pNtkCare );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Inserting sequential window has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: inswin [-ND <num>] [-vh] <file>\n" );
    fprintf( pErr, "\t         inserts sequential window into the AIG\n" );
    fprintf( pErr, "\t-N num : the ID of the object to use as the center [default = %d]\n", nObjId );
    fprintf( pErr, "\t-D num : the \"radius\" of the window [default = %d]\n", nDist );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tfile   : file with the AIG to be inserted\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandCec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char Buffer[16];
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk1, * pNtk2;
    int fDelete1, fDelete2;
    char ** pArgvNew;
    int nArgcNew;
    int c;
    int fSat;
    int fVerbose;
    int nSeconds;
    int nPartSize;
    int nConfLimit;
    int nInsLimit;
    int fPartition;
    int fIgnoreNames;

    extern void Abc_NtkCecSat( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nConfLimit, int nInsLimit );
    extern void Abc_NtkCecFraig( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nSeconds, int fVerbose );
    extern void Abc_NtkCecFraigPart( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nSeconds, int nPartSize, int fVerbose );
    extern void Abc_NtkCecFraigPartAuto( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nSeconds, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fSat     =  0;
    fVerbose =  0;
    nSeconds = 20;
    nPartSize  = 0;
    nConfLimit = 10000;   
    nInsLimit  = 0;
    fPartition = 0;
    fIgnoreNames = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "TCIPpsnvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            nSeconds = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nSeconds < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            nInsLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nInsLimit < 0 ) 
                goto usage;
            break;
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPartSize < 0 ) 
                goto usage;
            break;
        case 'p':
            fPartition ^= 1;
            break;
        case 's':
            fSat ^= 1;
            break;
        case 'n':
            fIgnoreNames ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
        return 1;
    
    if ( fIgnoreNames )
    {
        Abc_NtkShortNames( pNtk1 );
        Abc_NtkShortNames( pNtk2 );
    }

    // perform equivalence checking
    if ( fPartition )
        Abc_NtkCecFraigPartAuto( pNtk1, pNtk2, nSeconds, fVerbose );
    else if ( nPartSize )
        Abc_NtkCecFraigPart( pNtk1, pNtk2, nSeconds, nPartSize, fVerbose );
    else if ( fSat )
        Abc_NtkCecSat( pNtk1, pNtk2, nConfLimit, nInsLimit );
    else
        Abc_NtkCecFraig( pNtk1, pNtk2, nSeconds, fVerbose );

    if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
    if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
    return 0;

usage:
    if ( nPartSize == 0 )
        strcpy( Buffer, "unused" );
    else
        sprintf( Buffer, "%d", nPartSize );
    fprintf( pErr, "usage: cec [-T num] [-C num] [-I num] [-P num] [-psnvh] <file1> <file2>\n" );
    fprintf( pErr, "\t         performs combinational equivalence checking\n" );
    fprintf( pErr, "\t-T num : approximate runtime limit in seconds [default = %d]\n", nSeconds );
    fprintf( pErr, "\t-C num : limit on the number of conflicts [default = %d]\n",    nConfLimit );
    fprintf( pErr, "\t-I num : limit on the number of clause inspections [default = %d]\n", nInsLimit );
    fprintf( pErr, "\t-P num : partition size for multi-output networks [default = %s]\n", Buffer );
    fprintf( pErr, "\t-p     : toggle automatic partitioning [default = %s]\n", fPartition? "yes": "no" );
    fprintf( pErr, "\t-s     : toggle \"SAT only\" and \"FRAIG + SAT\" [default = %s]\n", fSat? "SAT only": "FRAIG + SAT" );
    fprintf( pErr, "\t-n     : toggle ignoring names when matching CIs/COs [default = %s]\n", fIgnoreNames? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tfile1  : (optional) the file with the first network\n");
    fprintf( pErr, "\tfile2  : (optional) the file with the second network\n");
    fprintf( pErr, "\t         if no files are given, uses the current network and its spec\n");
    fprintf( pErr, "\t         if one file is given, uses the current network and the file\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDCec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk1, * pNtk2;
    int fDelete1, fDelete2;
    char ** pArgvNew;
    int nArgcNew;
    int c;
    int fSat;
    int fVerbose;
    int nSeconds;
    int nConfLimit;
    int nInsLimit;
    int fPartition;
    int fMiter;

    extern int Abc_NtkDSat( Abc_Ntk_t * pNtk, ABC_INT64_T nConfLimit, ABC_INT64_T nInsLimit, int fAlignPol, int fAndOuts, int fVerbose );
    extern int Abc_NtkDarCec( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nConfLimit, int fPartition, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fSat     =  0;
    fVerbose =  0;
    nSeconds = 20;
    nConfLimit = 10000;   
    nInsLimit  = 0;
    fPartition = 0;
    fMiter     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "TCIpmsvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            nSeconds = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nSeconds < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            nInsLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nInsLimit < 0 ) 
                goto usage;
            break;
        case 'p':
            fPartition ^= 1;
            break;
        case 'm':
            fMiter ^= 1;
            break;
        case 's':
            fSat ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( fMiter )
    {
        if ( pNtk == NULL )
        {
            fprintf( pErr, "Empty network.\n" );
            return 1;
        }
        if ( Abc_NtkIsStrash(pNtk) )
        {
            pNtk1 = pNtk;
            fDelete1 = 0;
        }
        else
        {
            pNtk1 = Abc_NtkStrash( pNtk, 0, 1, 0 );
            fDelete1 = 1;
        }
        pNtk2 = NULL;
        fDelete2 = 0;
    }
    else
    {
        if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
            return 1;
    }

    if ( Abc_NtkLatchNum(pNtk1) || Abc_NtkLatchNum(pNtk2) )
    {
        if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
        if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
        printf( "Currently this command only works for networks without latches. Run \"comb\".\n" );
        return 1;
    }

    // perform equivalence checking
    if ( fSat && fMiter )
        Abc_NtkDSat( pNtk1, nConfLimit, nInsLimit, 0, 0, fVerbose );
    else
        Abc_NtkDarCec( pNtk1, pNtk2, nConfLimit, fPartition, fVerbose );

    if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
    if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
    return 0;

usage:
    fprintf( pErr, "usage: dcec [-T num] [-C num] [-I num] [-mpsvh] <file1> <file2>\n" );
    fprintf( pErr, "\t         performs combinational equivalence checking\n" );
    fprintf( pErr, "\t-T num : approximate runtime limit in seconds [default = %d]\n", nSeconds );
    fprintf( pErr, "\t-C num : limit on the number of conflicts [default = %d]\n",    nConfLimit );
    fprintf( pErr, "\t-I num : limit on the number of clause inspections [default = %d]\n", nInsLimit );
    fprintf( pErr, "\t-m     : toggle working on two networks or a miter [default = %s]\n", fMiter? "miter": "two networks" );
    fprintf( pErr, "\t-p     : toggle automatic partitioning [default = %s]\n", fPartition? "yes": "no" );
    fprintf( pErr, "\t-s     : toggle \"SAT only\" (miter) or \"FRAIG + SAT\" [default = %s]\n", fSat? "SAT only": "FRAIG + SAT" );
    fprintf( pErr, "\t-v     : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tfile1  : (optional) the file with the first network\n");
    fprintf( pErr, "\tfile2  : (optional) the file with the second network\n");
    fprintf( pErr, "\t         if no files are given, uses the current network and its spec\n");
    fprintf( pErr, "\t         if one file is given, uses the current network and the file\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk1, * pNtk2;
    int fDelete1, fDelete2;
    char ** pArgvNew;
    int nArgcNew;
    int c;
    int fRetime;
    int fSat;
    int fVerbose;
    int nFrames;
    int nSeconds;
    int nConfLimit;
    int nInsLimit;

    extern void Abc_NtkSecSat( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nConfLimit, int nInsLimit, int nFrames );
    extern int Abc_NtkSecFraig( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nSeconds, int nFrames, int fVerbose );


    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fRetime  =  0; // verification after retiming
    fSat     =  0;
    fVerbose =  0;
    nFrames  =  5;
    nSeconds = 20;
    nConfLimit = 10000;   
    nInsLimit  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FTCIsrvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames <= 0 ) 
                goto usage;
            break;
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            nSeconds = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nSeconds < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            nInsLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nInsLimit < 0 ) 
                goto usage;
            break;
        case 'r':
            fRetime ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 's':
            fSat ^= 1;
            break;
        default:
            goto usage;
        }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
        return 1;

    if ( Abc_NtkLatchNum(pNtk1) == 0 || Abc_NtkLatchNum(pNtk2) == 0 )
    {
        if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
        if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
        printf( "The network has no latches. Used combinational command \"cec\".\n" );
        return 0;
    }

    // perform equivalence checking
    if ( fSat )
        Abc_NtkSecSat( pNtk1, pNtk2, nConfLimit, nInsLimit, nFrames );
    else
        Abc_NtkSecFraig( pNtk1, pNtk2, nSeconds, nFrames, fVerbose );

    if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
    if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
    return 0;

usage:
    fprintf( pErr, "usage: sec [-F num] [-T num] [-C num] [-I num] [-srvh] <file1> <file2>\n" );
    fprintf( pErr, "\t         performs bounded sequential equivalence checking\n" );
    fprintf( pErr, "\t         (there is also an unbounded SEC commands, \"dsec\" and \"dprove\")\n" );
    fprintf( pErr, "\t-s     : toggle \"SAT only\" and \"FRAIG + SAT\" [default = %s]\n", fSat? "SAT only": "FRAIG + SAT" );
    fprintf( pErr, "\t-r     : toggles retiming verification [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-v     : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\t-F num : the number of time frames to use [default = %d]\n", nFrames );
    fprintf( pErr, "\t-T num : approximate runtime limit in seconds [default = %d]\n", nSeconds );
    fprintf( pErr, "\t-C num : limit on the number of conflicts [default = %d]\n",    nConfLimit );
    fprintf( pErr, "\t-I num : limit on the number of inspections [default = %d]\n", nInsLimit );
    fprintf( pErr, "\tfile1  : (optional) the file with the first network\n");
    fprintf( pErr, "\tfile2  : (optional) the file with the second network\n");
    fprintf( pErr, "\t         if no files are given, uses the current network and its spec\n");
    fprintf( pErr, "\t         if one file is given, uses the current network and the file\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDSec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Fra_Sec_t SecPar, * pSecPar = &SecPar;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk1, * pNtk2;
    int fDelete1, fDelete2;
    char ** pArgvNew;
    int nArgcNew;
    int c;
    int fIgnoreNames;

    extern void Fra_SecSetDefaultParams( Fra_Sec_t * p );
    extern int Abc_NtkDarSec( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Fra_Sec_t * p );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Fra_SecSetDefaultParams( pSecPar );
    pSecPar->TimeLimit = 0;
    fIgnoreNames = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FTarmfnwvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->nFramesMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->nFramesMax < 0 ) 
                goto usage;
            break;
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->TimeLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->TimeLimit < 0 ) 
                goto usage;
            break;
        case 'a':
            pSecPar->fPhaseAbstract ^= 1;
            break;
        case 'r':
            pSecPar->fRetimeFirst ^= 1;
            break;
        case 'm':
            pSecPar->fRetimeRegs ^= 1;
            break;
        case 'f':
            pSecPar->fFraiging ^= 1;
            break;
        case 'n':
            fIgnoreNames ^= 1;
            break;
        case 'w':
            pSecPar->fVeryVerbose ^= 1;
            break;
        case 'v':
            pSecPar->fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
        return 1;
    if ( Abc_NtkLatchNum(pNtk1) == 0 || Abc_NtkLatchNum(pNtk2) == 0 )
    {
        if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
        if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
        printf( "The network has no latches. Used combinational command \"cec\".\n" );
        return 0;
    }
    
    if ( fIgnoreNames )
    {
        Abc_NtkShortNames( pNtk1 );
        Abc_NtkShortNames( pNtk2 );
    }

    // perform verification
    Abc_NtkDarSec( pNtk1, pNtk2, pSecPar );

    if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
    if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
    return 0;

usage:
    fprintf( pErr, "usage: dsec [-F num] [-T num] [-armfnwvh] <file1> <file2>\n" );
    fprintf( pErr, "\t         performs inductive sequential equivalence checking\n" );
    fprintf( pErr, "\t-F num : the limit on the depth of induction [default = %d]\n", pSecPar->nFramesMax );
    fprintf( pErr, "\t-T num : the approximate runtime limit (in seconds) [default = %d]\n", pSecPar->TimeLimit );
    fprintf( pErr, "\t-a     : toggles the use of phase abstraction [default = %s]\n", pSecPar->fPhaseAbstract? "yes": "no" );
    fprintf( pErr, "\t-r     : toggles forward retiming at the beginning [default = %s]\n", pSecPar->fRetimeFirst? "yes": "no" );
    fprintf( pErr, "\t-m     : toggles min-register retiming [default = %s]\n", pSecPar->fRetimeRegs? "yes": "no" );
    fprintf( pErr, "\t-f     : toggles the internal use of fraiging [default = %s]\n", pSecPar->fFraiging? "yes": "no" );
    fprintf( pErr, "\t-n     : toggle ignoring names when matching PIs/POs [default = %s]\n", fIgnoreNames? "yes": "no" );
    fprintf( pErr, "\t-v     : toggles verbose output [default = %s]\n", pSecPar->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w     : toggles additional verbose output [default = %s]\n", pSecPar->fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    fprintf( pErr, "\tfile1  : (optional) the file with the first network\n");
    fprintf( pErr, "\tfile2  : (optional) the file with the second network\n");
    fprintf( pErr, "\t         if no files are given, uses the current network and its spec\n");
    fprintf( pErr, "\t         if one file is given, uses the current network and the file\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDProve( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Fra_Sec_t SecPar, * pSecPar = &SecPar;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    extern void Fra_SecSetDefaultParams( Fra_Sec_t * p );
    extern int Abc_NtkDarProve( Abc_Ntk_t * pNtk, Fra_Sec_t * pSecPar );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Fra_SecSetDefaultParams( pSecPar );
//    pSecPar->TimeLimit = 300;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "cbFCGDVBRarmfijwvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'c':
            pSecPar->fTryComb ^= 1;
            break;
        case 'b':
            pSecPar->fTryBmc ^= 1;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->nFramesMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->nFramesMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'G':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-G\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->nBTLimitGlobal = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->nBTLimitGlobal < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->nBTLimitInter = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->nBTLimitInter < 0 ) 
                goto usage;
            break;
        case 'V':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-V\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->nBddVarsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->nBddVarsMax < 0 ) 
                goto usage;
            break;
        case 'B':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-B\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->nBddMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->nBddMax < 0 ) 
                goto usage;
            break;
        case 'R':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-R\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->nBddIterMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->nBddIterMax < 0 ) 
                goto usage;
            break;
        case 'a':
            pSecPar->fPhaseAbstract ^= 1;
            break;
        case 'r':
            pSecPar->fRetimeFirst ^= 1;
            break;
        case 'm':
            pSecPar->fRetimeRegs ^= 1;
            break;
        case 'f':
            pSecPar->fFraiging ^= 1;
            break;
        case 'i':
            pSecPar->fInduction ^= 1;
            break;
        case 'j':
            pSecPar->fInterpolation ^= 1;
            break;
        case 'w':
            pSecPar->fVeryVerbose ^= 1;
            break;
        case 'v':
            pSecPar->fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash(pNtk) )
    {
        printf( "This command works only for structrally hashed networks. Run \"st\".\n" );
        return 0;
    }

    // perform verification
    Abc_NtkDarProve( pNtk, pSecPar );
    pAbc->pCex = pNtk->pSeqModel; // temporary ???

    // Fra_SmlWriteCounterExample( stdout, Aig_Man_t * pAig, Fra_Cex_t * p )

    return 0;

usage:
    fprintf( pErr, "usage: dprove [-FCGDVBR num] [-cbarmfijwvh]\n" );
    fprintf( pErr, "\t         performs SEC on the sequential miter\n" );
    fprintf( pErr, "\t-F num : the limit on the depth of induction [default = %d]\n", pSecPar->nFramesMax );
    fprintf( pErr, "\t-C num : the conflict limit at a node during induction [default = %d]\n", pSecPar->nBTLimit );
    fprintf( pErr, "\t-G num : the global conflict limit during induction [default = %d]\n", pSecPar->nBTLimitGlobal );
    fprintf( pErr, "\t-D num : the conflict limit during interpolation [default = %d]\n", pSecPar->nBTLimitInter );
    fprintf( pErr, "\t-V num : the flop count limit for BDD-based reachablity [default = %d]\n", pSecPar->nBddVarsMax );
    fprintf( pErr, "\t-B num : the BDD size limit in BDD-based reachablity [default = %d]\n", pSecPar->nBddMax );
    fprintf( pErr, "\t-R num : the max number of reachability iterations [default = %d]\n", pSecPar->nBddIterMax );
    fprintf( pErr, "\t-c     : toggles using CEC before attempting SEC [default = %s]\n", pSecPar->fTryComb? "yes": "no" );
    fprintf( pErr, "\t-b     : toggles using BMC before attempting SEC [default = %s]\n", pSecPar->fTryBmc? "yes": "no" );
    fprintf( pErr, "\t-a     : toggles the use of phase abstraction [default = %s]\n", pSecPar->fPhaseAbstract? "yes": "no" );
    fprintf( pErr, "\t-r     : toggles forward retiming at the beginning [default = %s]\n", pSecPar->fRetimeFirst? "yes": "no" );
    fprintf( pErr, "\t-m     : toggles min-register retiming [default = %s]\n", pSecPar->fRetimeRegs? "yes": "no" );
    fprintf( pErr, "\t-f     : toggles the internal use of fraiging [default = %s]\n", pSecPar->fFraiging? "yes": "no" );
    fprintf( pErr, "\t-i     : toggles the use of induction [default = %s]\n", pSecPar->fInduction? "yes": "no" );
    fprintf( pErr, "\t-j     : toggles the use of interpolation [default = %s]\n", pSecPar->fInterpolation? "yes": "no" );
//    fprintf( pErr, "\t-n     : toggles the use of different induction prover [default = %s]\n", pSecPar->fUseNewProver? "yes": "no" );
    fprintf( pErr, "\t-v     : toggles verbose output [default = %s]\n", pSecPar->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-w     : toggles additional verbose output [default = %s]\n", pSecPar->fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbSec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk1, * pNtk2;
    int fDelete1, fDelete2;
    char ** pArgvNew;
    int nArgcNew;
    int fMiter, nFrames, fVerbose, c;

    extern int Abc_NtkDarAbSec( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nFrames, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fMiter   = 1;
    nFrames  = 2;
    fVerbose = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Fmvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'm':
            fMiter ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }

    if ( fMiter )
    {
        if ( !Abc_NtkIsStrash(pNtk) )
        {
            printf( "This command works only for structrally hashed networks. Run \"st\".\n" );
            return 0;
        }
        Abc_NtkDarAbSec( pNtk, NULL, nFrames, fVerbose );
    }
    else
    {
        pArgvNew = argv + globalUtilOptind;
        nArgcNew = argc - globalUtilOptind;
        if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
            return 1;
        if ( Abc_NtkLatchNum(pNtk1) == 0 || Abc_NtkLatchNum(pNtk2) == 0 )
        {
            if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
            if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
            printf( "The network has no latches. Used combinational command \"cec\".\n" );
            return 0;
        }
        // perform verification
        Abc_NtkDarAbSec( pNtk1, pNtk2, nFrames, fVerbose );
        if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
        if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
    }
    return 0;

usage:
    fprintf( pErr, "usage: absec [-F num] [-mv] <file1> <file2>\n" );
    fprintf( pErr, "\t         performs SEC by applying CEC to several timeframes\n" );
    fprintf( pErr, "\t-F num : the total number of timeframes to use [default = %d]\n", nFrames );
    fprintf( pErr, "\t-m     : toggles miter vs. two networks [default = %s]\n", fMiter? "miter": "two networks" );
    fprintf( pErr, "\t-v     : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\tfile1  : (optional) the file with the first network\n");
    fprintf( pErr, "\tfile2  : (optional) the file with the second network\n");
    fprintf( pErr, "\t         if no files are given, uses the current network and its spec\n");
    fprintf( pErr, "\t         if one file is given, uses the current network and the file\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSimSec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Ssw_Pars_t Pars, * pPars = &Pars;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk1, * pNtk2;
    int fDelete1, fDelete2;
    char ** pArgvNew;
    int nArgcNew, c;
    int fMiter;

    extern int Abc_NtkDarSimSec( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, Ssw_Pars_t * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fMiter = 1;
    Ssw_ManSetDefaultParams( pPars );
    pPars->fPartSigCorr = 1;
    pPars->fVerbose     = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FDcymvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesK = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesK < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nIsleDist = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nIsleDist < 0 ) 
                goto usage;
            break;
        case 'm':
            fMiter ^= 1;
            break;
        case 'c':
            pPars->fPartSigCorr ^= 1;
            break;
        case 'y':
            pPars->fDumpSRInit ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }

    if ( fMiter )
    {
//        Abc_Ntk_t * pNtkA, * pNtkB;
        if ( !Abc_NtkIsStrash(pNtk) )
        {
            printf( "This command works only for structrally hashed networks. Run \"st\".\n" );
            return 0;
        }
        Abc_NtkDarSimSec( pNtk, NULL, pPars );
/*
        pNtkA = Abc_NtkDup( pNtk );
        pNtkB = Abc_NtkDup( pNtk );
        Abc_NtkDarSimSec( pNtkA, pNtkB, pPars );
        Abc_NtkDelete( pNtkA );
        Abc_NtkDelete( pNtkB );
*/
    }
    else
    {
        pArgvNew = argv + globalUtilOptind;
        nArgcNew = argc - globalUtilOptind;
        if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
            return 1;
        if ( Abc_NtkLatchNum(pNtk1) == 0 || Abc_NtkLatchNum(pNtk2) == 0 )
        {
            if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
            if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
            printf( "The network has no latches. Used combinational command \"cec\".\n" );
            return 0;
        }
        // perform verification
        Abc_NtkDarSimSec( pNtk1, pNtk2, pPars );
        if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
        if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
    }
    return 0;

usage:
    fprintf( pErr, "usage: simsec [-FD num] [-mcyv] <file1> <file2>\n" );
    fprintf( pErr, "\t         performs SEC using structural similarity\n" );
    fprintf( pErr, "\t-F num : the limit on the depth of induction [default = %d]\n", pPars->nFramesK );
    fprintf( pErr, "\t-D num : the distance for extending islands [default = %d]\n", pPars->nIsleDist );
    fprintf( pErr, "\t-m     : toggles miter vs. two networks [default = %s]\n", fMiter? "miter": "two networks" );
    fprintf( pErr, "\t-c     : uses partial vs. full signal correspondence [default = %s]\n", pPars->fPartSigCorr? "partial": "full" );
    fprintf( pErr, "\t-y     : dumps speculatively reduced miter of the classes [default = %s]\n", pPars->fDumpSRInit? "yes": "no" );
    fprintf( pErr, "\t-v     : toggles verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\tfile1  : (optional) the file with the first network\n");
    fprintf( pErr, "\tfile2  : (optional) the file with the second network\n");
    fprintf( pErr, "\t         if no files are given, uses the current network and its spec\n");
    fprintf( pErr, "\t         if one file is given, uses the current network and the file\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandMatch( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtk1, * pNtk2, * pNtkRes;
    int fDelete1, fDelete2;
    char ** pArgvNew;
    int nArgcNew, c;
    int fMiter;
    int nDist;
    int fVerbose;

    extern Abc_Ntk_t * Abc_NtkDarMatch( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2, int nDist, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fMiter = 0;
    nDist = 0;
    fVerbose = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Dmvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            nDist = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nDist < 0 ) 
                goto usage;
            break;
        case 'm':
            fMiter ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }

    if ( fMiter )
    {
//        Abc_Ntk_t * pNtkA, * pNtkB;
        if ( !Abc_NtkIsStrash(pNtk) )
        {
            printf( "This command works only for structrally hashed networks. Run \"st\".\n" );
            return 0;
        }
        pNtkRes = Abc_NtkDarMatch( pNtk, NULL, nDist, fVerbose );
/*
        pNtkA = Abc_NtkDup( pNtk );
        pNtkB = Abc_NtkDup( pNtk );
        Abc_NtkDarSimSec( pNtkA, pNtkB, pPars );
        Abc_NtkDelete( pNtkA );
        Abc_NtkDelete( pNtkB );
*/
    }
    else
    {
        pArgvNew = argv + globalUtilOptind;
        nArgcNew = argc - globalUtilOptind;
        if ( !Abc_NtkPrepareTwoNtks( pErr, pNtk, pArgvNew, nArgcNew, &pNtk1, &pNtk2, &fDelete1, &fDelete2 ) )
            return 1;
        if ( Abc_NtkLatchNum(pNtk1) == 0 || Abc_NtkLatchNum(pNtk2) == 0 )
        {
            if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
            if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
            printf( "The network has no latches. Used combinational command \"cec\".\n" );
            return 0;
        }
        // perform verification
        pNtkRes = Abc_NtkDarMatch( pNtk1, pNtk2, nDist, fVerbose );
        if ( fDelete1 ) Abc_NtkDelete( pNtk1 );
        if ( fDelete2 ) Abc_NtkDelete( pNtk2 );
    }
    if ( pNtkRes == NULL )
    {
        printf( "Matching has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;

usage:
    fprintf( pErr, "usage: match [-D num] [-mv] <file1> <file2>\n" );
    fprintf( pErr, "\t         detects structural similarity using simulation\n" );
    fprintf( pErr, "\t         replaces the current network by the miter of differences\n" );
    fprintf( pErr, "\t-D num : the distance for extending differences [default = %d]\n", nDist );
    fprintf( pErr, "\t-m     : toggles miter vs. two networks [default = %s]\n", fMiter? "miter": "two networks" );
    fprintf( pErr, "\t-v     : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\tfile1  : (optional) the file with the first network\n");
    fprintf( pErr, "\tfile2  : (optional) the file with the second network\n");
    fprintf( pErr, "\t         if no files are given, uses the current network and its spec\n");
    fprintf( pErr, "\t         if one file is given, uses the current network and the file\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandSat( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int RetValue;
    int fVerbose;
    int nConfLimit;
    int nInsLimit;
    int clk;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose   = 0;
    nConfLimit = 100000;   
    nInsLimit  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CIvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            nInsLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nInsLimit < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkLatchNum(pNtk) > 0 )
    {
        fprintf( stdout, "Currently can only solve the miter for combinational circuits.\n" );
        return 0;
    } 

    clk = clock();
    if ( Abc_NtkIsStrash(pNtk) )
    {
        RetValue = Abc_NtkMiterSat( pNtk, (ABC_INT64_T)nConfLimit, (ABC_INT64_T)nInsLimit, fVerbose, NULL, NULL );
    }
    else
    {
        assert( Abc_NtkIsLogic(pNtk) );
        Abc_NtkToBdd( pNtk );
        RetValue = Abc_NtkMiterSat( pNtk, (ABC_INT64_T)nConfLimit, (ABC_INT64_T)nInsLimit, fVerbose, NULL, NULL );
    }

    // verify that the pattern is correct
    if ( RetValue == 0 && Abc_NtkPoNum(pNtk) == 1 )
    {
        //int i;
        //Abc_Obj_t * pObj;
        int * pSimInfo = Abc_NtkVerifySimulatePattern( pNtk, pNtk->pModel );
        if ( pSimInfo[0] != 1 )
            printf( "ERROR in Abc_NtkMiterSat(): Generated counter example is invalid.\n" );
        ABC_FREE( pSimInfo );
        /*
        // print model
        Abc_NtkForEachPi( pNtk, pObj, i )
        {
            printf( "%d", (int)(pNtk->pModel[i] > 0) );
            if ( i == 70 )
                break;
        }
        printf( "\n" );
        */
    }

    if ( RetValue == -1 )
        printf( "UNDECIDED      " );
    else if ( RetValue == 0 )
        printf( "SATISFIABLE    " );
    else
        printf( "UNSATISFIABLE  " );
    //printf( "\n" );
    ABC_PRT( "Time", clock() - clk );
    return 0;

usage:
    fprintf( pErr, "usage: sat [-C num] [-I num] [-vh]\n" );
    fprintf( pErr, "\t         solves the combinational miter using SAT solver MiniSat-1.14\n" );
    fprintf( pErr, "\t         derives CNF from the current network and leave it unchanged\n" );
    fprintf( pErr, "\t         (there is also a newer SAT solving command \"dsat\")\n" );
    fprintf( pErr, "\t-C num : limit on the number of conflicts [default = %d]\n",    nConfLimit );
    fprintf( pErr, "\t-I num : limit on the number of inspections [default = %d]\n", nInsLimit );
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDSat( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int RetValue;
    int fAlignPol;
    int fAndOuts;
    int fVerbose;
    int nConfLimit;
    int nInsLimit;
    int clk;

    extern int Abc_NtkDSat( Abc_Ntk_t * pNtk, ABC_INT64_T nConfLimit, ABC_INT64_T nInsLimit, int fAlignPol, int fAndOuts, int fVerbose );


    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fAlignPol  = 0;
    fAndOuts   = 0;
    fVerbose   = 0;
    nConfLimit = 100000;   
    nInsLimit  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CIpavh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            nInsLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nInsLimit < 0 ) 
                goto usage;
            break;
        case 'p':
            fAlignPol ^= 1;
            break;
        case 'a':
            fAndOuts ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkLatchNum(pNtk) > 0 )
    {
        fprintf( stdout, "Currently can only solve the miter for combinational circuits.\n" );
        return 0;
    } 
/*
    if ( Abc_NtkPoNum(pNtk) != 1 )
    {
        fprintf( stdout, "Currently expects a single-output miter.\n" );
        return 0;
    } 
*/
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }

    clk = clock();
    RetValue = Abc_NtkDSat( pNtk, (ABC_INT64_T)nConfLimit, (ABC_INT64_T)nInsLimit, fAlignPol, fAndOuts, fVerbose );
    // verify that the pattern is correct
    if ( RetValue == 0 && Abc_NtkPoNum(pNtk) == 1 )
    {
        //int i;
        //Abc_Obj_t * pObj;
        int * pSimInfo = Abc_NtkVerifySimulatePattern( pNtk, pNtk->pModel );
        if ( pSimInfo[0] != 1 )
            printf( "ERROR in Abc_NtkMiterSat(): Generated counter example is invalid.\n" );
        ABC_FREE( pSimInfo );
        /*
        // print model
        Abc_NtkForEachPi( pNtk, pObj, i )
        {
            printf( "%d", (int)(pNtk->pModel[i] > 0) );
            if ( i == 70 )
                break;
        }
        printf( "\n" );
        */
    }

    if ( RetValue == -1 )
        printf( "UNDECIDED      " );
    else if ( RetValue == 0 )
        printf( "SATISFIABLE    " );
    else
        printf( "UNSATISFIABLE  " );
    //printf( "\n" );
    ABC_PRT( "Time", clock() - clk );
    return 0;

usage:
    fprintf( pErr, "usage: dsat [-C num] [-I num] [-pavh]\n" );
    fprintf( pErr, "\t         solves the combinational miter using SAT solver MiniSat-1.14\n" );
    fprintf( pErr, "\t         derives CNF from the current network and leave it unchanged\n" );
    fprintf( pErr, "\t-C num : limit on the number of conflicts [default = %d]\n",    nConfLimit );
    fprintf( pErr, "\t-I num : limit on the number of inspections [default = %d]\n", nInsLimit );
    fprintf( pErr, "\t-p     : alighn polarity of SAT variables [default = %s]\n", fAlignPol? "yes": "no" );  
    fprintf( pErr, "\t-a     : toggle ANDing/ORing of miter outputs [default = %s]\n", fAndOuts? "ANDing": "ORing" );  
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPSat( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int RetValue;
    int c, clk;
    int nAlgo;
    int nPartSize;
    int nConfPart;
    int nConfTotal;
    int fAlignPol;
    int fSynthesize;
    int fVerbose;

    extern int Abc_NtkPartitionedSat( Abc_Ntk_t * pNtk, int nAlgo, int nPartSize, int nConfPart, int nConfTotal, int fAlignPol, int fSynthesize, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nAlgo       =        0;
    nPartSize   =    10000;
    nConfPart   =        0;
    nConfTotal  =  1000000;
    fAlignPol   =        1;
    fSynthesize =        0;
    fVerbose    =        1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "APCpsvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'A':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-A\" should be followed by an integer.\n" );
                goto usage;
            }
            nAlgo = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nAlgo < 0 ) 
                goto usage;
            break;
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPartSize < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfTotal = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfTotal < 0 ) 
                goto usage;
            break;
        case 'p':
            fAlignPol ^= 1;
            break;
        case 's':
            fSynthesize ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkLatchNum(pNtk) > 0 )
    {
        fprintf( stdout, "Currently can only solve the miter for combinational circuits.\n" );
        return 0;
    } 
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }

    clk = clock();
    RetValue = Abc_NtkPartitionedSat( pNtk, nAlgo, nPartSize, nConfPart, nConfTotal, fAlignPol, fSynthesize, fVerbose );
    // verify that the pattern is correct
    if ( RetValue == 0 && Abc_NtkPoNum(pNtk) == 1 )
    {
        //int i;
        //Abc_Obj_t * pObj;
        int * pSimInfo = Abc_NtkVerifySimulatePattern( pNtk, pNtk->pModel );
        if ( pSimInfo[0] != 1 )
            printf( "ERROR in Abc_NtkMiterSat(): Generated counter example is invalid.\n" );
        ABC_FREE( pSimInfo );
        /*
        // print model
        Abc_NtkForEachPi( pNtk, pObj, i )
        {
            printf( "%d", (int)(pNtk->pModel[i] > 0) );
            if ( i == 70 )
                break;
        }
        printf( "\n" );
        */
    }

    if ( RetValue == -1 )
        printf( "UNDECIDED      " );
    else if ( RetValue == 0 )
        printf( "SATISFIABLE    " );
    else
        printf( "UNSATISFIABLE  " );
    //printf( "\n" );
    ABC_PRT( "Time", clock() - clk );
    return 0;

usage:
    fprintf( pErr, "usage: psat [-APC num] [-psvh]\n" );
    fprintf( pErr, "\t         solves the combinational miter using partitioning\n" );
    fprintf( pErr, "\t         (derives CNF from the current network and leave it unchanged)\n" );
    fprintf( pErr, "\t         for multi-output miters, tries to prove that the AND of POs is always 0\n" );
    fprintf( pErr, "\t         (if POs should be ORed instead of ANDed, use command \"orpos\")\n" );
    fprintf( pErr, "\t-A num : partitioning algorithm [default = %d]\n", nAlgo );
    fprintf( pErr, "\t         0 : no partitioning\n" );
    fprintf( pErr, "\t         1 : partitioning by level\n" );
    fprintf( pErr, "\t         2 : DFS post-order\n" );
    fprintf( pErr, "\t         3 : DFS pre-order\n" );
    fprintf( pErr, "\t         4 : bit-slicing\n" );
    fprintf( pErr, "\t         partitions are ordered by level (high level first)\n" );
    fprintf( pErr, "\t-P num : limit on the partition size [default = %d]\n", nPartSize );
    fprintf( pErr, "\t-C num : limit on the number of conflicts [default = %d]\n", nConfTotal );
    fprintf( pErr, "\t-p     : align polarity of SAT variables [default = %s]\n", fAlignPol? "yes": "no" );  
    fprintf( pErr, "\t-s     : apply logic synthesis to each partition [default = %s]\n", fSynthesize? "yes": "no" );  
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandProve( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkTemp;
    Prove_Params_t Params, * pParams = &Params;
    int c, clk, RetValue;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Prove_ParamsSetDefault( pParams );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NCFLIrfbvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nItersMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nItersMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nMiteringLimitStart = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nMiteringLimitStart < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nFraigingLimitStart = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nFraigingLimitStart < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nMiteringLimitLast = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nMiteringLimitLast < 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            pParams->nTotalInspectLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pParams->nTotalInspectLimit < 0 ) 
                goto usage;
            break;
        case 'r':
            pParams->fUseRewriting ^= 1;
            break;
        case 'f':
            pParams->fUseFraiging ^= 1;
            break;
        case 'b':
            pParams->fUseBdds ^= 1;
            break;
        case 'v':
            pParams->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkLatchNum(pNtk) > 0 )
    {
        fprintf( stdout, "Currently can only solve the miter for combinational circuits.\n" );
        return 0;
    } 
    if ( Abc_NtkCoNum(pNtk) != 1 )
    {
        fprintf( stdout, "Currently can only solve the miter with one output.\n" );
        return 0;
    } 
    clk = clock();

    if ( Abc_NtkIsStrash(pNtk) )
        pNtkTemp = Abc_NtkDup( pNtk );
    else
        pNtkTemp = Abc_NtkStrash( pNtk, 0, 0, 0 );

    RetValue = Abc_NtkMiterProve( &pNtkTemp, pParams );

    // verify that the pattern is correct
    if ( RetValue == 0 )
    {
        int * pSimInfo = Abc_NtkVerifySimulatePattern( pNtk, pNtkTemp->pModel );
        if ( pSimInfo[0] != 1 )
            printf( "ERROR in Abc_NtkMiterProve(): Generated counter-example is invalid.\n" );
        ABC_FREE( pSimInfo );
    }
 
    if ( RetValue == -1 ) 
        printf( "UNDECIDED      " );
    else if ( RetValue == 0 )
        printf( "SATISFIABLE    " );
    else
        printf( "UNSATISFIABLE  " );
    //printf( "\n" );

    ABC_PRT( "Time", clock() - clk );
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkTemp );
    return 0;

usage:
    fprintf( pErr, "usage: prove [-N num] [-C num] [-F num] [-L num] [-I num] [-rfbvh]\n" );
    fprintf( pErr, "\t         solves combinational miter by rewriting, FRAIGing, and SAT\n" );
    fprintf( pErr, "\t         replaces the current network by the cone modified by rewriting\n" );
    fprintf( pErr, "\t         (there are also newer CEC commands, \"iprove\" and \"dprove\")\n" );
    fprintf( pErr, "\t-N num : max number of iterations [default = %d]\n", pParams->nItersMax );
    fprintf( pErr, "\t-C num : max starting number of conflicts in mitering [default = %d]\n", pParams->nMiteringLimitStart );
    fprintf( pErr, "\t-F num : max starting number of conflicts in fraiging [default = %d]\n", pParams->nFraigingLimitStart );
    fprintf( pErr, "\t-L num : max last-gasp number of conflicts in mitering [default = %d]\n", pParams->nMiteringLimitLast );
    fprintf( pErr, "\t-I num : max number of clause inspections in all SAT calls [default = %d]\n", (int)pParams->nTotalInspectLimit );
    fprintf( pErr, "\t-r     : toggle the use of rewriting [default = %s]\n", pParams->fUseRewriting? "yes": "no" );  
    fprintf( pErr, "\t-f     : toggle the use of FRAIGing [default = %s]\n", pParams->fUseFraiging? "yes": "no" );  
    fprintf( pErr, "\t-b     : toggle the use of BDDs [default = %s]\n", pParams->fUseBdds? "yes": "no" );  
    fprintf( pErr, "\t-v     : prints verbose information [default = %s]\n", pParams->fVerbose? "yes": "no" );  
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandDebug( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    extern void Abc_NtkAutoDebug( Abc_Ntk_t * pNtk, int (*pFuncError) (Abc_Ntk_t *) );
    extern int Abc_NtkRetimeDebug( Abc_Ntk_t * pNtk );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsLogic(pNtk) )
    {
        fprintf( pErr, "This command is applicable to logic networks.\n" );
        return 1;
    }

    Abc_NtkAutoDebug( pNtk, Abc_NtkRetimeDebug );
    return 0;

usage:
    fprintf( pErr, "usage: debug [-h]\n" );
    fprintf( pErr, "\t        performs automated debugging of the given procedure\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects [] 

  SeeAlso     []

***********************************************************************/
int Abc_CommandBmc( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nFrames;
    int nSizeMax;
    int nBTLimit;
    int nBTLimitAll;
    int nNodeDelta;
    int fRewrite;
    int fNewAlgo;
    int nCofFanLit;
    int fVerbose;

    extern int Abc_NtkDarBmc( Abc_Ntk_t * pNtk, int nFrames, int nSizeMax, int nNodeDelta, int nBTLimit, int nBTLimitAll, int fRewrite, int fNewAlgo, int nCofFanLit, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFrames     =       20;
    nSizeMax    =   100000;
    nBTLimit    =    10000;
    nBTLimitAll = 10000000;
    nNodeDelta  =     1000;
    fRewrite    =        0;
    fNewAlgo    =        1;
    nCofFanLit  =        0;
    fVerbose    =        0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FNCGDLrvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nSizeMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nBTLimit < 0 ) 
                goto usage;
            break;
        case 'G':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-G\" should be followed by an integer.\n" );
                goto usage;
            }
            nBTLimitAll = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nBTLimitAll < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            nNodeDelta = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nNodeDelta < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            nCofFanLit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCofFanLit < 0 ) 
                goto usage;
            break;
        case 'r':
            fRewrite ^= 1;
            break;
        case 'a':
            fNewAlgo ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    { 
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }
    if ( Abc_NtkLatchNum(pNtk) == 0 )
    {
        fprintf( stdout, "Does not work for combinational networks.\n" );
        return 0;
    }
    Abc_NtkDarBmc( pNtk, nFrames, nSizeMax, nNodeDelta, nBTLimit, nBTLimitAll, fRewrite, fNewAlgo, nCofFanLit, fVerbose );
    pAbc->pCex = pNtk->pSeqModel; // temporary ???
    return 0;

usage:
//    fprintf( pErr, "usage: bmc [-FNCGD num] [-ravh]\n" );
    fprintf( pErr, "usage: bmc [-FNCL num] [-rcvh]\n" );
    fprintf( pErr, "\t         performs bounded model checking with static unrolling\n" );
    fprintf( pErr, "\t-F num : the number of time frames [default = %d]\n", nFrames );
    fprintf( pErr, "\t-N num : the max number of nodes in the frames [default = %d]\n", nSizeMax );
    fprintf( pErr, "\t-C num : the max number of conflicts at a node [default = %d]\n", nBTLimit );
//    fprintf( pErr, "\t-G num : the max number of conflicts globally [default = %d]\n", nBTLimitAll );
//    fprintf( pErr, "\t-D num : the delta in the number of nodes [default = %d]\n", nNodeDelta );
    fprintf( pErr, "\t-L num : the limit on fanout count of resets/enables to cofactor [default = %d]\n", nCofFanLit );
    fprintf( pErr, "\t-r     : toggle the use of rewriting [default = %s]\n", fRewrite? "yes": "no" );
//    fprintf( pErr, "\t-a     : toggle SAT sweeping and SAT solving [default = %s]\n", fNewAlgo? "SAT solving": "SAT sweeping" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects [] 

  SeeAlso     []

***********************************************************************/
int Abc_CommandBmc2( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int nFrames;
    int nSizeMax;
    int nBTLimit;
    int nBTLimitAll;
    int nNodeDelta;
    int fRewrite;
    int fNewAlgo;
    int fVerbose;

    extern int Abc_NtkDarBmc( Abc_Ntk_t * pNtk, int nFrames, int nSizeMax, int nNodeDelta, int nBTLimit, int nBTLimitAll, int fRewrite, int fNewAlgo, int nCofFanLit, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFrames     =     2000;
    nSizeMax    =   200000;
    nBTLimit    =     2000;
    nBTLimitAll =  2000000;
    nNodeDelta  =     2000;
    fRewrite    =        0;
    fNewAlgo    =        0;
    fVerbose    =        0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FNCGDrvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nSizeMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nBTLimit < 0 ) 
                goto usage;
            break;
        case 'G':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-G\" should be followed by an integer.\n" );
                goto usage;
            }
            nBTLimitAll = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nBTLimitAll < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            nNodeDelta = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nNodeDelta < 0 ) 
                goto usage;
            break;
        case 'r':
            fRewrite ^= 1;
            break;
        case 'a':
            fNewAlgo ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    { 
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }
    if ( Abc_NtkLatchNum(pNtk) == 0 )
    {
        fprintf( stdout, "Does not work for combinational networks.\n" );
        return 0;
    }
    Abc_NtkDarBmc( pNtk, nFrames, nSizeMax, nNodeDelta, nBTLimit, nBTLimitAll, fRewrite, fNewAlgo, 0, fVerbose );
    pAbc->pCex = pNtk->pSeqModel; // temporary ???
    return 0;

usage:
//    fprintf( pErr, "usage: bmc2 [-FNCGD num] [-ravh]\n" );
    fprintf( pErr, "usage: bmc2 [-FCGD num] [-vh]\n" );
    fprintf( pErr, "\t         performs bounded model checking with dynamic unrolling\n" );
    fprintf( pErr, "\t-F num : the max number of time frames [default = %d]\n", nFrames );
//    fprintf( pErr, "\t-N num : the max number of nodes in the frames [default = %d]\n", nSizeMax );
    fprintf( pErr, "\t-C num : the max number of conflicts at a node [default = %d]\n", nBTLimit );
    fprintf( pErr, "\t-G num : the max number of conflicts globally [default = %d]\n", nBTLimitAll );
    fprintf( pErr, "\t-D num : the delta in the number of nodes [default = %d]\n", nNodeDelta );
//    fprintf( pErr, "\t-r     : toggle the use of rewriting [default = %s]\n", fRewrite? "yes": "no" );
//    fprintf( pErr, "\t-a     : toggle SAT sweeping and SAT solving [default = %s]\n", fNewAlgo? "SAT solving": "SAT sweeping" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}
  

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandBmcInter( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Inter_ManParams_t Pars, * pPars = &Pars;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    extern int Abc_NtkDarBmcInter( Abc_Ntk_t * pNtk, Inter_ManParams_t * pPars );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Inter_ManSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CNFrtpomcgbvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesMax < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesK = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesK < 0 ) 
                goto usage;
            break;
        case 'r':
            pPars->fRewrite ^= 1;
            break;
        case 't':
            pPars->fTransLoop ^= 1;
            break;
        case 'p':
            pPars->fUsePudlak ^= 1;
            break;
        case 'o':
            pPars->fUseOther ^= 1;
            break;
        case 'm':
            pPars->fUseMiniSat ^= 1;
            break;
        case 'c':
            pPars->fCheckKstep ^= 1;
            break;
        case 'g':
            pPars->fUseBias ^= 1;
            break;
        case 'b':
            pPars->fUseBackward ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }
    if ( Abc_NtkLatchNum(pNtk) == 0 )
    {
        fprintf( stdout, "Does not work for combinational networks.\n" );
        return 0;
    }
    if ( Abc_NtkPoNum(pNtk) != 1 )
    {
        fprintf( stdout, "Currently only works for single-output miters (run \"orpos\").\n" );
        return 0;
    }
    Abc_NtkDarBmcInter( pNtk, pPars );
    return 0;

usage:
    fprintf( pErr, "usage: int [-CNF num] [-rtpomcgbvh]\n" );
    fprintf( pErr, "\t         uses interpolation to prove the property\n" );
    fprintf( pErr, "\t-C num : the limit on conflicts for one SAT run [default = %d]\n", pPars->nBTLimit );
    fprintf( pErr, "\t-N num : the limit on number of frames to unroll [default = %d]\n", pPars->nFramesMax );
    fprintf( pErr, "\t-F num : the number of steps in inductive checking [default = %d]\n", pPars->nFramesK );
    fprintf( pErr, "\t-r     : toggle rewriting of the unrolled timeframes [default = %s]\n", pPars->fRewrite? "yes": "no" );
    fprintf( pErr, "\t-t     : toggle adding transition into the initial state [default = %s]\n", pPars->fTransLoop? "yes": "no" );
    fprintf( pErr, "\t-p     : toggle using original Pudlak's interpolation procedure [default = %s]\n", pPars->fUsePudlak? "yes": "no" );
    fprintf( pErr, "\t-o     : toggle using optimized Pudlak's interpolation procedure [default = %s]\n", pPars->fUseOther? "yes": "no" );
    fprintf( pErr, "\t-m     : toggle using MiniSat-1.14p (now, Windows-only) [default = %s]\n", pPars->fUseMiniSat? "yes": "no" );
    fprintf( pErr, "\t-c     : toggle using inductive containment check [default = %s]\n", pPars->fCheckKstep? "yes": "no" );
    fprintf( pErr, "\t-g     : toggle using bias for global variables using SAT [default = %s]\n", pPars->fUseBias? "yes": "no" );
    fprintf( pErr, "\t-b     : toggle using backward interpolation [default = %s]\n", pPars->fUseBackward? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandIndcut( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int nFrames;
    int nPref;
    int nClauses;
    int nLutSize;
    int nLevels;
    int nCutsMax;
    int nBatches;
    int fStepUp;
    int fBmc;
    int fRegs;
    int fTarget;
    int fVerbose;
    int fVeryVerbose;
    int c;
    extern int Abc_NtkDarClau( Abc_Ntk_t * pNtk, int nFrames, int nPref, int nClauses, int nLutSize, int nLevels, int nCutsMax, int nBatches, int fStepUp, int fBmc, int fRegs, int fTarget, int fVerbose, int fVeryVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFrames      =    1;
    nPref        =    0;
    nClauses     = 5000;
    nLutSize     =    4;
    nLevels      =    8;
    nCutsMax     =   16;
    nBatches     =    1;
    fStepUp      =    0;
    fBmc         =    1;
    fRegs        =    1;
    fTarget      =    1;
    fVerbose     =    0;
    fVeryVerbose =    0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FPCMLNBsbrtvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nPref = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPref < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nClauses = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nClauses < 0 ) 
                goto usage;
            break;
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-K\" should be followed by an integer.\n" );
                goto usage;
            }
            nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLutSize < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            nLevels = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLevels < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCutsMax < 0 ) 
                goto usage;
            break;
        case 'B':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-B\" should be followed by an integer.\n" );
                goto usage;
            }
            nBatches = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nBatches < 0 ) 
                goto usage;
            break;
        case 's':
            fStepUp ^= 1;
            break;
        case 'b':
            fBmc ^= 1;
            break;
        case 'r':
            fRegs ^= 1;
            break;
        case 't':
            fTarget ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'w':
            fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 0;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }
    if ( nLutSize > 12 )
    {
        fprintf( stdout, "The cut size should be not exceed 12.\n" );
        return 0;
    }
    Abc_NtkDarClau( pNtk, nFrames, nPref, nClauses, nLutSize, nLevels, nCutsMax, nBatches, fStepUp, fBmc, fRegs, fTarget, fVerbose, fVeryVerbose );
    return 0;
usage:
    fprintf( pErr, "usage: indcut [-FPCMLNB num] [-sbrtvh]\n" );
    fprintf( pErr, "\t         K-step induction strengthened with cut properties\n" );
    fprintf( pErr, "\t-F num : number of time frames for induction (1=simple) [default = %d]\n", nFrames );
    fprintf( pErr, "\t-P num : number of time frames in the prefix (0=no prefix) [default = %d]\n", nPref );
    fprintf( pErr, "\t-C num : the max number of clauses to use for strengthening [default = %d]\n", nClauses );
    fprintf( pErr, "\t-M num : the cut size (2 <= M <= 12) [default = %d]\n", nLutSize );
    fprintf( pErr, "\t-L num : the max number of levels for cut computation [default = %d]\n", nLevels );
    fprintf( pErr, "\t-N num : the max number of cuts to compute at a node [default = %d]\n", nCutsMax );
    fprintf( pErr, "\t-B num : the max number of invariant batches to try [default = %d]\n", nBatches );
    fprintf( pErr, "\t-s     : toggle increment cut size in each batch [default = %s]\n", fStepUp? "yes": "no" );
    fprintf( pErr, "\t-b     : toggle enabling BMC check [default = %s]\n", fBmc? "yes": "no" );
    fprintf( pErr, "\t-r     : toggle enabling register clauses [default = %s]\n", fRegs? "yes": "no" );
    fprintf( pErr, "\t-t     : toggle proving target / computing don't-cares [default = %s]\n", fTarget? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
//    fprintf( pErr, "\t-w     : toggle printing very verbose information [default = %s]\n", fVeryVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandEnlarge( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int nFrames;
    int fVerbose;
    int c;
    extern Abc_Ntk_t * Abc_NtkDarEnlarge( Abc_Ntk_t * pNtk, int nFrames, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFrames      = 5;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Fvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFrames < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 0;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }

    // modify the current network
    pNtkRes = Abc_NtkDarEnlarge( pNtk, nFrames, fVerbose );
    if ( pNtkRes == NULL )
    {
        fprintf( pErr, "Target enlargement has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;
usage:
    fprintf( pErr, "usage: enlarge [-F num] [-vh]\n" );
    fprintf( pErr, "\t         performs structural K-step target enlargement\n" );
    fprintf( pErr, "\t-F num : the number of timeframes for enlargement [default = %d]\n", nFrames );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandInduction( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int nFramesMax;
    int nConfMax;
    int fVerbose;
    int c;
    extern void Abc_NtkDarInduction( Abc_Ntk_t * pNtk, int nFramesMax, int nConfMax, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFramesMax =   100;
    nConfMax   =  1000;
    fVerbose   =     0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FCvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFramesMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFramesMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfMax < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 0;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }
    if ( Abc_NtkPoNum(pNtk) != 1 )
    {
        fprintf( pErr, "Currently this command works only for single-output miter.\n" );
        return 0;
    }

    // modify the current network
    Abc_NtkDarInduction( pNtk, nFramesMax, nConfMax, fVerbose );
    return 0;
usage:
    fprintf( pErr, "usage: ind [-FC num] [-vh]\n" );
    fprintf( pErr, "\t         runs K-step induction for the property output\n" );
    fprintf( pErr, "\t-F num : the max number of timeframes [default = %d]\n", nFramesMax );
    fprintf( pErr, "\t-C num : the max number of conflicts by SAT solver [default = %d]\n", nConfMax );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandPBAbstraction( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk, * pNtkRes;
    int nFramesMax;
    int nConfMax;
    int fDynamic;
    int fExtend;
    int fSkipProof;
    int nFramesBmc;
    int nConfMaxBmc;
    int nRatio;
    int fVerbose;
    int c;
    extern Abc_Ntk_t * Abc_NtkDarPBAbstraction( Abc_Ntk_t * pNtk, int nFramesMax, int nConfMax, int fDynamic, int fExtend, int fSkipProof, int nFramesBmc, int nConfMaxBmc, int nRatio, int fVerbose );

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    nFramesMax  =    10;
    nConfMax    = 10000;
    fDynamic    =     1;
    fExtend     =     0;
    fSkipProof  =     0;
    nFramesBmc  =  2000;
    nConfMaxBmc =  5000;
    nRatio      =    10;
    fVerbose    =     0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FCGDRdesvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            nFramesMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFramesMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfMax < 0 ) 
                goto usage;
            break;
        case 'G':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-G\" should be followed by an integer.\n" );
                goto usage;
            }
            nFramesBmc = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFramesBmc < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfMaxBmc = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfMaxBmc < 0 ) 
                goto usage;
            break;
        case 'R':
            if ( globalUtilOptind >= argc )
            {
                fprintf( pErr, "Command line switch \"-R\" should be followed by an integer.\n" );
                goto usage;
            }
            nRatio = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nRatio < 0 ) 
                goto usage;
            break;
        case 'd':
            fDynamic ^= 1;
            break;
        case 'e':
            fExtend ^= 1;
            break;
        case 's':
            fSkipProof ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Abc_NtkIsComb(pNtk) )
    {
        fprintf( pErr, "The network is combinational.\n" );
        return 0;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( stdout, "Currently only works for structurally hashed circuits.\n" );
        return 0;
    }
    if ( !(0 <= nRatio && nRatio <= 100) )
    {
        fprintf( stdout, "Wrong value of parameter \"-R <num>\".\n" );
        return 0;
    }

    // modify the current network
    pNtkRes = Abc_NtkDarPBAbstraction( pNtk, nFramesMax, nConfMax, fDynamic, fExtend, fSkipProof, nFramesBmc, nConfMaxBmc, nRatio, fVerbose );
    if ( pNtkRes == NULL )
    {
        if ( pNtk->pSeqModel == NULL )
            printf( "Proof-based abstraction has failed.\n" );
        return 0;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    return 0;
usage:
    fprintf( pErr, "usage: abs [-FCGDR num] [-desvh]\n" );
    fprintf( pErr, "\t         proof-based abstraction (PBA) using UNSAT core of BMC\n" );
    fprintf( pErr, "\t         followed by counter-example-based abstraction\n" );
    fprintf( pErr, "\t-F num : the max number of timeframes for PBA [default = %d]\n", nFramesMax );
    fprintf( pErr, "\t-C num : the max number of conflicts by SAT solver for PBA [default = %d]\n", nConfMax );
    fprintf( pErr, "\t-G num : the max number of timeframes for BMC [default = %d]\n", nFramesBmc );
    fprintf( pErr, "\t-D num : the max number of conflicts by SAT solver for BMC [default = %d]\n", nConfMaxBmc );
    fprintf( pErr, "\t-R num : the %% of abstracted flops when refinement stops (0<=num<=100) [default = %d]\n", nRatio );
    fprintf( pErr, "\t-d     : toggle dynamic unrolling of timeframes [default = %s]\n", fDynamic? "yes": "no" );
    fprintf( pErr, "\t-e     : toggle extending abstraction using COI of flops [default = %s]\n", fExtend? "yes": "no" );
    fprintf( pErr, "\t-s     : toggle skipping proof-based abstraction [default = %s]\n", fSkipProof? "yes": "no" );
    fprintf( pErr, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandTraceStart( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command is applicable to AIGs.\n" );
        return 1;
    }
/*
    Abc_HManStart();
    if ( !Abc_HManPopulate( pNtk ) )
    {
        fprintf( pErr, "Failed to start the tracing database.\n" );
        return 1;
    }
*/
    return 0;

usage:
    fprintf( pErr, "usage: trace_start [-h]\n" );
    fprintf( pErr, "\t        starts verification tracing\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandTraceCheck( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash(pNtk) )
    {
        fprintf( pErr, "This command is applicable to AIGs.\n" );
        return 1;
    }
/*
    if ( !Abc_HManIsRunning(pNtk) )
    {
        fprintf( pErr, "The tracing database is not available.\n" );
        return 1;
    }

    if ( !Abc_HManVerify( 1, pNtk->Id ) )
        fprintf( pErr, "Verification failed.\n" );
    Abc_HManStop();
*/
    return 0;

usage:
    fprintf( pErr, "usage: trace_check [-h]\n" );
    fprintf( pErr, "\t        checks the current network using verification trace\n" );
    fprintf( pErr, "\t-h    : print the command usage\n");
    return 1;
}



/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Read( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pFile;
    char * pFileName;
    int c;
    int fMapped;
    int fTest;
    int fAlter;
    extern void * Ioa_ReadBlif( char * pFileName, int fCheck );
    extern void Ioa_WriteBlif( void * p, char * pFileName );
    extern Aig_Man_t * Ntl_ManExtract( void * p );
    extern void * Ntl_ManExtractNwk( void * p, Aig_Man_t * pAig, Tim_Man_t * pManTime );
    extern void Ntl_ManPrintStats( void * p );

    // set defaults
    fMapped = 0;
    fTest   = 0;
    fAlter  = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "mtah" ) ) != EOF )
    {
        switch ( c )
        {
        case 'm':
            fMapped ^= 1;
            break;
        case 't':
            fTest ^= 1;
            break;
        case 'a':
            fAlter ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    // get the input file name
    pFileName = argv[globalUtilOptind];
    if ( (pFile = fopen( pFileName, "r" )) == NULL )
    {
        fprintf( stdout, "Cannot open input file \"%s\". ", pFileName );
        if ( (pFileName = Extra_FileGetSimilarName( pFileName, ".blif", NULL, NULL, NULL, NULL )) )
            fprintf( stdout, "Did you mean \"%s\"?", pFileName );
        fprintf( stdout, "\n" );
        return 1;
    }
    fclose( pFile );
    if ( fTest )
    {
        extern void Ntl_ManFree( void * p );
        void * pTemp = Ioa_ReadBlif( pFileName, 1 );
        if ( pTemp )
        {
        Ntl_ManPrintStats( pTemp );
//        Ioa_WriteBlif( pTemp, "test_boxes.blif" );
        Ntl_ManFree( pTemp );
        }
        return 0; 
    }

    Abc_FrameClearDesign(); 
    if ( !fAlter )
    {
        pAbc->pAbc8Ntl = Ioa_ReadBlif( pFileName, 1 );
        if ( pAbc->pAbc8Ntl == NULL )
        {
            printf( "Abc_CommandAbc8Read(): Reading BLIF has failed.\n" );
            return 1;
        }
    }
    else
    {
//        extern void * Nal_ManRead( char * pFileName );
        pAbc->pAbc8Ntl = NULL;
//        pAbc->pAbc8Ntl = Nal_ManRead( pFileName );
//        Ioa_WriteBlif( pAbc->pAbc8Ntl, "test_boxes.blif" );
        if ( pAbc->pAbc8Ntl == NULL )
        {
            printf( "Abc_CommandAbc8Read(): Reading design has failed.\n" );
            return 1;
        }
    }
    pAbc->pAbc8Aig = Ntl_ManExtract( pAbc->pAbc8Ntl );
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8Read(): AIG extraction has failed.\n" );
        return 1;
    }
    if ( fMapped )
    {
        pAbc->pAbc8Nwk = Ntl_ManExtractNwk( pAbc->pAbc8Ntl, pAbc->pAbc8Aig, NULL );
        if ( pAbc->pAbc8Nwk == NULL )
            printf( "Abc_CommandAbc8Read(): Warning! Mapped network is not extracted.\n" );
    }
    return 0;

usage:
    fprintf( stdout, "usage: *r [-mtah]\n" );
    fprintf( stdout, "\t        reads the design with whiteboxes\n" );
    fprintf( stdout, "\t-m    : toggle extracting mapped network [default = %s]\n", fMapped? "yes": "no" );
    fprintf( stdout, "\t-t    : toggle reading in the test mode [default = %s]\n", fTest? "yes": "no" );
    fprintf( stdout, "\t-a    : toggle reading another file type [default = %s]\n", fAlter? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8ReadLogic( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pFile;
    char * pFileName;
    void * pNtkNew;
    int c;
    extern void * Ntl_ManReadNwk( char * pFileName, Aig_Man_t * pAig, Tim_Man_t * pManTime );
    extern Tim_Man_t * Ntl_ManReadTimeMan( void * p );
    extern void Nwk_ManFree( void * p );

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    // get the input file name
    pFileName = argv[globalUtilOptind];
    if ( (pFile = fopen( pFileName, "r" )) == NULL )
    {
        fprintf( stdout, "Cannot open input file \"%s\". ", pFileName );
        if ( (pFileName = Extra_FileGetSimilarName( pFileName, ".blif", NULL, NULL, NULL, NULL )) )
            fprintf( stdout, "Did you mean \"%s\"?", pFileName );
        fprintf( stdout, "\n" );
        return 1;
    }
    fclose( pFile );

    if ( pAbc->pAbc8Ntl == NULL || pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8ReadLogic(): There is no design or its AIG.\n" );
        return 1;
    }

    // read the new logic
    pNtkNew = Ntl_ManReadNwk( pFileName, pAbc->pAbc8Aig, Ntl_ManReadTimeMan(pAbc->pAbc8Ntl) );
    if ( pNtkNew == NULL )
    {
        printf( "Abc_CommandAbc8ReadLogic(): Procedure has failed.\n" );
        return 1;
    }
    if ( pAbc->pAbc8Nwk != NULL )
        Nwk_ManFree( pAbc->pAbc8Nwk );
    pAbc->pAbc8Nwk = pNtkNew;    
    return 0;

usage:
    fprintf( stdout, "usage: *rlogic [-h]\n" );
    fprintf( stdout, "\t        reads the logic part of the design without whiteboxes\n" );
    fprintf( stdout, "\t        and sets the new logic as the current mapped network\n" );
    fprintf( stdout, "\t        (the logic part should be comb and with the same PIs/POs)\n" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Write( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char * pFileName;
    void * pTemp;
    int fAig;
    int fBlif;
    int fCollapsed;
    int c;
    extern void Ioa_WriteBlif( void * p, char * pFileName );
    extern void * Ntl_ManInsertNtk( void * p, void * pNtk );
    extern void * Ntl_ManInsertAig( void * p, Aig_Man_t * pAig );
    extern void * Ntl_ManDup( void * pOld );
    extern void Ntl_ManFree( void * p );
    extern Aig_Man_t * Ntl_ManCollapseSeq( void * p, int nMinDomSize );

    // set defaults
    fAig = 0;
    fBlif = 1;
    fCollapsed = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "abch" ) ) != EOF )
    { 
        switch ( c )
        {
        case 'a':
            fAig ^= 1;
            break;
        case 'b':
            fBlif ^= 1;
            break;
        case 'c':
            fCollapsed ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Write(): There is no design to write.\n" );
        return 1;
    } 
    // create the design to write
    pFileName = argv[globalUtilOptind];
    if ( fAig )
    {
        if ( fCollapsed )
        {
            extern void Ioa_WriteAiger( Aig_Man_t * pMan, char * pFileName, int fWriteSymbols, int fCompact );
            pTemp = Ntl_ManCollapseSeq( pAbc->pAbc8Ntl, 0 );
            if ( fBlif )
                Saig_ManDumpBlif( pTemp, pFileName );
            else
                Ioa_WriteAiger( pTemp, pFileName, 0, 0 );
            Aig_ManStop( pTemp );
        }
        else
        {
            if ( pAbc->pAbc8Aig != NULL )
            {
                if ( fBlif )
                {
                    pTemp = Ntl_ManInsertAig( pAbc->pAbc8Ntl, pAbc->pAbc8Aig );
                    if ( pTemp == NULL )
                    {
                        printf( "Abc_CommandAbc8Write(): Inserting AIG has failed.\n" );
                        return 1;
                    }
                    Ioa_WriteBlif( pTemp, pFileName );
                    Ntl_ManFree( pTemp );
                }
                else
                    Ioa_WriteAiger( pAbc->pAbc8Aig, pFileName, 0, 0 );
            }
            else
            {
                printf( "There is no AIG to write.\n" );
                return 1;
            }
        }
    }
    else 
    {
        if ( pAbc->pAbc8Nwk != NULL ) 
        {
            pTemp = Ntl_ManInsertNtk( pAbc->pAbc8Ntl, pAbc->pAbc8Nwk );
            if ( pTemp == NULL )
            {
                printf( "Abc_CommandAbc8Write():  Inserting mapped network has failed.\n" );
                return 1;
            }
            Ioa_WriteBlif( pTemp, pFileName );
            Ntl_ManFree( pTemp );
        }
        else
        {
            printf( "Writing the unmapped netlist.\n" );
            pTemp = pAbc->pAbc8Ntl;
            Ioa_WriteBlif( pTemp, pFileName );
        }
    }
    return 0;

usage:
    fprintf( stdout, "usage: *w [-abch]\n" );
    fprintf( stdout, "\t        write the design with whiteboxes\n" );
    fprintf( stdout, "\t-a    : toggle writing mapped network or AIG [default = %s]\n", fAig? "AIG": "mapped" );
    fprintf( stdout, "\t-b    : toggle writing AIG as BLIF or AIGER [default = %s]\n", fBlif? "BLIF": "AIGER" );
    fprintf( stdout, "\t-c    : toggle writing collapsed sequential AIG [default = %s]\n", fCollapsed? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8WriteLogic( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Vec_Ptr_t * vCiNames = NULL, * vCoNames = NULL;
    char * pFileName;
    int fAig;
    int c;
    extern Vec_Ptr_t * Ntl_ManCollectCiNames( void * p );
    extern Vec_Ptr_t * Ntl_ManCollectCoNames( void * p );
    extern void Nwk_ManDumpBlif( void * p, char * pFileName, Vec_Ptr_t * vCiNames, Vec_Ptr_t * vCoNames );

    // set defaults
    fAig = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ah" ) ) != EOF )
    {
        switch ( c )
        {
        case 'a':
            fAig ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Write(): There is no design to write.\n" );
        return 1;
    }
    // create the design to write
    pFileName = argv[globalUtilOptind];
//    vCiNames = Ntl_ManCollectCiNames( pAbc->pAbc8Ntl );
//    vCoNames = Ntl_ManCollectCoNames( pAbc->pAbc8Ntl );
    // the problem is duplicated CO names...
    if ( fAig )
    {
        if ( pAbc->pAbc8Aig != NULL )
            Aig_ManDumpBlif( pAbc->pAbc8Aig, pFileName, vCiNames, vCoNames );
        else
        {
            printf( "There is no AIG to write.\n" );
            return 1;
        }
    }
    else 
    {
        if ( pAbc->pAbc8Nwk != NULL ) 
            Nwk_ManDumpBlif( pAbc->pAbc8Nwk, pFileName, vCiNames, vCoNames );
        else
        {
            printf( "There is no mapped network to write.\n" );
            return 1;
        }
    }
    if ( vCiNames )  Vec_PtrFree( vCiNames );
    if ( vCoNames )  Vec_PtrFree( vCoNames );
    return 0;

usage:
    fprintf( stdout, "usage: *wlogic [-ah]\n" );
    fprintf( stdout, "\t        write the logic part of the design without whiteboxes\n" );
    fprintf( stdout, "\t-a    : toggle writing mapped network or AIG [default = %s]\n", fAig? "AIG": "mapped" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    [Command procedure to read LUT libraries.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8ReadLut( Abc_Frame_t * pAbc, int argc, char **argv )
{
    FILE * pFile;
    char * FileName;
    void * pLib;
    int c;
    extern If_Lib_t * If_LutLibRead( char * FileName );
    extern void If_LutLibFree( If_Lib_t * pLutLib );

    // set the defaults
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "h")) != EOF ) 
    {
        switch (c) 
        {
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    }


    if ( argc != globalUtilOptind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[globalUtilOptind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( stdout, "Cannot open input file \"%s\". ", FileName );
        if ( (FileName = Extra_FileGetSimilarName( FileName, ".lut", NULL, NULL, NULL, NULL )) )
            fprintf( stdout, "Did you mean \"%s\"?", FileName );
        fprintf( stdout, "\n" );
        return 1;
    }
    fclose( pFile );

    // set the new network
    pLib = If_LutLibRead( FileName );
    if ( pLib == NULL )
    {
        fprintf( stdout, "Reading LUT library has failed.\n" );
        goto usage;
    }
    // replace the current library
    if ( pAbc->pAbc8Lib != NULL )
        If_LutLibFree( pAbc->pAbc8Lib );
    pAbc->pAbc8Lib = pLib;
    return 0;

usage:
    fprintf( stdout, "\nusage: *rlut [-h]\n");
    fprintf( stdout, "\t          read the LUT library from the file\n" );  
    fprintf( stdout, "\t-h      : print the command usage\n");
    fprintf( stdout, "\t                                        \n");
    fprintf( stdout, "\t          File format for a LUT library:\n");
    fprintf( stdout, "\t          (the default library is shown)\n");
    fprintf( stdout, "\t                                        \n");
    fprintf( stdout, "\t          # The area/delay of k-variable LUTs:\n");
    fprintf( stdout, "\t          # k  area   delay\n");
    fprintf( stdout, "\t          1      1      1\n");
    fprintf( stdout, "\t          2      2      2\n");
    fprintf( stdout, "\t          3      4      3\n");
    fprintf( stdout, "\t          4      8      4\n");
    fprintf( stdout, "\t          5     16      5\n");
    fprintf( stdout, "\t          6     32      6\n");
    return 1;       /* error exit */
}

/**Function*************************************************************

  Synopsis    [Command procedure to read LUT libraries.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8PrintLut( Abc_Frame_t * pAbc, int argc, char **argv )
{
    int c;
    extern void If_LutLibPrint( If_Lib_t * pLutLib );

    // set the defaults
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "h")) != EOF ) 
    {
        switch (c) 
        {
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    }

    if ( argc != globalUtilOptind )
    {
        goto usage;
    }

    // set the new network
    if ( pAbc->pAbc8Lib == NULL )
        printf( "Abc_CommandAbc8PrintLut(): LUT library is not specified.\n" );
    else
        If_LutLibPrint( pAbc->pAbc8Lib );
    return 0;

usage:
    fprintf( stdout, "\nusage: *plut [-h]\n");
    fprintf( stdout, "\t          print the current LUT library\n" );  
    fprintf( stdout, "\t-h      : print the command usage\n");
    return 1;       /* error exit */
}

/**Function*************************************************************

  Synopsis    [Command procedure to read LUT libraries.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Check( Abc_Frame_t * pAbc, int argc, char **argv )
{
    int c;
    extern int Nwk_ManCheck( void * p );

    // set the defaults
    Extra_UtilGetoptReset();
    while ( (c = Extra_UtilGetopt(argc, argv, "h")) != EOF ) 
    {
        switch (c) 
        {
            case 'h':
                goto usage;
                break;
            default:
                goto usage;
        }
    }

    if ( argc != globalUtilOptind )
    {
        goto usage;
    }

    // set the new network
    if ( pAbc->pAbc8Nwk == NULL )
        printf( "Abc_CommandAbc8Check(): There is no mapped network.\n" );
    else
        Nwk_ManCheck( pAbc->pAbc8Nwk );
    return 0;

usage:
    fprintf( stdout, "\nusage: *check [-h]\n");
    fprintf( stdout, "\t          checks if the current mapped network has duplicated fanins\n" );  
    fprintf( stdout, "\t-h      : print the command usage\n");
    return 1;       /* error exit */
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Ps( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    int c;
    int fSaveBest;
    int fDumpResult;
    int fPower;
    int fShort;
    extern void Ntl_ManPrintStats( void * p );
    extern void Nwk_ManPrintStats( void * p, void * pLutLib, int fSaveBest, int fDumpResult, int fPower, void * pNtl );
    extern void Nwk_ManPrintStatsShort( void * p, void * pAig, void * pNtk );

    // set defaults
    fSaveBest = 0;
    fDumpResult = 0;
    fPower = 0;
    fShort = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "bdpsh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'b':
            fSaveBest ^= 1;
            break;
        case 'd':
            fDumpResult ^= 1;
            break;
        case 'p':
            fPower ^= 1;
            break;
        case 's':
            fShort ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Ps(): There is no design to show.\n" );
        return 1;
    }

    if ( fShort )
    {
        Nwk_ManPrintStatsShort( pAbc->pAbc8Ntl, pAbc->pAbc8Aig, pAbc->pAbc8Nwk );
        return 0;
    }
    // get the input file name
    if ( pAbc->pAbc8Ntl )
    {
        printf( "NETLIST: " );
        Ntl_ManPrintStats( pAbc->pAbc8Ntl );
    }
    if ( pAbc->pAbc8Aig )
    {
        printf( "AIG:     " );
        Aig_ManPrintStats( pAbc->pAbc8Aig );
    }
    if ( pAbc->pAbc8Nwk )
    {
        if ( pAbc->pAbc8Lib == NULL )
        {
            printf( "LUT library is not given. Using default LUT library.\n" );
            pAbc->pAbc8Lib = If_SetSimpleLutLib( 6 );
        }
        printf( "MAPPED:  " );
        Nwk_ManPrintStats( pAbc->pAbc8Nwk, pAbc->pAbc8Lib, fSaveBest, fDumpResult, fPower, pAbc->pAbc8Ntl );
    }
    return 0;

usage:
    fprintf( stdout, "usage: *ps [-bdpsh]\n" );
    fprintf( stdout, "\t        prints design statistics\n" );
    fprintf( stdout, "\t-b    : toggles saving the best logic network in \"best.blif\" [default = %s]\n", fSaveBest? "yes": "no" );
    fprintf( stdout, "\t-d    : toggles dumping network into file \"<input_file_name>_dump.blif\" [default = %s]\n", fDumpResult? "yes": "no" );
    fprintf( stdout, "\t-p    : toggles printing power dissipation due to switching [default = %s]\n", fPower? "yes": "no" );
    fprintf( stdout, "\t-s    : toggles short printing mode [default = %s]\n", fShort? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Pfan( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    int c;
    extern void Nwk_ManPrintFanioNew( void * p );

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Nwk == NULL )
    {
        printf( "Abc_CommandAbc8Pfan(): There is no mapped network for print fanin/fanout.\n" );
        return 1;
    }
    Nwk_ManPrintFanioNew( pAbc->pAbc8Nwk );
    return 0;

usage:
    fprintf( stdout, "usage: *pfan [-h]\n" );
    fprintf( stdout, "\t        prints fanin/fanout stats of the mapped network\n" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8If( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char Buffer[200];
    char LutSize[200];
    If_Par_t Pars, * pPars = &Pars;
    void * pNtkNew;
    int c;
    extern void * Nwk_MappingIf( Aig_Man_t * p, Tim_Man_t * pManTime, If_Par_t * pPars );
    extern Tim_Man_t * Ntl_ManReadTimeMan( void * p );
    extern If_Lib_t * If_SetSimpleLutLib( int nLutSize );
    extern void Nwk_ManSetIfParsDefault( If_Par_t * pPars );
    extern void Nwk_ManFree( void * p );

    if ( pAbc->pAbc8Lib == NULL )
    {
        printf( "LUT library is not given. Using default LUT library.\n" );
        pAbc->pAbc8Lib = If_SetSimpleLutLib( 6 );
    }

    // set defaults
    Nwk_ManSetIfParsDefault( pPars );
    pPars->pLutLib = pAbc->pAbc8Lib;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "KCFADEqaflepmrstbvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-K\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nLutSize < 0 ) 
                goto usage;
            // if the LUT size is specified, disable library
            pPars->pLutLib = NULL; 
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nCutsMax < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nFlowIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFlowIters < 0 ) 
                goto usage;
            break;
        case 'A':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-A\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nAreaIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nAreaIters < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by a floating point number.\n" );
                goto usage;
            }
            pPars->DelayTarget = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->DelayTarget <= 0.0 ) 
                goto usage;
            break;
        case 'E':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-E\" should be followed by a floating point number.\n" );
                goto usage;
            }
            pPars->Epsilon = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->Epsilon < 0.0 || pPars->Epsilon > 1.0 ) 
                goto usage;
            break;
        case 'q':
            pPars->fPreprocess ^= 1;
            break;
        case 'a':
            pPars->fArea ^= 1;
            break;
        case 'r':
            pPars->fExpRed ^= 1;
            break;
        case 'f':
            pPars->fFancy ^= 1;
            break;
        case 'l':
            pPars->fLatchPaths ^= 1;
            break;
        case 'e':
            pPars->fEdge ^= 1;
            break;
        case 'p':
            pPars->fPower ^= 1;
            break;
        case 'm':
            pPars->fCutMin ^= 1;
            break;
        case 's':
            pPars->fSeqMap ^= 1;
            break;
        case 't':
            pPars->fLiftLeaves ^= 1;
            break;
        case 'b':
            pPars->fBidec ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8If(): There is no AIG to map.\n" );
        return 1;
    }

    if ( pPars->nLutSize < 3 || pPars->nLutSize > IF_MAX_LUTSIZE )
    {
        fprintf( stdout, "Incorrect LUT size (%d).\n", pPars->nLutSize );
        return 1;
    }

    if ( pPars->nCutsMax < 1 || pPars->nCutsMax >= (1<<12) )
    {
        fprintf( stdout, "Incorrect number of cuts.\n" );
        return 1;
    }

    // enable truth table computation if choices are selected
    if ( (c = Aig_ManChoiceNum( pAbc->pAbc8Aig )) )
    {
        printf( "Performing LUT mapping with %d choices.\n", c );
        pPars->fExpRed = 0;
    }
    // enable truth table computation if cut minimization is selected
    if ( pPars->fCutMin )
    {
        pPars->fTruth = 1;
        pPars->fExpRed = 0;
    }

    // complain if truth tables are requested but the cut size is too large
    if ( pPars->fTruth && pPars->nLutSize > IF_MAX_FUNC_LUTSIZE )
    {
        fprintf( stdout, "Truth tables cannot be computed for LUT larger than %d inputs.\n", IF_MAX_FUNC_LUTSIZE );
        return 1;
    }

    pNtkNew = Nwk_MappingIf( pAbc->pAbc8Aig, Ntl_ManReadTimeMan(pAbc->pAbc8Ntl), pPars );
    if ( pNtkNew == NULL )
    {
        printf( "Abc_CommandAbc8If(): Mapping of the AIG has failed.\n" );
        return 1;
    }
    if ( pAbc->pAbc8Nwk != NULL )
        Nwk_ManFree( pAbc->pAbc8Nwk );
    pAbc->pAbc8Nwk = pNtkNew;
    return 0;

usage:
    if ( pPars->DelayTarget == -1 ) 
        sprintf( Buffer, "best possible" );
    else
        sprintf( Buffer, "%.2f", pPars->DelayTarget );
    if ( pPars->nLutSize == -1 ) 
        sprintf( LutSize, "library" );
    else
        sprintf( LutSize, "%d", pPars->nLutSize );
    fprintf( stdout, "usage: *if [-KCFA num] [-DE float] [-qarlepmbvh]\n" );
    fprintf( stdout, "\t           performs FPGA technology mapping of the network\n" );
    fprintf( stdout, "\t-K num   : the number of LUT inputs (2 < num < %d) [default = %s]\n", IF_MAX_LUTSIZE+1, LutSize );
    fprintf( stdout, "\t-C num   : the max number of priority cuts (0 < num < 2^12) [default = %d]\n", pPars->nCutsMax );
    fprintf( stdout, "\t-F num   : the number of area flow recovery iterations (num >= 0) [default = %d]\n", pPars->nFlowIters );
    fprintf( stdout, "\t-A num   : the number of exact area recovery iterations (num >= 0) [default = %d]\n", pPars->nAreaIters );
    fprintf( stdout, "\t-D float : sets the delay constraint for the mapping [default = %s]\n", Buffer );  
    fprintf( stdout, "\t-E float : sets epsilon used for tie-breaking [default = %f]\n", pPars->Epsilon );  
    fprintf( stdout, "\t-q       : toggles preprocessing using several starting points [default = %s]\n", pPars->fPreprocess? "yes": "no" );
    fprintf( stdout, "\t-a       : toggles area-oriented mapping [default = %s]\n", pPars->fArea? "yes": "no" );
//    fprintf( stdout, "\t-f       : toggles one fancy feature [default = %s]\n", pPars->fFancy? "yes": "no" );
    fprintf( stdout, "\t-r       : enables expansion/reduction of the best cuts [default = %s]\n", pPars->fExpRed? "yes": "no" );
    fprintf( stdout, "\t-l       : optimizes latch paths for delay, other paths for area [default = %s]\n", pPars->fLatchPaths? "yes": "no" );
    fprintf( stdout, "\t-e       : uses edge-based cut selection heuristics [default = %s]\n", pPars->fEdge? "yes": "no" );
    fprintf( stdout, "\t-p       : uses power-aware cut selection heuristics [default = %s]\n", pPars->fPower? "yes": "no" );
    fprintf( stdout, "\t-m       : enables cut minimization by removing vacuous variables [default = %s]\n", pPars->fCutMin? "yes": "no" );
//    fprintf( stdout, "\t-s       : toggles sequential mapping [default = %s]\n", pPars->fSeqMap? "yes": "no" );
//    fprintf( stdout, "\t-t       : toggles the use of true sequential cuts [default = %s]\n", pPars->fLiftLeaves? "yes": "no" );
    fprintf( stdout, "\t-b       : toggles deriving local AIGs using bi-decomposition [default = %s]\n", pPars->fBidec? "yes": "no" );
    fprintf( stdout, "\t-v       : toggles verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h       : prints the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8DChoice( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Aig_Man_t * pAigNew;
    int fBalance, fVerbose, fUpdateLevel, fConstruct, c;
    int nConfMax, nLevelMax;
    extern Aig_Man_t * Ntl_ManPerformChoicing( Aig_Man_t * pAig, int fBalance, int fUpdateLevel, int fConstruct, int nConfMax, int nLevelMax, int fVerbose );

    // set defaults
    fBalance     = 1;
    fUpdateLevel = 1;
    fConstruct   = 0;
    nConfMax     = 1000;
    nLevelMax    = 0;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CLblcvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfMax < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            nLevelMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLevelMax < 0 ) 
                goto usage;
            break;
        case 'b':
            fBalance ^= 1;
            break;
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'c':
            fConstruct ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8DChoice(): There is no AIG to synthesize.\n" );
        return 1;
    }

    // get the input file name
    pAigNew = Ntl_ManPerformChoicing( pAbc->pAbc8Aig, fBalance, fUpdateLevel, fConstruct, nConfMax, nLevelMax, fVerbose );
    if ( pAigNew == NULL )
    {
        printf( "Abc_CommandAbc8DChoice(): Tranformation of the AIG has failed.\n" );
        return 1;
    }
    Aig_ManStop( pAbc->pAbc8Aig );
    pAbc->pAbc8Aig = pAigNew;
    return 0;

usage:
    fprintf( stdout, "usage: *dchoice [-C num] [-L num] [-blcvh]\n" );
    fprintf( stdout, "\t         performs AIG-based synthesis and derives choices\n" );
    fprintf( stdout, "\t-C num : the max number of conflicts at a node [default = %d]\n", nConfMax );
    fprintf( stdout, "\t-L num : the max level of nodes to consider (0 = not used) [default = %d]\n", nLevelMax );
    fprintf( stdout, "\t-b     : toggle internal balancing [default = %s]\n", fBalance? "yes": "no" );
    fprintf( stdout, "\t-l     : toggle updating level [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( stdout, "\t-c     : toggle constructive computation of choices [default = %s]\n", fConstruct? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Dch( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Dch_Pars_t Pars, * pPars = &Pars;
    Aig_Man_t * pAigNew;
    int c;
    extern Aig_Man_t * Ntl_ManPerformChoicingNew( Aig_Man_t * pAig, Dch_Pars_t * pPars );

    // set defaults
    Dch_ManSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "WCSsptvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWords = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWords < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nSatVarMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nSatVarMax < 0 ) 
                goto usage;
            break;
        case 's':
            pPars->fSynthesis ^= 1;
            break;
        case 'p':
            pPars->fPower ^= 1;
            break;
        case 't':
            pPars->fSimulateTfo ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8Dch(): There is no AIG to synthesize.\n" );
        return 1;
    }

    // get the input file name
    pAigNew = Ntl_ManPerformChoicingNew( pAbc->pAbc8Aig, pPars );
    if ( pAigNew == NULL )
    {
        printf( "Abc_CommandAbc8Dch(): Tranformation of the AIG has failed.\n" );
        return 1;
    }
    Aig_ManStop( pAbc->pAbc8Aig );
    pAbc->pAbc8Aig = pAigNew;
    return 0;

usage:
    fprintf( stdout, "usage: *dch [-WCS num] [-sptvh]\n" );
    fprintf( stdout, "\t         computes structural choices using a new approach\n" );
    fprintf( stdout, "\t-W num : the max number of simulation words [default = %d]\n", pPars->nWords );
    fprintf( stdout, "\t-C num : the max number of conflicts at a node [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-S num : the max number of SAT variables [default = %d]\n", pPars->nSatVarMax );
    fprintf( stdout, "\t-s     : toggle synthesizing three snapshots [default = %s]\n", pPars->fSynthesis? "yes": "no" );
    fprintf( stdout, "\t-p     : toggle power-aware rewriting [default = %s]\n", pPars->fPower? "yes": "no" );
    fprintf( stdout, "\t-t     : toggle simulation of the TFO classes [default = %s]\n", pPars->fSimulateTfo? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle verbose printout [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8DC2( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Aig_Man_t * pAigNew;
    int c;
    int fBalance;
    int fUpdateLevel;
    int fVerbose;
    int fPower;

    extern Aig_Man_t * Dar_ManCompress2( Aig_Man_t * pAig, int fBalance, int fUpdateLevel, int fFanout, int fPower, int fVerbose );

    // set defaults
    fBalance     = 1;
    fUpdateLevel = 1;
    fPower       = 0;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "blpvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'b':
            fBalance ^= 1;
            break;
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'p':
            fPower ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8DC2(): There is no AIG to synthesize.\n" );
        return 1;
    }

    // get the input file name
    pAigNew = Dar_ManCompress2( pAbc->pAbc8Aig, fBalance, fUpdateLevel, 1, fPower, fVerbose );
    if ( pAigNew == NULL )
    {
        printf( "Abc_CommandAbc8DC2(): Tranformation of the AIG has failed.\n" );
        return 1;
    }
    Aig_ManStop( pAbc->pAbc8Aig );
    pAbc->pAbc8Aig = pAigNew;
    return 0;

usage:
    fprintf( stdout, "usage: *dc2 [-blpvh]\n" );
    fprintf( stdout, "\t        performs AIG-based synthesis without deriving choices\n" );
    fprintf( stdout, "\t-b    : toggle internal balancing [default = %s]\n", fBalance? "yes": "no" );
    fprintf( stdout, "\t-l    : toggle updating level [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( stdout, "\t-p    : toggle power-aware rewriting [default = %s]\n", fPower? "yes": "no" );
    fprintf( stdout, "\t-v    : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Bidec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    int c;
    extern void Nwk_ManBidecResyn( void * pNtk, int fVerbose );

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Nwk == NULL )
    {
        printf( "Abc_CommandAbc8Bidec(): There is no mapped network to strash.\n" );
        return 1;
    }
    Nwk_ManBidecResyn( pAbc->pAbc8Nwk, 0 );
    return 0;

usage:
    fprintf( stdout, "usage: *bidec [-h]\n" );
    fprintf( stdout, "\t        performs bi-decomposition of local functions\n" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Strash( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Aig_Man_t * pAigNew;
    int c;
    extern Aig_Man_t * Nwk_ManStrash( void * pNtk );

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Nwk == NULL )
    {
        printf( "Abc_CommandAbc8Strash(): There is no mapped network to strash.\n" );
        return 1;
    } 

    pAigNew = Nwk_ManStrash( pAbc->pAbc8Nwk );
    if ( pAigNew == NULL )
    {
        printf( "Abc_CommandAbc8Strash(): Tranformation of the AIG has failed.\n" );
        return 1;
    }
    Aig_ManStop( pAbc->pAbc8Aig );
    pAbc->pAbc8Aig = pAigNew;
    return 0;

usage:
    fprintf( stdout, "usage: *st [-h]\n" );
    fprintf( stdout, "\t        performs structural hashing of mapped network\n" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Mfs( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Mfx_Par_t Pars, * pPars = &Pars;
    int c;
    extern int Mfx_Perform( void * pNtk, Mfx_Par_t * pPars, If_Lib_t * pLutLib );

    // set defaults
    Mfx_ParsDefault( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "WFDMLCraespvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWinTfoLevs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWinTfoLevs < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFanoutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFanoutsMax < 1 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nDepthMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nDepthMax < 0 ) 
                goto usage;
            break;
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-M\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWinSizeMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWinSizeMax < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nGrowthLevel = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nGrowthLevel < 0 || pPars->nGrowthLevel > ABC_INFINITY ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'r':
            pPars->fResub ^= 1;
            break;
        case 'a':
            pPars->fArea ^= 1;
            break;
        case 'e':
            pPars->fMoreEffort ^= 1;
            break;
        case 's':
            pPars->fSwapEdge ^= 1;
            break;
        case 'p':
            pPars->fPower ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Nwk == NULL )
    {
        printf( "Abc_CommandAbc8Mfs(): There is no mapped network.\n" );
        return 1;
    }
    if ( pAbc->pAbc8Lib == NULL )
    {
        printf( "Abc_CommandAbc8Mfs(): There is no LUT library.\n" );
        return 1;
    }
    if ( If_LutLibDelaysAreDifferent(pAbc->pAbc8Lib) )
    {
        printf( "Abc_CommandAbc8Mfs(): Cannot perform don't-care simplication with variable-pin-delay LUT model.\n" );
        printf( "The delay model should be fixed-pin-delay, for example, the delay of all pins of all LUTs is 0.4.\n" );
        return 1;
    }


    // modify the current network
    if ( !Mfx_Perform( pAbc->pAbc8Nwk, pPars, pAbc->pAbc8Lib ) )
    {
        fprintf( stdout, "Abc_CommandAbc8Mfs(): Command has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( stdout, "usage: *mfs [-WFDMLC <num>] [-raespvh]\n" );
    fprintf( stdout, "\t           performs don't-care-based optimization of logic networks\n" );
    fprintf( stdout, "\t-W <num> : the number of levels in the TFO cone (0 <= num) [default = %d]\n", pPars->nWinTfoLevs );
    fprintf( stdout, "\t-F <num> : the max number of fanouts to skip (1 <= num) [default = %d]\n", pPars->nFanoutsMax );
    fprintf( stdout, "\t-D <num> : the max depth nodes to try (0 = no limit) [default = %d]\n", pPars->nDepthMax );
    fprintf( stdout, "\t-M <num> : the max node count of windows to consider (0 = no limit) [default = %d]\n", pPars->nWinSizeMax );
    fprintf( stdout, "\t-L <num> : the max increase in node level after resynthesis (0 <= num) [default = %d]\n", pPars->nGrowthLevel );
    fprintf( stdout, "\t-C <num> : the max number of conflicts in one SAT run (0 = no limit) [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-r       : toggle resubstitution and dc-minimization [default = %s]\n", pPars->fResub? "resub": "dc-min" );
    fprintf( stdout, "\t-a       : toggle minimizing area or area+edges [default = %s]\n", pPars->fArea? "area": "area+edges" );
    fprintf( stdout, "\t-e       : toggle high-effort resubstitution [default = %s]\n", pPars->fMoreEffort? "yes": "no" );
    fprintf( stdout, "\t-s       : toggle evaluation of edge swapping [default = %s]\n", pPars->fSwapEdge? "yes": "no" );
    fprintf( stdout, "\t-p       : toggle power-aware optimization [default = %s]\n", pPars->fPower? "yes": "no" );
    fprintf( stdout, "\t-v       : toggle printing optimization summary [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-w       : toggle printing detailed stats for each node [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( stdout, "\t-h       : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Lutpack( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    int c;

    printf( "This command is temporarily disabled.\n" );
    return 0;

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Nwk == NULL )
    {
        printf( "Abc_CommandAbc8Lutpack(): There is no mapped network to strash.\n" );
        return 1;
    }

    
    return 0;
usage:
/*
    fprintf( stdout, "usage: *lp [-h]\n" );
    fprintf( stdout, "usage: lutpack [-N <num>] [-Q <num>] [-S <num>] [-L <num>] [-szfovwh]\n" );
    fprintf( stdout, "\t           performs \"rewriting\" for LUT network;\n" );
    fprintf( stdout, "\t           determines LUT size as the max fanin count of a node;\n" );
    fprintf( stdout, "\t           if the network is not LUT-mapped, packs it into 6-LUTs\n" );
    fprintf( stdout, "\t           (there is another command for resynthesis after LUT mapping, \"imfs\")\n" );
    fprintf( stdout, "\t-N <num> : the max number of LUTs in the structure (2 <= num) [default = %d]\n", pPars->nLutsMax );
    fprintf( stdout, "\t-Q <num> : the max number of LUTs not in MFFC (0 <= num) [default = %d]\n", pPars->nLutsOver );
    fprintf( stdout, "\t-S <num> : the max number of LUT inputs shared (0 <= num <= 3) [default = %d]\n", pPars->nVarsShared );
    fprintf( stdout, "\t-L <num> : max level increase after resynthesis (0 <= num) [default = %d]\n", pPars->nGrowthLevel );
    fprintf( stdout, "\t-s       : toggle iteration till saturation [default = %s]\n", pPars->fSatur? "yes": "no" );
    fprintf( stdout, "\t-z       : toggle zero-cost replacements [default = %s]\n", pPars->fZeroCost? "yes": "no" );
    fprintf( stdout, "\t-f       : toggle using only first node and first cut [default = %s]\n", pPars->fFirst? "yes": "no" );
    fprintf( stdout, "\t-o       : toggle using old implementation [default = %s]\n", pPars->fOldAlgo? "yes": "no" );
    fprintf( stdout, "\t-v       : toggle verbose printout [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-w       : toggle detailed printout of decomposed functions [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( stdout, "\t-h       : print the command usage\n");
*/
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Balance( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Aig_Man_t * pAigNew;
    int c;
    int fExor;
    int fUpdateLevel;
    int fVerbose;
    extern Aig_Man_t *  Dar_ManBalanceXor( Aig_Man_t * pAig, int fExor, int fUpdateLevel, int fVerbose );

    // set defaults
    fExor        = 0;
    fUpdateLevel = 1;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "xlh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'x':
            fExor ^= 1;
            break;
        case 'l':
            fUpdateLevel ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8Balance(): There is no AIG to synthesize.\n" );
        return 1;
    }

    // get the input file name
    pAigNew = Dar_ManBalanceXor( pAbc->pAbc8Aig, fExor, fUpdateLevel, fVerbose );
    if ( pAigNew == NULL )
    {
        printf( "Abc_CommandAbc8Balance(): Tranformation of the AIG has failed.\n" );
        return 1;
    }
    Aig_ManStop( pAbc->pAbc8Aig );
    pAbc->pAbc8Aig = pAigNew;
    return 0;

usage:
    fprintf( stdout, "usage: *b [-xlvh]\n" );
    fprintf( stdout, "\t        performs balanacing of the AIG\n" );
    fprintf( stdout, "\t-x    : toggle using XOR-balancing [default = %s]\n", fExor? "yes": "no" );
    fprintf( stdout, "\t-l    : toggle updating level [default = %s]\n", fUpdateLevel? "yes": "no" );
    fprintf( stdout, "\t-v    : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Speedup( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Aig_Man_t * pAigNew;
    int c;
    int fUseLutLib = 0;
    int Percentage = 100;
    int Degree = 5;
    int fVerbose = 0;
    int fVeryVerbose = 0;
    extern Aig_Man_t * Nwk_ManSpeedup( void * pNtk, int fUseLutLib, int Percentage, int Degree, int fVerbose, int fVeryVerbose );

    // set defaults
    fUseLutLib = 0;
    Percentage = 5;
    Degree     = 2;
    fVerbose   = 0;
    fVeryVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PNlvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            Percentage = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Percentage < 1 || Percentage > 100 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            Degree = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( Degree < 1 || Degree > 5 ) 
                goto usage;
            break;
        case 'l':
            fUseLutLib ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'w':
            fVeryVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Nwk == NULL )
    {
        printf( "Abc_CommandAbc8Speedup(): There is no mapped network to strash.\n" );
        return 1;
    }
       
    pAigNew = Nwk_ManSpeedup( pAbc->pAbc8Nwk, fUseLutLib, Percentage, Degree, fVerbose, fVeryVerbose );
    if ( pAigNew == NULL )
    {
        printf( "Abc_CommandAbc8Speedup(): Tranformation of the AIG has failed.\n" );
        return 1;
    }
    Aig_ManStop( pAbc->pAbc8Aig );
    pAbc->pAbc8Aig = pAigNew;
    return 0;

usage:
    fprintf( stdout, "usage: *speedup [-P num] [-N num] [-lvwh]\n" );
    fprintf( stdout, "\t           transforms LUT-mapped network into an AIG with choices;\n" );
    fprintf( stdout, "\t           the choices are added to speedup the next round of mapping\n" );
    fprintf( stdout, "\t-P <num> : delay delta defining critical path for library model [default = %d%%]\n", Percentage );
    fprintf( stdout, "\t-N <num> : the max critical path degree for resynthesis (0 < num < 6) [default = %d]\n", Degree );
    fprintf( stdout, "\t-l       : toggle using unit- or LUT-library-delay model [default = %s]\n", fUseLutLib? "lib" : "unit" );
    fprintf( stdout, "\t-v       : toggle printing optimization summary [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-w       : toggle printing detailed stats for each node [default = %s]\n", fVeryVerbose? "yes": "no" );
    fprintf( stdout, "\t-h       : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Merge( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Nwk_LMPars_t Pars, * pPars = &Pars;
    Vec_Int_t * vResult;
    int c;
    extern Vec_Int_t * Nwk_ManLutMerge( void * pNtk, Nwk_LMPars_t * pPars );

    // set defaults
    memset( pPars, 0, sizeof(Nwk_LMPars_t) );
    pPars->nMaxLutSize    = 5;   // the max LUT size for merging (N=5)
    pPars->nMaxSuppSize   = 5;   // the max total support size after merging (S=5)
    pPars->nMaxDistance   = 3;   // the max number of nodes separating LUTs
    pPars->nMaxLevelDiff  = 2;   // the max difference in levels
    pPars->nMaxFanout     = 100; // the max number of fanouts to traverse
    pPars->fUseDiffSupp   = 0;   // enables the use of nodes with different support
    pPars->fUseTfiTfo     = 0;   // enables the use of TFO/TFO nodes as candidates
    pPars->fVeryVerbose   = 0;   // enables additional verbose output
    pPars->fVerbose       = 1;   // enables verbose output
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "NSDLFscvwh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxLutSize < 2 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxSuppSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxSuppSize < 2 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxDistance = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxDistance < 2 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxLevelDiff = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxLevelDiff < 2 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxFanout = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxFanout < 2 ) 
                goto usage;
            break;
        case 's':
            pPars->fUseDiffSupp ^= 1;
            break;
        case 'c':
            pPars->fUseTfiTfo ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Nwk == NULL )
    {
        printf( "Abc_CommandAbc8Speedup(): There is no mapped network to merge LUTs.\n" );
        return 1;
    }
       
    vResult = Nwk_ManLutMerge( pAbc->pAbc8Nwk, pPars );
    Vec_IntFree( vResult );
    return 0;

usage:
    fprintf( stdout, "usage: *merge [-NSDLF num] [-scwvh]\n" );
    fprintf( stdout, "\t           creates pairs of topologically-related LUTs\n" );
    fprintf( stdout, "\t-N <num> : the max LUT size for merging (1 < num) [default = %d]\n", pPars->nMaxLutSize );
    fprintf( stdout, "\t-S <num> : the max total support size after merging (1 < num) [default = %d]\n", pPars->nMaxSuppSize );
    fprintf( stdout, "\t-D <num> : the max distance in terms of LUTs (0 < num) [default = %d]\n", pPars->nMaxDistance );
    fprintf( stdout, "\t-L <num> : the max difference in levels (0 <= num) [default = %d]\n", pPars->nMaxLevelDiff );
    fprintf( stdout, "\t-F <num> : the max number of fanouts to stop traversal (0 < num) [default = %d]\n", pPars->nMaxFanout );
    fprintf( stdout, "\t-s       : toggle the use of nodes without support overlap [default = %s]\n", pPars->fUseDiffSupp? "yes" : "no" );
    fprintf( stdout, "\t-c       : toggle the use of TFI/TFO nodes as candidates [default = %s]\n", pPars->fUseTfiTfo? "yes" : "no" );
    fprintf( stdout, "\t-w       : toggle printing detailed stats for each node [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( stdout, "\t-v       : toggle printing optimization summary [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h       : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Insert( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern void * Ntl_ManInsertNtk( void * p, void * pNtk );
    void * pNtlNew;
    int c;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Insert(): There is no design.\n" );
        return 1;
    }
    if ( pAbc->pAbc8Nwk == NULL ) 
    {
        printf( "Abc_CommandAbc8Insert(): There is no network to insert.\n" );
        return 1;
    }
    pNtlNew = Ntl_ManInsertNtk( pAbc->pAbc8Ntl, pAbc->pAbc8Nwk );
    Abc_FrameClearDesign(); 
    pAbc->pAbc8Ntl = pNtlNew;
    return 0;

usage:
    fprintf( stdout, "usage: *insert [-h]\n" );
    fprintf( stdout, "\t         inserts the mapped network into the netlist\n" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Fraig( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    void * pNtlNew;
    int c, fVerbose;
    int nPartSize;
    int nConfLimit;
    int nLevelMax;
    extern Aig_Man_t * Ntl_ManExtract( void * p );
    extern void * Ntl_ManFraig( void * p, int nPartSize, int nConfLimit, int nLevelMax, int fVerbose );
    extern void Ntl_ManFree( void * p );

    // set defaults
    nPartSize    = 0;
    nConfLimit   = 100;   
    nLevelMax    = 0;
    fVerbose     = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PCLvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPartSize < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
         case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            nLevelMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLevelMax < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Fraig(): There is no design to SAT sweep.\n" );
        return 1;
    }

    // get the input file name
    pNtlNew = Ntl_ManFraig( pAbc->pAbc8Ntl, nPartSize, nConfLimit, nLevelMax, fVerbose );
    if ( pNtlNew == NULL )
    {
        printf( "Abc_CommandAbc8Fraig(): Tranformation of the AIG has failed.\n" );
        return 1;
    }

    Abc_FrameClearDesign(); 
    pAbc->pAbc8Ntl = pNtlNew;
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Fraig(): Reading BLIF has failed.\n" );
        return 1;
    }
    pAbc->pAbc8Aig = Ntl_ManExtract( pAbc->pAbc8Ntl );
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8Fraig(): AIG extraction has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( stdout, "usage: *fraig [-P num] [-C num] [-L num] [-vh]\n" );
    fprintf( stdout, "\t         applies SAT sweeping to netlist with white-boxes\n" );
    fprintf( stdout, "\t-P num : partition size (0 = partitioning is not used) [default = %d]\n", nPartSize );
    fprintf( stdout, "\t-C num : limit on the number of conflicts [default = %d]\n", nConfLimit );
    fprintf( stdout, "\t-L num : limit on node level to fraig (0 = fraig all nodes) [default = %d]\n", nLevelMax );
    fprintf( stdout, "\t-v     : toggle verbose printout [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Scl( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    void * pNtlNew;
    int c;
    int fLatchConst;
    int fLatchEqual;
    int fVerbose;
    extern Aig_Man_t * Ntl_ManExtract( void * p );
    extern void * Ntl_ManScl( void * p, int fLatchConst, int fLatchEqual, int fVerbose );
    extern int Ntl_ManIsComb( void * p );

    // set defaults
    fLatchConst = 1;
    fLatchEqual = 1;
    fVerbose    = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "cevh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'c':
            fLatchConst ^= 1;
            break;
        case 'e':
            fLatchEqual ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Scl(): There is no design to SAT sweep.\n" );
        return 1;
    }

    if ( Ntl_ManIsComb(pAbc->pAbc8Ntl) )
    {
        fprintf( stdout, "Abc_CommandAbc8Scl(): The network is combinational (run \"*fraig\").\n" );
        return 0;
    }

    // get the input file name
    pNtlNew = Ntl_ManScl( pAbc->pAbc8Ntl, fLatchConst, fLatchEqual, fVerbose );
    if ( pNtlNew == NULL )
    {
        printf( "Abc_CommandAbc8Scl(): Tranformation of the AIG has failed.\n" );
        return 1;
    }

    Abc_FrameClearDesign(); 
    pAbc->pAbc8Ntl = pNtlNew;
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Scl(): Reading BLIF has failed.\n" );
        return 1;
    }
    pAbc->pAbc8Aig = Ntl_ManExtract( pAbc->pAbc8Ntl );
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8Scl(): AIG extraction has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( stdout, "usage: *scl [-cevh]\n" );
    fprintf( stdout, "\t         performs sequential cleanup of the netlist\n" );
    fprintf( stdout, "\t         by removing nodes and latches that do not feed into POs\n" );
    fprintf( stdout, "\t-c     : sweep stuck-at latches detected by ternary simulation [default = %s]\n", fLatchConst? "yes": "no" );
    fprintf( stdout, "\t-e     : merge equal latches (same data inputs and init states) [default = %s]\n", fLatchEqual? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Lcorr( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    void * pNtlNew;
    int c;
    int nFramesP;
    int nConfMax;
    int fVerbose;
    extern Aig_Man_t * Ntl_ManExtract( void * p );
    extern void * Ntl_ManLcorr( void * p, int nConfMax, int fVerbose );
    extern int Ntl_ManIsComb( void * p );

    // set defaults
    nFramesP   = 0;
    nConfMax   = 10000;
    fVerbose   = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PCvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nFramesP = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nFramesP < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfMax < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Lcorr(): There is no design to SAT sweep.\n" );
        return 1;
    }

    if ( Ntl_ManIsComb(pAbc->pAbc8Ntl) )
    {
        fprintf( stdout, "Abc_CommandAbc8Lcorr(): The network is combinational (run \"*fraig\").\n" );
        return 0;
    }

    // get the input file name
    pNtlNew = Ntl_ManLcorr( pAbc->pAbc8Ntl, nConfMax, fVerbose );
    if ( pNtlNew == NULL )
    {
        printf( "Abc_CommandAbc8Lcorr(): Tranformation of the AIG has failed.\n" );
        return 1;
    }

    Abc_FrameClearDesign(); 
    pAbc->pAbc8Ntl = pNtlNew;
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Lcorr(): Reading BLIF has failed.\n" );
        return 1;
    }
    pAbc->pAbc8Aig = Ntl_ManExtract( pAbc->pAbc8Ntl );
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8Lcorr(): AIG extraction has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( stdout, "usage: *lcorr [-C num] [-vh]\n" );
    fprintf( stdout, "\t         computes latch correspondence for the netlist\n" );
//    fprintf( stdout, "\t-P num : number of time frames to use as the prefix [default = %d]\n", nFramesP );
    fprintf( stdout, "\t-C num : max conflict number when proving latch equivalence [default = %d]\n", nConfMax );
    fprintf( stdout, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Ssw( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    void * pNtlNew;
    Fra_Ssw_t Pars, * pPars = &Pars;
    int c;
    extern Aig_Man_t * Ntl_ManExtract( void * p );
    extern void * Ntl_ManSsw( void * p, Fra_Ssw_t * pPars );
    extern int Ntl_ManIsComb( void * p );

    // set defaults
    pPars->nPartSize   = 0;
    pPars->nOverSize   = 0;
    pPars->nFramesP    = 0;
    pPars->nFramesK    = 1;
    pPars->nMaxImps    = 5000;
    pPars->nMaxLevs    = 0;
    pPars->nMinDomSize = 100;
    pPars->fUseImps    = 0;
    pPars->fRewrite    = 0;
    pPars->fFraiging   = 0;
    pPars->fLatchCorr  = 0;
    pPars->fWriteImps  = 0;
    pPars->fUse1Hot    = 0;
    pPars->fVerbose    = 0;
    pPars->TimeLimit   = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PQNFILDirfletvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nPartSize < 2 ) 
                goto usage;
            break;
        case 'Q':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-Q\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nOverSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nOverSize < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesP = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesP < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesK = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesK <= 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxImps = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxImps <= 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxLevs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxLevs <= 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMinDomSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMinDomSize <= 0 ) 
                goto usage;
            break;
        case 'i':
            pPars->fUseImps ^= 1;
            break;
        case 'r':
            pPars->fRewrite ^= 1;
            break;
        case 'f':
            pPars->fFraiging ^= 1;
            break;
        case 'l':
            pPars->fLatchCorr ^= 1;
            break;
        case 'e':
            pPars->fWriteImps ^= 1;
            break;
        case 't':
            pPars->fUse1Hot ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Ssw(): There is no design to SAT sweep.\n" );
        return 1;
    }

    if ( Ntl_ManIsComb(pAbc->pAbc8Ntl) )
    {
        fprintf( stdout, "The network is combinational (run \"*fraig\").\n" );
        return 0;
    }

    if ( pPars->nFramesK > 1 && pPars->fUse1Hot )
    {
        printf( "Currrently can only use one-hotness for simple induction (K=1).\n" );
        return 0;
    }

    if ( pPars->nFramesP && pPars->fUse1Hot )
    {
        printf( "Currrently can only use one-hotness without prefix.\n" );
        return 0;
    }

    // get the input file name
    pNtlNew = Ntl_ManSsw( pAbc->pAbc8Ntl, pPars );
    if ( pNtlNew == NULL )
    {
        printf( "Abc_CommandAbc8Ssw(): Tranformation of the AIG has failed.\n" );
        return 1;
    }

    Abc_FrameClearDesign(); 
    pAbc->pAbc8Ntl = pNtlNew;
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Ssw(): Reading BLIF has failed.\n" );
        return 1;
    }
    pAbc->pAbc8Aig = Ntl_ManExtract( pAbc->pAbc8Ntl );
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8Ssw(): AIG extraction has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( stdout, "usage: *ssw [-PQNFLD num] [-lrfetvh]\n" );
    fprintf( stdout, "\t         performs sequential sweep using K-step induction on the netlist \n" );
    fprintf( stdout, "\t-P num : max partition size (0 = no partitioning) [default = %d]\n", pPars->nPartSize );
    fprintf( stdout, "\t-Q num : partition overlap (0 = no overlap) [default = %d]\n", pPars->nOverSize );
    fprintf( stdout, "\t-N num : number of time frames to use as the prefix [default = %d]\n", pPars->nFramesP );
    fprintf( stdout, "\t-F num : number of time frames for induction (1=simple) [default = %d]\n", pPars->nFramesK );
    fprintf( stdout, "\t-L num : max number of levels to consider (0=all) [default = %d]\n", pPars->nMaxLevs );
    fprintf( stdout, "\t-D num : min size of a clock domain used for synthesis [default = %d]\n", pPars->nMinDomSize );
//    fprintf( stdout, "\t-I num : max number of implications to consider [default = %d]\n", pPars->nMaxImps );
//    fprintf( stdout, "\t-i     : toggle using implications [default = %s]\n", pPars->fUseImps? "yes": "no" );
    fprintf( stdout, "\t-l     : toggle latch correspondence only [default = %s]\n", pPars->fLatchCorr? "yes": "no" );
    fprintf( stdout, "\t-r     : toggle AIG rewriting [default = %s]\n", pPars->fRewrite? "yes": "no" );
    fprintf( stdout, "\t-f     : toggle fraiging (combinational SAT sweeping) [default = %s]\n", pPars->fFraiging? "yes": "no" );
    fprintf( stdout, "\t-e     : toggle writing implications as assertions [default = %s]\n", pPars->fWriteImps? "yes": "no" );
    fprintf( stdout, "\t-t     : toggle using one-hotness conditions [default = %s]\n", pPars->fUse1Hot? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Scorr( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    void * pNtlNew;
    Ssw_Pars_t Pars, * pPars = &Pars;
    int c;
    extern Aig_Man_t * Ntl_ManExtract( void * p );
    extern void * Ntl_ManScorr( void * p, Ssw_Pars_t * pPars );
    extern int Ntl_ManIsComb( void * p );

    // set defaults
    Ssw_ManSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "PQFCLNSDVMpldsncvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nPartSize < 2 ) 
                goto usage;
            break;
        case 'Q':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-Q\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nOverSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nOverSize < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesK = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesK <= 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit <= 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMaxLevs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMaxLevs <= 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nConstrs = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nConstrs < 0 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFramesAddSim = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFramesAddSim < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nMinDomSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nMinDomSize < 0 ) 
                goto usage;
            break;
        case 'V':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-V\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nSatVarMax2 = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nSatVarMax2 < 0 ) 
                goto usage;
            break;
        case 'M':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-M\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nRecycleCalls2 = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nRecycleCalls2 < 0 ) 
                goto usage;
            break;
        case 'p':
            pPars->fPolarFlip ^= 1;
            break;
        case 'l':
            pPars->fLatchCorr ^= 1;
            break;
        case 'd':
            pPars->fDynamic ^= 1;
            break;
        case 's':
            pPars->fLocalSim ^= 1;
            break;
        case 'n':
            pPars->fScorrGia ^= 1;
            break;
        case 'c':
            pPars->fUseCSat ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Ssw(): There is no design to SAT sweep.\n" );
        return 1;
    }

    if ( Ntl_ManIsComb(pAbc->pAbc8Ntl) )
    {
        fprintf( stdout, "The network is combinational (run \"*fraig\").\n" );
        return 0;
    }

    if ( pPars->fDynamic && (pPars->nPartSize || pPars->nOverSize) )
    {
        pPars->nPartSize = 0;
        pPars->nOverSize = 0;
        printf( "With dynamic partitioning (-d) enabled, static one (-P <num> -Q <num>) is ignored.\n" );
    }

    // get the input file name
    pNtlNew = Ntl_ManScorr( pAbc->pAbc8Ntl, pPars );
    if ( pNtlNew == NULL )
    {
        printf( "Abc_CommandAbc8Scorr(): Tranformation of the AIG has failed.\n" );
        return 1;
    }

    Abc_FrameClearDesign(); 
    pAbc->pAbc8Ntl = pNtlNew;
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Scorr(): Reading BLIF has failed.\n" );
        return 1;
    }
    pAbc->pAbc8Aig = Ntl_ManExtract( pAbc->pAbc8Ntl );
    if ( pAbc->pAbc8Aig == NULL )
    {
        printf( "Abc_CommandAbc8Scorr(): AIG extraction has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( stdout, "usage: *scorr [-PQFCLNSDVM <num>] [-pldsncvh]\n" );
    fprintf( stdout, "\t         performs sequential sweep using K-step induction\n" );
    fprintf( stdout, "\t-P num : max partition size (0 = no partitioning) [default = %d]\n", pPars->nPartSize );
    fprintf( stdout, "\t-Q num : partition overlap (0 = no overlap) [default = %d]\n", pPars->nOverSize );
    fprintf( stdout, "\t-F num : number of time frames for induction (1=simple) [default = %d]\n", pPars->nFramesK );
    fprintf( stdout, "\t-C num : max number of conflicts at a node (0=inifinite) [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-L num : max number of levels to consider (0=all) [default = %d]\n", pPars->nMaxLevs );
    fprintf( stdout, "\t-N num : number of last POs treated as constraints (0=none) [default = %d]\n", pPars->nConstrs );
    fprintf( stdout, "\t-S num : additional simulation frames for c-examples (0=none) [default = %d]\n", pPars->nFramesAddSim );
    fprintf( stdout, "\t-D num : min size of a clock domain used for synthesis [default = %d]\n", pPars->nMinDomSize );
    fprintf( stdout, "\t-V num : min var num needed to recycle the SAT solver [default = %d]\n", pPars->nSatVarMax2 );
    fprintf( stdout, "\t-M num : min call num needed to recycle the SAT solver [default = %d]\n", pPars->nRecycleCalls2 );
    fprintf( stdout, "\t-p     : toggle alighning polarity of SAT variables [default = %s]\n", pPars->fPolarFlip? "yes": "no" );
    fprintf( stdout, "\t-l     : toggle latch correspondence only [default = %s]\n", pPars->fLatchCorr? "yes": "no" );
    fprintf( stdout, "\t-d     : toggle dynamic addition of constraints [default = %s]\n", pPars->fDynamic? "yes": "no" );
    fprintf( stdout, "\t-s     : toggle local simulation in the cone of influence [default = %s]\n", pPars->fLocalSim? "yes": "no" );
    fprintf( stdout, "\t-n     : toggle using new AIG package [default = %s]\n", pPars->fScorrGia? "yes": "no" );
    fprintf( stdout, "\t-c     : toggle using new AIG package and SAT solver [default = %s]\n", pPars->fUseCSat? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Sweep( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    void * pNtlTemp;
    int Counter;
    int fMapped;
    int fVerbose;
    int c;
    extern int Ntl_ManSweep( void * p, int fVerbose );
    extern void * Ntl_ManInsertNtk( void * p, void * pNtk );
    extern Aig_Man_t * Ntl_ManExtract( void * p );
    extern void * Ntl_ManExtractNwk( void * p, Aig_Man_t * pAig, Tim_Man_t * pManTime );

    // set defaults
    fMapped  = 0;
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "mvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'm':
            fMapped ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Sweep(): There is no design to sweep.\n" );
        return 1;
    }

    // if mapped, insert the network
    if ( fMapped )
    {
        if ( pAbc->pAbc8Nwk == NULL )
        {
            printf( "Abc_CommandAbc8Sweep(): There is no mapped network to sweep.\n" );
            return 1;
        }
        pNtlTemp = Ntl_ManInsertNtk( pAbc->pAbc8Ntl, pAbc->pAbc8Nwk );
        if ( pNtlTemp == NULL )
        {
            printf( "Abc_CommandAbc8Sweep():  Inserting mapped network has failed.\n" );
            return 1;
        }
        Abc_FrameClearDesign(); 
        pAbc->pAbc8Ntl = pNtlTemp;
    }
 
    // sweep the current design
    Counter = Ntl_ManSweep( pAbc->pAbc8Ntl, fVerbose );
    if ( Counter == 0 )
        printf( "The netlist is unchanged by sweep.\n" );

    // if mapped, create new AIG and new mapped network
    if ( fMapped )
    {
        pAbc->pAbc8Aig = Ntl_ManExtract( pAbc->pAbc8Ntl );
        if ( pAbc->pAbc8Aig == NULL )
        {
            printf( "Abc_CommandAbc8Sweep(): AIG extraction has failed.\n" );
            return 1;
        }
        pAbc->pAbc8Nwk = Ntl_ManExtractNwk( pAbc->pAbc8Ntl, pAbc->pAbc8Aig, NULL );
        if ( pAbc->pAbc8Nwk == NULL )
        {
            printf( "Abc_CommandAbc8Sweep(): Failed to extract the mapped network.\n" );
            return 1;
        }
    }
    else // remove old AIG/mapped and create new AIG
    {
        pNtlTemp = pAbc->pAbc8Ntl; pAbc->pAbc8Ntl = NULL;
        Abc_FrameClearDesign(); 
        pAbc->pAbc8Ntl = pNtlTemp;
        // extract new AIG
        pAbc->pAbc8Aig = Ntl_ManExtract( pAbc->pAbc8Ntl );
        if ( pAbc->pAbc8Aig == NULL )
        {
            printf( "Abc_CommandAbc8Sweep(): AIG extraction has failed.\n" );
            return 1;
        }
    }
    return 0;

usage:
    fprintf( stdout, "usage: *sw [-mvh]\n" );
    fprintf( stdout, "\t        performs structural sweep of the netlist\n" );
    fprintf( stdout, "\t        removes dangling nodes, registers, and white-boxes\n" );
    fprintf( stdout, "\t-m    : inserts mapped network into netlist and sweeps it [default = %s]\n", fMapped? "yes": "no" );
    fprintf( stdout, "\t-v    : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Zero( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    void * pNtlNew;
    int fVerbose;
    int c;
    extern void Ntl_ManTransformInitValues( void * p );

    // set defaults
    fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Zero(): There is no design to convert.\n" );
        return 1;
    }

    // transform the registers
    pNtlNew = pAbc->pAbc8Ntl;
    pAbc->pAbc8Ntl = NULL;
    Ntl_ManTransformInitValues( pNtlNew );

    // replace the design
    Abc_FrameClearDesign(); 
    pAbc->pAbc8Ntl = pNtlNew;
    return 0;

usage:
    fprintf( stdout, "usage: *zero [-h]\n" );
    fprintf( stdout, "\t        converts registers to have constant-0 initial value\n" );
    fprintf( stdout, "\t-v    : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8Cec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Aig_Man_t * pAig1, * pAig2;
    void * pTemp1, * pTemp2;
    char ** pArgvNew;
    int nArgcNew;
    int c;
    int fVerbose;
    int nConfLimit;
    int fSmart;
    int nPartSize;
    extern Aig_Man_t * Ntl_ManCollapse( void * p, int fSeq );
    extern void * Ntl_ManDup( void * pOld );
    extern void Ntl_ManFree( void * p );
    extern void * Ntl_ManInsertNtk( void * p, void * pNtk );

    extern void  Ntl_ManPrepareCecMans( void * pMan1, void * pMan2, Aig_Man_t ** ppAig1, Aig_Man_t ** ppAig2 );
    extern void Ntl_ManPrepareCec( char * pFileName1, char * pFileName2, Aig_Man_t ** ppMan1, Aig_Man_t ** ppMan2 );
    extern int Fra_FraigCecTop( Aig_Man_t * pMan1, Aig_Man_t * pMan2, int nConfLimit, int nPartSize, int fSmart, int fVerbose );

    // set defaults
    nConfLimit = 100000;   
    nPartSize  = 100;
    fSmart     = 0;
    fVerbose   = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CPsvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            nConfLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nConfLimit < 0 ) 
                goto usage;
            break;
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            nPartSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nPartSize < 0 ) 
                goto usage;
            break;
        case 's':
            fSmart ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( nArgcNew != 0 && nArgcNew != 2 )
    {
        printf( "Currently can only compare current mapped network against the spec, or designs derived from two files.\n" );
        return 0;
    }
    if ( nArgcNew == 2 )
    {
        Ntl_ManPrepareCec( pArgvNew[0], pArgvNew[1], &pAig1, &pAig2 );
        if ( !pAig1 || !pAig2 )
            return 1;
        Fra_FraigCecTop( pAig1, pAig2, nConfLimit, nPartSize, fSmart, fVerbose );
        Aig_ManStop( pAig1 );
        Aig_ManStop( pAig2 );
        return 0;
    }

    if ( pAbc->pAbc8Ntl == NULL )
    {
        printf( "Abc_CommandAbc8Cec(): There is no design to verify.\n" );
        return 1;
    }
    if ( pAbc->pAbc8Nwk == NULL )
    {
        printf( "Abc_CommandAbc8Cec(): There is no mapped network to verify.\n" );
        return 1;
    }
//    printf( "Currently *cec works only for two designs given on command line.\n" );

    // insert the mapped network
    pTemp1 = Ntl_ManDup( pAbc->pAbc8Ntl );
    pTemp2 = Ntl_ManInsertNtk( pAbc->pAbc8Ntl, pAbc->pAbc8Nwk );
    if ( pTemp2 == NULL )
    {
        Ntl_ManFree( pTemp1 );
        printf( "Abc_CommandAbc8Cec(): Inserting the design has failed.\n" );
        return 1;
    }
    Ntl_ManPrepareCecMans( pTemp1, pTemp2, &pAig1, &pAig2 );
    Ntl_ManFree( pTemp1 );
    Ntl_ManFree( pTemp2 );
    if ( !pAig1 || !pAig2 )
        return 1;
    Fra_FraigCecTop( pAig1, pAig2, nConfLimit, nPartSize, fSmart, fVerbose );
    Aig_ManStop( pAig1 );
    Aig_ManStop( pAig2 );
    return 0;

usage:
    fprintf( stdout, "usage: *cec [-C num] [-P num] [-svh] <file1> <file2>\n" );
    fprintf( stdout, "\t         performs combinational equivalence checking\n" );
    fprintf( stdout, "\t-C num : limit on the number of conflicts [default = %d]\n", nConfLimit );
    fprintf( stdout, "\t-P num : the partition size for partitioned CEC [default = %d]\n", nPartSize );
    fprintf( stdout, "\t-s     : toggle smart and natural output partitioning [default = %s]\n", fSmart? "smart": "natural" );
    fprintf( stdout, "\t-v     : toggles verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    fprintf( stdout, "\tfile1  : (optional) the file with the first network\n");
    fprintf( stdout, "\tfile2  : (optional) the file with the second network\n");
    fprintf( stdout, "\t         if no files are given, uses the current network and its spec\n");
    fprintf( stdout, "\t         if two files are given, compares designs derived from files\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc8DSec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Fra_Sec_t SecPar, * pSecPar = &SecPar;
    Aig_Man_t * pAig;
    char ** pArgvNew;
    int nArgcNew;
    int c;

    extern void Fra_SecSetDefaultParams( Fra_Sec_t * pSecPar );
    extern int Fra_FraigSec( Aig_Man_t * p, Fra_Sec_t * pSecPar, Aig_Man_t ** ppResult );
    extern Aig_Man_t * Ntl_ManPrepareSec( char * pFileName1, char * pFileName2 );

    // set defaults
    Fra_SecSetDefaultParams( pSecPar );
    pSecPar->nFramesMax     = 4;
    pSecPar->fPhaseAbstract = 0;
    pSecPar->fRetimeFirst   = 0;
    pSecPar->fRetimeRegs    = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Farmfwvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pSecPar->nFramesMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pSecPar->nFramesMax < 0 ) 
                goto usage;
            break;
        case 'a':
            pSecPar->fPhaseAbstract ^= 1;
            break;
        case 'r':
            pSecPar->fRetimeFirst ^= 1;
            break;
        case 'm':
            pSecPar->fRetimeRegs ^= 1;
            break;
        case 'f':
            pSecPar->fFraiging ^= 1;
            break;
        case 'w':
            pSecPar->fVeryVerbose ^= 1;
            break;
        case 'v':
            pSecPar->fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( nArgcNew != 2 )
    {
        printf( "Only works for two designs written from files specified on the command line.\n" );
        return 1;
    }

    pAig = Ntl_ManPrepareSec( pArgvNew[0], pArgvNew[1] );
    if ( pAig == NULL )
        return 0;
    Fra_FraigSec( pAig, pSecPar, NULL );
    Aig_ManStop( pAig );
    return 0;

usage:
    fprintf( stdout, "usage: *dsec [-F num] [-armfwvh] <file1> <file2>\n" );
    fprintf( stdout, "\t         performs sequential equivalence checking for two designs\n" );
    fprintf( stdout, "\t-F num : the limit on the depth of induction [default = %d]\n", pSecPar->nFramesMax );
    fprintf( stdout, "\t-a     : toggles the use of phase abstraction [default = %s]\n", pSecPar->fPhaseAbstract? "yes": "no" );
    fprintf( stdout, "\t-r     : toggles forward retiming at the beginning [default = %s]\n", pSecPar->fRetimeFirst? "yes": "no" );
    fprintf( stdout, "\t-m     : toggles min-register retiming [default = %s]\n", pSecPar->fRetimeRegs? "yes": "no" );
    fprintf( stdout, "\t-f     : toggles the internal use of fraiging [default = %s]\n", pSecPar->fFraiging? "yes": "no" );
    fprintf( stdout, "\t-v     : toggles verbose output [default = %s]\n", pSecPar->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-w     : toggles additional verbose output [default = %s]\n", pSecPar->fVeryVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    fprintf( stdout, "\tfile1  : the file with the first design\n");
    fprintf( stdout, "\tfile2  : the file with the second design\n");
//    fprintf( stdout, "\t         if no files are given, uses the current network and its spec\n");
//    fprintf( stdout, "\t         if one file is given, uses the current network and the file\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Read( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pAig;
    FILE * pFile;
    char ** pArgvNew;
    char * FileName, * pTemp;
    int nArgcNew;
    int c, fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }
    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( nArgcNew != 1 )
    {
        printf( "There is no file name.\n" );
        return 1;
    }

    // get the input file name
    FileName = pArgvNew[0];
    // fix the wrong symbol
    for ( pTemp = FileName; *pTemp; pTemp++ )
        if ( *pTemp == '>' )
            *pTemp = '\\';
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( (FileName = Extra_FileGetSimilarName( FileName, ".aig", ".blif", ".pla", ".eqn", ".bench" )) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );

    pAig = Gia_ReadAiger( FileName, 0 );
    if ( pAig == NULL )
    {
        printf( "Reading AIGER has failed.\n" );
        return 0;
    }
    if ( pAbc->pAig )
        Gia_ManStop( pAbc->pAig );
    pAbc->pAig = pAig;
    return 0;

usage:
    fprintf( stdout, "usage: &r [-vh] <file>\n" );
    fprintf( stdout, "\t         reads the current AIG from the AIGER file\n" );
    fprintf( stdout, "\t-v     : toggles additional verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    fprintf( stdout, "\t<file> : the file name\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Get( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern Aig_Man_t * Abc_NtkToDar( Abc_Ntk_t * pNtk, int fExors, int fRegisters );
    extern Aig_Man_t * Abc_NtkToDarChoices( Abc_Ntk_t * pNtk );
    Gia_Man_t * pAig;
    Aig_Man_t * pMan;
    int c, fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }
    if ( pAbc->pNtkCur == NULL )
    {
        printf( "There is no current network\n" );
        return 1;
    }
    if ( !Abc_NtkIsStrash( pAbc->pNtkCur ) )
    {
        printf( "The current network should be strashed.\n" );
        return 1;
    }
//    if ( Abc_NtkGetChoiceNum(pAbc->pNtkCur) )
//    {
//        printf( "Removing %d choices from the AIG.\n", Abc_NtkGetChoiceNum(pAbc->pNtkCur) );
//        Abc_AigCleanup(pAbc->pNtkCur->pManFunc);
//    }
    if ( Abc_NtkGetChoiceNum(pAbc->pNtkCur) )
        pMan = Abc_NtkToDarChoices( pAbc->pNtkCur );
    else
        pMan = Abc_NtkToDar( pAbc->pNtkCur, 0, 1 );
    pAig = Gia_ManFromAig( pMan );
    Aig_ManStop( pMan );
    if ( pAig == NULL )
    {
        printf( "Transferring AIG has failed.\n" );
        return 0;
    }
    if ( pAbc->pAig )
        Gia_ManStop( pAbc->pAig );
    pAbc->pAig = pAig;
    return 0;

usage:
    fprintf( stdout, "usage: &get [-vh] <file>\n" );
    fprintf( stdout, "\t         transfer the current network from the old ABC\n" );
    fprintf( stdout, "\t-v     : toggles additional verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    fprintf( stdout, "\t<file> : the file name\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Put( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern Abc_Ntk_t * Abc_NtkFromAigPhase( Aig_Man_t * pMan );
    extern Abc_Ntk_t * Abc_NtkFromDarChoices( Abc_Ntk_t * pNtkOld, Aig_Man_t * pMan );
    Aig_Man_t * pMan;
    FILE * pOut, * pErr;
    Abc_Ntk_t * pNtk;
    int c;
    int fVerbose;

    pNtk = Abc_FrameReadNtk(pAbc);
    pOut = Abc_FrameReadOut(pAbc);
    pErr = Abc_FrameReadErr(pAbc);

    // set defaults
    fVerbose    = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        fprintf( pErr, "Empty network.\n" );
        return 1;
    }
    if ( Gia_ManHasDandling(pAbc->pAig) == 0 )
    {
        pMan = Gia_ManToAig( pAbc->pAig, 0 );
        pNtk = Abc_NtkFromAigPhase( pMan );
        pNtk->pName = Extra_UtilStrsav(pMan->pName);
        Aig_ManStop( pMan );
    }
    else
    {
        Abc_Ntk_t * pNtkNoCh;
//        printf( "Transforming AIG with %d choice nodes.\n", Gia_ManEquivCountClasses(pAbc->pAig) );
        // create network without choices
        pMan = Gia_ManToAig( pAbc->pAig, 0 );
        pNtkNoCh = Abc_NtkFromAigPhase( pMan );
        pNtkNoCh->pName = Extra_UtilStrsav(pMan->pName);
        Aig_ManStop( pMan );
        // derive network with choices
        pMan = Gia_ManToAig( pAbc->pAig, 1 );
        pNtk = Abc_NtkFromDarChoices( pNtkNoCh, pMan );
        Abc_NtkDelete( pNtkNoCh );
        Aig_ManStop( pMan );
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    return 0;

usage:
    fprintf( pErr, "usage: &put [-vh]\n" );
    fprintf( pErr, "\t         transfer the current network into the old ABC\n" );
    fprintf( pErr, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( pErr, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Write( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char * pFileName;
    char ** pArgvNew;
    int c, nArgcNew;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( nArgcNew != 1 )
    {
        printf( "There is no file name.\n" );
        return 1;
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Write(): There is no AIG to write.\n" );
        return 1;
    } 
    // create the design to write
    pFileName = argv[globalUtilOptind];
    Gia_WriteAiger( pAbc->pAig, pFileName, 0, 0 );
    return 0;

usage:
    fprintf( stdout, "usage: &w [-h] <file>\n" );
    fprintf( stdout, "\t        writes the current AIG into the AIGER file\n" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    fprintf( stdout, "\t<file> : the file name\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Ps( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    int c;
    int fSwitch = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ph" ) ) != EOF )
    {
        switch ( c )
        {
        case 'p':
            fSwitch ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Ps(): There is no AIG.\n" );
        return 1;
    } 
    Gia_ManPrintStats( pAbc->pAig, fSwitch );
    return 0;

usage:
    fprintf( stdout, "usage: &ps [-ph]\n" );
    fprintf( stdout, "\t        prints stats of the current AIG\n" );
    fprintf( stdout, "\t-p    : toggle printing switching activity [default = %s]\n", fSwitch? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9PFan( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    int c;
    int nNodes = 40;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Nh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nNodes = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nNodes < 0 ) 
                goto usage;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9PFan(): There is no AIG.\n" );
        return 1;
    } 
    Gia_ManPrintFanio( pAbc->pAig, nNodes );
    return 0;

usage:
    fprintf( stdout, "usage: &pfan [-N num] [-h]\n" );
    fprintf( stdout, "\t         prints fanin/fanout statistics\n" );
    fprintf( stdout, "\t-N num : the number of high-fanout nodes to explore [default = %d]\n", nNodes );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9PSig( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    int c;
    int fSetReset = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "rh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'r':
            fSetReset ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9PSigs(): There is no AIG.\n" );
        return 1;
    } 
    if ( Gia_ManRegNum(pAbc->pAig) == 0 )
    {
        printf( "Abc_CommandAbc9PSigs(): Works only for sequential circuits.\n" );
        return 1;
    } 
    Gia_ManDetectSeqSignals( pAbc->pAig, fSetReset, 1 );
    return 0;

usage:
    fprintf( stdout, "usage: &psig [-rh]\n" );
    fprintf( stdout, "\t         prints enable/set/reset statistics\n" );
    fprintf( stdout, "\t-r     : toggle printing set/reset signals [default = %s]\n", fSetReset? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Status( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    int c;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Status(): There is no AIG.\n" );
        return 1;
    } 
    Gia_ManPrintMiterStatus( pAbc->pAig );
    return 0;

usage:
    fprintf( stdout, "usage: &status [-h]\n" );
    fprintf( stdout, "\t         prints status of the miter\n" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Show( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Aig_Man_t * pMan;
    int c;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Show(): There is no AIG.\n" );
        return 1;
    } 
    pMan = Gia_ManToAig( pAbc->pAig, 0 );
    Aig_ManShow( pMan, 0, NULL );
    Aig_ManStop( pMan );
    return 0;

usage:
    fprintf( stdout, "usage: &show [-h]\n" );
    fprintf( stdout, "\t        shows the current AIG using GSView\n" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Hash( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c;
    int fAddStrash = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ah" ) ) != EOF )
    {
        switch ( c )
        {
        case 'a':
            fAddStrash ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Hash(): There is no AIG.\n" );
        return 1;
    } 
    pAbc->pAig = Gia_ManRehash( pTemp = pAbc->pAig, fAddStrash );
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &st [-ah]\n" );
    fprintf( stdout, "\t        performs structural hashing\n" );
    fprintf( stdout, "\t-a    : toggle additional hashing [default = %s]\n", fAddStrash? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Topand( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c, fVerbose = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "vh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Topand(): There is no AIG.\n" );
        return 1;
    } 
    if ( Gia_ManRegNum(pAbc->pAig) > 0 )
    {
        printf( "Abc_CommandAbc9Topand(): Can only be applied to a combinational miter.\n" );
        return 1;
    } 
    pAbc->pAig = Gia_ManDupTopAnd( pTemp = pAbc->pAig, fVerbose );
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &topand [-vh]\n" );
    fprintf( stdout, "\t        performs AND decomposition for combinational miter\n" );
    fprintf( stdout, "\t-v    : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Cof( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c, fVerbose = 0;
    int iVar = 0, nLimFan = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "VLvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'V':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-V\" should be followed by an integer.\n" );
                goto usage;
            }
            iVar = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( iVar < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            } 
            nLimFan = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nLimFan < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Cof(): There is no AIG.\n" );
        return 1;
    } 
    if ( nLimFan )
    {
        printf( "Cofactoring all variables whose fanout count is higher than %d.\n", nLimFan );
        pAbc->pAig = Gia_ManDupCofAll( pTemp = pAbc->pAig, nLimFan, fVerbose );
    }
    else if ( iVar )
    {
        printf( "Cofactoring one variable with object ID %d.\n", iVar );
        pAbc->pAig = Gia_ManDupCof( pTemp = pAbc->pAig, iVar );
    }
    else
    {
        printf( "One of the paramters, -V <num> or -L <num>, should be set on the command line.\n" );
        goto usage;
    }
    if ( pAbc->pAig == NULL )
    {
        pAbc->pAig = pTemp;
        printf( "Abc_CommandAbc9Cof(): Transformation has failed.\n" );
        return 1;
    }
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &cof [-VL num] [-vh]\n" );
    fprintf( stdout, "\t         performs cofactoring w.r.t. variable(s)\n" );
    fprintf( stdout, "\t-V num : the zero-based ID of one variable to cofactor [default = %d]\n", iVar );
    fprintf( stdout, "\t-L num : cofactor vars with fanout count higher than this [default = %d]\n", nLimFan );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Trim( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c;
    int fTrimCis = 1;
    int fTrimCos = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "ioh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'i':
            fTrimCis ^= 1;
            break;
        case 'o':
            fTrimCos ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Trim(): There is no AIG.\n" );
        return 1;
    } 
    pAbc->pAig = Gia_ManDupTrimmed( pTemp = pAbc->pAig, fTrimCis, fTrimCos );
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &trim [-ioh]\n" );
    fprintf( stdout, "\t         removes PIs without fanout and PO driven by constants\n" );
    fprintf( stdout, "\t-i     : toggle removing PIs [default = %s]\n", fTrimCis? "yes": "no" );
    fprintf( stdout, "\t-o     : toggle removing POs [default = %s]\n", fTrimCos? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Dfs( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c;
    int fNormal  = 0;
    int fReverse = 0;
    int fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "nrvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'n':
            fNormal ^= 1;
            break;
        case 'r':
            fReverse ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Dfs(): There is no AIG.\n" );
        return 1;
    } 
    if ( fNormal )
    {
        pAbc->pAig = Gia_ManDupOrderAiger( pTemp = pAbc->pAig );
        if ( fVerbose )
            printf( "AIG objects are reordered as follows: CIs, ANDs, COs.\n" );
    }
    else if ( fReverse )
    {
        pAbc->pAig = Gia_ManDupOrderDfsReverse( pTemp = pAbc->pAig );
        if ( fVerbose )
            printf( "AIG objects are reordered in the reserve DFS order.\n" );
    }
    else 
    {
        pAbc->pAig = Gia_ManDupOrderDfs( pTemp = pAbc->pAig );
        if ( fVerbose )
            printf( "AIG objects are reordered in the DFS order.\n" );
    }
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &dfs [-nrvh]\n" );
    fprintf( stdout, "\t        orders objects in the DFS order\n" );
    fprintf( stdout, "\t-n    : toggle using normalized ordering [default = %s]\n", fNormal? "yes": "no" );
    fprintf( stdout, "\t-r    : toggle using reverse DFS ordering [default = %s]\n", fReverse? "yes": "no" );
    fprintf( stdout, "\t-v    : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Sim( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_ParSim_t Pars, * pPars = &Pars;
    int c;
    Gia_ManSimSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "WFTmvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWords = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWords < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nIters < 0 ) 
                goto usage;
            break;
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->TimeLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->TimeLimit < 0 ) 
                goto usage;
            break;
        case 'm':
            pPars->fCheckMiter ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Sim(): There is no AIG.\n" );
        return 1;
    } 
    Gia_ManSimSimulate( pAbc->pAig, pPars );
    return 0;

usage:
    fprintf( stdout, "usage: &sim [-WFT <num>] [-mvh]\n" );
    fprintf( stdout, "\t         performs random simulation of the sequential miter\n" );
    fprintf( stdout, "\t-W num : the number of words to simulate [default = %d]\n", pPars->nWords );
    fprintf( stdout, "\t-F num : the number of frames to simulate [default = %d]\n", pPars->nIters );
    fprintf( stdout, "\t-T num : approximate runtime limit in seconds [default = %d]\n", pPars->TimeLimit );
    fprintf( stdout, "\t-m     : toggle miter vs. any circuit [default = %s]\n", pPars->fCheckMiter? "miter": "not miter" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Resim( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cec_ParSim_t Pars, * pPars = &Pars;
    int c;
    Cec_ManSimSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Fmvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nRounds = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nRounds < 0 ) 
                goto usage;
            break;
        case 'm':
            pPars->fCheckMiter ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Resim(): There is no AIG.\n" );
        return 1;
    } 
    Cec_ManSeqResimulateCounter( pAbc->pAig, pPars, pAbc->pCex );
    return 0;

usage:
    fprintf( stdout, "usage: &resim [-F <num>] [-mvh]\n" );
    fprintf( stdout, "\t         resimulates equivalence classes using counter-example\n" );
    fprintf( stdout, "\t-F num : the number of additinal frames to simulate [default = %d]\n", pPars->nRounds );
    fprintf( stdout, "\t-m     : toggle miter vs. any circuit [default = %s]\n", pPars->fCheckMiter? "miter": "not miter" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Equiv( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cec_ParSim_t Pars, * pPars = &Pars;
    int c;
    Cec_ManSimSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "WRTsmfdvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWords = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWords < 0 ) 
                goto usage;
            break;
        case 'R':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-R\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nRounds = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nRounds < 0 ) 
                goto usage;
            break;
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->TimeLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->TimeLimit < 0 ) 
                goto usage;
            break;
        case 's':
            pPars->fSeqSimulate ^= 1;
            break;
        case 'm':
            pPars->fCheckMiter ^= 1;
            break;
        case 'f':
            pPars->fFirstStop ^= 1;
            break;
        case 'd':
            pPars->fDualOut ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Equiv(): There is no AIG.\n" );
        return 1;
    } 
    Cec_ManSimulation( pAbc->pAig, pPars );
    return 0;

usage:
    fprintf( stdout, "usage: &equiv [-WRT <num>] [-smfdvh]\n" );
    fprintf( stdout, "\t         computes candidate equivalence classes\n" );
    fprintf( stdout, "\t-W num : the number of words to simulate [default = %d]\n", pPars->nWords );
    fprintf( stdout, "\t-R num : the number of rounds to simulate [default = %d]\n", pPars->nRounds );
    fprintf( stdout, "\t-T num : approximate runtime limit in seconds [default = %d]\n", pPars->TimeLimit );
    fprintf( stdout, "\t-s     : toggle seq vs. comb simulation [default = %s]\n", pPars->fSeqSimulate? "yes": "no" );
    fprintf( stdout, "\t-m     : toggle miter vs. any circuit [default = %s]\n", pPars->fCheckMiter? "yes": "no" );
    fprintf( stdout, "\t-f     : toggle quitting when one PO is asserted [default = %s]\n", pPars->fFirstStop? "yes": "no" );
    fprintf( stdout, "\t-d     : toggle using two POs intead of XOR [default = %s]\n", pPars->fDualOut? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Semi( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cec_ParSmf_t Pars, * pPars = &Pars;
    int c;
    Cec_ManSmfSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "WRFCTmfdvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWords = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWords < 0 ) 
                goto usage;
            break;
        case 'R':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-R\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nRounds = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nRounds < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFrames < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->TimeLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->TimeLimit < 0 ) 
                goto usage;
            break;
        case 'm':
            pPars->fCheckMiter ^= 1;
            break;
        case 'f':
            pPars->fFirstStop ^= 1;
            break;
        case 'd':
            pPars->fDualOut ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Resim(): There is no AIG.\n" );
        return 1;
    } 
    Cec_ManSeqSemiformal( pAbc->pAig, pPars );
    return 0;

usage:
    fprintf( stdout, "usage: &semi [-WRFCT <num>] [-mfdvh]\n" );
    fprintf( stdout, "\t         performs semiformal refinement of equivalence classes\n" );
    fprintf( stdout, "\t-W num : the number of words to simulate [default = %d]\n", pPars->nWords );
    fprintf( stdout, "\t-R num : the number of rounds to simulate [default = %d]\n", pPars->nRounds );
    fprintf( stdout, "\t-F num : the number of frames to simulate [default = %d]\n", pPars->nFrames );
    fprintf( stdout, "\t-C num : the max number of conflicts at a node [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-T num : approximate runtime limit in seconds [default = %d]\n", pPars->TimeLimit );
    fprintf( stdout, "\t-m     : toggle miter vs. any circuit [default = %s]\n", pPars->fCheckMiter? "yes": "no" );
    fprintf( stdout, "\t-f     : toggle quitting when one PO is asserted [default = %s]\n", pPars->fFirstStop? "yes": "no" );
    fprintf( stdout, "\t-d     : toggle using two POs intead of XOR [default = %s]\n", pPars->fDualOut? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Times( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c, nTimes = 2, fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Nvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            nTimes = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nTimes < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Times(): There is no AIG.\n" );
        return 1;
    } 
    pAbc->pAig = Gia_ManDupTimes( pTemp = pAbc->pAig, nTimes );
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &times [-N <num>] [-vh]\n" );
    fprintf( stdout, "\t         creates several \"parallel\" copies of the design\n" );
    fprintf( stdout, "\t-N num : number of copies to create [default = %d]\n", nTimes );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Frames( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    Gia_ParFra_t Pars, * pPars = &Pars;
    int c;
    int nCofFanLit = 0;
    Gia_ManFraSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FLivh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFrames < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            nCofFanLit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nCofFanLit < 0 ) 
                goto usage;
            break;
        case 'i':
            pPars->fInit ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Frames(): There is no AIG.\n" );
        return 1;
    } 
    if ( nCofFanLit )
        pAbc->pAig = Gia_ManUnrollAndCofactor( pTemp = pAbc->pAig, pPars->nFrames, nCofFanLit, pPars->fVerbose );
    else
        pAbc->pAig = Gia_ManFrames( pTemp = pAbc->pAig, pPars );
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &frames [-FL <num>] [-ivh]\n" );
    fprintf( stdout, "\t         unrolls the design for several timeframes\n" );
    fprintf( stdout, "\t-F num : the number of frames to unroll [default = %d]\n", pPars->nFrames );
    fprintf( stdout, "\t-L num : the limit on fanout count of resets/enables to cofactor [default = %d]\n", nCofFanLit );
    fprintf( stdout, "\t-i     : toggle initializing registers [default = %s]\n", pPars->fInit? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Retime( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c;
    int nMaxIters = 100;
    int fVerbose  =   0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Nvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            nMaxIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nMaxIters < 0 ) 
                goto usage;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Retime(): There is no AIG.\n" );
        return 1;
    } 
    pAbc->pAig = Gia_ManRetimeForward( pTemp = pAbc->pAig, nMaxIters, fVerbose );
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &retime [-N <num>] [-vh]\n" );
    fprintf( stdout, "\t         performs most-forward retiming\n" );
    fprintf( stdout, "\t-N num : the number of incremental iterations [default = %d]\n", nMaxIters );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Enable( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c;
    int fRemove  = 0;
    int fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "rvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'r':
            fRemove ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Enable(): There is no AIG.\n" );
        return 1;
    } 
    if ( fRemove )
        pAbc->pAig = Gia_ManRemoveEnables( pTemp = pAbc->pAig );
    else
        pAbc->pAig = Gia_ManDupSelf( pTemp = pAbc->pAig );
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &enable [-rvh]\n" );
    fprintf( stdout, "\t         adds or removes flop enable signals\n" );
    fprintf( stdout, "\t-r     : toggle adding vs. removing enables [default = %s]\n", fRemove? "remove": "add" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Miter( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    FILE * pFile;
    Gia_Man_t * pAux;
    Gia_Man_t * pSecond;
    char * FileName, * pTemp;
    char ** pArgvNew;
    int nArgcNew;
    int c;
    int fDualOut = 0;
    int fSeq     = 0;
    int fTrans   = 0;
    int fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dstvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'd':
            fDualOut ^= 1;
            break;
        case 's':
            fSeq ^= 1;
            break;
        case 't':
            fTrans ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( fTrans )
    {
        if ( (Gia_ManPoNum(pAbc->pAig) & 1) == 1 )
        {
            printf( "Abc_CommandAbc9Miter(): The number of outputs should be even.\n" );
            return 0;
        }
        pAbc->pAig = Gia_ManTransformMiter( pAux = pAbc->pAig );
        Gia_ManStop( pAux );
        printf( "The miter (current AIG) is transformed by XORing POs pair-wise.\n" );
        return 0;
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( nArgcNew != 1 )
    {
        printf( "File name is not given on the command line.\n" );
        return 1;
    }

    // get the input file name
    FileName = pArgvNew[0];
    // fix the wrong symbol
    for ( pTemp = FileName; *pTemp; pTemp++ )
        if ( *pTemp == '>' )
            *pTemp = '\\';
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( (FileName = Extra_FileGetSimilarName( FileName, ".aig", NULL, NULL, NULL, NULL )) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );
    pSecond = Gia_ReadAiger( FileName, 0 );
    if ( pSecond == NULL )
    {
        printf( "Reading AIGER has failed.\n" );
        return 0;
    }
    // compute the miter
    pAbc->pAig = Gia_ManMiter( pAux = pAbc->pAig, pSecond, fDualOut, fSeq, fVerbose );
    Gia_ManStop( pSecond );
    if ( pAbc->pAig == NULL )
    {
        printf( "Miter construction has failed.\n" );
        pAbc->pAig = pAux;
    }
    else
        Gia_ManStop( pAux );
    return 0;

usage:
    fprintf( stdout, "usage: &miter [-dstvh] <file>\n" );
    fprintf( stdout, "\t         creates miter of two designs (current AIG vs. <file>)\n" );
    fprintf( stdout, "\t-d     : toggle creating dual output miter [default = %s]\n", fDualOut? "yes": "no" );
    fprintf( stdout, "\t-s     : toggle creating sequential miter [default = %s]\n", fSeq? "yes": "no" );
    fprintf( stdout, "\t-t     : toggle XORing pair-wise POs of the miter [default = %s]\n", fTrans? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    fprintf( stdout, "\t<file> : AIGER file with the design to miter\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Scl( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c;
    int fConst = 1;
    int fEquiv = 1;
    int fVerbose = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "cevh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'c':
            fConst ^= 1;
            break;
        case 'e':
            fEquiv ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Scl(): There is no AIG.\n" );
        return 1;
    } 
    pAbc->pAig = Gia_ManSeqStructSweep( pTemp = pAbc->pAig, fConst, fEquiv, fVerbose );
    Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &scl [-cevh]\n" );
    fprintf( stdout, "\t         performs structural sequential cleanup\n" );
    fprintf( stdout, "\t-c     : toggle removing stuck-at constant registers [default = %s]\n", fConst? "yes": "no" );
    fprintf( stdout, "\t-e     : toggle removing equivalent-driver registers [default = %s]\n", fEquiv? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Lcorr( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cec_ParCor_t Pars, * pPars = &Pars;
    Gia_Man_t * pTemp;
    int c;
    Cec_ManCorSetDefaultParams( pPars );
    pPars->fLatchCorr = 1;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FCPfrcvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFrames < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nPrefix = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nPrefix < 0 ) 
                goto usage;
            break;
        case 'f':
            pPars->fFirstStop ^= 1;
            break;
        case 'r':
            pPars->fUseRings ^= 1;
            break;
        case 'c':
            pPars->fUseCSat ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Lcorr(): There is no AIG.\n" );
        return 1;
    } 
    pAbc->pAig = Cec_ManLSCorrespondence( pTemp = pAbc->pAig, pPars );
    if ( pAbc->pAig == NULL )
    {
        pAbc->pAig = pTemp;
        printf( "Abc_CommandAbc9Lcorr(): Command has failed.\n" );
    }
    else 
        Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &lcorr [-FCP num] [-frcvh]\n" );
    fprintf( stdout, "\t         performs latch correpondence computation\n" );
    fprintf( stdout, "\t-C num : the max number of conflicts at a node [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-F num : the number of timeframes in inductive case [default = %d]\n", pPars->nFrames );
    fprintf( stdout, "\t-P num : the number of timeframes in the prefix [default = %d]\n", pPars->nPrefix );
    fprintf( stdout, "\t-f     : toggle quitting when one PO is asserted [default = %s]\n", pPars->fFirstStop? "yes": "no" );
    fprintf( stdout, "\t-r     : toggle using implication rings during refinement [default = %s]\n", pPars->fUseRings? "yes": "no" );
    fprintf( stdout, "\t-c     : toggle using circuit-based SAT solver [default = %s]\n", pPars->fUseCSat? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Scorr( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cec_ParCor_t Pars, * pPars = &Pars;
    Gia_Man_t * pTemp;
    int c;
    Cec_ManCorSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "FCPfrecvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nFrames = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFrames < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'P':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-P\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nPrefix = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nPrefix < 0 ) 
                goto usage;
            break;
        case 'f':
            pPars->fFirstStop ^= 1;
            break;
        case 'r':
            pPars->fUseRings ^= 1;
            break;
        case 'e':
            pPars->fMakeChoices ^= 1;
            break;
        case 'c':
            pPars->fUseCSat ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Scorr(): There is no AIG.\n" );
        return 1;
    } 
    pAbc->pAig = Cec_ManLSCorrespondence( pTemp = pAbc->pAig, pPars );
    if ( pAbc->pAig == NULL )
    {
        pAbc->pAig = pTemp;
        printf( "Abc_CommandAbc9Scorr(): Command has failed.\n" );
    }
    else 
        Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &scorr [-FCP num] [-frecvh]\n" );
    fprintf( stdout, "\t         performs signal correpondence computation\n" );
    fprintf( stdout, "\t-C num : the max number of conflicts at a node [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-F num : the number of timeframes in inductive case [default = %d]\n", pPars->nFrames );
    fprintf( stdout, "\t-P num : the number of timeframes in the prefix [default = %d]\n", pPars->nPrefix );
    fprintf( stdout, "\t-f     : toggle quitting when one PO is asserted [default = %s]\n", pPars->fFirstStop? "yes": "no" );
    fprintf( stdout, "\t-r     : toggle using implication rings during refinement [default = %s]\n", pPars->fUseRings? "yes": "no" );
    fprintf( stdout, "\t-e     : toggle using equivalences as choices [default = %s]\n", pPars->fMakeChoices? "yes": "no" );
    fprintf( stdout, "\t-c     : toggle using circuit-based SAT solver [default = %s]\n", pPars->fUseCSat? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Choice( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cec_ParChc_t Pars, * pPars = &Pars;
    Gia_Man_t * pTemp;
    int c;
    Cec_ManChcSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Ccvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'c':
            pPars->fUseCSat ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Choice(): There is no AIG.\n" );
        return 1;
    } 
    pAbc->pAig = Cec_ManChoiceComputation( pTemp = pAbc->pAig, pPars );
    if ( pAbc->pAig == NULL )
    {
        pAbc->pAig = pTemp;
        printf( "Abc_CommandAbc9Choice(): Command has failed.\n" );
    }
    else 
        Gia_ManStop( pTemp );
    return 0;

usage:
    fprintf( stdout, "usage: &choice [-C num] [-cvh]\n" );
    fprintf( stdout, "\t         performs computation of structural choices\n" );
    fprintf( stdout, "\t-C num : the max number of conflicts at a node [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-c     : toggle using circuit-based SAT solver [default = %s]\n", pPars->fUseCSat? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Sat( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cec_ParSat_t ParsSat, * pPars = &ParsSat;
    Gia_Man_t * pTemp;
    int c;
    int fCSat = 0;
    Cec_ManSatSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CSNnmfcvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'S':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-S\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nSatVarMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nSatVarMax < 0 ) 
                goto usage;
            break;
        case 'N':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-N\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nCallsRecycle = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nCallsRecycle < 0 ) 
                goto usage;
            break;
        case 'n':
            pPars->fNonChrono ^= 1;
            break;
        case 'm':
            pPars->fCheckMiter ^= 1;
            break;
        case 'f':
            pPars->fFirstStop ^= 1;
            break;
        case 'c':
            fCSat ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Sat(): There is no AIG.\n" );
        return 1;
    } 
    if ( fCSat )
    {
        Vec_Int_t * vCounters;
        Vec_Str_t * vStatus;
        if ( pPars->fNonChrono )
            vCounters = Cbs_ManSolveMiterNc( pAbc->pAig, pPars->nBTLimit, &vStatus, pPars->fVerbose );
        else
            vCounters = Cbs_ManSolveMiter( pAbc->pAig, pPars->nBTLimit, &vStatus, pPars->fVerbose );
        Vec_IntFree( vCounters );
        Vec_StrFree( vStatus );
    }
    else
    {
        pAbc->pAig = Cec_ManSatSolving( pTemp = pAbc->pAig, pPars );
        Gia_ManStop( pTemp );
    }
    return 0;

usage:
    fprintf( stdout, "usage: &sat [-CSN <num>] [-nmfcvh]\n" );
    fprintf( stdout, "\t         performs SAT solving for the combinational outputs\n" );
    fprintf( stdout, "\t-C num : the max number of conflicts at a node [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-S num : the min number of variables to recycle the solver [default = %d]\n", pPars->nSatVarMax );
    fprintf( stdout, "\t-N num : the min number of calls to recycle the solver [default = %d]\n", pPars->nCallsRecycle );
    fprintf( stdout, "\t-n     : toggle using non-chronological backtracking [default = %s]\n", pPars->fNonChrono? "yes": "no" );
    fprintf( stdout, "\t-m     : toggle miter vs. any circuit [default = %s]\n", pPars->fCheckMiter? "yes": "no" );
    fprintf( stdout, "\t-f     : toggle quitting when one PO is asserted [default = %s]\n", pPars->fFirstStop? "yes": "no" );
    fprintf( stdout, "\t-c     : toggle using circuit-based SAT solver [default = %s]\n", fCSat? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Fraig( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cec_ParFra_t ParsFra, * pPars = &ParsFra;
    Gia_Man_t * pTemp;
    int c;
    Cec_ManFraSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "WRILDCrmfdwvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'W':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-W\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nWords = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nWords < 0 ) 
                goto usage;
            break;
        case 'R':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-R\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nRounds = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nRounds < 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nItersMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nItersMax < 0 ) 
                goto usage;
            break;
        case 'L':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-L\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nLevelMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nLevelMax < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nDepthMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nDepthMax < 0 ) 
                goto usage;
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'r':
            pPars->fRewriting ^= 1;
            break;
        case 'm':
            pPars->fCheckMiter ^= 1;
            break;
        case 'f':
            pPars->fFirstStop ^= 1;
            break;
        case 'd':
            pPars->fDualOut ^= 1;
            break;
        case 'w':
            pPars->fVeryVerbose ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Fraig(): There is no AIG.\n" );
        return 1;
    } 
    pTemp = Cec_ManSatSweeping( pAbc->pAig, pPars );
    if ( pTemp )
    {
        Gia_ManStop( pAbc->pAig );
        pAbc->pAig = pTemp;
    }
    return 0;

usage:
    fprintf( stdout, "usage: &fraig [-WRILDC <num>] [-rmfdwvh]\n" );
    fprintf( stdout, "\t         performs combinational SAT sweeping\n" );
    fprintf( stdout, "\t-W num : the number of simulation words [default = %d]\n", pPars->nWords );
    fprintf( stdout, "\t-R num : the number of simulation rounds [default = %d]\n", pPars->nRounds );
    fprintf( stdout, "\t-I num : the number of sweeping iterations [default = %d]\n", pPars->nItersMax );
    fprintf( stdout, "\t-L num : the max number of levels of nodes to consider [default = %d]\n", pPars->nLevelMax );
    fprintf( stdout, "\t-D num : the max number of steps of speculative reduction [default = %d]\n", pPars->nDepthMax );
    fprintf( stdout, "\t-C num : the max number of conflicts at a node [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-r     : toggle the use of AIG rewriting [default = %s]\n", pPars->fRewriting? "yes": "no" );
    fprintf( stdout, "\t-m     : toggle miter vs. any circuit [default = %s]\n", pPars->fCheckMiter? "yes": "no" );
    fprintf( stdout, "\t-f     : toggle quitting when one PO is asserted [default = %s]\n", pPars->fFirstStop? "yes": "no" );
    fprintf( stdout, "\t-d     : toggle using double output miters [default = %s]\n", pPars->fDualOut? "yes": "no" );
    fprintf( stdout, "\t-w     : toggle printing even more verbose information [default = %s]\n", pPars->fVeryVerbose? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Srm( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char * pFileName = "gsrm.aig";
    Gia_Man_t * pTemp;
    int c, fVerbose = 0;
    int fDualOut = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "dvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'd':
            fDualOut ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Srm(): There is no AIG.\n" );
        return 1;
    } 
//    pAbc->pAig = Gia_ManSpecReduce( pTemp = pAbc->pAig, fDualOut );
//    Gia_ManStop( pTemp );
    pTemp = Gia_ManSpecReduce( pAbc->pAig, fDualOut, fVerbose );
    if ( pTemp )
    {
        Gia_WriteAiger( pTemp, pFileName, 0, 0 );
        printf( "Speculatively reduced model was written into file \"%s\".\n", pFileName );
        Gia_ManPrintStatsShort( pTemp );
        Gia_ManStop( pTemp );
    }
    return 0;

usage:
    fprintf( stdout, "usage: &srm [-dvh]\n" );
    fprintf( stdout, "\t         writes speculatively reduced model into file \"%s\"\n", pFileName );
    fprintf( stdout, "\t-d     : toggle creating dual output miter [default = %s]\n", fDualOut? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Reduce( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp;
    int c, fVerbose = 0;
    int fUseAll = 0;
    int fDualOut = 0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "advh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'a':
            fUseAll ^= 1;
            break;
        case 'd':
            fDualOut ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Reduce(): There is no AIG.\n" );
        return 1;
    } 
    pAbc->pAig = Gia_ManEquivReduce( pTemp = pAbc->pAig, fUseAll, fDualOut, fVerbose );
    if ( pAbc->pAig == NULL )
        pAbc->pAig = pTemp;
    else
    {
        Gia_ManStop( pTemp );
        pAbc->pAig = Gia_ManSeqStructSweep( pTemp = pAbc->pAig, 1, 1, 0 );
        Gia_ManStop( pTemp );
    }
    return 0;

usage:
    fprintf( stdout, "usage: &reduce [-advh]\n" );
    fprintf( stdout, "\t         reduces the circuit using equivalence classes\n" );
    fprintf( stdout, "\t-a     : toggle merging all equivalences [default = %s]\n", fUseAll? "yes": "no" );
    fprintf( stdout, "\t-d     : toggle using dual-output merging [default = %s]\n", fDualOut? "yes": "no" );
    fprintf( stdout, "\t-v     : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Cec( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Cec_ParCec_t ParsCec, * pPars = &ParsCec;
    FILE * pFile;
    Gia_Man_t * pSecond, * pMiter;
    char * FileName, * pTemp;
    char ** pArgvNew;
    int c, nArgcNew, fMiter = 0;
    Cec_ManCecSetDefaultParams( pPars );
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "CTmvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nBTLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nBTLimit < 0 ) 
                goto usage;
            break;
        case 'T':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-T\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->TimeLimit = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->TimeLimit < 0 ) 
                goto usage;
            break;
        case 'm':
            fMiter ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( fMiter )
    {
        printf( "Assuming the current network is a double-output miter. (Conflict limit = %d.)\n", pPars->nBTLimit );
        Cec_ManVerify( pAbc->pAig, pPars );
        return 0;
    }

    pArgvNew = argv + globalUtilOptind;
    nArgcNew = argc - globalUtilOptind;
    if ( nArgcNew != 1 )
    {
        printf( "File name is not given on the command line.\n" );
        return 1;
    }

    // get the input file name
    FileName = pArgvNew[0];
    // fix the wrong symbol
    for ( pTemp = FileName; *pTemp; pTemp++ )
        if ( *pTemp == '>' )
            *pTemp = '\\';
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( (FileName = Extra_FileGetSimilarName( FileName, ".aig", NULL, NULL, NULL, NULL )) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );
    pSecond = Gia_ReadAiger( FileName, 0 );
    if ( pSecond == NULL )
    {
        printf( "Reading AIGER has failed.\n" );
        return 0;
    }
    // compute the miter
    pMiter = Gia_ManMiter( pAbc->pAig, pSecond, 1, 0, pPars->fVerbose );
    if ( pMiter )
    {
        Cec_ManVerify( pMiter, pPars );
        Gia_ManStop( pMiter );
    }
    Gia_ManStop( pSecond );
    return 0;

usage:
    fprintf( stdout, "usage: &cec [-CT num] [-mvh]\n" );
    fprintf( stdout, "\t         new combinational equivalence checker\n" );
    fprintf( stdout, "\t-C num : the max number of conflicts at a node [default = %d]\n", pPars->nBTLimit );
    fprintf( stdout, "\t-T num : approximate runtime limit in seconds [default = %d]\n", pPars->TimeLimit );
    fprintf( stdout, "\t-m     : toggle miter vs. two circuits [default = %s]\n", fMiter? "miter":"two circuits");
    fprintf( stdout, "\t-v     : toggle verbose output [default = %s]\n", pPars->fVerbose? "yes":"no");
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Force( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    int nIters     = 20;
    int fClustered =  1;
    int fVerbose   =  1;
    int c;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "Icvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-I\" should be followed by an integer.\n" );
                goto usage;
            }
            nIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( nIters < 0 ) 
                goto usage;
            break;
        case 'c':
            fClustered ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Force(): There is no AIG.\n" );
        return 1;
    } 
    For_ManExperiment( pAbc->pAig, nIters, fClustered, fVerbose );
    return 0;

usage:
    fprintf( stdout, "usage: &force [-I <num>] [-cvh]\n" );
    fprintf( stdout, "\t         one-dimensional placement algorithm FORCE introduced by\n" );
    fprintf( stdout, "\t         F. A. Aloul, I. L. Markov, and K. A. Sakallah (GLSVLSI�03).\n" );
    fprintf( stdout, "\t-I num : the number of refinement iterations [default = %d]\n", nIters );
    fprintf( stdout, "\t-c     : toggle clustered representation [default = %s]\n", fClustered? "yes":"no");
    fprintf( stdout, "\t-v     : toggle verbose output [default = %s]\n", fVerbose? "yes":"no");
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Embed( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Emb_Par_t Pars, * pPars = &Pars;
    int c;
    pPars->nDims      = 30;
    pPars->nIters     = 10;
    pPars->nSols      =  2;
    pPars->fRefine    =  0; 
    pPars->fCluster   =  0;
    pPars->fDump      =  0; 
    pPars->fDumpLarge =  0; 
    pPars->fShowImage =  0;
    pPars->fVerbose   =  0;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "DIrcdlsvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nDims = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nDims < 0 ) 
                goto usage;
            break;
        case 'I':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by an integer.\n" );
                goto usage;
            }
            pPars->nIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nIters < 0 ) 
                goto usage;
            break;
        case 'r':
            pPars->fRefine ^= 1;
            break;
        case 'c':
            pPars->fCluster ^= 1;
            break;
        case 'd':
            pPars->fDump ^= 1;
            break;
        case 'l':
            pPars->fDumpLarge ^= 1;
            break;
        case 's': 
            pPars->fShowImage ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Embed(): There is no AIG.\n" );
        return 1;
    } 
    Gia_ManSolveProblem( pAbc->pAig, pPars );
    return 0;

usage:
    fprintf( stdout, "usage: &embed [-DI <num>] [-rdlscvh]\n" );
    fprintf( stdout, "\t         fast placement based on high-dimensional embedding from\n" );
    fprintf( stdout, "\t         D. Harel and Y. Koren, \"Graph drawing by high-dimensional\n" );
    fprintf( stdout, "\t         embedding\", J. Graph Algs & Apps, 2004, Vol 8(2), pp. 195-217\n" );
    fprintf( stdout, "\t-D num : the number of dimensions for embedding [default = %d]\n", pPars->nDims );
    fprintf( stdout, "\t-I num : the number of refinement iterations [default = %d]\n", pPars->nIters );
    fprintf( stdout, "\t-r     : toggle the use of refinement [default = %s]\n", pPars->fRefine? "yes":"no");
    fprintf( stdout, "\t-c     : toggle clustered representation [default = %s]\n", pPars->fCluster? "yes":"no");
    fprintf( stdout, "\t-d     : toggle dumping placement into a Gnuplot file [default = %s]\n", pPars->fDump? "yes":"no");
    fprintf( stdout, "\t-l     : toggle dumping Gnuplot for large placement [default = %s]\n", pPars->fDumpLarge? "yes":"no");
    fprintf( stdout, "\t-s     : toggle showing image if Gnuplot is installed [default = %s]\n", pPars->fShowImage? "yes":"no");
    fprintf( stdout, "\t-v     : toggle verbose output [default = %s]\n", pPars->fVerbose? "yes":"no");
    fprintf( stdout, "\t-h     : print the command usage\n");
    return 1;
}
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9If( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    char Buffer[200];
    char LutSize[200];
    If_Par_t Pars, * pPars = &Pars;
    int c;
    extern void Gia_ManSetIfParsDefault( If_Par_t * pPars );
    extern int Gia_MappingIf( Gia_Man_t * p, If_Par_t * pPars );
    if ( pAbc->pAbc8Lib == NULL )
    {
        printf( "LUT library is not given. Using default LUT library.\n" );
        pAbc->pAbc8Lib = If_SetSimpleLutLib( 6 );
    }
    // set defaults
    Gia_ManSetIfParsDefault( pPars );
    pPars->pLutLib = pAbc->pAbc8Lib;
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "KCFADEqaflepmrstbvh" ) ) != EOF )
    {
        switch ( c )
        {
        case 'K':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-K\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nLutSize = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nLutSize < 0 ) 
                goto usage;
            // if the LUT size is specified, disable library
            pPars->pLutLib = NULL; 
            break;
        case 'C':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-C\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nCutsMax = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nCutsMax < 0 ) 
                goto usage;
            break;
        case 'F':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-F\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nFlowIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nFlowIters < 0 ) 
                goto usage;
            break;
        case 'A':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-A\" should be followed by a positive integer.\n" );
                goto usage;
            }
            pPars->nAreaIters = atoi(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->nAreaIters < 0 ) 
                goto usage;
            break;
        case 'D':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-D\" should be followed by a floating point number.\n" );
                goto usage;
            }
            pPars->DelayTarget = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->DelayTarget <= 0.0 ) 
                goto usage;
            break;
        case 'E':
            if ( globalUtilOptind >= argc )
            {
                fprintf( stdout, "Command line switch \"-E\" should be followed by a floating point number.\n" );
                goto usage;
            }
            pPars->Epsilon = (float)atof(argv[globalUtilOptind]);
            globalUtilOptind++;
            if ( pPars->Epsilon < 0.0 || pPars->Epsilon > 1.0 ) 
                goto usage;
            break;
        case 'q':
            pPars->fPreprocess ^= 1;
            break;
        case 'a':
            pPars->fArea ^= 1;
            break;
        case 'r':
            pPars->fExpRed ^= 1;
            break;
        case 'f':
            pPars->fFancy ^= 1;
            break;
        case 'l':
            pPars->fLatchPaths ^= 1;
            break;
        case 'e':
            pPars->fEdge ^= 1;
            break;
        case 'p':
            pPars->fPower ^= 1;
            break;
        case 'm':
            pPars->fCutMin ^= 1;
            break;
        case 's':
            pPars->fSeqMap ^= 1;
            break;
        case 't':
            pPars->fLiftLeaves ^= 1;
            break;
        case 'b':
            pPars->fBidec ^= 1;
            break;
        case 'v':
            pPars->fVerbose ^= 1;
            break;
        case 'h':
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9If(): There is no AIG to map.\n" );
        return 1;
    }

    if ( pPars->nLutSize < 3 || pPars->nLutSize > IF_MAX_LUTSIZE )
    {
        fprintf( stdout, "Incorrect LUT size (%d).\n", pPars->nLutSize );
        return 1;
    }

    if ( pPars->nCutsMax < 1 || pPars->nCutsMax >= (1<<12) )
    {
        fprintf( stdout, "Incorrect number of cuts.\n" );
        return 1;
    }
/*
    // enable truth table computation if choices are selected
    if ( (c = Aig_ManChoiceNum( pAbc->pAbc8Aig )) )
    {
        printf( "Performing LUT mapping with %d choices.\n", c );
        pPars->fExpRed = 0;
    }
*/
    // enable truth table computation if cut minimization is selected
    if ( pPars->fCutMin )
    {
        pPars->fTruth = 1;
        pPars->fExpRed = 0;
    }

    // complain if truth tables are requested but the cut size is too large
    if ( pPars->fTruth && pPars->nLutSize > IF_MAX_FUNC_LUTSIZE )
    {
        fprintf( stdout, "Truth tables cannot be computed for LUT larger than %d inputs.\n", IF_MAX_FUNC_LUTSIZE );
        return 1;
    }

    if ( !Gia_MappingIf( pAbc->pAig, pPars ) )
    {
        printf( "Abc_CommandAbc9If(): Mapping of the AIG has failed.\n" );
        return 1;
    }
    return 0;

usage:
    if ( pPars->DelayTarget == -1 ) 
        sprintf( Buffer, "best possible" );
    else
        sprintf( Buffer, "%.2f", pPars->DelayTarget );
    if ( pPars->nLutSize == -1 ) 
        sprintf( LutSize, "library" );
    else
        sprintf( LutSize, "%d", pPars->nLutSize );
    fprintf( stdout, "usage: &if [-KCFA num] [-DE float] [-qarlepmbvh]\n" );
    fprintf( stdout, "\t           performs FPGA technology mapping of the network\n" );
    fprintf( stdout, "\t-K num   : the number of LUT inputs (2 < num < %d) [default = %s]\n", IF_MAX_LUTSIZE+1, LutSize );
    fprintf( stdout, "\t-C num   : the max number of priority cuts (0 < num < 2^12) [default = %d]\n", pPars->nCutsMax );
    fprintf( stdout, "\t-F num   : the number of area flow recovery iterations (num >= 0) [default = %d]\n", pPars->nFlowIters );
    fprintf( stdout, "\t-A num   : the number of exact area recovery iterations (num >= 0) [default = %d]\n", pPars->nAreaIters );
    fprintf( stdout, "\t-D float : sets the delay constraint for the mapping [default = %s]\n", Buffer );  
    fprintf( stdout, "\t-E float : sets epsilon used for tie-breaking [default = %f]\n", pPars->Epsilon );  
    fprintf( stdout, "\t-q       : toggles preprocessing using several starting points [default = %s]\n", pPars->fPreprocess? "yes": "no" );
    fprintf( stdout, "\t-a       : toggles area-oriented mapping [default = %s]\n", pPars->fArea? "yes": "no" );
//    fprintf( stdout, "\t-f       : toggles one fancy feature [default = %s]\n", pPars->fFancy? "yes": "no" );
    fprintf( stdout, "\t-r       : enables expansion/reduction of the best cuts [default = %s]\n", pPars->fExpRed? "yes": "no" );
    fprintf( stdout, "\t-l       : optimizes latch paths for delay, other paths for area [default = %s]\n", pPars->fLatchPaths? "yes": "no" );
    fprintf( stdout, "\t-e       : uses edge-based cut selection heuristics [default = %s]\n", pPars->fEdge? "yes": "no" );
    fprintf( stdout, "\t-p       : uses power-aware cut selection heuristics [default = %s]\n", pPars->fPower? "yes": "no" );
    fprintf( stdout, "\t-m       : enables cut minimization by removing vacuous variables [default = %s]\n", pPars->fCutMin? "yes": "no" );
//    fprintf( stdout, "\t-s       : toggles sequential mapping [default = %s]\n", pPars->fSeqMap? "yes": "no" );
//    fprintf( stdout, "\t-t       : toggles the use of true sequential cuts [default = %s]\n", pPars->fLiftLeaves? "yes": "no" );
    fprintf( stdout, "\t-b       : toggles deriving local AIGs using bi-decomposition [default = %s]\n", pPars->fBidec? "yes": "no" );
    fprintf( stdout, "\t-v       : toggles verbose output [default = %s]\n", pPars->fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h       : prints the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbc9Test( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Gia_Man_t * pTemp = NULL;
    int c, fVerbose = 0;
    int fSwitch = 0;
    extern void Gia_SatSolveTest( Gia_Man_t * p );
    extern void Cbs_ManSolveTest( Gia_Man_t * pGia );

    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "svh" ) ) != EOF )
    {
        switch ( c )
        {
        case 's':
            fSwitch ^= 1;
            break;
        case 'v':
            fVerbose ^= 1;
            break;
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }
    if ( pAbc->pAig == NULL )
    {
        printf( "Abc_CommandAbc9Test(): There is no AIG.\n" );
        return 1;
    } 
//    Gia_ManFrontTest( pAbc->pAig );
//    Gia_ManReduceConst( pAbc->pAig, 1 );
//    Sat_ManTest( pAbc->pAig, Gia_ManCo(pAbc->pAig, 0), 0 );
//    Gia_ManTestDistance( pAbc->pAig );
//    Gia_SatSolveTest( pAbc->pAig );
//    For_ManExperiment( pAbc->pAig, 20, 1, 1 );
//    Gia_ManUnrollSpecial( pAbc->pAig, 5, 100, 1 );

    if ( fSwitch )
        pAbc->pAig = Gia_ManDupSelf( pTemp = pAbc->pAig );
    else 
        pAbc->pAig = Gia_ManRemoveEnables( pTemp = pAbc->pAig );
    Gia_ManStop( pTemp );
//    Cbs_ManSolveTest( pAbc->pAig );
    return 0;

usage:
    fprintf( stdout, "usage: &test [-svh]\n" );
    fprintf( stdout, "\t        testing various procedures\n" );
    fprintf( stdout, "\t-s    : toggle enable (yes) vs. disable (no) [default = %s]\n", fSwitch? "yes": "no" );
    fprintf( stdout, "\t-v    : toggle printing verbose information [default = %s]\n", fVerbose? "yes": "no" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Abc_CommandAbcTestNew( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    extern int Abc_NtkTestProcedure( Abc_Ntk_t * pNtk1, Abc_Ntk_t * pNtk2 );

    Abc_Ntk_t * pNtk;
    int c;

    pNtk = Abc_FrameReadNtk(pAbc);

    // set defaults
    Extra_UtilGetoptReset();
    while ( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
        case 'h':
            goto usage;
        default:
            goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( stdout, "Empty network.\n" );
        return 1;
    }

    if ( !Abc_NtkIsStrash( pNtk) )
    {
        fprintf( stdout, "The current network is not an AIG. Cannot continue.\n" );
        return 1;
    }

//    Abc_NtkTestProcedure( pNtk, NULL );

    return 0;

usage:
    fprintf( stdout, "usage: testnew [-h]\n" );
    fprintf( stdout, "\t        new testing procedure\n" );
    fprintf( stdout, "\t-h    : print the command usage\n");
    return 1;
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


