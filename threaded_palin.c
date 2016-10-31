#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

char words[300000][50];


pthread_t pthreads;
/*master list of words*/
typedef struct _list_t_{

         char *string;

         struct list_t *next;

 } list_t;
/*declaring hash tables*/
 typedef struct _hash_table_t{
         int size;
         struct list_t **table;
         int threadnumber;

 }hash_table_t;
 /* hash table is a struct containing multiple linked lists*/


/*global hash table*/
hash_table_t * words_table; 

/* reversing string function*/
void revstr(char* str)
{
    int i;
    char cpstr[strlen(str)+1];
    for(i=0; i < strlen(str); i++)
    {
       /* //printf("%d\t%d\t", i, strlen(str)-i-1);*/
        cpstr[i] = str[strlen(str)-i-1];
    }
    cpstr[i] = '\0';

    memcpy(str, cpstr, strlen(cpstr)+1);

}
/* creating the hash table*/
hash_table_t *create_hash_table(int size){
         int i = 0;
          hash_table_t *word_table;
         if (size<1){
                 return NULL;
 }

        /*allocate memory for chosen size*/
         if ((word_table->table = (struct list_t*)malloc(sizeof(list_t *) * size)) == NULL){

                 return NULL;

         }
	 /*set all table pointers to NULL*/
         for (i=0; i<size; i++){
                 word_table->table[i] = NULL;


         }
		/* set size variable*/
         word_table-> size = size;

         return word_table;

 }
/*hash function*/
/*the hash function itself was taken from an online resource*/
 unsigned int hash (hash_table_t *hashtable, char *str){


/*each word will multiply the hashvalue by 31*/
		
		/*modulo the table size to make sure it'll fit*/
		
		/*should be relatively unique per string*/
         unsigned int hashval;

         hashval = 0;

         for (; *str!='\0'; str++){

                 hashval = *str + (hashval << 5) - hashval;
         }

         return hashval % hashtable-> size;
 }
/*function for string lookup*/
 list_t *lookup_string (hash_table_t *hashtable, char *str)
 {
	
         list_t *list;
         unsigned int hashval = hash(hashtable, str);

          for(list = hashtable->table[hashval]; list != NULL; list = list->next) {
        if (strcmp(str, list->string) == 0) return list;
    }
    return NULL;

 }
/*funstion for adding string*/
 int add_string(hash_table_t *hashtable, char *str){

         list_t *new_list;
         list_t *current_list;
         unsigned int hashval = hash(hashtable, str);
	/*malloc a new node*/
         if ((new_list = malloc(sizeof(list_t))) == NULL) {
                 return 1;

         }
	/*make sure it was insertred*/
         current_list = lookup_string(hashtable, str);
         if(current_list != NULL) {

		
                 return 2;

         }
         new_list->string =  strdup(str);
         new_list->next = hashtable->table[hashval];
         hashtable->table[hashval] = new_list;
	
         return 0;

 }

 

void *palindrome(void* data){


        /* check each string from the master hash table and the backwards list*/

       
       /*each thread will enter the program here*/
	int threadnumber = *((int*)data); 
	char b[50];
	int i=0;
    list_t *lookup;
        
        for (i=(threadnumber)*235866/20;i<((threadnumber+1)*235866/20);i++){
		
                memcpy(b, words[i], strlen(words[i])+1);
		
                revstr(b);
		
	
		
		int result = strncmp (b, words[i],strlen(b));
	
                if (result == 0)
                {

                        printf("%s\n", words[i]);

                }

                else{
		
		lookup= lookup_string(words_table, b);
		if (lookup!=NULL){
		
		
				printf("%s\n" , words[i]);
		}
                        
                }

        }

return NULL;
}

void  readfile(hash_table_t* words_table){

	printf("let's try this\n");
	
		/*read ffrom stdin and parse into the hash table*/
     char line[80];
	 int count = 0;
    
        
        while (fgets(line, 80, stdin) != NULL)
        {
               
		int len = strlen(line);
		if( line[len-1] == '\n' ){
    		line[len-1] = '\0';
		}
				add_string(words_table, line);
                strcpy(words[count], line);
                count ++;

        }
	printf("Loop exited\n");

}

int main ()
{
	pthread_t pthreads[20];
		int threads= 0;
		

      	words_table = create_hash_table(10000);
 

          readfile(words_table);
                
         for (threads=0 ; threads<20 ; threads++){
			 
        
		words_table->threadnumber = threads;
				
		pthread_create(&pthreads[threads], NULL, palindrome, &threads);
		
		pthread_join(pthreads[threads], NULL);
		
	}
	
	
				
                
                

return 0;
}
