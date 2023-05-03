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

int tabpos[MAXLINE] ;
int LAST_TAB=0;
int main(int argc, char **argv)
{
    int TAP = TRUE; /* TAP: [Tab Arguments Processing] */
    int tabval=8, processed=argc, initial_only=FALSE;
    void set_eventabs( int tab_sz ) ;
    void set_random_tab( int tab_sz ) ;
    void show_help();
    extern int errno ;
    
    printf("initial argc: %d\n",argc);
    while ( argc > 1 ) {
         /* printf("argc > 1\n"); */
       ++argv;
       --argc;
       if (!strcmp(*argv,"-i") || !strcmp(*argv,"--initial") ) {
            /* printf("do not convert tabs after non blanks\n") ; */
            initial_only=TRUE;
            --processed ;
            /* help  -h comes here */

        } else if (!strcmp(*argv,"-h")||!strcmp(*argv,"--help")) { 
            show_help() ; /* show help and exit.*/
        } else if (!strncmp(*argv,"-t=",3) || !strncmp(*argv,"--tabs=",7) ) {
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
                /* We have a regular -{t|tabval}= situation.
                 * Or we are getting a space separated list with arguments.
                 * And: we are keeping in line with regards to incrementing argc at the 
                 * top of the loop, without any hanky panky.
                 */

                tabval= atoi(&(*argv)[3]) ;
                if (tabval == 0 ) {
                    fprintf(stderr,"%s: Bad numeric argument for "
                            "\"-t=\" or \"-tabs=\" : \"%s\""
                            "\n...Exiting.\n","detab",&(*argv)[3] );
                    exit(2) ;
                } else if (argc <= 1 ) { /* no point in looking for more */

                    set_eventabs(tabval) ;
                   
                } else  { /* check to see if we got a list. */
                    int tmpval;
                    /* We look ahead, at this point we know we have an
                     * extra argument left */
                    if (tmpval=atoi(*(argv+1)) != 0 ) {
                       /* '+; or '/' prefix from here */
                       set_random_tab(tabval) ;
                       set_random_tab(tmpval );
                       /* we used the argument, so we need to adjust 
                        * the current argument, which is already processed
                        * and the argument counter */
                       --argc;
                       ++argv;
                        while ( argc > 1 && (tmpval=atoi(*(argv+1))) != 0 ) {
                            /* '+; or '/' prefix from here */
                           set_random_tab(tmpval) ;
                           --argc;    
                           ++argv;
                        }
                    } else {
                        set_eventabs(tabval) ;
                    }
                }
            }
        } else if (*argv[0] == '-' && (strpbrk((*argv)+1,"1234567890") == (*argv)+1 )) {
            /* TODO: settabs function that starts with the n offset */
            /* start of a "-m +n" construct */
            int m=atoi((*argv)+1);
            if (argc >1 ) {
                --argc ;
                ++argv ;
                if (*argv[0] == '+' && (strpbrk((*argv)+1,"1234567890") == (*argv)+1 )) {
                    int n = atoi((*argv)+1);
                    /* setting tabs as we should */
                } else {
                    fprintf(stderr,"detab: Incomplete \"-n +m\" construct, missing \"+m\" argument.\n");
                    exit(2) ;
                }
            } else {
                fprintf(stderr,"detab: Incomplete \"-n +m\" construct, missing \"+m\" argument.\n");
                exit(2) ;
            }
        } else if (!strcmp(*argv,"--")) {
           TAP=FALSE;
            ++argv ;
           --processed ;
           break;
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



 *
 *
 * */

  if (!tabs_been_set() ) 
      set_eventabs( tabval ) ;

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

