#include <string.h>
#include <stdlib.h>
#include <stdio.h>
char words[300000][50];
/*master list of words*/
typedef struct _list_t_{

         char *string;

         struct list_t *next;

 } list_t;
/*declaring hash tables*/
 typedef struct _hash_table_t{
         int size;
         list_t **table;

 }hash_table_t;
/* hash table is a struct containing multiple linked lists*/


/* reversing string function*/
void revstr(char* str)
{
    int i;
    char cpstr[strlen(str)+1];
    for(i=0; i < strlen(str); i++)
    {
      
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
         if ((word_table->table = (list_t*)malloc(sizeof(list_t *) * size)) == NULL){

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

         unsigned int hashval;

         hashval = 0;
		/*each word will multiply the hashvalue by 31*/
		
		/*modulo the table size to make sure it'll fit*/
		
		/*should be relatively unique per string*/
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
		/*set dependencies*/
         return 0;

 }
/* free table functions causes stack errors
 void free_table(hash_table_t *hashtable){

         int i;
         list_t *list, *temp;
	printf("Freeing table\n");

         if (hashtable==NULL) {
			printf("Freeing table\n");
                 return;
         }

         for (i=0; i<hashtable->size;i++)
         {
	
                 list = hashtable->table[i];
              cpstr   while(list!=NULL) {
			
                         temp = list;
                         list = list->next;
			
                         free(temp->string);
                         free(temp);
                 }
         }
		printf("Freeing table2\n");
         free(hashtable->table);
         free(hashtable);
 }

*/

void palindrome(hash_table_t *words_table){


        /* check each string from the master hash table and the backwards list*/

	char b[50];
	int i=0;
         list_t *lookup;
        for (i=0;i<235886;i++){
		
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
		printf("%s\n" , words[i]); /*print if found*/
		
		}
                        
                }

        }
printf("Done\n");

}

void  readfile(hash_table_t* words_table){

/*read ffrom stdin and parse into the hash table*/
	printf("let's try this\n");
	
		
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
        palindrome(words_table);
	



}

int main ()
{
 
		printf("why is the happening\n");

      	 hash_table_t *words_table = create_hash_table(10000);

               readfile(words_table);


	return EXIT_SUCCESS;
}
