/*---------------------------------------------------------------------- 
 * formatter.c:
 *
 * compilation: 
 *	gcc -o formatter formatter.c
 * usage:
 *	./formatter <input-file> <output-file> <width>
 * 
 *-----------------------------------------------------------------------
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/*definitions*/
#define MAX_SIZE	4

typedef struct list_s {
	char *str;
	struct list_s *next;
	struct list_s *prev;
	int size;
	int num_spaces_after;
} list_t; 

/*prototypes*/
list_t *make_list( char * );
char *tokenizer( FILE *ifp);

void formatter( FILE *, FILE *, int );

void print( FILE *ofp, int width, list_t list);

int main( int argc, char *argv[] )
{
	FILE *ifp, *ofp;
	clock_t begin_time, end_time;
	double time_used;


	if (argc < 4) {
	  fprintf(stderr,"Not enough arguments\n");
	  exit(1);
	}
	if (!(ifp = fopen(argv[1],"r"))) {
	  fprintf(stderr,"Cannot open file %s\n",argv[1]);
	  exit(1);
	}
	if (!(ofp = fopen(argv[2],"w"))) {
	  fprintf(stderr,"Cannot open file %s\n",argv[2]);
	  exit(1);
	}

	begin_time = clock();
	formatter( ifp, ofp, atoi(argv[3]));
	end_time = clock();
	time_used = (double)(end_time - begin_time)/CLOCKS_PER_SEC;
	fprintf( stderr, "Time usage = %17.13f\n", time_used );

  	fclose(ifp);
  	fclose(ofp);

    return 0;
}

/*get word*/
char *tokenizer(FILE *ifp)
{
	char *buffer, *output;
	int i, size, c;

	while (1) {
		c = getc(ifp);
		if ( isspace(c) ) 
			continue;
		if ( c == EOF ) {
			fprintf( stderr, "[EOF]\n" );
			return NULL;
		}
		if ( isalnum(c) || ispunct(c) ) {
			size = MAX_SIZE;
			buffer = (char *)malloc(size * sizeof(char));
			i = 0;
			do { 
				if ( i >= size ) {
					size <<= 1;
					buffer = realloc( buffer, size * sizeof(char) );
				}
				buffer[i++] = c;
				c = getc(ifp);
			} 
			while ( isalnum(c) || ispunct(c) );
			ungetc( c, stdin );
			buffer[i] = '\0';
			output = strdup( buffer );
			free( buffer );
			return output;
		}
		else {
			fprintf( stderr, "Unexpected character: {%c}\n", c );
			exit(1);
		}
	}
}

/*list constructor*/
list_t *make_list( char *s )
{
	list_t *ptr;
	
	ptr = (list_t *)malloc( sizeof(list_t) );
	assert( ptr != NULL );
	ptr->num_spaces_after = 1;
	ptr->str = strdup( s );
	ptr->next = NULL;
	ptr->prev = NULL;

	return ptr;
}

/*formatter*/
void formatter( FILE *ifp, FILE *ofp, int width )
{
    char *word;
	list_t *top = NULL, *curr = NULL, *begin = NULL, *end = NULL;

	/*get first word*/
	word = tokenizer( ifp );
	while ( word != NULL ) {
		/*build linked list*/
		if ( top == NULL ) {
			begin = end = curr = top = make_list( word );
		}
		else {
			curr->next = make_list( word );
			curr->next->prev = curr;
			curr = curr->next;
		}

		/*get next word*/
		word = tokenizer( ifp );
	}

	curr = begin;
	int total_length = 0;

	while(curr!=NULL){
		/*finds the end of the line*/
		while( end->num_spaces_after!=0 ){
			if (curr != NULL){
				if( total_length + strlen(curr->str)+1 < width ){
					total_length += strlen(curr->str)+1;
					curr = curr->next;
				} else {
					end = curr->prev;
					end->num_spaces_after = 0;
					if(curr!=NULL)
						curr=begin;
						while(total_length<width){
							if(curr!=end){
								curr->num_spaces_after++;
								total_length++;
								curr = curr->next;
							} else {
								curr = begin;
							}
						}
						total_length=0;
					curr = end;
				}
			} else {
				break;
			}
		}

		if (curr != NULL){
			begin = end = curr->next;
			curr = begin;
		}
	}

	int total_length1 = 0;

	while(curr!=NULL){
		/*pads spaces til total_length = width*/
		while ( total_length <= width ){
			if ( curr != end && curr != NULL ){
				curr->num_spaces_after += 1;
				total_length += 1;
				curr = curr->next;
			} else {
				curr=begin;
			}
		}

	}

	//fprintf(ofp, "\n");

	curr = top;
	/*prints the linked list with # of spaces in between*/
	while( curr->next != NULL ){
		fprintf( ofp, "%s", curr->str );
		int i;
		if(curr->num_spaces_after == 0)
			fprintf( ofp, "%c", '\n' );
		for(i=0; i<curr->num_spaces_after; i++) {
			fprintf( ofp, "%c", ' ' );
		}
		curr = curr->next;
	}
	fprintf( ofp, "%s", curr->str);

}