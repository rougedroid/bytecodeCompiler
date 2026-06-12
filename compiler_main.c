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

typedef enum {
  OPR_ADD = 10,
  OPR_SUB = 11,
  OPR_MUL = 12,
  OPR_DIV = 13,
}Operations;

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

typedef enum{
  WRITE_CONST_INT = 0x0020, // OPCODE [REG] [Value] Value is 16 bits so 15 bits of signed int. INT Range = -32768 to +32768.
  OP_NONE  = 0x0000, // SKIP
  OP_RETURN = 0x0022, // OPCODE [REG] Print out [REG]
  OP_ADD = 0x0024, // OPCODE [REG 1] [REG 2] -->Store in default regs/stack. --> default addr SMTN. then do 
  OP_LOAD_REG = 0x0025, // OPCODE [Source REG] [Destination REG]
  LOAD_REG = 0x0023, // OPCODE [REG] 
  OP_SUB = 0x0026, // OPCODE [REG 1] [REG 2] --> again put in default reg. have 1 bit for carry. 
  OP_JUMP = 0x0027, // OPCODE [REG]
  OP_CMP = 0x0028, // OPCODE [REG1] [REG2] [JMP1] [JMP2] --> compares if both values in REG1 and REG2 are same. If true, it jumps toJMP1 pointer. If false it jumps to JMP2 pointer. 
  OP_JMP_RELP = 0x0029, // OPCODE [VALUE] --> Goes forward by value number of bytes. (value must be even number) 
  OP_JMP_RELN = 0x0030, // OPCODE [VALUE] --> Goes back by value number of bytes. (value must be even)
  OP_CMP_JMP = 0x0031, // OPCODE [REG1] [REG2] [JMP] --> jumps JMP number of bytes forward. ( JMP must be even )
  OP_MUL = 0X0032, // OPCODE [REG1] [REG2] --> Stored in 0xFFF7 and 0xFFF8
  OP_DIV = 0x0033, // OPCODE [REG1] [REG2] --> Store quotent in 0xFFF9 and 0xFFFA ; Remainder in 0xFFFB and 0xFFFC;

}Opcodes;

// Forward Declarations
ASTNode_t *parse_expression(void);
ASTNode_t *parse_term(void);
ASTNode_t *parse_primary(void);


int used_reg_rev=0;
uint16_t last_reg = 0xFFF0;

ByteStream_t * output_stream;


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
  
  int offset;
  uint16_t * write_ptr;
  if ((output_stream->capacity-output_stream->length)<=8){
    output_stream->capacity *= 2;
    output_stream->binstream = realloc(output_stream->binstream, sizeof(uint16_t) * output_stream->capacity);

  }
  
  uint16_t instruction_set[6];
  switch (op) {
    case OPR_ADD:
      uint16_t result_reg = output_reg;
      printf("OP_ADD [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF1] [%d] \n", output_reg);
      uint16_t instruction_set_1[6] = {OP_ADD, left_reg, right_reg, OP_LOAD_REG, 0xFFF1, output_reg};
      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;
      memcpy(write_ptr, instruction_set_1, sizeof(instruction_set_1));
      
      
    case OPR_SUB:
      printf("OP_SUB [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF3] [%d] \n", output_reg);
      uint16_t instruction_set_2[6] = {OP_SUB, left_reg, right_reg, OP_LOAD_REG, 0xFFF3, output_reg};
      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;
      memcpy(write_ptr, instruction_set_2, sizeof(instruction_set_2));
    case OPR_MUL:
      printf("OP_MUL [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF7] [%d] \n", output_reg);
      uint16_t instruction_set_3[6] = {OP_MUL, left_reg, right_reg, OP_LOAD_REG, 0xFFF7, output_reg};
      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;
      memcpy(write_ptr, instruction_set_3, sizeof(instruction_set_3));
    case OPR_DIV:
      printf("OP_DIV [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF9] [%d] \n", left_reg, right_reg);
      uint16_t instruction_set_4[6] = {OP_DIV, left_reg, right_reg, OP_LOAD_REG, 0xFFF9, output_reg};
      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;
      memcpy(write_ptr, instruction_set_4, sizeof(instruction_set_4));
    default:
      printf("OP_NONE \n"); 
      uint16_t instruction_set_5[1] = {OP_NONE};
      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;
      memcpy(write_ptr, instruction_set_5, sizeof(instruction_set_5));
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
//  strcpy(program, "i/=/0/;/input/=/7/;/if/(/input/==/7/)/{/i/=/1/;/}/else/{/i/=/0/;/}//for/(/i/</5/)/{/return/(/i/)/;/i/=/i/+/1/;/}/EOF/(/(/(/(/)/)/)/)");
  strcpy(program, "1/+/3/+/5/+/(/6/+/7/)/");
  return program;
}





// AI GENERATED FUNCTION
bool is_valid_int(const char *str) {
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
    }else if (is_valid_int(token_ptr->token)) {
      token_ptr->type = VALUE;
    }else if (strstr("EOF", token_text)!=NULL){
      token_ptr->type = EOF_Flag;
    }else{
      token_ptr->type = VARIABLE;
    }
  }
  return tokenarray;

}

token_array_t * tokenarray;  
int read_pointer =0;

ASTNode_t * ASTNodePool;
int NodePool_Length = 0;

token_t * peek(){
  return (tokenarray->tokenarray_ptr+read_pointer);
}


ASTNode_t * parse_primary(){
  token_t * cur_token;
  cur_token = tokenarray->tokenarray_ptr + read_pointer;
  
  ASTNode_t * outputNode = malloc (sizeof(ASTNode_t));

  if (cur_token->type == VALUE){
    outputNode->left = NULL;
    outputNode->right = NULL;
    outputNode->NodeType = NODE_VALUE;
    (outputNode->data).value = (int) strtol(cur_token->token, NULL, 10);
    read_pointer++;
    return outputNode;
  }else{
    ;
  }

}

ASTNode_t * parse_term(){
  ASTNode_t * nextnode = malloc(sizeof(ASTNode_t));
  nextnode = parse_primary();
  if (strstr("/*",(tokenarray->tokenarray_ptr+read_pointer)->token)){
    read_pointer++;
    ASTNode_t * newNode = ASTNodePool + NodePool_Length;
    NodePool_Length++;
    newNode->left = nextnode;
    newNode->right = parse_term();
    newNode->NodeType = NODE_OPERATOR;
    if (strstr("/",(tokenarray->tokenarray_ptr+read_pointer-2)->token)){
      (newNode->data).op = OPR_DIV;
    } else {
      newNode->data.op = OPR_MUL;
    }
    return newNode;

  }else {
    return nextnode;
  }
}
ASTNode_t * parse_expression(){
  ASTNode_t * leftnode = parse_primary();
  if (strstr("+-",(tokenarray->tokenarray_ptr + read_pointer)->token)){
    ASTNode_t * newNode = ASTNodePool + NodePool_Length;
    newNode->left = leftnode;
    newNode->right = parse_term();
    newNode->NodeType = NODE_OPERATOR;

    if (strstr("+", (tokenarray->tokenarray_ptr+read_pointer -1)->token)){
      newNode->data.op = OPR_ADD;
    }else{
      newNode->data.op = OPR_SUB;
    }
    return newNode;
  
  }else {
    read_pointer--;

    return parse_term();

  }
  
}

void print_ast(ASTNode_t *node, int level) {
    if (node == NULL) return;

    // Print indentation based on current tree depth
    for (int i = 0; i < level; i++) {
        printf("    "); // 4 spaces per depth level
    }

    // Print node data depending on its type
    if (node->NodeType == NODE_VALUE) {
        printf("[VAL: %d]\n", node->data.value);
    } else if (node->NodeType == NODE_OPERATOR) {
        // Mapping your operation enums to printable characters
        char op_char = '?';
        switch (node->data.op) {
            case OP_ADD: op_char = '+'; break;
            case OP_SUB: op_char = '-'; break;
            case OP_MUL: op_char = '*'; break;
            case OP_DIV: op_char = '/'; break;
        }
        printf("[OP: %c]\n", op_char);
    }

    // Recursively print children, stepping up the indentation level
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

int main(){
  // Writing the test program in the form that will be loaded later. Note that the / separator is assigned for every place with a space or a \n character. I do not expect the user to use it. It will be added automatically when loaded by file loader. 
  // Also every semicolon and bracket is preceeded and followed by a / and a semicolon signals the end of a command. 
  // For the purposes of this demonstration only 2 variables are allowed initially atleast. input and i. 
  // EOF is added automatically at end of program. 
  ASTNodePool = malloc(sizeof(ASTNode_t)*4096); 
  output_stream = malloc(sizeof(ByteStream_t));
  output_stream->length = 0;
  output_stream->capacity = 10;
  output_stream->binstream = malloc(sizeof(uint16_t)*10);
  
  char * program = malloc(sizeof(char)*500);
  load_code(program);
  tokenarray = tokenizer(program);
  // For Debugging:
  classifier(tokenarray);
  /*
  for (int i = 0; i < (tokenarray->length); i++){
    //printf("Token Number: %d \n", i);
    //printf("Token String: %s \n", (tokenarray->tokenarray_ptr + i)->token);
    
    printf("token: %d : %s : %d \n",i, (tokenarray->tokenarray_ptr + i)->token, (tokenarray->tokenarray_ptr +i)->type);
    
  };*/

//  printf("Deepest Brac: %d \n", find_deepest_brac(tokenarray));
//  translate(parse_expression());
  print_ast(parse_expression(), 0);
  printf("%d \n", output_stream->binstream[1]);
}
