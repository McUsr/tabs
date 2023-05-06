/* K&R exercise p. 112? 
 * make a detab that takes arguments, even a list of tab stops from command line.
 * detab -- convert tabs to spaces.
 *
 * Original exercise p. 34.
 *
 * If we are to take file name arguments, then we could indeed first write the input to a temp file,
 * then reopen and process with stdout duped and reopened?
 *
 * Let us build detab so that when a tab is encountered after the last tab stop it is converted to a
 * single blank.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
/*
#define getch() getc(stdin) 
#define ungetch(c) ungetc(c,stdin)
*/
#define MAXLINE 1000
#define FALSE 0
#define TRUE 1
#define TAB '\t'
#define NEWLINE '\n'
#define BACKSPACE '\b'
#define MINUS '-'
#define PLUS '+'
#define NUL '\0' 
#define SPACE ' '
#define DIV '/'
#define max(a,b) ((a)>(b)) ? (a) : (b)  
#define istabpos(i) (tabpos[i-1] == 1 ) ? 1 : 0 
#define redir_tostdin(mf,fname) mf = freopen(fname,"r",stdin) ;\
    if (mf == NULL ) {\
        fprintf(stderr,"detab: Couldn't open %s.\nTerminating...\n",fname);\
        exit(1);\
    } else {\
        ++argv;\
        --argc;\
    }

#define SHIFT --argc;\
    *++argv
#define exit(n) _exit(n)
int tabpos[MAXLINE] ;
int LAST_TAB=0;

int main(int argc, char **argv)
{
    int tabval=8, processed=argc, initial_only=FALSE;
    char *modus=NULL;
    void show_help();
    void abort_with_msg( char *s ) ;
    void abort_list_arg_error( char *bad_arg) ;
    void abort_bad_num_arg( char *bad_arg) ;
    void add_tabs( int tabpos, char prefix);
    int xtatou(char s[], char *prefix) ;
    int tabs_been_set() ;
    void set_eventabs( int tab_sz ) ;
    void set_random_tab( int tab_sz ) ;
    void set_mPlusn_tabs( int m, int n) ;
    void print_tab_positions() ;

    extern int errno ;
    
    printf("initial argc: %d\n",argc);
    while ( argc > 1 ) {
         /* printf("argc > 1\n"); */
       ++argv;
       --argc;
       printf("Argv== %s\n",*argv) ;
       if (!strcmp(*argv,"-i") || !strcmp(*argv,"--initial") ) {
            /* printf("do not convert tabs after non blanks\n") ; */
            initial_only=TRUE;
            --processed ;
            /* help  -h comes here */

        } else if (!strcmp(*argv,"-h")||!strcmp(*argv,"--help")) { 
            show_help() ; /* show help and exit.*/
        } else if (!strncmp(*argv,"-t=",3) || !strncmp(*argv,"--tabs=",7) ) {
            int ofs;
            char prefix = NUL, prev_prefix=NUL ;
            /* so, we when we get a value with a prefix, then ENDVAL is true,
             * any numbers after an ENDVAL == TRUE is an error.
             */
            printf("About to set tab positions.\n") ;
            if (strstr(*argv,",")) {
                int j=1, ENDVAL=FALSE;
                char *str1,*token;
                modus=strdup("List of tabs");
                ofs = (!strncmp(*argv,"-t=",3)) ?  3 : 7 ;

                for (str1 = &(*argv)[ofs]; ; j++, str1 = NULL) {

                    token = strtok(str1, ",");
                    if (token == NULL)
                        break;
                    /* printf("%d: %s\n", j, token); */
                    /* TODO check for plus or div if found, it is the last
                     * argument we parse, and if there are any other on the list
                     * when the list is comma separated, we signal that it is an error.
                     */
                    printf("token == %s\n",token);
                    if ((tabval=xtatou(token,&prefix)) != 0 && (prev_prefix != PLUS && prev_prefix != DIV )) {
                        printf("setting tabpos: %d  prefix == ",tabval); 
                        prev_prefix= prefix ;
                        add_tabs( tabval,prefix) ;
                    } else if ( prev_prefix == PLUS || prev_prefix == DIV ) {
                        abort_list_arg_error( token ) ;
                        exit(2);
                        /* ERROR CONDITION: we simply ignore MINUS at this point */
                    } else {
                        fprintf(stderr,"%s: NaN: \"%s\"...Exiting.\n","detab",token );
                        exit(2) ;
                    }

                }
                /* printf("We are having a list of tab positions\n") ; */
                --processed ;
            } else {
                /* We have a regular -{t|tabval}= situation.
                 * Or we are getting a space separated list with arguments.
                 * And: we are keeping in line with regards to incrementing argc at the 
                 * top of the loop, without any hanky panky.
                 */
                ofs = (!strncmp(*argv,"-t=",3)) ?  3 : 7 ;

                tabval= xtatou(&(*argv)[ofs], &prefix) ;
                printf("tabval == %d\n",tabval );
                if (tabval == 0 || prefix != NUL) {
                    abort_bad_num_arg( &(*argv)[ofs]) ;
                } else if (argc <= 1 ) { /* no point in looking for more */
                    modus=strdup("Even tab settings");
                    set_eventabs(tabval) ;
                   
                } else  { /* check to see if we got a list. */
                    int tmpval;
                    modus=strdup("List of tabs");
                    /* We LOOK AHEAD, at this point we know we have an
                     * extra argument left */
                    if ((tmpval=xtatou(*(argv+1),&prefix)) != 0 ) {
                        
                       set_random_tab(tabval) ;

                        prev_prefix= prefix ;
                       /* maybe '+; or '/' prefix from here */
                        add_tabs( tmpval,prefix) ;

                       /* we used the argument, so we need to adjust 
                        * the current argument, which is already processed
                        * and the argument counter */
                       --argc;
                       ++argv;
                        for ( ; argc > 1 ; --argc,++argv  ) {
                            if ((tmpval=xtatou(*(argv+1),&prefix)) != 0 && ( prev_prefix != PLUS && prev_prefix != DIV)) {
                            /* '+; or '/' prefix from here */
                                printf("List of tabs! val = %d prefix == ",tmpval);
                                prev_prefix= prefix ;
                                add_tabs( tmpval,prefix) ;
                            } else if ( prev_prefix == PLUS || prev_prefix == DIV ) {
                                abort_list_arg_error( *(argv+1) ) ;
                                exit(2);
                            } else  /* tmpval == 0 */ {
                                break ; /* no more numbers to process */
                            }
                        }
                    } else { /* we didn't have a list, only even tabs. */
                        modus=strdup("Even tab settings");
                        set_eventabs(tabval) ;
                    }
                }
            }
        } else if ((*(argv))[0] == '-' && (strpbrk(&(*(argv))[1],"1234567890") == &(*(argv))[1] )) {
            /* TODO: settabs function that starts with the n offset */
            /* start of a "-m +n" construct */
            printf("HERE IN M +N\n") ;
            modus=strdup("-m +n construct");
            int m=atoi((*argv)+1);
            if (argc >1 ) {
                --argc ;
                ++argv ;
                if (*argv[0] == '+' && (strpbrk((*argv)+1,"1234567890") == (*argv)+1 )) {
                    printf("HERE IN N +M\n") ;
                    int n = atoi((*argv)+1);
                    /* setting tabs as we should */
                   set_mPlusn_tabs( m,  n) ;
                } else {
                    abort_with_msg("Incomplete \"-n +m\" construct, missing \"+m\" argument.");
                }
            } else {
                abort_with_msg("Incomplete \"-n +m\" construct, missing \"+m\" argument.");
            }
        } else if (!strcmp(*argv,"--")) {
            ++argv ;
           --processed ;
           break;
       } else {
           break ;
           /* we check if the argument may be a valid file, before we conclude with error,
            * -we are somewhat forgiving with regards to the "--" option, not making it totally
            * mandatary
            */
       }
    }
/* TODO: error function/macro 
 *
 * implement a flag for list processing? If nest argument is a number, with or without delimiter,
 * we're list processing.
 *
 * The function for delimiters.
 *
 has_delimiter_prefix
 (returns NULL  if not.)

 is_unsigned_int. takes the address, based on has_delimiter prefix.)
 */

  if (!tabs_been_set() ) {
      printf("Tabs NOT set initially\n");
      set_eventabs( tabval ) ;
      modus=strdup("Even tab settings");
  }

/* DEBUG block:
 */
    printf("Modus == %s\n",modus) ;

    print_tab_positions() ;

    return 1 ;


    printf("DONE Options  processing argc== %d argv==%s\n",argc,*argv);
    if (processed != argc ) {
        printf("there were errors on the command line\n") ;
        printf("processed == %d argc == %d",processed,argc ) ;
        exit(2) ;
    } else
        printf(" about to run through input \n") ;

    int first_file=TRUE ;
    int new_stdin = dup(fileno(stdin)) ;
    if (new_stdin == -1 )
        fprintf(stderr,"something went wrong DUPLICATING stdin\n");
    FILE *mf = NULL ;
    do {
        /* so, this is our tab expansion program 
         * it EXPANDS  the tabs into spaces.
         * IDEA: if we find a tab character, we replace it with spaces until
         * the next tabstop from the tabpos array. 
         *
         * We need to keep track of any non whitespace characters, for the case of 
         * the only initial flag.
         *
         * we need to reset the char counter, whenever we get a newline.
         *
         * at this time, we consider file names as superfluous, but we did dup and reopen earlier.
         */
        if (argc > 1 ) {
            char *fname;
            if (first_file ) {
                first_file=FALSE ;
                if (!strcmp(*argv,"-")) {
                    --argc;
                    ++argv;
                } else {
                    fname=strdup(*argv) ;
                    redir_tostdin(mf,fname) ;
                }
            } else {
                /* if (mf != NULL ) fclose(mf) ; */

                if (!strcmp(*argv,"-")) {
                    fprintf(stderr,"HAHAHAHAHAHA,reopening stdin!!!\n" );
                    if (new_stdin == -1 )
                        fprintf(stderr,"MAJOR F**P\n");

                    --argc;
                    ++argv;
                    if (dup2(fileno(stdin),new_stdin) == -1 ) 
                        fprintf(stderr,"HAHAHAHAHAHA,something went wrong reopening stdin: !!!\n");
                } else {
                    fname=strdup(*argv) ;
                    redir_tostdin(mf,fname) ;
                } 
            } 
        }
        printf("after processing arguments\n") ;
        int c,i=0,nonwhi=0 ;
        while ((c=getchar()) != EOF) {
            ++i;
            if (c == TAB ) {
                if (!nonwhi || ( nonwhi && !initial_only ) && i < LAST_TAB ) {
                    while (!istabpos(i) ) {
                        putchar(SPACE) ;
                        ++ i ;
                    }
                } else if ( i>LAST_TAB ) {
                    putchar( SPACE );
                } else 
                    putchar( TAB );

            } else if (!isspace(c) ) {
                nonwhi=TRUE ;
                putchar(c) ;
            } else if (c == BACKSPACE ) {
                --i ;
                putchar(c) ;
            } else if (c == NEWLINE ) {
                nonwhi=FALSE;
                i=0;
                putchar(c) ;
            } else
                putchar(c) ;
        }

    } while (argc > 1 ) ;
    return 0; 
}

void abort_with_msg( char *s )
{
    fprintf(stderr,"detab: %s\n");
    exit(2) ;
}
void abort_list_arg_error( char *bad_arg)
{
    fprintf(stderr,"detab: it is illegal to supply more numeric arguments \"%s\""
           " after a '+|/' argument\n",bad_arg);
     exit(2) ; 
}
void abort_bad_num_arg( char *bad_arg)
{
    fprintf(stderr,"detab: Bad numeric argument for "
            "\"-t=\" or \"-tabs=\" single form: \"%s\""
            "\n...Exiting.\n",bad_arg );
    exit(2) ;
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


void show_help() 
{
    printf("\n\033[1mdetab\033[0m expands tabs to spaces upto but not including the first tabpos\n"
            "and prints the output to stdout.\n\n"
            "Options:\n\n"
            "-i --initial : do not convert tabs after non blanks.\n\n"
            /* here comes the options from the tabs program */
            "-t= or --tabs=N : have tabs N characters apart, not 8\n\n"
            "-t= or --tabs=LIST : Use comma separated list of tab positions,\n"
            "or space separated arguments. The last value can be prefixed:\n\n"
            "    '/' : specify a a tab size to use after last explicitly set\n"
            "\ttab stop.\n"
            "    '+' can be used to align remaining tabstops relative to the\n"
            "    last specified tab stop instead of the first column.\n\n"
            "-m +n :set tabstops n spaces apart, starting at position m\n\n"
            "-- :signals the end of options and the beginning of file arguments.");
    exit(0) ;
}
/* we really need LAST_TAB in all tabsetting
 * routines, since we test for it in the expand
 * algorithm.
 */

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
            exit(2) ;
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
