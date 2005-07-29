/**CFile****************************************************************

  FileName    [io.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Command processing package.]

  Synopsis    [Command file.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: io.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "io.h"
#include "mainInt.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

static int IoCommandRead        ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadBlif    ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadBench   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadVerilog ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandReadPla     ( Abc_Frame_t * pAbc, int argc, char **argv );

static int IoCommandWriteBlif   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteGate   ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteBench  ( Abc_Frame_t * pAbc, int argc, char **argv );
static int IoCommandWriteCnf    ( Abc_Frame_t * pAbc, int argc, char **argv );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFITIONS                           ///
////////////////////////////////////////////////////////////////////////

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Io_Init( Abc_Frame_t * pAbc )
{
    Cmd_CommandAdd( pAbc, "I/O", "read",          IoCommandRead,         1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_blif",     IoCommandReadBlif,     1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_bench",    IoCommandReadBench,    1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_verilog",  IoCommandReadVerilog,  1 );
    Cmd_CommandAdd( pAbc, "I/O", "read_pla",      IoCommandReadPla,      1 );

    Cmd_CommandAdd( pAbc, "I/O", "write_blif",    IoCommandWriteBlif,    0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_gate",    IoCommandWriteGate,    0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_bench",   IoCommandWriteBench,   0 );
    Cmd_CommandAdd( pAbc, "I/O", "write_cnf",     IoCommandWriteCnf,     0 );
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Io_End()
{
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandRead( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk;
    char * FileName;
    FILE * pFile;
    int fCheck;
    int c;

    fCheck = 1;
    util_getopt_reset();
    while ( ( c = util_getopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( argc != util_optind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[util_optind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( FileName = Extra_FileGetSimilarName( FileName, ".mv", ".blif", ".pla", ".mvpla", NULL ) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );
 
    // set the new network
    pNtk = Io_Read( FileName, fCheck );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Reading network from file has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         read the network from file in Verilog/BLIF/BENCH format\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadBlif( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk, * pTemp;
    char * FileName;
    FILE * pFile;
    int fCheck;
    int c;

    fCheck = 1;
    util_getopt_reset();
    while ( ( c = util_getopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( argc != util_optind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[util_optind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( FileName = Extra_FileGetSimilarName( FileName, ".mv", ".blif", ".pla", ".mvpla", NULL ) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );
 
    // set the new network
    pNtk = Io_ReadBlif( FileName, fCheck );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Reading network from BLIF file has failed.\n" );
        return 1;
    }

    pNtk = Abc_NtkLogic( pTemp = pNtk );
    Abc_NtkDelete( pTemp );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Converting to logic network after reading has failed.\n" );
        return 1;
    }

    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_blif [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         read the network in binary BLIF format\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadBench( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk, * pTemp;
    char * FileName;
    FILE * pFile;
    int fCheck;
    int c;

    fCheck = 1;
    util_getopt_reset();
    while ( ( c = util_getopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( argc != util_optind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[util_optind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( FileName = Extra_FileGetSimilarName( FileName, ".mv", ".blif", ".pla", ".mvpla", NULL ) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );

    // set the new network
    pNtk = Io_ReadBench( FileName, fCheck );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Reading network from BENCH file has failed.\n" );
        return 1;
    }

    pNtk = Abc_NtkLogic( pTemp = pNtk );
    Abc_NtkDelete( pTemp );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Converting to logic network after reading has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_bench [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         read the network in BENCH format\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadVerilog( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk, * pTemp;
    char * FileName;
    FILE * pFile;
    int fCheck;
    int c;

    fCheck = 1;
    util_getopt_reset();
    while ( ( c = util_getopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( argc != util_optind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[util_optind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( FileName = Extra_FileGetSimilarName( FileName, ".mv", ".blif", ".pla", ".mvpla", NULL ) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );

    // set the new network
    pNtk = Io_ReadVerilog( FileName, fCheck );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Reading network from Verilog file has failed.\n" );
        return 1;
    }

    pNtk = Abc_NtkLogic( pTemp = pNtk );
    Abc_NtkDelete( pTemp );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Converting to logic network after reading has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_verilog [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         read the network in Verilog (IWLS 2005 subset)\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandReadPla( Abc_Frame_t * pAbc, int argc, char ** argv )
{
    Abc_Ntk_t * pNtk, * pTemp;
    char * FileName;
    FILE * pFile;
    int fCheck;
    int c;

    fCheck = 1;
    util_getopt_reset();
    while ( ( c = util_getopt( argc, argv, "ch" ) ) != EOF )
    {
        switch ( c )
        {
            case 'c':
                fCheck ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( argc != util_optind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[util_optind];
    if ( (pFile = fopen( FileName, "r" )) == NULL )
    {
        fprintf( pAbc->Err, "Cannot open input file \"%s\". ", FileName );
        if ( FileName = Extra_FileGetSimilarName( FileName, ".mv", ".blif", ".pla", ".mvpla", NULL ) )
            fprintf( pAbc->Err, "Did you mean \"%s\"?", FileName );
        fprintf( pAbc->Err, "\n" );
        return 1;
    }
    fclose( pFile );

    // set the new network
//    pNtk = Io_ReadPla( FileName, fCheck );
    fprintf( pAbc->Err, "This command is currently not implemented.\n" );
    pNtk = NULL;
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Reading network from PLA file has failed.\n" );
        return 1;
    }

    pNtk = Abc_NtkLogic( pTemp = pNtk );
    Abc_NtkDelete( pTemp );
    if ( pNtk == NULL )
    {
        fprintf( pAbc->Err, "Converting to logic network after reading has failed.\n" );
        return 1;
    }
    // replace the current network
    Abc_FrameReplaceCurrentNetwork( pAbc, pNtk );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: read_pla [-ch] <file>\n" );
    fprintf( pAbc->Err, "\t         read the network in PLA\n" );
    fprintf( pAbc->Err, "\t-c     : toggle network check after reading [default = %s]\n", fCheck? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : prints the command summary\n" );
    fprintf( pAbc->Err, "\tfile   : the name of a file to read\n" );
    return 1;
}


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteBlif( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * FileName;
    int fMadeComb;
    int fWriteLatches;
    int c;

    fWriteLatches = 1;
    util_getopt_reset();
    while ( ( c = util_getopt( argc, argv, "lh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'l':
                fWriteLatches ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }

    if ( argc != util_optind + 1 )
    {
        goto usage;
    }

    if ( Abc_NtkIsLogicMap(pAbc->pNtkCur) )
    {
        fprintf( pAbc->Out, "Use \"write_gate\" or unmap the network (\"unmap\").\n" );
        return 1;
    }

    // get the input file name
    FileName = argv[util_optind];
    // write the file
    if ( Abc_NtkIsNetlist(pAbc->pNtkCur) )
    {
        if ( !fWriteLatches )
            fMadeComb = Abc_NtkMakeComb( pAbc->pNtkCur );
        Io_WriteBlif( pAbc->pNtkCur, FileName );
        if ( !fWriteLatches && fMadeComb )
            Abc_NtkMakeSeq( pAbc->pNtkCur );
    }
    else if ( Abc_NtkIsLogicSop(pAbc->pNtkCur) || Abc_NtkIsAig(pAbc->pNtkCur) )
    {
        Io_WriteBlifLogic( pAbc->pNtkCur, FileName, fWriteLatches );
    }
    else if ( Abc_NtkIsLogicBdd(pAbc->pNtkCur) )
    {
//        printf( "Converting node functions from BDD to SOP.\n" );
        Abc_NtkBddToSop(pAbc->pNtkCur);
        Io_WriteBlifLogic( pAbc->pNtkCur, FileName, fWriteLatches );
    }
    else
    {
        assert( 0 );
    }
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_blif [-lh] <file>\n" );
    fprintf( pAbc->Err, "\t         write the network into a BLIF file\n" );
    fprintf( pAbc->Err, "\t-l     : toggle writing latches [default = %s]\n", fWriteLatches? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteGate( Abc_Frame_t * pAbc, int argc, char **argv )
{
    Abc_Ntk_t * pNtk;
    char * FileName;
    int fWriteLatches;
    int c;

    pNtk = pAbc->pNtkCur;
    fWriteLatches = 1;
    util_getopt_reset();
    while ( ( c = util_getopt( argc, argv, "lh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'l':
                fWriteLatches ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }

    if ( !Abc_NtkIsLogicMap(pNtk) )
    {
        fprintf( pAbc->Out, "The network is not mapped.\n" );
        return 0;
    }
/*
    if ( Abc_NtkLatchNum(pNtk) > 0 )
    {
        fprintf( pAbc->Out, "The network has latches.\n" );
        return 0;
    }
*/
    if ( argc != util_optind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[util_optind];
    // write the file
    Io_WriteGate( pNtk, FileName );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_gate [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         write the network into a mapped BLIF file (.gate ...)\n" );
//    fprintf( pAbc->Err, "\t-l     : toggle writing latches [default = %s]\n", fWriteLatches? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteBench( Abc_Frame_t * pAbc, int argc, char **argv )
{
    Abc_Ntk_t * pNtk;
    char * FileName;
    int fWriteLatches;
    int c;

    pNtk = pAbc->pNtkCur;
    fWriteLatches = 1;
    util_getopt_reset();
    while ( ( c = util_getopt( argc, argv, "lh" ) ) != EOF )
    {
        switch ( c )
        {
            case 'l':
                fWriteLatches ^= 1;
                break;
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( pNtk == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }

    if ( !Abc_NtkIsAig(pNtk) )
    {
        fprintf( pAbc->Out, "The network should be an AIG.\n" );
        return 0;
    }

    if ( argc != util_optind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[util_optind];
    // write the file
    Io_WriteBench( pNtk, FileName );
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_bench [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         write the network in BENCH format\n" );
//    fprintf( pAbc->Err, "\t-l     : toggle writing latches [default = %s]\n", fWriteLatches? "yes":"no" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int IoCommandWriteCnf( Abc_Frame_t * pAbc, int argc, char **argv )
{
    char * FileName;
    int c;

    util_getopt_reset();
    while ( ( c = util_getopt( argc, argv, "h" ) ) != EOF )
    {
        switch ( c )
        {
            case 'h':
                goto usage;
            default:
                goto usage;
        }
    }

    if ( pAbc->pNtkCur == NULL )
    {
        fprintf( pAbc->Out, "Empty network.\n" );
        return 0;
    }

    if ( argc != util_optind + 1 )
    {
        goto usage;
    }

    // get the input file name
    FileName = argv[util_optind];
    // write the file
    if ( !Io_WriteCnf( pAbc->pNtkCur, FileName ) )
    {
        printf( "Writing CNF has failed.\n" );
        return 1;
    }
    return 0;

usage:
    fprintf( pAbc->Err, "usage: write_cnf [-h] <file>\n" );
    fprintf( pAbc->Err, "\t         write the miter cone into a CNF file\n" );
    fprintf( pAbc->Err, "\t-h     : print the help massage\n" );
    fprintf( pAbc->Err, "\tfile   : the name of the file to write\n" );
    return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


