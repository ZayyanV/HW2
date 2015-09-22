#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define num_of_threads 4

typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;


typedef struct threads {
int f;
double inf;
}threads_t;


pthread_mutex_t lock;



char *
make_word( char *word ) {
  return strcpy( malloc( strlen( word )+1 ), word );
}

dict_t *
make_dict(char *word) {
  dict_t *nd = (dict_t *) malloc( sizeof(dict_t) );
  nd->word = make_word( word );
  nd->count = 1;
  nd->next = NULL;
  return nd;
}

dict_t *final_dict;
dict_t *insert_word( dict_t *d, char *word ) {
  
  //   Insert word into dict or increment count if already there
  //   return pointer to the updated dict
  
  dict_t *nd;
  dict_t *pd = NULL;		// prior to insertion point 
  dict_t *di = d;		// following insertion point
  // Search down list to find if present or point of insertion
  while(di && ( strcmp(word, di->word ) >= 0) ) { 
    if( strcmp( word, di->word ) == 0 ) { 
      di->count++;		// increment count 
      return d;			// return head 
    }
    pd = di;			// advance ptr pair
    di = di->next;
  }
  nd = make_dict(word);		// not found, make entry 
  nd->next = di;		// entry bigger than word or tail 
  if (pd) {
    pd->next = nd;
    return d;			// insert beond head 
  }
  final_dict = nd;
  return nd;
}

void print_dict(dict_t *d) {
  while (d) {
    printf("[%d] %s\n", d->count, d->word);
    d = d->next;
  }
}

int
get_word( char *buf, int n, FILE *infile) {
  int inword = 0;
  int c;  
  while( (c = fgetc(infile)) != EOF ) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	// terminate the word string
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
  return 0;			// no more words
}

#define MAXWORD 1024
//

void *words(void *args)
{
	threads_t *word = (threads_t *) args;
	FILE *infile = word->f;
	
	pthread_mutex_lock(&lock);
	char wordbuf[MAXWORD];
	dict_t *data = NULL;
	
	
	while( get_word( wordbuf, MAXWORD, infile ) ) {
    data = insert_word(data, wordbuf); // add to dict
  }
  
  pthread_mutex_unlock(&lock);
  pthread_exit(NULL);
}

int
main( int argc, char *argv[] ) {
  dict_t *d = NULL;
  FILE *infile = stdin;
  if (argc >= 2) {
    infile = fopen (argv[1],"r");
  }
  if( !infile ) {
    printf("Unable to open %s\n",argv[1]);
    exit( EXIT_FAILURE );
  }
  
  pthread_t thrd[num_of_threads];
   
  threads_t threads[num_of_threads];
  
  pthread_mutex_init(&lock,NULL);
	int l;
	for (int j= 0; j < num_of_threads;++j)
	{
	threads[j].f = infile;
	threads[j].inf = (j+1) * num_of_threads;
	if((l = pthread_create(&thrd[j],NULL,words,&threads[j])))	{
		return EXIT_FAILURE;
	}
	}	

for (int k = 0;k < num_of_threads; ++k)
{
pthread_join(thrd[k],NULL);
}

  print_dict( final_dict );
  fclose( infile );
	return EXIT_SUCCESS;
}
