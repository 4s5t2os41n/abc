/**CFile****************************************************************

  FileName    [sclIo.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  Synopsis    [Standard-cell library representation.]

  Author      [Alan Mishchenko, Niklas Een]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - August 24, 2012.]

  Revision    [$Id: sclIo.c,v 1.0 2012/08/24 00:00:00 alanmi Exp $]

***********************************************************************/

#include "sclInt.h"

ABC_NAMESPACE_IMPL_START


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

extern void Extra_PrintHex( FILE * pFile, unsigned Sign[], int nBits );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    [Binary I/O for numbers (int/float/etc) and strings (char *).]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
static inline void Vec_StrPutI_ne( Vec_Str_t * vOut, int Val )
{
    int i;
    for ( i = 0; i < 4; i++ )
        Vec_StrPush( vOut, (char)(Val >> (8*i)) );
}
static inline int Vec_StrGetI_ne( Vec_Str_t * vOut, int * pPos )
{
    int i;
    int Val = 0;
    for ( i = 0; i < 4; i++ )
        Val |= (int)(unsigned char)Vec_StrEntry(vOut, (*pPos)++) << (8*i);
    return Val;
}

static inline void Vec_StrPutI( Vec_Str_t * vOut, int Val )
{
    for ( ; Val >= 0x80; Val >>= 7 )
        Vec_StrPush( vOut, (unsigned char)(Val | 0x80) );
    Vec_StrPush( vOut, (unsigned char)Val );
}
static inline int Vec_StrGetI( Vec_Str_t * vOut, int * pPos )
{
    unsigned char ch;
    int i = 0, Val = 0;
    while ( (ch = Vec_StrEntry(vOut, (*pPos)++)) & 0x80 )
        Val |= (ch & 0x7f) << (7 * i++);
    return Val | (ch << (7 * i));
}

static inline void Vec_StrPutW( Vec_Str_t * vOut, word Val )
{
    int i;
    for ( i = 0; i < 8; i++ )
        Vec_StrPush( vOut, (char)(Val >> (8*i)) );
}
static inline word Vec_StrGetW( Vec_Str_t * vOut, int * pPos )
{
    int i;
    word Val = 0;
    for ( i = 0; i < 8; i++ )
        Val |= (word)(unsigned char)Vec_StrEntry(vOut, (*pPos)++) << (8*i);
    return Val;
}

static inline void Vec_StrPutF( Vec_Str_t * vOut, float Val )
{
    union { float num; unsigned char data[4]; } tmp;
    tmp.num = Val;
    Vec_StrPush( vOut, tmp.data[0] );
    Vec_StrPush( vOut, tmp.data[1] );
    Vec_StrPush( vOut, tmp.data[2] );
    Vec_StrPush( vOut, tmp.data[3] );
}
static inline float Vec_StrGetF( Vec_Str_t * vOut, int * pPos )
{
    union { float num; unsigned char data[4]; } tmp;
    tmp.data[0] = Vec_StrEntry( vOut, (*pPos)++ );
    tmp.data[1] = Vec_StrEntry( vOut, (*pPos)++ );
    tmp.data[2] = Vec_StrEntry( vOut, (*pPos)++ );
    tmp.data[3] = Vec_StrEntry( vOut, (*pPos)++ );
    return tmp.num;
}

static inline void Vec_StrPutD( Vec_Str_t * vOut, double Val )
{
    union { double num; unsigned char data[8]; } tmp;
    int i, Lim = sizeof(double);
    tmp.num = Val;
    for ( i = 0; i < Lim; i++ )
        Vec_StrPush( vOut, tmp.data[i] );
}
static inline double Vec_StrGetD( Vec_Str_t * vOut, int * pPos )
{
    union { double num; unsigned char data[8]; } tmp;
    int i, Lim = sizeof(double);
    for ( i = 0; i < Lim; i++ )
        tmp.data[i] = Vec_StrEntry( vOut, (*pPos)++ );
    return tmp.num;
}

static inline void Vec_StrPutS( Vec_Str_t * vOut, char * pStr )
{
    while ( *pStr )
        Vec_StrPush( vOut, *pStr++ );
    Vec_StrPush( vOut, (char)0 );
}
static inline char * Vec_StrGetS( Vec_Str_t * vOut, int * pPos )
{
    char * pStr = Vec_StrEntryP( vOut, *pPos );
    while ( Vec_StrEntry(vOut, (*pPos)++) );
    return Abc_UtilStrsav(pStr);
}

static inline void Vec_StrPutC( Vec_Str_t * vOut, char c )
{
    Vec_StrPush( vOut, c );
}
static inline char Vec_StrGetC( Vec_Str_t * vOut, int * pPos )
{
    return Vec_StrEntry(vOut, (*pPos)++);
}


/**Function*************************************************************

  Synopsis    [Reading library from file.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
static void Abc_SclReadSurface( Vec_Str_t * vOut, int * pPos, SC_Surface * p )
{
    Vec_Flt_t * vVec;
    int i, j;

    for ( i = Vec_StrGetI(vOut, pPos); i != 0; i-- )
        Vec_FltPush( p->vIndex0, Vec_StrGetF(vOut, pPos) );

    for ( i = Vec_StrGetI(vOut, pPos); i != 0; i-- )
        Vec_FltPush( p->vIndex1, Vec_StrGetF(vOut, pPos) );

    for ( i = 0; i < Vec_FltSize(p->vIndex0); i++ )
    {
        vVec = Vec_FltAlloc( Vec_FltSize(p->vIndex1) );
        Vec_PtrPush( p->vData, vVec );
        for ( j = 0; j < Vec_FltSize(p->vIndex1); j++ )
            Vec_FltPush( vVec, Vec_StrGetF(vOut, pPos) );
    }

    for ( i = 0; i < 3; i++ ) 
        p->approx[0][i] = Vec_StrGetF( vOut, pPos );
    for ( i = 0; i < 4; i++ ) 
        p->approx[1][i] = Vec_StrGetF( vOut, pPos );
    for ( i = 0; i < 6; i++ ) 
        p->approx[2][i] = Vec_StrGetF( vOut, pPos );
}
static void Abc_SclReadLibrary( Vec_Str_t * vOut, int * pPos, SC_Lib * p )
{
    int i, j, k, n;
    int version = Vec_StrGetI( vOut, pPos );
    assert( version == ABC_SCL_CUR_VERSION ); // wrong version of the file

    // Read non-composite fields:
    p->lib_name              = Vec_StrGetS(vOut, pPos);
    p->default_wire_load     = Vec_StrGetS(vOut, pPos);
    p->default_wire_load_sel = Vec_StrGetS(vOut, pPos);
    p->default_max_out_slew  = Vec_StrGetF(vOut, pPos);

    p->unit_time             = Vec_StrGetI(vOut, pPos);
    p->unit_cap_fst          = Vec_StrGetF(vOut, pPos);
    p->unit_cap_snd          = Vec_StrGetI(vOut, pPos);

    // Read 'wire_load' vector:
    for ( i = Vec_StrGetI(vOut, pPos); i != 0; i-- )
    {
        SC_WireLoad * pWL = Abc_SclWireLoadAlloc();
        Vec_PtrPush( p->vWireLoads, pWL );

        pWL->name = Vec_StrGetS(vOut, pPos);
        pWL->res  = Vec_StrGetF(vOut, pPos);
        pWL->cap  = Vec_StrGetF(vOut, pPos);

        for ( j = Vec_StrGetI(vOut, pPos); j != 0; j-- )
        {
            Vec_IntPush( pWL->vFanout, Vec_StrGetI(vOut, pPos) );
            Vec_FltPush( pWL->vLen,    Vec_StrGetF(vOut, pPos) );
        }
    }

    // Read 'wire_load_sel' vector:
    for ( i = Vec_StrGetI(vOut, pPos); i != 0; i-- )
    {
        SC_WireLoadSel * pWLS = Abc_SclWireLoadSelAlloc();
        Vec_PtrPush( p->vWireLoadSels, pWLS );

        pWLS->name = Vec_StrGetS(vOut, pPos);
        for ( j = Vec_StrGetI(vOut, pPos); j != 0; j-- )
        {
            Vec_FltPush( pWLS->vAreaFrom,      Vec_StrGetF(vOut, pPos) );
            Vec_FltPush( pWLS->vAreaTo,        Vec_StrGetF(vOut, pPos) );
            Vec_PtrPush( pWLS->vWireLoadModel, Vec_StrGetS(vOut, pPos) );
        }
    }

    for ( i = Vec_StrGetI(vOut, pPos); i != 0; i-- )
    {
        SC_Cell * pCell = Abc_SclCellAlloc();
        Vec_PtrPush( p->vCells, pCell );

        pCell->name           = Vec_StrGetS(vOut, pPos);     
        pCell->area           = Vec_StrGetF(vOut, pPos);
        pCell->drive_strength = Vec_StrGetI(vOut, pPos);

        pCell->n_inputs       = Vec_StrGetI(vOut, pPos);
        pCell->n_outputs      = Vec_StrGetI(vOut, pPos);

        for ( j = 0; j < pCell->n_inputs; j++ )
        {
            SC_Pin * pPin = Abc_SclPinAlloc();
            Vec_PtrPush( pCell->vPins, pPin );

            pPin->dir      = sc_dir_Input;
            pPin->name     = Vec_StrGetS(vOut, pPos); 
            pPin->rise_cap = Vec_StrGetF(vOut, pPos);
            pPin->fall_cap = Vec_StrGetF(vOut, pPos);
        }

        for ( j = 0; j < pCell->n_outputs; j++ )
        {
            SC_Pin * pPin = Abc_SclPinAlloc();
            Vec_PtrPush( pCell->vPins, pPin );

            pPin->dir          = sc_dir_Output;
            pPin->name         = Vec_StrGetS(vOut, pPos); 
            pPin->max_out_cap  = Vec_StrGetF(vOut, pPos);
            pPin->max_out_slew = Vec_StrGetF(vOut, pPos);

            k = Vec_StrGetI(vOut, pPos);
            assert( k == pCell->n_inputs );

            // read functions
            assert( Vec_WrdSize(pPin->vFunc) == 0 );
            Vec_WrdGrow( pPin->vFunc, Abc_Truth6WordNum(pCell->n_inputs) );
            for ( k = 0; k < Vec_WrdCap(pPin->vFunc); k++ )
                Vec_WrdPush( pPin->vFunc, Vec_StrGetW(vOut, pPos) );

            // Read 'rtiming': (pin-to-pin timing tables for this particular output)
            for ( k = 0; k < pCell->n_inputs; k++ )
            {
                SC_Timings * pRTime = Abc_SclTimingsAlloc();
                Vec_PtrPush( pPin->vRTimings, pRTime );

                pRTime->name = Vec_StrGetS(vOut, pPos);
                n = Vec_StrGetI(vOut, pPos); assert( n <= 1 );
                if ( n == 1 )
                {
                    SC_Timing * pTime = Abc_SclTimingAlloc();
                    Vec_PtrPush( pRTime->vTimings, pTime );

                    pTime->tsense = (SC_TSense)Vec_StrGetI(vOut, pPos);
                    Abc_SclReadSurface( vOut, pPos, pTime->pCellRise );
                    Abc_SclReadSurface( vOut, pPos, pTime->pCellFall );
                    Abc_SclReadSurface( vOut, pPos, pTime->pRiseTrans );
                    Abc_SclReadSurface( vOut, pPos, pTime->pFallTrans );
                }
                else
                    assert( Vec_PtrSize(pPin->vRTimings) == 0 );
            }
        }
    }
}
SC_Lib * Abc_SclRead( char * pFileName )
{
    SC_Lib * p;
    FILE * pFile;
    Vec_Str_t * vOut;
    int nFileSize, Pos = 0;

    pFile = fopen( pFileName, "rb" );
    if ( pFile == NULL )
    {
        printf( "Cannot open file \"%s\" for reading.\n", pFileName );
        return NULL;
    }
    // get the file size, in bytes
    fseek( pFile, 0, SEEK_END );  
    nFileSize = ftell( pFile );  
    rewind( pFile ); 
    // load the contents
    vOut = Vec_StrAlloc( nFileSize );
    vOut->nSize = vOut->nCap;
    assert( nFileSize == Vec_StrSize(vOut) );
    nFileSize = fread( Vec_StrArray(vOut), 1, Vec_StrSize(vOut), pFile );
    assert( nFileSize == Vec_StrSize(vOut) );
    fclose( pFile );
    // read the library
    p = Abc_SclLibAlloc();
    Abc_SclReadLibrary( vOut, &Pos, p );
    assert( Pos == Vec_StrSize(vOut) );
    Vec_StrFree( vOut );
    // hash gates by name
    Abc_SclHashCells( p );
    Abc_SclLinkCells( p );
    return p;
}
void Abc_SclLoad( char * pFileName, SC_Lib ** ppScl )
{
    if ( *ppScl )
    {
        Abc_SclLibFree( *ppScl );
        *ppScl = NULL;
    }
    assert( *ppScl == NULL );
    if ( pFileName )
        *(SC_Lib **)ppScl = Abc_SclRead( pFileName );
}


/**Function*************************************************************

  Synopsis    [Writing library into file.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
static void Abc_SclWriteSurface( Vec_Str_t * vOut, SC_Surface * p )
{
    Vec_Flt_t * vVec;
    float Entry;
    int i, k;

    Vec_StrPutI( vOut, Vec_FltSize(p->vIndex0) );
    Vec_FltForEachEntry( p->vIndex0, Entry, i )
        Vec_StrPutF( vOut, Entry );

    Vec_StrPutI( vOut, Vec_FltSize(p->vIndex1) );
    Vec_FltForEachEntry( p->vIndex1, Entry, i )
        Vec_StrPutF( vOut, Entry );

    Vec_PtrForEachEntry( Vec_Flt_t *, p->vData, vVec, i )
        Vec_FltForEachEntry( vVec, Entry, k )
            Vec_StrPutF( vOut, Entry );

    for ( i = 0; i < 3; i++ ) 
        Vec_StrPutF( vOut, p->approx[0][i] );
    for ( i = 0; i < 4; i++ ) 
        Vec_StrPutF( vOut, p->approx[1][i] );
    for ( i = 0; i < 6; i++ ) 
        Vec_StrPutF( vOut, p->approx[2][i] );
}
static void Abc_SclWriteLibrary( Vec_Str_t * vOut, SC_Lib * p )
{
    SC_WireLoad * pWL;
    SC_WireLoadSel * pWLS;
    SC_Cell * pCell;
    SC_Pin * pPin;
    int n_valid_cells;
    int i, j, k;

    Vec_StrPutI( vOut, ABC_SCL_CUR_VERSION );

    // Write non-composite fields:
    Vec_StrPutS( vOut, p->lib_name );
    Vec_StrPutS( vOut, p->default_wire_load );
    Vec_StrPutS( vOut, p->default_wire_load_sel );
    Vec_StrPutF( vOut, p->default_max_out_slew );

    assert( p->unit_time >= 0 );
    assert( p->unit_cap_snd >= 0 );
    Vec_StrPutI( vOut, p->unit_time );
    Vec_StrPutF( vOut, p->unit_cap_fst );
    Vec_StrPutI( vOut, p->unit_cap_snd );

    // Write 'wire_load' vector:
    Vec_StrPutI( vOut, Vec_PtrSize(p->vWireLoads) );
    Vec_PtrForEachEntry( SC_WireLoad *, p->vWireLoads, pWL, i )
    {
        Vec_StrPutS( vOut, pWL->name );
        Vec_StrPutF( vOut, pWL->res );
        Vec_StrPutF( vOut, pWL->cap );

        Vec_StrPutI( vOut, Vec_IntSize(pWL->vFanout) );
        for ( j = 0; j < Vec_IntSize(pWL->vFanout); j++ )
        {
            Vec_StrPutI( vOut, Vec_IntEntry(pWL->vFanout, j) );
            Vec_StrPutF( vOut, Vec_FltEntry(pWL->vLen, j) );
        }
    }

    // Write 'wire_load_sel' vector:
    Vec_StrPutI( vOut, Vec_PtrSize(p->vWireLoadSels) );
    Vec_PtrForEachEntry( SC_WireLoadSel *, p->vWireLoadSels, pWLS, i )
    {
        Vec_StrPutS( vOut, pWLS->name );
        Vec_StrPutI( vOut, Vec_FltSize(pWLS->vAreaFrom) );
        for ( j = 0; j < Vec_FltSize(pWLS->vAreaFrom); j++)
        {
            Vec_StrPutF( vOut, Vec_FltEntry(pWLS->vAreaFrom, j) );
            Vec_StrPutF( vOut, Vec_FltEntry(pWLS->vAreaTo, j) );
            Vec_StrPutS( vOut, (char *)Vec_PtrEntry(pWLS->vWireLoadModel, j) );
        }
    }

    // Write 'cells' vector:
    n_valid_cells = 0;
    SC_LitForEachCell( p, pCell, i )
        if ( !(pCell->seq || pCell->unsupp) )
            n_valid_cells++;

    Vec_StrPutI( vOut, n_valid_cells );
    SC_LitForEachCell( p, pCell, i )
    {
        if ( pCell->seq || pCell->unsupp )
            continue;

        Vec_StrPutS( vOut, pCell->name );
        Vec_StrPutF( vOut, pCell->area );
        Vec_StrPutI( vOut, pCell->drive_strength );

        // Write 'pins': (sorted at this point; first inputs, then outputs)
        Vec_StrPutI( vOut, pCell->n_inputs);
        Vec_StrPutI( vOut, pCell->n_outputs);

        Vec_PtrForEachEntryStop( SC_Pin *, pCell->vPins, pPin, j, pCell->n_inputs )
        {
            assert(pPin->dir == sc_dir_Input);
            Vec_StrPutS( vOut, pPin->name );
            Vec_StrPutF( vOut, pPin->rise_cap );
            Vec_StrPutF( vOut, pPin->fall_cap );
        }

        Vec_PtrForEachEntryStart( SC_Pin *, pCell->vPins, pPin, j, pCell->n_inputs )
        {
            SC_Timings * pRTime;
            word uWord;

            assert(pPin->dir == sc_dir_Output);
            Vec_StrPutS( vOut, pPin->name );
            Vec_StrPutF( vOut, pPin->max_out_cap );
            Vec_StrPutF( vOut, pPin->max_out_slew );

            // write function
            assert( Vec_WrdSize(pPin->vFunc) == Abc_Truth6WordNum(pCell->n_inputs) );
            Vec_StrPutI( vOut, pCell->n_inputs );
            Vec_WrdForEachEntry( pPin->vFunc, uWord, k ) // -- 'size = 1u << (n_vars - 6)'
                Vec_StrPutW( vOut, uWord );  // -- 64-bit number, written uncompressed (low-byte first)

            // Write 'rtiming': (pin-to-pin timing tables for this particular output)
            assert( Vec_PtrSize(pPin->vRTimings) == pCell->n_inputs );
            Vec_PtrForEachEntry( SC_Timings *, pPin->vRTimings, pRTime, k )
            {
                Vec_StrPutS( vOut, pRTime->name );
                Vec_StrPutI( vOut, Vec_PtrSize(pRTime->vTimings) );
                    // -- NOTE! After post-processing, the size of the 'rtiming[k]' vector is either
                    // 0 or 1 (in static timing, we have merged all tables to get the worst case).
                    // The case with size 0 should only occur for multi-output gates.
                if ( Vec_PtrSize(pRTime->vTimings) == 1 )
                {
                    SC_Timing * pTime = (SC_Timing *)Vec_PtrEntry( pRTime->vTimings, 0 );
                        // -- NOTE! We don't need to save 'related_pin' string because we have sorted 
                        // the elements on input pins.
                    Vec_StrPutI( vOut, (int)pTime->tsense);
                    Abc_SclWriteSurface( vOut, pTime->pCellRise );
                    Abc_SclWriteSurface( vOut, pTime->pCellFall );
                    Abc_SclWriteSurface( vOut, pTime->pRiseTrans );
                    Abc_SclWriteSurface( vOut, pTime->pFallTrans );
                }
                else
                    assert( Vec_PtrSize(pRTime->vTimings) == 0 );
            }
        }
    }
}
void Abc_SclWrite( char * pFileName, SC_Lib * p )
{
    Vec_Str_t * vOut;
    vOut = Vec_StrAlloc( 10000 );
    Abc_SclWriteLibrary( vOut, p );
    if ( Vec_StrSize(vOut) > 0 )
    {
        FILE * pFile = fopen( pFileName, "wb" );
        if ( pFile == NULL )
            printf( "Cannot open file \"%s\" for writing.\n", pFileName );
        else
        {
            fwrite( Vec_StrArray(vOut), 1, Vec_StrSize(vOut), pFile );
            fclose( pFile );
        }
    }
    Vec_StrFree( vOut );    
}
void Abc_SclSave( char * pFileName, SC_Lib * pScl )
{
    if ( pScl == NULL ) return;
    Abc_SclWrite( pFileName, pScl );
}


/**Function*************************************************************

  Synopsis    [Writing library into text file.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
static void Abc_SclWriteSurfaceText( FILE * s, SC_Surface * p )
{
    Vec_Flt_t * vVec;
    float Entry;
    int i, k;

    fprintf( s, "          index_1(\"" );  
    Vec_FltForEachEntry( p->vIndex0, Entry, i )
        fprintf( s, "%f%s", Entry, i == Vec_FltSize(p->vIndex0)-1 ? "":", " );
    fprintf( s, "\");\n" );

    fprintf( s, "          index_2(\"" );  
    Vec_FltForEachEntry( p->vIndex1, Entry, i )
        fprintf( s, "%f%s", Entry, i == Vec_FltSize(p->vIndex1)-1 ? "":", " );
    fprintf( s, "\");\n" );

    fprintf( s, "          values (\"" );  
    Vec_PtrForEachEntry( Vec_Flt_t *, p->vData, vVec, i )
    {
        Vec_FltForEachEntry( vVec, Entry, k )
            fprintf( s, "%f%s", Entry, i == Vec_PtrSize(p->vData)-1 && k == Vec_FltSize(vVec)-1 ? "\");":", " );
        if ( i == Vec_PtrSize(p->vData)-1 )
            fprintf( s, "\n" );
        else
        {
            fprintf( s, "\\\n" );
            fprintf( s, "                   " );  
        }
    }
/*
    fprintf( s, "       approximations: \n" );
    fprintf( s, "       " );
    for ( i = 0; i < 3; i++ ) 
        fprintf( s, "%f ", p->approx[0][i] );
    fprintf( s, "\n" );
    fprintf( s, "       " );
    for ( i = 0; i < 4; i++ ) 
        fprintf( s, "%f ", p->approx[1][i] );
    fprintf( s, "\n" );
    fprintf( s, "       " );
    for ( i = 0; i < 6; i++ ) 
        fprintf( s, "%f ", p->approx[2][i] );
    fprintf( s, "\n" );
    fprintf( s, "       \n" );
*/
}
static void Abc_SclWriteLibraryText( FILE * s, SC_Lib * p )
{
    SC_WireLoad * pWL;
    SC_WireLoadSel * pWLS;
    SC_Cell * pCell;
    SC_Pin * pPin;
    int n_valid_cells;
    int i, j, k;

//    fprintf( s, "%d", ABC_SCL_CUR_VERSION );
    fprintf( s, "library(%s) {\n\n",                         p->lib_name );
    if ( p->default_wire_load && strlen(p->default_wire_load) )
    fprintf( s, "  default_wire_load : \"%s\";\n",           p->default_wire_load );
    if ( p->default_wire_load_sel && strlen(p->default_wire_load_sel) )
    fprintf( s, "  default_wire_load_selection : \"%s\";\n", p->default_wire_load_sel );
    if ( p->default_max_out_slew != -1 )
    fprintf( s, "  default_max_transition : %f;\n",          p->default_max_out_slew );
    if ( p->unit_time == 9 )
    fprintf( s, "  time_unit : \"1ns\";\n" );
    else if ( p->unit_time == 10 )
    fprintf( s, "  time_unit : \"100ps\";\n" );
    else if ( p->unit_time == 11 )
    fprintf( s, "  time_unit : \"10ps\";\n" );
    else if ( p->unit_time == 12 )
    fprintf( s, "  time_unit : \"1ps\";\n" );
    else assert( 0 );
    fprintf( s, "  capacitive_load_unit(%.1f,%s);\n",        p->unit_cap_fst, p->unit_cap_snd == 12 ? "pf" : "ff" );
    fprintf( s, "\n" );

    // Write 'wire_load' vector:
    Vec_PtrForEachEntry( SC_WireLoad *, p->vWireLoads, pWL, i )
    {
        fprintf( s, "  wire_load(\"%s\") {\n", pWL->name );
        fprintf( s, "    capacitance : %f;\n", pWL->cap );
        fprintf( s, "    resistance : %f;\n", pWL->res );
        for ( j = 0; j < Vec_IntSize(pWL->vFanout); j++ )
            fprintf( s, "    fanout_length( %d, %f );\n", Vec_IntEntry(pWL->vFanout, j), Vec_FltEntry(pWL->vLen, j) );
        fprintf( s, "  }\n\n" );
    }

    // Write 'wire_load_sel' vector:
    Vec_PtrForEachEntry( SC_WireLoadSel *, p->vWireLoadSels, pWLS, i )
    {
        fprintf( s, "  wire_load_selection(\"%s\") {\n", pWLS->name );
        for ( j = 0; j < Vec_FltSize(pWLS->vAreaFrom); j++)
            fprintf( s, "    wire_load_from_area( %f, %f, \"%s\" );\n", 
                Vec_FltEntry(pWLS->vAreaFrom, j), 
                Vec_FltEntry(pWLS->vAreaTo, j), 
                (char *)Vec_PtrEntry(pWLS->vWireLoadModel, j) );
        fprintf( s, "  }\n\n" );
    }

    // Write 'cells' vector:
    n_valid_cells = 0;
    SC_LitForEachCell( p, pCell, i )
        if ( !(pCell->seq || pCell->unsupp) )
            n_valid_cells++;

    SC_LitForEachCell( p, pCell, i )
    {
        if ( pCell->seq || pCell->unsupp )
            continue;

        fprintf( s, "\n" );
        fprintf( s, "  cell(%s) {\n", pCell->name );
        fprintf( s, "    /*  n_inputs = %d  n_outputs = %d */\n", pCell->n_inputs, pCell->n_outputs );
        fprintf( s, "    area : %f;\n", pCell->area );
        fprintf( s, "    drive_strength : %d;\n", pCell->drive_strength );

        Vec_PtrForEachEntryStop( SC_Pin *, pCell->vPins, pPin, j, pCell->n_inputs )
        {
            assert(pPin->dir == sc_dir_Input);
            fprintf( s, "    pin(%s) {\n", pPin->name );
            fprintf( s, "      direction : %s;\n", "input" );
            fprintf( s, "      fall_capacitance : %f;\n", pPin->fall_cap );
            fprintf( s, "      rise_capacitance : %f;\n", pPin->rise_cap );
            fprintf( s, "    }\n" );
        }

        Vec_PtrForEachEntryStart( SC_Pin *, pCell->vPins, pPin, j, pCell->n_inputs )
        {
            SC_Timings * pRTime;
//            word uWord;
            assert(pPin->dir == sc_dir_Output);
            fprintf( s, "    pin(%s) {\n", pPin->name );
            fprintf( s, "      direction : %s;\n", "output" );
            fprintf( s, "      max_capacitance : %f;\n", pPin->max_out_cap );
            fprintf( s, "      max_transition : %f;\n", pPin->max_out_slew );
            fprintf( s, "      function : \"%s\";\n", pPin->func_text ? pPin->func_text : "?" );
            fprintf( s, "      /*  truth table = " );
            Extra_PrintHex( s, (unsigned *)Vec_WrdArray(pPin->vFunc), pCell->n_inputs );
            fprintf( s, "  */\n" );

            // Write 'rtiming': (pin-to-pin timing tables for this particular output)
            assert( Vec_PtrSize(pPin->vRTimings) == pCell->n_inputs );
            Vec_PtrForEachEntry( SC_Timings *, pPin->vRTimings, pRTime, k )
            {
                if ( Vec_PtrSize(pRTime->vTimings) == 1 )
                {
                    SC_Timing * pTime = (SC_Timing *)Vec_PtrEntry( pRTime->vTimings, 0 );
                    fprintf( s, "      timing() {\n" );
                    fprintf( s, "        related_pin : \"%s\"\n", pRTime->name );
                    if ( pTime->tsense == sc_ts_Pos )
                        fprintf( s, "        timing_sense : positive_unate;\n" );
                    else if ( pTime->tsense == sc_ts_Neg )
                        fprintf( s, "        timing_sense : negative_unate;\n" );
                    else if ( pTime->tsense == sc_ts_Non )
                        fprintf( s, "        timing_sense : non_unate;\n" );
                    else assert( 0 );

                    fprintf( s, "        cell_rise() {\n" );
                    Abc_SclWriteSurfaceText( s, pTime->pCellRise );
                    fprintf( s, "        }\n" );

                    fprintf( s, "        cell_fall() {\n" );
                    Abc_SclWriteSurfaceText( s, pTime->pCellFall );
                    fprintf( s, "        }\n" );

                    fprintf( s, "        rise_transition() {\n" );
                    Abc_SclWriteSurfaceText( s, pTime->pRiseTrans );
                    fprintf( s, "        }\n" );

                    fprintf( s, "        fall_transition() {\n" );
                    Abc_SclWriteSurfaceText( s, pTime->pFallTrans );
                    fprintf( s, "        }\n" );
                    fprintf( s, "      }\n" );
                }
                else
                    assert( Vec_PtrSize(pRTime->vTimings) == 0 );
            }
            fprintf( s, "    }\n" );
        }
        fprintf( s, "  }\n" );
    }
    fprintf( s, "}\n\n" );
}
void Abc_SclWriteText( char * pFileName, SC_Lib * p )
{
    FILE * pFile = fopen( pFileName, "wb" );
    if ( pFile == NULL )
        printf( "Cannot open text file \"%s\" for writing.\n", pFileName );
    else
    {
        Abc_SclWriteLibraryText( pFile, p );
        fclose( pFile );
    }
}


////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

