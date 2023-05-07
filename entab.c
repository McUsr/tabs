/* K&R exercise p. 112? 
 * make a detab that takes arguments, even a list of tab stops from command line.
 * detab -- convert tabs to spaces.
 *
 * Original exercise p. 34.
 *
 * If we are to take file name arguments, then we could indeed first write the input to a temp file,
 * then reopen and process with stdout duped and reopened?
 *
 * The opposite idea of detab see, we are going to unexpand spaces to tabs (compress).
 * so there are options amiss concerning lists of tabstops.
 * 
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "tab_consts.h"
#include "tab_settings.h"

#define PROGNAME "\033[1entab\033[0m"
#define getchar() getc(in)
int main(int argc, char **argv)
{
    int tabval=8, initial_only=FALSE;
#ifdef HANOI  
    char *modus=NULL;
#endif 
    FILE *in=stdin ;
    int processed=0; 
    void show_help();
#ifdef HANOI  
    printf("initial argc: %d\n",argc);
#endif 
    while ( argc > 1 ) {
       ++argv;
       --argc;
       printf("Argv== %s\n",*argv) ;
       if (!strcmp(*argv,"-i") || !strcmp(*argv,"--initial") ) {
            initial_only=TRUE;
        } else if (!strcmp(*argv,"-h")||!strcmp(*argv,"--help")) { 
            show_help() ; /* show help and exit.*/
        } else if (!strncmp(*argv,"-t=",3) || !strncmp(*argv,"--tabs=",7) ) {
            int ofs;
            char prefix = NUL, prev_prefix=NUL ;
#ifdef HANOI  
            printf("About to set tab positions.\n") ;
#endif 
            if (strstr(*argv,",")) {
                int j=1, ENDVAL=FALSE;
                char *str1,*token;
#ifdef HANOI  
                modus=strdup("List of tabs");
#endif 
                ofs = (!strncmp(*argv,"-t=",3)) ?  3 : 7 ;

                for (str1 = &(*argv)[ofs]; ; j++, str1 = NULL) {

                    token = strtok(str1, ",");
                    if (token == NULL)
                        break;
#ifdef HANOI  
                    printf("token == %s\n",token);
#endif 
                    if ((tabval=xtatou(token,&prefix)) != 0 && (prev_prefix != PLUS && prev_prefix != DIV )) {
#ifdef HANOI  
                        printf("setting tabpos: %d  prefix == ",tabval); 
#endif 
                        prev_prefix= prefix ;
                        add_tabs( tabval,prefix) ;
                    } else if ( prev_prefix == PLUS || prev_prefix == DIV ) {
                        abort_list_arg_error(PROGNAME, token ) ;
                        exit(2);
                        /* ERROR CONDITION: we simply ignore MINUS at this point */
                    } else {
                        fprintf(stderr,"%s: NaN: \"%s\"...Exiting.\n","detab",token );
                        exit(2) ;
                    }

                }
            } else {
                /* We have a regular -{t|tabval}= situation.  Or we are getting
                 * a space separated list with arguments.  And: we are keeping
                 * in line with regards to incrementing argc at the top of the
                 * loop, without any hanky panky.
                 */
                ofs = (!strncmp(*argv,"-t=",3)) ?  3 : 7 ;

                tabval= xtatou(&(*argv)[ofs], &prefix) ;
#ifdef HANOI  
                printf("tabval == %d\n",tabval );
#endif 
                if (tabval == 0 || prefix != NUL) {
                    abort_bad_num_arg(PROGNAME, &(*argv)[ofs]) ;
                } else if (argc <= 1 ) { /* no point in looking for more */
#ifdef HANOI  
                    modus=strdup("Even tab settings");
#endif 
                    set_eventabs(tabval) ;
                   
                } else  { /* check to see if we got a list. */
                    int tmpval;
#ifdef HANOI  
                    modus=strdup("List of tabs");
#endif 
                    /* We LOOK AHEAD, at this point we know we have an extra
                     * argument left */

                    if ((tmpval=xtatou(*(argv+1),&prefix)) != 0 ) {
                        
                       set_random_tab(tabval) ;

                        prev_prefix= prefix ;
                       /* maybe '+; or '/' prefix from here */
                        add_tabs( tmpval,prefix) ;

                       /* we used the argument, so we need to adjust the
                        * current argument, which is already processed and the
                        * argument counter */
                       --argc;
                       ++argv;
                        for ( ; argc > 1 ; --argc,++argv  ) {
                            if ((tmpval=xtatou(*(argv+1),&prefix)) != 0 && ( prev_prefix != PLUS && prev_prefix != DIV)) {
                            /* '+; or '/' prefix from here */
#ifdef HANOI  
                                printf("List of tabs! val = %d prefix == ",tmpval);
#endif 
                                prev_prefix= prefix ;
                                add_tabs( tmpval,prefix) ;
                            } else if ( prev_prefix == PLUS || prev_prefix == DIV ) {
                                abort_list_arg_error(PROGNAME, *(argv+1) ) ;
                                exit(2);
                            } else  /* tmpval == 0 */ {
                                break ; /* no more numbers to process */
                            }
                        }
                    } else { /* we didn't have a list, only even tabs. */
#ifdef HANOI  
                        modus=strdup("Even tab settings");
#endif 
                        set_eventabs(tabval) ;
                    }
                }
            }
        } else if ((*(argv))[0] == '-' && (strpbrk(&(*(argv))[1],"1234567890") == &(*(argv))[1] )) {
#ifdef HANOI  
            printf("HERE IN M +N\n") ;
            modus=strdup("-m +n construct");
#endif 
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
                    abort_with_msg(PROGNAME,"Incomplete \"-n +m\" construct, missing \"+m\" argument.");
                }
            } else {
                abort_with_msg(PROGNAME,"Incomplete \"-n +m\" construct, missing \"+m\" argument.");
            }
        } else if (!strcmp(*argv,"--")) {
            ++argv ;
            --argc ;
           break;
       } else {
           ++argc ;
           break ;
       }
    }
#ifdef HANOI  
    if (argc > 1 ) {
        printf("Next argument is: %s \n",*argv);
        return 1 ;
    } 
#endif 
    if (!tabs_been_set() ) {
      set_eventabs( tabval ) ;
    }
    for (;;) {
        if ( argc > 1 ) {
            --argc;
            *argv++ ;
            if (!strcmp(*argv,"-") ) {
                if (processed > 0 )
                    in=stdin;
            } else if ( (in = fopen(*argv,"r") ) == NULL ) {
                fprintf(stderr,"fileargs1: Can't open %s\n",*argv );
                ++processed ;
                continue;
            }
        }

        int c,col=1,newcol,nonwhi=0 ;
        do {
            newcol=col ;
            while ((c=getchar()) == SPACE ) {
                ++newcol;
                if (istabpos(newcol) && !nonwhi || ( nonwhi && !initial_only ) && newcol < LAST_TAB ) {
                    putchar(TAB) ;
                    col=newcol;
                }
            }
            while (col<newcol ) {
                putchar(SPACE);
                ++col ;
            }
            if(c != EOF) {
                if (!isspace(c) ) {
                    ++col;
                    nonwhi=TRUE ;
                    putchar(c) ;
                } else if (c == BACKSPACE ) {
                    --col ;
                    putchar(c) ;
                } else if (c == NEWLINE ) {
                    nonwhi=FALSE;
                    col=1;
                    putchar(c) ;
                } else 
                    putchar(c) ;
            }
        } while (c != EOF) ;
        ++processed ;
        if (argc == 1 ) break ;
    }
    return 0; 
}
void show_help() 
{
    printf("\n\033[1mentab\033[0m unexpands spaces to tabs upto and including the tabpos\n"
            "and prints the output to stdout.\n\n"
            "Options:\n\n"
            "-i --initial : do not convert spaces to tabs after non blanks.\n\n"
            /* here comes the options from the tabs program */
            "-t= or --tabs=N : have tabs N characters apart, not 8\n\n"
            "-t= or --tabs=LIST : Use comma separated list of tab positions,\n"
            "or space separated arguments. The last value can be prefixed:\n\n"
            "    '/' : specify a a tab size to use after last explicitly set\n"
            "\ttab stop.\n"
            "    '+' can be used to align remaining tabstops relative to the\n"
            "    last specified tab stop instead of the first column.\n\n"
            "-m +n :set tabstops n spaces apart, starting at position m\n\n"
            "-- :signals the end of options and the beginning of file arguments.\n\n");
    exit(0) ;
}
