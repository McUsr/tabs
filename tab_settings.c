#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tab_consts.h"
#define MAXLINE 1000
int tabpos[MAXLINE] ;
int LAST_TAB=0;
/*
void abort_list_arg_error( char *bad_arg) ;
void abort_bad_num_arg( char *bad_arg) ;
void add_tabs( int tabpos, char prefix);
int xtatou(char s[], char *prefix) ;
int tabs_been_set() ;
void set_eventabs( int tab_sz ) ;
void set_random_tab( int tab_sz ) ;
void set_mPlusn_tabs( int m, int n) ;
void print_tab_positions() ;
*/

void abort_with_msg( char *prgname, char *s )
{
    fprintf(stderr,"%s: %s\n",prgname,s);
    exit(2) ;
}

void abort_list_arg_error( char *prgname, char *bad_arg)
{
    fprintf(stderr,"%s: it is illegal to supply more numeric arguments \"%s\""
           " after a '+|/' argument\n",prgname,bad_arg);
     _Exit(2) ; 
}
void abort_bad_num_arg(char *prgname, char *bad_arg)
{
    fprintf(stderr,"%s: Bad numeric argument for "
            "\"-t=\" or \"-tabs=\" single form: \"%s\""
            "\n...Exiting.\n",prgname, bad_arg );
    _Exit(2) ;
}

/* Options debugging machinery
 * we print out the tab positions,
 * and try it with different use cases, from a script.
 */
/* print_tab_positions: print out the tab positions
 */
void print_tab_positions()
{
    /* we need to add one, so that we have a one based index */

    for( int i=0 ; i<=LAST_TAB ; i++ )
    {
        if (tabpos[i] )
            printf("tabpos: %-2d\n",i+1) ;
    }

}

/* add_tabs: adds tabs using the correct system for the current option,
 * it  is a higher level routine, interfacing the underlying set_tab
 * routines.
 */
void add_tabs( int tabpos, char prefix)
{
    void set_random_tab( int tab_sz ) ;
    void set_mPlusn_tabs( int m, int n) ;
    void set_tabsize_after_last_tab( int n) ;
    switch (prefix) 
    {
        case NUL :
            printf("NUL:\n");
            set_random_tab(tabpos) ;
            break;
        case MINUS :
            set_random_tab(tabpos) ;
            break;
        case PLUS : 
            printf("PLUS\n");
            printf("LAST_TAB == %d\n",LAST_TAB);
            set_mPlusn_tabs( LAST_TAB, tabpos) ;
            break ;
        case DIV :
            set_tabsize_after_last_tab( tabpos ) ;
    }
}

/* xtoau: returns an unsigned, 0 if no number, NUL '\0'
 * if no prefix, otherwise '-', '+' or '/'.
 * Only the last argument can be prefixed with '+' and '/'
 * during list mode, only '+' * during the "-m +n" mode.
 * '-' only at the start arg during the "-m +n" mode.
 */
int xtatou(char s[], char *prefix)
{
    *prefix=NUL ;
    /* We look for a prefix: */
    if (strpbrk(&s[0],"+-/") == &s[0] ) {
        /* printf("A Hit!!!\n") ; */
        *prefix=s[0] ;
        s++ ;
    }
    int  n;
    for ( n = 0 ; isdigit(*s); s++ )
        n = 10 * n + (*s - '0' ) ;
    return n;
}

/* tabs_been_set returns true if LAST_TAB != 0
 * we use this to communicate to options parsing
 * whether tabs have been set or not before we
 * abort the parsing of options. */

int tabs_been_set() {
    return LAST_TAB;
}

void set_eventabs( int tab_sz )
{
    int i=-1;            /* zerobazed array */
    /* i=tab_sz-1; */
    while ( (i+tab_sz) < (MAXLINE-1) ) {
        i+=tab_sz;
        tabpos[i] = 1 ;
        LAST_TAB = max(LAST_TAB,i) ;
    }
}

void set_random_tab( int tab_sz ) 
{
        if ( tab_sz > LAST_TAB ) {
            tabpos[tab_sz-1] = 1 ;
            LAST_TAB = max(LAST_TAB,tab_sz ) ;
        } else {
            fprintf(stderr,"detab: Tab positions not in ascending order."
                    "\nTerminating...\n");
            _Exit(2) ;
        }
}

/* set_mPlusn_tabs: sets tabs relative to  the pos "m'
 * n is evenly spaced from that position, to MAXLINE
 */
void set_mPlusn_tabs( int m, int n) {
    int i=m-1;
    while ( (i+n) < (MAXLINE-1) ) {
        i+=n ;
        tabpos[i] = 1 ;
        LAST_TAB = max(LAST_TAB,i) ;
    }
}

/* set_tabsize_after_last_tab;
 * The are calculated from the start, and only set
 * after the last set tab, which is kind of special:
 * We only update the last tab after we have set the
 * last one, This is something to test for otherwise:
 * That we only set tabs, if the positions are greater than 
 * the last one, (when in list mode, and if the situation
 * happens, then we should error out! 
 */
void set_tabsize_after_last_tab( int n) 
{
    int i=n-1,j=0;            /* zerobazed array */
    while ( (i+n)<= LAST_TAB ) 
        i+=n;

    while ( (i+n)< MAXLINE ) {
        i+=n;
        tabpos[i] = 1 ;
        LAST_TAB = max(LAST_TAB,i) ;
    }
}
