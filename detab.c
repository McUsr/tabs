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
#define SPACE ' '
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
int tabpos[MAXLINE] ;
int LAST_TAB=0;
int main(int argc, char **argv)
{
    int tabval=8, processed=argc, initial_only=FALSE;
    void set_eventabs( int tab_sz ) ;
    void set_random_tab( int tab_sz ) ;
    extern int errno ;
    
   /* if (argc == 1 || (argv[1][0] == '-' && argv[1][1] == '-' && argv[1][2] == '\0' ) ) { */
   if (argc == 1  ) {
       /* rewrite */
      printf("setting tabs ourselves, 8 spaces per tab. \n") ;
      set_eventabs( tabval ) ;
  else if
  } else {
      /* We need to process arguments for tabs from the command line. */
      /* -i --initial : do not convert tabs after non blanks */
      /* -t --tabs=N have tabs N characgters apart, not 8 */
      /* -t --tabs=LIST USE comma separated list of tab position. last can be prefixed with
       * '/' to specify a a tab size to use after the last explicitly specified tab stop.
       * Also a prefix of + can be used to align remaining tabstops relative to the last 
       * specified tab stop instead of the first column.
       *
       * 
       * siste vi gjør er å sjekke for pluss, sette ifra siste tabstop.!
       * Her kan vi ha vaar egen spesial utgave av atoi, med en pluss vi kan teste på,
       * samt noe med maskineriet.
       *
       * når vi har plussen, kan vi også gjøre oppgave 5.12 som er -m +n pattern.
       * -m +n betyr: tabstops every n columns starting at column n.
       *
       *  fra n'te posisjon +n stop
       *
       * Jeg husker ikke helt hva '/betyr'. 
       */
      printf("initial argc: %d\n",argc);
    while ( argc > 1 ) {
        /* printf("argc > 1\n"); */
        ++argv;
        --argc;
        if (strcmp(*argv,"-i") == 0 ) {
            /* printf("do not convert tabs after non blanks\n") ; */
            initial_only=TRUE;
            --processed ;
            /* help  -h comes here */
       } else if (strncmp(*argv,"-t=",3) == 0 ) {
            printf("About to set tab positions.\n") ;
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
                        --argv ;
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
            ++argv ;
           --processed ;
           break;
       }
     }
  }
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

