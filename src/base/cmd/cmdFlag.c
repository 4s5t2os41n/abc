/**CFile****************************************************************

  FileName    [cmdFlag.c]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [Command processing package.]

  Synopsis    [Procedures working with flags.]

  Author      [Alan Mishchenko]
  
  Affiliation [UC Berkeley]

  Date        [Ver. 1.0. Started - June 20, 2005.]

  Revision    [$Id: cmdFlag.c,v 1.00 2005/06/20 00:00:00 alanmi Exp $]

***********************************************************************/

#include "mainInt.h"

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFITIONS                           ///
////////////////////////////////////////////////////////////////////////


/**Function********************************************************************

  Synopsis    [Looks up value of flag in table of named values.]

  Description [The command parser maintains a table of named values.  These
  are manipulated using the 'set' and 'unset' commands.  The value of the
  named flag is returned, or NIL(char) is returned if the flag has not been
  set.]

  SideEffects []

******************************************************************************/
char * Cmd_FlagReadByName( Abc_Frame_t * pAbc, char * flag )
{
  char *value;

  if (st_lookup(pAbc->tFlags, flag, &value)) {
    return value;
  }
  else {
    return NIL(char);
  }
}


/**Function********************************************************************

  Synopsis    [Updates a set value by calling instead of set command.]

  Description [Updates a set value by calling instead of set command.]

  SideEffects []

******************************************************************************/
void Cmd_FlagUpdateValue( Abc_Frame_t * pAbc, char * key, char * value )
{
  char *oldValue, *newValue;

  if (!key)
    return;
  if (value)
    newValue = util_strsav(value);
  else
    newValue = util_strsav("");

  if (st_delete(pAbc->tFlags, &key, &oldValue))
    FREE(oldValue);

  st_insert(pAbc->tFlags, key, newValue);
}


/**Function********************************************************************

  Synopsis    [Deletes a set value by calling instead of unset command.]

  Description [Deletes a set value by calling instead of unset command.]

  SideEffects []

******************************************************************************/
void Cmd_FlagDeleteByName( Abc_Frame_t * pAbc, char * key )
{
  char *value;

  if (!key)
    return;

  if (st_delete(pAbc->tFlags, &key, &value)) {
    FREE(key);
    FREE(value);
  }
}



////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


