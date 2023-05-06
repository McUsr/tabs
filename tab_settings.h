#define MAXLINE 1000
#define exit(n) _exit(n)
extern int tabpos[MAXLINE] ;
extern int LAST_TAB;
void abort_list_arg_error( char *bad_arg) ;
void abort_bad_num_arg( char *bad_arg) ;
void add_tabs( int tabpos, char prefix);
int xtatou(char s[], char *prefix) ;
int tabs_been_set() ;
void set_eventabs( int tab_sz ) ;
void set_random_tab( int tab_sz ) ;
void set_mPlusn_tabs( int m, int n) ;
void print_tab_positions() ;
