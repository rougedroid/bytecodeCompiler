#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

/*
Goal: 
Convert this program to bytecode and run it: 

i = 0;
input = 7;
if (input == 7){
i = 1;
}else {
i = 0;
}

for (i<5){
return(i);
i ++;
}


Program in test_prog

*/

typedef struct Token {
  char token[10];
  int length;
}token_t;

typedef struct TokenArray {
  int length;
  int capacity;
  token_t * tokenarray_ptr;
}token_array_t;

token_array_t * tokenizer(char * input_text){
  token_array_t * tokenarray = malloc(sizeof(token_array_t));
  char * token_str = strtok(input_text, "/");
  tokenarray->length = 0;
  tokenarray->capacity = 1;
  tokenarray->tokenarray_ptr = malloc(sizeof(token_t));
  token_t * token;
  while (token_str != NULL){
    if (tokenarray->length < tokenarray->capacity){
      token = (tokenarray->tokenarray_ptr + tokenarray->length);
      strcpy(token->token, token_str);
      tokenarray->length ++;
      token_str = strtok(NULL, "/");
      continue;
    }
    else {
      tokenarray->capacity *= 2;
      tokenarray->tokenarray_ptr=realloc(tokenarray->tokenarray_ptr, sizeof(sizeof(token_t)*tokenarray->capacity));     
      continue;
    }


  }
  return tokenarray;
  
  
}

char * load_code(char * program){
  // This is placeholder code. Put code to actually read from file. 
  strcpy(program, "i/=/0/;/input/=/7/;/if/(/input/==/7/)/{/i/=/1/;/}/else/{/i/=/0/;/}//for/(/i/</5/)/{/return/(/i/)/;/i/++/;/}/EOF");
   
  
  return program;
}




int main(){
  // Writing the test program in the form that will be loaded later. Note that the / separator is assigned for every place with a space or a \n character. I do not expect the user to use it. It will be added automatically when loaded by file loader. 
  // Also every semicolon and bracket is preceeded and followed by a / and a semicolon signals the end of a command. 
  // For the purposes of this demonstration only 2 variables are allowed initially atleast. input and i. 
  // EOF is added automatically at end of program. 
  token_array_t * tokenarray;  

  
  char * program = malloc(sizeof(char)*500);
  load_code(program);
  tokenarray = tokenizer(program);
  // For Debugging:
  for (int i = 0; i < (tokenarray->length); i++){
    //printf("Token Number: %d \n", i);
    //printf("Token String: %s \n", (tokenarray->tokenarray_ptr + i)->token);
    
    printf(" %s ", (tokenarray->tokenarray_ptr + i)->token);
    
  };
  
}
