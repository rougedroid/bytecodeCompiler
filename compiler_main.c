#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>

#define punctuations ";:{}(),"
#define a_operators "+-//*="
#define keywords "if else return while" // 4 keywords. idc enough to deal with string arrays in C. 
#define c_operators "< > ==" // !->NOT | -> OR & -> AND ? -> make true. Basically it will make the next token true. even if its not a boolean and was supposed to give value error. If the next token is not a value, then ? inserts a token saying false. 
#define l_operators "? ! | & " // !->NOT | -> OR & -> AND ? -> make true. Basically it will make the next token true. even if its not a boolean and was supposed to give value error. If the next token is not a value, then ? inserts a token saying false. 
// ? is just a joke not intended to be used really, unless you want to confuse a bunch of people by using non bool values in a logical operation 
// intended use: 
// if (?10) -> 10 isn't a logical operator so if (10) would give error. but ?10 gives true.
// if (?) will give false cuz ) is not a value. 
// in future if you have a variable getting a value from a func, you can use ?var_from_func to see if it has a value or not. 
// i included it cuz it was mad funny. ;)

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
i = i + 1;
}


Program in test_prog

*/
// Rewrite code using while loop. 



// For data and variable assignment, we have address spaces:
// 0               x0000 to 0xFFF0. 
// so, we use the last bunch to make sure the program doesn't overwrite itself. 
// Or...... we could write the program with a placeholder address, and then at the end, we calculate how long our program is, and assign address spaces outside this length 
// second method is preffered cuz we need to maintain variable list with registers anyways cuz variables will be called later on also. 

typedef enum {
  VARIABLE = 10,
  KEYWORD = 20,
  VALUE = 11,
  A_OPERATOR = 21,
  L_OPERATOR = 22,
  C_OPERATOR = 23,
  PUNCTUATION = 30,
  EOF_Flag = 69,
}TokenTypes;

typedef struct Token {
  char token[10];
  int length;
  int type;
}token_t;

typedef struct TokenArray {
  int length;
  int capacity;
  token_t * tokenarray_ptr;
}token_array_t;


typedef struct ASTNode {
  int NodeType;
  struct ASTNode * left;
  struct ASTNode * right;
  union {
    int value;
    int op;
  }data;
}ASTNode_t;

typedef enum {
  NODE_VALUE = 10,
  NODE_OPERATOR = 11,
}Node_Types;

typedef struct ByteStream{
  int length;
  int capacity;
  uint16_t * binstream;
}ByteStream_t;

uint16_t translate(ASTNode_t * rootNode){ //, ByteStream_t * outputstream){

  uint16_t left_reg;
  uint16_t right_reg;
  uint16_t output_reg;
  int op;
  
  output_reg = last_reg - used_reg_rev;
  used_reg_rev+=2;

  printf("Value type. output reg, used reg: [%d] [%d] [%d] \n", (rootNode->NodeType),output_reg, used_reg_rev);
  if (rootNode->NodeType == NODE_VALUE){
    //return (rootNode->data).value;
    printf("WRITE_CONST_INT [%d] [%d] \n", output_reg, (rootNode->data).value);
    
    
    
  }else if(rootNode->NodeType == NODE_OPERATOR){
    left_reg = translate(rootNode->left);
    right_reg = translate(rootNode->right);
    op = (rootNode->data).op;
  }

  switch (op) {
    case OPR_ADD:
      uint16_t result_reg = output_reg;
      printf("OP_ADD [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF1] [%d] \n", output_reg);
    case OPR_SUB:
      printf("OP_SUB [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF3] [%d] \n", output_reg);
    case OPR_MUL:
      printf("OP_MUL [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF7] [%d] \n", output_reg);
    case OPR_DIV:
      printf("OP_DIV [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF9] [%d] \n", left_reg, right_reg);
    default:
     printf("OP_NONE \n"); 
  }
  return output_reg;

}



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
      tokenarray->tokenarray_ptr=realloc(tokenarray->tokenarray_ptr, (sizeof(token_t)*tokenarray->capacity));     
      continue;
    }
  }
  return tokenarray;
}



char * load_code(char * program){
  // This is placeholder code. Put code to actually read from file. 
  strcpy(program, "i/=/0/;/input/=/7/;/if/(/input/==/7/)/{/i/=/1/;/}/else/{/i/=/0/;/}//for/(/i/</5/)/{/return/(/i/)/;/i/=/i/+/1/;/}/EOF/(/(/(/(/)/)/)/)");
  
  return program;
}





// AI GENERATED FUNCTION
bool is_valid_int(const char *str, int *out_value) {
    // If the string is NULL or empty, it's not a valid int
    if (str == NULL || *str == '\0') {
        return false;
    }

    char *endptr;
    // strtol(string_to_convert, pointer_to_end_of_conversion, base)
    long val = strtol(str, &endptr, 10);

    // If endptr didn't move, no digits were found at all
    if (endptr == str) {
        return false;
    }

    // Check if there are trailing characters (ignoring trailing whitespace)
    while (*endptr != '\0') {
        if (!isspace((unsigned char)*endptr)) {
            return false; // Found a non-space character after the number (e.g., "123abc")
        }
        endptr++;
    }

    // Optional: Check for integer overflow/underflow if you need strictly an 'int'
    // (Since 'long' might be larger than 'int' depending on the system)
    if (val > 2147483647 || val < -2147483648) {
        return false; 
    }

    // If we passed all checks, save the value and return true
    //*out_value = (int)val;
    return true;
}



token_array_t * classifier(token_array_t * tokenarray){
  // Basically the entire AST arch here and rest in process() func. written in a bunch of if statements. 
  token_t * token_ptr;
  char token_text[10];

  for (int i=0; i < (tokenarray->length + 1); i++){
    token_ptr = tokenarray->tokenarray_ptr + i;
    strcpy(token_text, token_ptr->token);
//    printf("%d\n", i);
//  printf("IN for loop \n");
    if (strstr(keywords, token_text)!=NULL){
      token_ptr->type = KEYWORD;

    }else if (strstr(a_operators, token_text)!=NULL) {
      token_ptr->type = A_OPERATOR;
    }else if (strstr(punctuations, token_text )!=NULL){
      token_ptr->type = PUNCTUATION;
    }else if (strstr(l_operators,token_text)!=NULL) {
      token_ptr->type = L_OPERATOR;
    }else if (strstr(c_operators,token_text)!=NULL) {
      token_ptr->type = C_OPERATOR;
    }else if (is_valid_int(token_ptr->token, NULL)) {
      token_ptr->type = VALUE;
    }else if (strstr("EOF", token_text)!=NULL){
      token_ptr->type = EOF_Flag;
    }else{
      token_ptr->type = VARIABLE;
    }
  }
  return tokenarray;

}



int process(token_array_t * tokenarray) {
     
}

int find_deepest_brac(token_array_t * tokenarray){
  int deepest_brac = 0;
  int live_counter = 0;
  int highest = 0; 
  int highest_i = 0;
  token_t * root_token = tokenarray->tokenarray_ptr; 
  token_t * cur_token;
  for (int i = 0; i < tokenarray->length; i++){
    cur_token = root_token + i;
    if (cur_token->type ==  PUNCTUATION){
      if (strstr(cur_token->token,"(")){
        deepest_brac = i;
        live_counter++;
        if (live_counter > highest){
          highest = live_counter;
          highest_i = i;
        }
      }else if(strstr(cur_token->token,")")){
        live_counter--;
        
      }


    }
  };
  deepest_brac = highest_i; // Further additions for { need to be made
  return deepest_brac;

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
  classifier(tokenarray);
  for (int i = 0; i < (tokenarray->length); i++){
    //printf("Token Number: %d \n", i);
    //printf("Token String: %s \n", (tokenarray->tokenarray_ptr + i)->token);
    
    printf("tokan: %d : %s : %d \n",i, (tokenarray->tokenarray_ptr + i)->token, (tokenarray->tokenarray_ptr +i)->type);
    
  };

  printf("Deepest Brac: %d \n", find_deepest_brac(tokenarray));
  
}
