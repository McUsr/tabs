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
#define BLANK ' '
#define max(a,b) ((a)>(b)) ? (a) : (b)  
#define istabpos(i) (tabpos[i-1] == 1 ) ? 1 : 0 
int tabpos[MAXLINE] ;
int LAST_TAB=0;
int main(int argc, char **argv)
{
    int tabval=8, processed=argc, initial_only=FALSE;
    void set_eventabs( int tab_sz ) ;
    void set_random_tab( int tab_sz ) ;
    
   if (argc == 1 || (argv[1][0] == '-' && argv[1][1] == '-' && argv[1][2] == '\0' ) ) {
      printf("setting tabs ourselves, 8 spaces per tab. \n") ;
      set_eventabs( tabval ) ;
  } else {
      /* We need to process arguments for tabs from the command line. */
      /* -i --initial : do not convert tabs after non blanks */
      /* -t --tabs=N have tabs N characgters apart, not 8 */
      /* -t --tabs=LIST USE comma separated list of tab position. last can be prefixed with
       * '/' to specify a a tab size to use after the last explicitly specified tab stop.
       * Also a prefix of + can be used to align remaining tabstops relative to the last 
       * specified tab stop instead of the first column.
       *


       processes files / - for stdin, and sends to stdout.

       --first-only
              convert only leading sequences of blanks (overrides -a)

       -t, --tabs=N
              have tabs N characters apart instead of 8 (enables -a)

       -t, --tabs=LIST
              use  comma separated list of tab positions The last specified position can be prefixed with '/' to specify a tab size
              to use after the last explicitly specified tab stop.  Also a prefix of '+' can be used to align remaining  tab  stops
              relative to the last specified tab stop instead of the first column

       --help display this help and exit

       --version
              output version information and exit


       */

    while ( argc > 1 ) {
        /* printf("argc > 1\n"); */
        ++argv;

        if (strcmp(*argv,"-i") == 0 ) {
            /* printf("do not convert tabs after non blanks\n") ; */
            initial_only=TRUE;
            --processed ;
       } else if (strncmp(*argv,"-t=",3) == 0 ) {
            /* printf("About to set tab positions.\n") ; */
            if (strstr(*argv,",")) {
                int j=1;
                char *str1,*token;
                for (str1 = &(*argv)[3]; ; j++, str1 = NULL) {

                    token = strtok(str1, ",");
                    if (token == NULL)
                        break;
                    /* printf("%d: %s\n", j, token); */
                    if ((tabval=atoi(token)) != 0 ) {
                        /* printf("setting tabpos: %d\n",tabval); */
                        set_random_tab(tabval) ;
                    } else {
                        fprintf(stderr,"%s: NaN: \"%s\"...Exiting.\n","detab",token );
                        exit(2) ;
                    }

                }
                /* printf("We are having a list of tab positions\n") ; */
                --processed ;
            } else {
                tabval= atoi(&(*argv)[3]) ;
                if (tabval != 0 ) {
                    int tmpval;
                    ++argv;
                    if ( argc > 1 && (tmpval=atoi(*argv)) != 0 ) {
                       set_random_tab(tabval) ;
                       set_random_tab(tmpval );
                       --argc;
                       ++argv;
                       processed-=2 ;
                        while ( argc > 1 && (tmpval=atoi(*argv)) != 0 ) {
                           set_random_tab(tmpval) ;
                           --argc;    
                           ++argv;
                           --processed ;
                        }
                        --argv;
                    } else {
                        set_eventabs(tabval) ;
                        --processed ;
                    }
                } else {
                    fprintf(stderr,"%s: NaN: \"%s\"...Exiting.\n","detab",&(*argv)[3] );
                    exit(2) ;
                }
            }
       } else if (strncmp(*argv,"--",2) == 0 ) {
           ++argv;
           --processed ;
           break;
       }
       --argc; 
     }
  }
    if (processed != argc ) {
        printf("there were errors on the command line\n") ;
        exit(2) ;
    }

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
    int c,col=1,newcol,nonwhi=0 ;
    do {
        newcol=col ;
        while ((c=getchar()) == BLANK ) {
            ++newcol;
            if (istabpos(newcol) && !nonwhi || ( nonwhi && !initial_only ) && newcol < LAST_TAB ) {
                putchar(TAB) ;
                col=newcol;
            }
        }
        while (col<newcol ) {
            putchar(BLANK);
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
        /* exit(0); */
    } while (c != EOF) ;
    return 0; 
}

void set_eventabs( int tab_sz )
{
    int i=tab_sz-1,j=0;            /* zerobazed array */
    while ( (i+=(tab_sz*j++))< MAXLINE ) {
        tabpos[i] = 1 ;
        LAST_TAB = max(LAST_TAB,(i+1)) ;
    }
}

void set_random_tab( int tab_sz ) 
{
        tabpos[tab_sz-1] = 1 ;
        LAST_TAB = max(LAST_TAB,tab_sz ) ;
}

