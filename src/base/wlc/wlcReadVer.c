/**CFile****************************************************************

  FileName    [wlcReadVer.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Verilog parser.]

  Synopsis    [Parses several flavors of word-level Verilog.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - August 22, 2014.]

  Revision    [$Id: wlcReadVer.c,v 1.00 2014/09/12 00:00:00 alanmi Exp $]

***********************************************************************/

#include "wlc.h"

ABC_NAMESPACE_IMPL_START


////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

// Word-level Verilog file parser
#define WLV_PRS_MAX_LINE   1000

typedef struct Wlc_Prs_t_  Wlc_Prs_t;
struct Wlc_Prs_t_ 
{
    int                    nFileSize;
    char *                 pFileName;
    char *                 pBuffer;
    Vec_Int_t *            vLines;
    Vec_Int_t *            vStarts;
    Vec_Int_t *            vFanins;
    Wlc_Ntk_t *            pNtk;
    Mem_Flex_t *           pMemTable;
    Vec_Ptr_t *            vTables;
    int                    nConsts;
    char                   sError[WLV_PRS_MAX_LINE];
};

static inline int          Wlc_PrsOffset( Wlc_Prs_t * p, char * pStr )  { return pStr - p->pBuffer;                     }
static inline char *       Wlc_PrsStr( Wlc_Prs_t * p, int iOffset )     { return p->pBuffer + iOffset;                  }
static inline int          Wlc_PrsStrCmp( char * pStr, char * pWhat )   { return !strncmp( pStr, pWhat, strlen(pWhat)); }

#define Wlc_PrsForEachLine( p, pLine, i )             \
    for ( i = 0; (i < Vec_IntSize((p)->vStarts)) && ((pLine) = Wlc_PrsStr(p, Vec_IntEntry((p)->vStarts, i))); i++ )
#define Wlc_PrsForEachLineStart( p, pLine, i, Start ) \
    for ( i = Start; (i < Vec_IntSize((p)->vStarts)) && ((pLine) = Wlc_PrsStr(p, Vec_IntEntry((p)->vStarts, i))); i++ )


////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////


/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
Wlc_Prs_t * Wlc_PrsStart( char * pFileName )
{
    Wlc_Prs_t * p;
    if ( !Extra_FileCheck( pFileName ) )
        return NULL;
    p = ABC_CALLOC( Wlc_Prs_t, 1 );
    p->pFileName = pFileName;
    p->pBuffer   = Extra_FileReadContents( pFileName );
    p->nFileSize = strlen(p->pBuffer);  assert( p->nFileSize > 0 );
    p->vLines    = Vec_IntAlloc( p->nFileSize / 50 );
    p->vStarts   = Vec_IntAlloc( p->nFileSize / 50 );
    p->vFanins   = Vec_IntAlloc( 100 );
    p->vTables   = Vec_PtrAlloc( 1000 );
    p->pMemTable = Mem_FlexStart();
    return p;
}
void Wlc_PrsStop( Wlc_Prs_t * p )
{
    if ( p->pNtk )
        Wlc_NtkFree( p->pNtk );
    if ( p->pMemTable )
        Mem_FlexStop( p->pMemTable, 0 );
    Vec_PtrFreeP( &p->vTables );
    Vec_IntFree( p->vLines );
    Vec_IntFree( p->vStarts );
    Vec_IntFree( p->vFanins );
    ABC_FREE( p->pBuffer );
    ABC_FREE( p );
}

/**Function*************************************************************

  Synopsis    [Prints the error message including the file name and line number.]

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
int Wlc_PrsWriteErrorMessage( Wlc_Prs_t * p, char * pCur, const char * format, ... )
{
    char * pMessage;
    // derive message
    va_list args;
    va_start( args, format );
    pMessage = vnsprintf( format, args );
    va_end( args );
    // print messsage
    assert( strlen(pMessage) < WLV_PRS_MAX_LINE - 100 );
    assert( p->sError[0] == 0 );
    if ( pCur == NULL ) // the line number is not given
        sprintf( p->sError, "%s: %s\n", p->pFileName, pMessage );
    else                // print the error message with the line number
    {
        int Entry, iLine = 0;
        Vec_IntForEachEntry( p->vLines, Entry, iLine )
            if ( Entry > pCur - p->pBuffer )
                break;
        sprintf( p->sError, "%s (line %d): %s\n", p->pFileName, iLine+1, pMessage );
    }
    free( pMessage );
    return 0;
}
void Wlc_PrsPrintErrorMessage( Wlc_Prs_t * p )
{
    if ( p->sError[0] == 0 )
        return;
    fprintf( stdout, "%s", p->sError );
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
static inline int Wlc_PrsIsDigit( char * pStr )
{
    return (pStr[0] >= '0' && pStr[0] <= '9');
}
static inline int Wlc_PrsIsChar( char * pStr )
{
    return (pStr[0] >= 'a' && pStr[0] <= 'z') || 
           (pStr[0] >= 'A' && pStr[0] <= 'Z') || 
           (pStr[0] >= '0' && pStr[0] <= '9') || 
            pStr[0] == '_' || pStr[0] == '$';
}
static inline char * Wlc_PrsSkipSpaces( char * pStr )
{
    while ( *pStr && *pStr == ' ' )
        pStr++;
    return pStr;
}
static inline char * Wlc_PrsFindSymbol( char * pStr, char Symb )
{
    for ( ; *pStr; pStr++ )
        if ( *pStr == Symb )
            return pStr;
    return NULL;
}
static inline char * Wlc_PrsFindSymbolTwo( char * pStr, char Symb, char Symb2 )
{
    for ( ; pStr[1]; pStr++ )
        if ( pStr[0] == Symb  && pStr[1] == Symb2 )
            return pStr;
    return NULL;
}
static inline char * Wlc_PrsFindClosingParanthesis( char * pStr, char Open, char Close )
{
    int Counter = 0;
    int fNotName = 1;
    assert( *pStr == Open );
    for ( ; *pStr; pStr++ )
    {
        if ( fNotName )
        {
            if ( *pStr == Open )
                Counter++;
            if ( *pStr == Close )
                Counter--;
            if ( Counter == 0 )
                return pStr;
        }
        if ( *pStr == '\\' )
            fNotName = 0;
        else if ( !fNotName && *pStr == ' ' )
            fNotName = 1;
    }
    return NULL;
}
int Wlc_PrsRemoveComments( Wlc_Prs_t * p )
{
    int fSpecifyFound = 0;
    char * pCur, * pNext, * pEnd = p->pBuffer + p->nFileSize;
    for ( pCur = p->pBuffer; pCur < pEnd; pCur++ )
    {
        // regular comment (//)
        if ( *pCur == '/' && pCur[1] == '/' )
        {
            if ( pCur + 5 < pEnd && pCur[2] == 'a' && pCur[3] == 'b' && pCur[4] == 'c' && pCur[5] == '2' )
                pCur[0] = pCur[1] = pCur[2] = pCur[3] = pCur[4] = pCur[5] = ' ';
            else
            {
                pNext = Wlc_PrsFindSymbol( pCur, '\n' );
                if ( pNext == NULL )
                    return Wlc_PrsWriteErrorMessage( p, pCur, "Cannot find end-of-line after symbols \"//\"." );
                for ( ; pCur < pNext; pCur++ )
                    *pCur = ' ';
            }
        }
        // skip preprocessor directive (`timescale, `celldefine, etc)
        else if ( *pCur == '`' )
        {
            pNext = Wlc_PrsFindSymbol( pCur, '\n' );
            if ( pNext == NULL )
                return Wlc_PrsWriteErrorMessage( p, pCur, "Cannot find end-of-line after symbols \"`\"." );
            for ( ; pCur < pNext; pCur++ )
                *pCur = ' ';
        }
        // regular comment (/* ... */)
        else if ( *pCur == '/' && pCur[1] == '*' )
        {
            pNext = Wlc_PrsFindSymbolTwo( pCur, '*', '/' );
            if ( pNext == NULL )
                return Wlc_PrsWriteErrorMessage( p, pCur, "Cannot find symbols \"*/\" after symbols \"/*\"." );
            // overwrite comment
            for ( ; pCur < pNext + 2; pCur++ )
                *pCur = ' ';
        }
        // 'specify' treated as comments
        else if ( *pCur == 's' && pCur[1] == 'p' && pCur[2] == 'e' && !strncmp(pCur, "specify", 7) )
        {
            for ( pNext = pCur; pNext < pEnd - 10; pNext++ )
                if ( *pNext == 'e' && pNext[1] == 'n' && pNext[2] == 'd' && !strncmp(pNext, "endspecify", 10) )
                {
                    // overwrite comment
                    for ( ; pCur < pNext + 10; pCur++ )
                        *pCur = ' ';
                    if ( fSpecifyFound == 0 )
                        Abc_Print( 0, "Ignoring specify/endspecify directives.\n" );
                    fSpecifyFound = 1;
                    break;
                }
        }
        // insert semicolons
        else if ( *pCur == 'e' && pCur[1] == 'n' && pCur[2] == 'd' && !strncmp(pCur, "endmodule", 9) )
            pCur[strlen("endmodule")] = ';';
        // overwrite end-of-lines with spaces (less checking to do later on)
        if ( *pCur == '\n' || *pCur == '\r'  || *pCur == '\t' )
            *pCur = ' ';
    }
    return 1;
}
int Wlc_PrsPrepare( Wlc_Prs_t * p )
{
    int fPrettyPrint = 0;
    int fNotName = 1;
    char * pTemp, * pPrev, * pThis;
    // collect info about lines
    assert( Vec_IntSize(p->vLines) == 0 );
    for ( pTemp = p->pBuffer; *pTemp; pTemp++ )
        if ( *pTemp == '\n' )
            Vec_IntPush( p->vLines, pTemp - p->pBuffer );
    // delete comments and insert breaks
    if ( !Wlc_PrsRemoveComments( p ) )
        return 0;
    // collect info about breaks
    assert( Vec_IntSize(p->vStarts) == 0 );
    for ( pPrev = pThis = p->pBuffer; *pThis; pThis++ )
    {
        if ( fNotName && *pThis == ';' )
        {
            *pThis = 0;
            Vec_IntPush( p->vStarts, Wlc_PrsOffset(p, Wlc_PrsSkipSpaces(pPrev)) );
            pPrev = pThis + 1;
        }
        if ( *pThis == '\\' )
            fNotName = 0;
        else if ( !fNotName && *pThis == ' ' )
            fNotName = 1;
    }

    if ( fPrettyPrint )
    {
        int i, k;
        // print the line types
        Wlc_PrsForEachLine( p, pTemp, i )
        {
            if ( Wlc_PrsStrCmp( pTemp, "module" ) )
                printf( "\n" );
            if ( !Wlc_PrsStrCmp( pTemp, "module" ) && !Wlc_PrsStrCmp( pTemp, "endmodule" ) )
                printf( "    " );
            printf( "%c", pTemp[0] );
            for ( k = 1; pTemp[k]; k++ )
                if ( pTemp[k] != ' ' || pTemp[k-1] != ' ' )
                    printf( "%c", pTemp[k] );
            printf( ";\n" );
        }
/*    
        // print the line types
        Wlc_PrsForEachLine( p, pTemp, i )
        {
            int k;  
            if ( !Wlc_PrsStrCmp( pTemp, "module" ) )
                continue;
            printf( "%3d : ", i );
            for ( k = 0; k < 40; k++ )
                printf( "%c", pTemp[k] ? pTemp[k] : ' ' );
            printf( "\n" );
        }
*/
    }
    return 1;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
static inline char * Wlc_PrsFindRange( char * pStr, int * End, int * Beg )
{
    *End = *Beg = 0;
    pStr = Wlc_PrsSkipSpaces( pStr );
    if ( pStr[0] != '[' )
        return pStr;
    pStr = Wlc_PrsSkipSpaces( pStr+1 );
    if ( !Wlc_PrsIsDigit(pStr) )
        return NULL;
    *End = *Beg = atoi( pStr );
    if ( Wlc_PrsFindSymbol( pStr, ':' ) == NULL )
    {
        pStr = Wlc_PrsFindSymbol( pStr, ']' );
        if ( pStr == NULL )
            return NULL;
    }
    else
    {
        pStr = Wlc_PrsFindSymbol( pStr, ':' );
        pStr = Wlc_PrsSkipSpaces( pStr+1 );
        if ( !Wlc_PrsIsDigit(pStr) )
            return NULL;
        *Beg = atoi( pStr );
        pStr = Wlc_PrsFindSymbol( pStr, ']' );
        if ( pStr == NULL )
            return NULL;
    }
    assert( *End >= *Beg );
    return pStr + 1;
}
static inline char * Wlc_PrsFindWord( char * pStr, char * pWord, int * fFound )
{
    *fFound = 0;
    pStr = Wlc_PrsSkipSpaces( pStr );
    if ( !Wlc_PrsStrCmp(pStr, pWord) )
        return pStr;
    *fFound = 1;
    return pStr + strlen(pWord);
}
static inline char * Wlc_PrsFindName( char * pStr, char ** ppPlace )
{
    static char Buffer[WLV_PRS_MAX_LINE];
    char * pThis = *ppPlace = Buffer;
    pStr = Wlc_PrsSkipSpaces( pStr );
    if ( !Wlc_PrsIsChar(pStr) )
        return NULL;
    while ( Wlc_PrsIsChar(pStr) )
        *pThis++ = *pStr++;
    *pThis = 0;
    return pStr;
}
static inline char * Wlc_PrsReadConstant( Wlc_Prs_t * p, char * pStr, Vec_Int_t * vFanins, int * pRange, int * pSigned )
{
    int nDigits, nBits = atoi( pStr );
    *pRange = -1;
    *pSigned = 0;
    pStr = Wlc_PrsSkipSpaces( pStr );
    if ( Wlc_PrsFindSymbol( pStr, '\'' ) == NULL )
    {
        // handle decimal number
        int Number = atoi( pStr );
        *pRange = Abc_Base2Log( Number );
        while ( Wlc_PrsIsDigit(pStr) )
            pStr++;
        Vec_IntFill( vFanins, 1, Number );
        return pStr;
    }
    pStr = Wlc_PrsFindSymbol( pStr, '\'' );
    if ( pStr[1] == 's' )
    {
        *pSigned = 1;
        pStr++;
    }
    if ( pStr[1] != 'h' )
        return (char *)(ABC_PTRINT_T)Wlc_PrsWriteErrorMessage( p, pStr, "Expecting hexadecimal constant and not \"%c\".", pStr[1] );
    Vec_IntFill( vFanins, Abc_BitWordNum(nBits), 0 );
    nDigits = Abc_TtReadHexNumber( (word *)Vec_IntArray(vFanins), pStr+2 );
    if ( nDigits != (nBits + 3)/4 )
    {
//        return (char *)(ABC_PTRINT_T)Wlc_PrsWriteErrorMessage( p, pStr, "The length of a constant does not match." );
//        printf( "Warning: The length of a constant (%d hex digits) does not match the number of bits (%d).\n", nDigits, nBits );
    }
    *pRange = nBits;
    pStr += 2;
    while ( Wlc_PrsIsChar(pStr) )
        pStr++;
    return pStr;
}
static inline char * Wlc_PrsReadName( Wlc_Prs_t * p, char * pStr, Vec_Int_t * vFanins )
{
    int NameId, fFound, iObj;
    pStr = Wlc_PrsSkipSpaces( pStr );
    if ( Wlc_PrsIsDigit(pStr) )
    {
        char Buffer[100];
        int Range, Signed;
        Vec_Int_t * vFanins = Vec_IntAlloc(0);
        pStr = Wlc_PrsReadConstant( p, pStr, vFanins, &Range, &Signed );
        if ( pStr == NULL )
        {
            Vec_IntFree( vFanins );
            return 0;
        }
        // create new node
        iObj = Wlc_ObjAlloc( p->pNtk, WLC_OBJ_CONST, Signed, Range-1, 0 );
        Wlc_ObjAddFanins( p->pNtk, Wlc_NtkObj(p->pNtk, iObj), vFanins );
        Vec_IntFree( vFanins );
        // add node's name
        sprintf( Buffer, "_c%d_", p->nConsts++ );
        NameId = Abc_NamStrFindOrAdd( p->pNtk->pManName, Buffer, &fFound );
        if ( fFound )
            return (char *)(ABC_PTRINT_T)Wlc_PrsWriteErrorMessage( p, pStr, "Name %s is already used.", Buffer );
        assert( iObj == NameId );
    }
    else
    {
        char * pName;
        pStr = Wlc_PrsFindName( pStr, &pName );
        if ( pStr == NULL )
            return (char *)(ABC_PTRINT_T)Wlc_PrsWriteErrorMessage( p, pStr, "Cannot read name." );
        NameId = Abc_NamStrFindOrAdd( p->pNtk->pManName, pName, &fFound );
        if ( !fFound )
            return (char *)(ABC_PTRINT_T)Wlc_PrsWriteErrorMessage( p, pStr, "Name %s is used but not declared.", pName );
    }
    Vec_IntPush( vFanins, NameId );
    return Wlc_PrsSkipSpaces( pStr );
}
static inline int Wlc_PrsFindDefinition( Wlc_Prs_t * p, char * pStr, Vec_Int_t * vFanins )
{
    char * pName;
    int Type = WLC_OBJ_NONE;
    int fRotating = 0;
    Vec_IntClear( vFanins );
    pStr = Wlc_PrsSkipSpaces( pStr );
    if ( pStr[0] != '=' )
        return 0;
    pStr = Wlc_PrsSkipSpaces( pStr+1 );
    if ( pStr[0] == '(' )
    {
        // consider rotating shifter
        if ( Wlc_PrsFindSymbolTwo(pStr, '>', '>') && Wlc_PrsFindSymbolTwo(pStr, '<', '<') )
        {
            // THIS IS A HACK TO DETECT rotating shifters
            char * pClose = Wlc_PrsFindClosingParanthesis( pStr, '(', ')' );
            if ( pClose == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStr, "Expecting closing paranthesis." );
            *pStr = ' '; *pClose = 0;
            pStr = Wlc_PrsSkipSpaces( pStr );
            fRotating = 1;
        }
        else
        {
            char * pClose = Wlc_PrsFindClosingParanthesis( pStr, '(', ')' );
            if ( pClose == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStr, "Expecting closing paranthesis." );
            *pStr = *pClose = ' ';
            pStr = Wlc_PrsSkipSpaces( pStr );
        }
    }
    if ( Wlc_PrsIsDigit(pStr) )
    {
        int Range, Signed;
        pStr = Wlc_PrsReadConstant( p, pStr, vFanins, &Range, &Signed );
        if ( pStr == NULL )
            return 0;
        Type = WLC_OBJ_CONST;
    }
    else if ( pStr[0] == '!' || pStr[0] == '~' )
    {
        if ( pStr[0] == '!' )
            Type = WLC_OBJ_LOGIC_NOT;
        else if ( pStr[0] == '~' )
            Type = WLC_OBJ_BIT_NOT;
        else assert( 0 );
        // skip parantheses
        pStr = Wlc_PrsSkipSpaces( pStr+1 );
        if ( pStr[0] == '(' )
        {
            char * pClose = Wlc_PrsFindClosingParanthesis( pStr, '(', ')' );
            if ( pClose == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStr, "Expecting closing paranthesis." );
            *pStr = *pClose = ' ';
        }
        if ( !(pStr = Wlc_PrsReadName(p, pStr, vFanins)) )
            return Wlc_PrsWriteErrorMessage( p, pStr, "Cannot read name after !." );
    }
    else if ( pStr[0] == '&' || pStr[0] == '|' || pStr[0] == '^' || pStr[0] == '-' )
    {
        if ( pStr[0] == '&' )
            Type = WLC_OBJ_REDUCT_AND;
        else if ( pStr[0] == '|' )
            Type = WLC_OBJ_REDUCT_OR;
        else if ( pStr[0] == '^' )
            Type = WLC_OBJ_REDUCT_XOR;
        else if ( pStr[0] == '-' )
            Type = WLC_OBJ_ARI_MINUS;
        else assert( 0 );
        if ( !(pStr = Wlc_PrsReadName(p, pStr+1, vFanins)) )
            return Wlc_PrsWriteErrorMessage( p, pStr, "Cannot read name after a unary operator." );
    }
    else if ( pStr[0] == '{' )
    {
        // THIS IS A HACK TO DETECT zero padding AND sign extension
        if ( Wlc_PrsFindSymbol(pStr+1, '{') )
        {
            if ( Wlc_PrsFindSymbol(pStr+1, '\'') )
                Type = WLC_OBJ_BIT_ZEROPAD;
            else
                Type = WLC_OBJ_BIT_SIGNEXT;
            pStr = Wlc_PrsFindSymbol(pStr+1, ',');
            if ( pStr == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStr, "Expecting one comma in this line." );
            if ( !(pStr = Wlc_PrsReadName(p, pStr+1, vFanins)) )
                return Wlc_PrsWriteErrorMessage( p, pStr, "Cannot read name in sign-extension." );
            pStr = Wlc_PrsSkipSpaces( pStr );
            if ( pStr[0] != '}' )
                return Wlc_PrsWriteErrorMessage( p, pStr, "There is no closing brace (})." );
        }
        else // concatenation
        {
            while ( 1 )
            {
                pStr = Wlc_PrsSkipSpaces( pStr+1 );
                if ( !(pStr = Wlc_PrsReadName(p, pStr, vFanins)) )
                    return Wlc_PrsWriteErrorMessage( p, pStr, "Cannot read name in concatenation." );
                if ( pStr[0] == '}' )
                    break;
                if ( pStr[0] != ',' )
                    return Wlc_PrsWriteErrorMessage( p, pStr, "Expected comma (,) in this place." );
            }
            Type = WLC_OBJ_BIT_CONCAT;
        }
        assert( pStr[0] == '}' );
        pStr++;
    }
    else
    {
        if ( !(pStr = Wlc_PrsReadName(p, pStr, vFanins)) )
            return 0;
        // get the next symbol
        if ( pStr[0] == 0 )
            Type = WLC_OBJ_BUF;
        else if ( pStr[0] == '?' )
        {
            if ( !(pStr = Wlc_PrsReadName(p, pStr+1, vFanins)) )
                return Wlc_PrsWriteErrorMessage( p, pStr, "Cannot read name in MUX." );
            if ( pStr[0] != ':' )
                return Wlc_PrsWriteErrorMessage( p, pStr, "MUX lacks the colon symbol (:)." );
            if ( !(pStr = Wlc_PrsReadName(p, pStr+1, vFanins)) )
                return Wlc_PrsWriteErrorMessage( p, pStr, "Cannot read name in MUX." );
            Type = WLC_OBJ_MUX;
        }
        else if ( pStr[0] == '[' )
        {
            int End, Beg;
            pStr = Wlc_PrsFindRange( pStr, &End, &Beg );
            Vec_IntPush( vFanins, (End << 16) | Beg );
            Type = WLC_OBJ_BIT_SELECT;
        }
        else 
        {
                 if ( pStr[0] == '>' && pStr[1] == '>' && pStr[2] != '>' ) pStr += 2, Type = fRotating ? WLC_OBJ_ROTATE_R : WLC_OBJ_SHIFT_R;
            else if ( pStr[0] == '>' && pStr[1] == '>' && pStr[2] == '>' ) pStr += 3, Type = WLC_OBJ_SHIFT_RA;      
            else if ( pStr[0] == '<' && pStr[1] == '<' && pStr[2] != '<' ) pStr += 2, Type = fRotating ? WLC_OBJ_ROTATE_L : WLC_OBJ_SHIFT_L;
            else if ( pStr[0] == '<' && pStr[1] == '<' && pStr[2] == '<' ) pStr += 3, Type = WLC_OBJ_SHIFT_LA;      
            else if ( pStr[0] == '&' && pStr[1] != '&'                   ) pStr += 1, Type = WLC_OBJ_BIT_AND;       
            else if ( pStr[0] == '|' && pStr[1] != '|'                   ) pStr += 1, Type = WLC_OBJ_BIT_OR;        
            else if ( pStr[0] == '^' && pStr[1] != '^'                   ) pStr += 1, Type = WLC_OBJ_BIT_XOR;       
            else if ( pStr[0] == '&' && pStr[1] == '&'                   ) pStr += 2, Type = WLC_OBJ_LOGIC_AND;     
            else if ( pStr[0] == '|' && pStr[1] == '|'                   ) pStr += 2, Type = WLC_OBJ_LOGIC_OR;      
            else if ( pStr[0] == '=' && pStr[1] == '='                   ) pStr += 2, Type = WLC_OBJ_COMP_EQU;      
            else if ( pStr[0] == '!' && pStr[1] == '='                   ) pStr += 2, Type = WLC_OBJ_COMP_NOTEQU;      
            else if ( pStr[0] == '<' && pStr[1] != '='                   ) pStr += 1, Type = WLC_OBJ_COMP_LESS;     
            else if ( pStr[0] == '>' && pStr[1] != '='                   ) pStr += 1, Type = WLC_OBJ_COMP_MORE;     
            else if ( pStr[0] == '<' && pStr[1] == '='                   ) pStr += 2, Type = WLC_OBJ_COMP_LESSEQU;
            else if ( pStr[0] == '>' && pStr[1] == '='                   ) pStr += 2, Type = WLC_OBJ_COMP_MOREEQU;  
            else if ( pStr[0] == '+'                                     ) pStr += 1, Type = WLC_OBJ_ARI_ADD;       
            else if ( pStr[0] == '-'                                     ) pStr += 1, Type = WLC_OBJ_ARI_SUB;       
            else if ( pStr[0] == '*' && pStr[1] != '*'                   ) pStr += 1, Type = WLC_OBJ_ARI_MULTI;     
            else if ( pStr[0] == '/'                                     ) pStr += 1, Type = WLC_OBJ_ARI_DIVIDE;        
            else if ( pStr[0] == '%'                                     ) pStr += 1, Type = WLC_OBJ_ARI_MODULUS;   
            else if ( pStr[0] == '*' && pStr[1] == '*'                   ) pStr += 2, Type = WLC_OBJ_ARI_POWER;
            else return Wlc_PrsWriteErrorMessage( p, pStr, "Unsupported operation (%c).", pStr[0] );
            if ( !(pStr = Wlc_PrsReadName(p, pStr+1, vFanins)) )
                return 0;
        }
    }
    // make sure there is nothing left there
    if ( pStr )
    {
        pStr = Wlc_PrsFindName( pStr, &pName );
        if ( pStr != NULL )
            return Wlc_PrsWriteErrorMessage( p, pStr, "Name %s is left at the end of the line.", pName );   
    }
    return Type;
}
int Wlc_PrsReadDeclaration( Wlc_Prs_t * p, char * pStart )
{
    int fFound = 0, Type = WLC_OBJ_NONE, iObj; 
    int Signed = 0, Beg = 0, End = 0, NameId;
    if ( Wlc_PrsStrCmp( pStart, "input" ) )
        Type = WLC_OBJ_PI, pStart += strlen("input");
    else if ( Wlc_PrsStrCmp( pStart, "output" ) )
        Type = WLC_OBJ_PO, pStart += strlen("output");
    pStart = Wlc_PrsSkipSpaces( pStart );
    if ( Wlc_PrsStrCmp( pStart, "wire" ) )
        pStart += strlen("wire");
    // read 'signed'
    pStart = Wlc_PrsFindWord( pStart, "signed", &Signed );
    // read range
    pStart = Wlc_PrsFindRange( pStart, &End, &Beg );
    if ( pStart == NULL )
        return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read range." );
    while ( 1 )
    {
        char * pName;
        // read name
        pStart = Wlc_PrsFindName( pStart, &pName );
        if ( pStart == NULL )
            return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read name." );
        NameId = Abc_NamStrFindOrAdd( p->pNtk->pManName, pName, &fFound );
        if ( fFound )
            return Wlc_PrsWriteErrorMessage( p, pStart, "Name %s is declared more than once.", pName );
        iObj = Wlc_ObjAlloc( p->pNtk, Type, Signed, End, Beg );
        assert( iObj == NameId );
        // check next definition
        pStart = Wlc_PrsSkipSpaces( pStart );
        if ( pStart[0] == ',' )
        {
            pStart++;
            continue;
        }
        // check definition
        Type = Wlc_PrsFindDefinition( p, pStart, p->vFanins );
        if ( Type )
        {
            Wlc_Obj_t * pObj = Wlc_NtkObj( p->pNtk, iObj );
            Wlc_ObjUpdateType( p->pNtk, pObj, Type );
            Wlc_ObjAddFanins( p->pNtk, pObj, p->vFanins );
        }
        break;
    }
    return 1;
}
int Wlc_PrsDerive( Wlc_Prs_t * p )
{
    char * pStart, * pName;
    int i;
    // go through the directives
    Wlc_PrsForEachLine( p, pStart, i )
    {
        if ( Wlc_PrsStrCmp( pStart, "module" ) )
        {
            // get module name
            pName = strtok( pStart + strlen("module"), " \r\n\t(,)" );
            if ( pName == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read model name." );
            if ( Wlc_PrsStrCmp( pName, "table" ) )
            {
                // THIS IS A HACK TO DETECT tables
                int Width1 = -1, Width2 = -1;
                int v, b, Value, nBits, nInts;
                unsigned * pTable;
                Vec_Int_t * vValues = Vec_IntAlloc( 256 );
                Wlc_PrsForEachLineStart( p, pStart, i, i+1 )
                {
                    if ( Wlc_PrsStrCmp( pStart, "endcase" ) )
                        break;
                    pStart = Wlc_PrsFindSymbol( pStart, '\'' );
                    if ( pStart == NULL )
                        continue;
                    Width1 = atoi(pStart-1);
                    pStart = Wlc_PrsFindSymbol( pStart+2, '\'' );
                    if ( pStart == NULL )
                        continue;
                    Width2 = atoi(pStart-1);
                    Value = 0;
                    Abc_TtReadHexNumber( (word *)&Value, pStart+2 );
                    Vec_IntPush( vValues, Value );
                }
                //Vec_IntPrint( vValues );
                nBits = Abc_Base2Log( Vec_IntSize(vValues) );
                if ( Vec_IntSize(vValues) != (1 << nBits) )
                {
                    Vec_IntFree( vValues );
                    return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read module \"%s\".", pName );
                }
                assert( Width1 == nBits );
                // create bitmap
                nInts = Abc_BitWordNum( Width2 * Vec_IntSize(vValues) );
                pTable = (unsigned *)Mem_FlexEntryFetch( p->pMemTable, nInts * sizeof(unsigned) );
                memset( pTable, 0, nInts * sizeof(unsigned) );
                Vec_IntForEachEntry( vValues, Value, v )
                    for ( b = 0; b < Width2; b++ )
                        if ( (Value >> b) & 1 )
                            Abc_InfoSetBit( pTable, v * Width2 + b );
                Vec_PtrPush( p->vTables, pTable );
                Vec_IntFree( vValues );
                continue;
            }
            if ( p->pNtk != NULL )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Network is already defined." );
            p->pNtk = Wlc_NtkAlloc( pName, Vec_IntSize(p->vStarts) );
            p->pNtk->pManName = Abc_NamStart( Vec_IntSize(p->vStarts), 20 );
            p->pNtk->pMemTable = p->pMemTable; p->pMemTable = NULL;
            p->pNtk->vTables = p->vTables; p->vTables = NULL;
            // read the argument definitions
            while ( (pName = strtok( NULL, "(,)" )) )
            {
                pName = Wlc_PrsSkipSpaces( pName );
                if ( Wlc_PrsStrCmp( pName, "input" ) || Wlc_PrsStrCmp( pName, "output" ) || Wlc_PrsStrCmp( pName, "wire" ) )
                {
                    if ( !Wlc_PrsReadDeclaration( p, pName ) )
                        return 0;
                }
            }
        }
        else if ( Wlc_PrsStrCmp( pStart, "endmodule" ) )
        {
            Vec_Int_t * vTemp = Vec_IntStartNatural( Wlc_NtkObjNumMax(p->pNtk) );
            Vec_IntAppend( &p->pNtk->vNameIds, vTemp );
            Vec_IntFree( vTemp );
            break;
        }
        // these are read as part of the interface
        else if ( Wlc_PrsStrCmp( pStart, "input" ) || Wlc_PrsStrCmp( pStart, "output" ) || Wlc_PrsStrCmp( pStart, "wire" ) )
        {
            if ( !Wlc_PrsReadDeclaration( p, pStart ) )
                return 0;
        }
        else if ( Wlc_PrsStrCmp( pStart, "assign" ) )
        {
            int Type, NameId, fFound;
            pStart += strlen("assign");
            // read name
            pStart = Wlc_PrsFindName( pStart, &pName );
            if ( pStart == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read name after assign." );
            NameId = Abc_NamStrFindOrAdd( p->pNtk->pManName, pName, &fFound );
            if ( !fFound )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Name %s is not declared.", pName );
            // read definition
            Type = Wlc_PrsFindDefinition( p, pStart, p->vFanins );
            if ( Type )
            {
                Wlc_Obj_t * pObj = Wlc_NtkObj( p->pNtk, NameId );
                Wlc_ObjUpdateType( p->pNtk, pObj, Type );
                Wlc_ObjAddFanins( p->pNtk, pObj, p->vFanins );
            }
            else
                return 0;
        }
        else if ( Wlc_PrsStrCmp( pStart, "table" ) )
        {
            // THIS IS A HACK TO DETECT tables
            int NameId, fFound, iTable = atoi( pStart + strlen("table") );
            // find opening
            pStart = Wlc_PrsFindSymbol( pStart, '(' );
            if ( pStart == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read table." );
            // read input
            pStart = Wlc_PrsFindName( pStart+1, &pName );
            if ( pStart == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read name after assign." );
            NameId = Abc_NamStrFindOrAdd( p->pNtk->pManName, pName, &fFound );
            if ( !fFound )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Name %s is not declared.", pName );
            // save inputs
            Vec_IntClear( p->vFanins );
            Vec_IntPush( p->vFanins, NameId );
            Vec_IntPush( p->vFanins, iTable );
            // find comma
            pStart = Wlc_PrsFindSymbol( pStart, ',' );
            if ( pStart == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read table." );
            // read output
            pStart = Wlc_PrsFindName( pStart+1, &pName );
            if ( pStart == NULL )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read name after assign." );
            NameId = Abc_NamStrFindOrAdd( p->pNtk->pManName, pName, &fFound );
            if ( !fFound )
                return Wlc_PrsWriteErrorMessage( p, pStart, "Name %s is not declared.", pName );
            {
                Wlc_Obj_t * pObj = Wlc_NtkObj( p->pNtk, NameId );
                Wlc_ObjUpdateType( p->pNtk, pObj, WLC_OBJ_TABLE );
                Wlc_ObjAddFanins( p->pNtk, pObj, p->vFanins );
            }
        }
//        else if ( Wlc_PrsStrCmp( pStart, "CPL_FF" ) )
        else
        {
            pStart = Wlc_PrsFindName( pStart, &pName );
            return Wlc_PrsWriteErrorMessage( p, pStart, "Cannot read line beginning with %s.", pName );
        }
    }
    return 1;
}
Wlc_Ntk_t * Wlc_ReadVer( char * pFileName )
{
    Wlc_Prs_t * p;
    Wlc_Ntk_t * pNtk = NULL;
    // start the parser 
    p = Wlc_PrsStart( pFileName );
    if ( p == NULL )
        return NULL;
    // detect lines
    if ( !Wlc_PrsPrepare( p ) )
        goto finish;
    // parse models
    if ( !Wlc_PrsDerive( p ) )
        goto finish;
    // derive topological order
    pNtk = Wlc_NtkDupDfs( p->pNtk );
    Wlc_NtkTransferNames( pNtk, p->pNtk );
finish:
    Wlc_PrsPrintErrorMessage( p );
    Wlc_PrsStop( p );
    return pNtk;
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/
void Io_ReadWordTest( char * pFileName )
{
    Gia_Man_t * pNew;
    Wlc_Ntk_t * pNtk = Wlc_ReadVer( pFileName );
    if ( pNtk == NULL )
        return;
    Wlc_WriteVer( pNtk, "test.v" );

    pNew = Wlc_NtkBitBlast( pNtk );
    Gia_AigerWrite( pNew, "test.aig", 0, 0 );
    Gia_ManStop( pNew );

    Wlc_NtkFree( pNtk );
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

