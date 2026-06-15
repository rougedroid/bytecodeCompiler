#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>
#include "program_formatter.h"

#define punctuations ";:{}(),"
#define a_operators "+-/+*="
#define keywords "if else return while" // 4 keywords. idc enough to deal with string arrays in C.
#define c_operators "< > =="            // !->NOT | -> OR & -> AND ? -> make true. Basically it will make the next token true. even if its not a boolean and was supposed to give value error. If the next token is not a value, then ? inserts a token saying false.
#define l_operators "? ! | & "          // !->NOT | -> OR & -> AND ? -> make true. Basically it will make the next token true. even if its not a boolean and was supposed to give value error. If the next token is not a value, then ? inserts a token saying false.

typedef enum
{
  VARIABLE = 10,
  KEYWORD = 20,
  VALUE = 11,
  A_OPERATOR = 21,
  L_OPERATOR = 22,
  C_OPERATOR = 23,
  PUNCTUATION = 30,
  VALUE_REG = 31,

  EOF_Flag = 69,
} TokenTypes;

typedef struct Variable
{
  char name[10];
  uint16_t reg;

} variable_t;

typedef struct VariablePool
{
  int length;
  int capacity;
  variable_t *varptr;

} var_pool_t;

typedef struct Token
{
  char token[10];
  int length;
  int type;
  uint16_t reg;
} token_t;

typedef struct TokenArray
{
  int length;
  int capacity;
  token_t *tokenarray_ptr;
} token_array_t;

typedef enum
{
  OPR_ADD = 10,
  OPR_SUB = 11,
  OPR_MUL = 12,
  OPR_DIV = 13,
} Operations;

typedef struct ASTNode
{
  int NodeType;
  struct ASTNode *left;
  struct ASTNode *right;
  union
  {
    int value;
    int op;
    uint16_t reg;
  } data;
} ASTNode_t;

typedef enum
{
  NODE_VALUE = 10,
  NODE_OPERATOR = 11,
  NODE_VALUE_REG = 12,
} Node_Types;

typedef struct ByteStream
{
  int length;
  int capacity;
  uint16_t *binstream;
} ByteStream_t;

typedef enum
{
  WRITE_CONST_INT = 0x0020, // OPCODE [REG] [Value] Value is 16 bits so 15 bits of signed int. INT Range = -32768 to +32768.
  OP_NONE = 0x0000,         // SKIP
  OP_RETURN = 0x0022,       // OPCODE [REG] Print out [REG]
  OP_ADD = 0x0024,          // OPCODE [REG 1] [REG 2] -->Store in default regs/stack. --> default addr SMTN. then do
  OP_LOAD_REG = 0x0025,     // OPCODE [Source REG] [Destination REG]
  LOAD_REG = 0x0023,        // OPCODE [REG]
  OP_SUB = 0x0026,          // OPCODE [REG 1] [REG 2] --> again put in default reg. have 1 bit for carry.
  OP_JUMP = 0x0027,         // OPCODE [REG]
  OP_CMP = 0x0028,          // OPCODE [REG1] [REG2] [JMP1] [JMP2] --> compares if both values in REG1 and REG2 are same. If true, it jumps toJMP1 pointer. If false it jumps to JMP2 pointer.
  OP_JMP_RELP = 0x0029,     // OPCODE [VALUE] --> Goes forward by value number of bytes. (value must be even number)
  OP_JMP_RELN = 0x0030,     // OPCODE [VALUE] --> Goes back by value number of bytes. (value must be even)
  OP_CMP_JMP = 0x0031,      // OPCODE [REG1] [REG2] [JMP] --> jumps JMP number of bytes forward. ( JMP must be even )
  OP_MUL = 0X0032,          // OPCODE [REG1] [REG2] --> Stored in 0xFFF7 and 0xFFF8
  OP_DIV = 0x0033,          // OPCODE [REG1] [REG2] --> Store quotent in 0xFFF9 and 0xFFFA ; Remainder in 0xFFFB and 0xFFFC;
  OP_CMP_GTR = 0x0034,      // OPCODE [REG1] [REG2] [JMP1] [JMP2] --> compares values. true if reg1>reg2. If true, it jumps toJMP1 pointer. If false it jumps to JMP2 pointer.
  OP_CMP_LSR = 0x0035,      // OPCODE [REG1] [REG2] [JMP1] [JMP2] --> compares values. true if reg1<reg2. If true, it jumps toJMP1 pointer. If false it jumps to JMP2 pointer.
  OP_CMP_GTR_JMP = 0x0036,  // OPCODE [REG1] [REG2] [JMP1] [JMP2] --> compares values. true if reg1>reg2. If true, it jumps toJMP1 pointer. If false it jumps to JMP2 pointer.
  OP_CMP_LSR_JMP = 0x0037,  // OPCODE [REG1] [REG2] [JMP1] [JMP2] --> compares values. true if reg1<reg2. If true, it jumps toJMP1 pointer. If false it jumps to JMP2 pointer.

} Opcodes;

typedef struct Logical_Result
{
  uint16_t Opcode;
  uint16_t reg_left;
  uint16_t reg_right;
} logical_result_t;

// Forward Declarations
ASTNode_t *parse_expression(void);
ASTNode_t *parse_term(void);
ASTNode_t *parse_primary(void);
void parse_program(void);
char* readFile(const char *filename); 
char* formatProgramString(const char *rawContent);
int writeBinaryStream(const char *filename, const uint16_t *data, size_t count);

void print_ast(ASTNode_t *, int);

int used_reg_rev = 0;
uint16_t last_reg = 0xFFF0;
var_pool_t *varpool;

ByteStream_t *output_stream;

uint16_t translate(ASTNode_t *rootNode)
{ //, ByteStream_t * outputstream){

  uint16_t left_reg;
  uint16_t right_reg;
  uint16_t output_reg;
  int op;

  output_reg = last_reg - used_reg_rev;
  used_reg_rev += 2;


  if (rootNode->NodeType == NODE_VALUE)
  {
    printf("WRITE_CONST_INT [%d] [%d] \n", output_reg, (rootNode->data).value);
  }
  else if (rootNode->NodeType == NODE_VALUE_REG)
  {
    output_reg = (rootNode->data).reg;
  }
  else if (rootNode->NodeType == NODE_OPERATOR)
  {
    left_reg = translate(rootNode->left);
    right_reg = translate(rootNode->right);
    op = (rootNode->data).op;
    int offset;
    uint16_t *write_ptr;
    if ((output_stream->capacity - output_stream->length) <= 8)
    {
      output_stream->capacity *= 2;
      output_stream->binstream = realloc(output_stream->binstream, sizeof(uint16_t) * output_stream->capacity);
    }

    uint16_t instruction_set[6];
    switch (op)
    {
    case OPR_ADD:
      uint16_t result_reg = output_reg;
      printf("OP_ADD [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF1] [%d] \n", output_reg);
      uint16_t instruction_set_1[6] = {OP_ADD, left_reg, right_reg, OP_LOAD_REG, 0xFFF1, output_reg};

      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;

      memcpy(write_ptr, instruction_set_1, sizeof(instruction_set_1));
      output_stream->length += 6;

      break;

    case OPR_SUB:
      printf("OP_SUB [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF3] [%d] \n", output_reg);
      uint16_t instruction_set_2[6] = {OP_SUB, left_reg, right_reg, OP_LOAD_REG, 0xFFF3, output_reg};
      
      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;

      memcpy(write_ptr, instruction_set_2, sizeof(instruction_set_2));
      output_stream->length+=6;

      break;
    case OPR_MUL:
      printf("OP_MUL [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF7] [%d] \n", output_reg);
      uint16_t instruction_set_3[6] = {OP_MUL, left_reg, right_reg, OP_LOAD_REG, 0xFFF7, output_reg};
      
      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;

      memcpy(write_ptr, instruction_set_3, sizeof(instruction_set_3));
      output_stream += 6;
      break;
    case OPR_DIV:
      printf("OP_DIV [%d] [%d] \n", left_reg, right_reg);
      printf("OP_LOAD_REG [0xFFF9] [%d] \n", left_reg, right_reg);
      uint16_t instruction_set_4[6] = {OP_DIV, left_reg, right_reg, OP_LOAD_REG, 0xFFF9, output_reg};
      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;
      memcpy(write_ptr, instruction_set_4, sizeof(instruction_set_4));
      output_stream->length += 6;
      break;
    default:
      printf("OP_NONE \n");
      uint16_t instruction_set_5[1] = {OP_NONE};
      offset = output_stream->length;
      write_ptr = output_stream->binstream + offset;
      memcpy(write_ptr, instruction_set_5, sizeof(instruction_set_5));
      output_stream->length++;
    }
  }

  return output_reg;
}

token_array_t *tokenizer(char *input_text)
{
  token_array_t *tokenarray = malloc(sizeof(token_array_t));
  char *token_str = strtok(input_text, " ");
  tokenarray->length = 0;
  tokenarray->capacity = 1;
  tokenarray->tokenarray_ptr = malloc(sizeof(token_t));
  token_t *token;
  while (token_str != NULL)
  {
    if (tokenarray->length < tokenarray->capacity)
    {
      token = (tokenarray->tokenarray_ptr + tokenarray->length);
      strcpy(token->token, token_str);
      tokenarray->length++;
      token_str = strtok(NULL, " ");
      continue;
    }
    else
    {
      tokenarray->capacity *= 2;
      tokenarray->tokenarray_ptr = realloc(tokenarray->tokenarray_ptr, (sizeof(token_t) * tokenarray->capacity));
      continue;
    }
  }
  return tokenarray;
}

int writeBinaryStream(const char *filename, const uint16_t *data, size_t count) {
    // 1. Open the file in "wb" mode (Write Binary)
    // The 'b' flag is crucial—it prevents the OS from changing newline characters!
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Error: Could not open file '%s' for writing.\n", filename);
        return 0;
    }

    /* 2. Write the buffer to disk
     * fwrite arguments:
     * - data:   pointer to the start of your memory block
     * - size:   size of a single element (sizeof(uint16_t) is 2 bytes)
     * - count:  how many elements to write
     * - file:   the file pointer
     */
    size_t written = fwrite(data, sizeof(uint16_t), count, file);

    // 3. Close the file to flush the stream to disk
    fclose(file);

    // Verify if everything was written successfully
    if (written != count) {
        printf("Error: Only wrote %zu of %zu elements.\n", written, count);
        return 0;
    }

    return 1;
}
/*
void load_code(char *program)
{
  char *raw = readFile("program.bsp");
  if (!raw) return;

  program = formatProgramString(raw);
  if (!program) {
    free(raw);
    return;
  }
  free(raw);
  

  
}
*/
char * load_code(){
  printf("[DEBUG] Attempting to read program.bso...\n");

  char *raw = readFile("program.bsp");
  if (raw == NULL) {
    printf("[DEBUG] ERROR: readFile returned NULL! Is the file in the right folder?\n");
    return NULL; // or whatever your exit strategy is
  }

  printf("[DEBUG] Raw file read successfully. Length: %zu chars.\n", strlen(raw));
  printf("[DEBUG] Raw content is: '%s'\n", raw);

  char *program = formatProgramString(raw);
  if (program == NULL) {
      printf("[DEBUG] ERROR: formatProgramString returned NULL!\n");
      free(raw);
      return program;
  }

  printf("[DEBUG] SUCCESS! Formatted string is:\n%s\n", program);
  return program;
}

bool is_valid_int(const char *str)
{
  if (str == NULL || *str == '\0')
  {
    return false;
  }

  char *endptr;
  long val = strtol(str, &endptr, 10);

  if (endptr == str)
  {
    return false;
  }

  while (*endptr != '\0')
  {
    if (!isspace((unsigned char)*endptr))
    {
      return false;
    }
    endptr++;
  }

  if (val > 2147483647 || val < -2147483648)
  {
    return false;
  }

  return true;
}

token_array_t *classifier(token_array_t *tokenarray)
{
  token_t *token_ptr;
  char token_text[10];

  for (int i = 0; i < tokenarray->length; i++)
  {
    token_ptr = tokenarray->tokenarray_ptr + i;
    strcpy(token_text, token_ptr->token);
    if (strstr(keywords, token_text) != NULL)
    {
      token_ptr->type = KEYWORD;
    }
    else if (strstr(a_operators, token_text) != NULL)
    {
      token_ptr->type = A_OPERATOR;
    }
    else if (strstr(punctuations, token_text) != NULL)
    {
      token_ptr->type = PUNCTUATION;
    }
    else if (strstr(l_operators, token_text) != NULL)
    {
      token_ptr->type = L_OPERATOR;
    }
    else if (strstr(c_operators, token_text) != NULL)
    {
      token_ptr->type = C_OPERATOR;
    }
    else if (is_valid_int(token_ptr->token))
    {
      token_ptr->type = VALUE;
    }
    else if (strstr("EOF", token_text) != NULL)
    {
      token_ptr->type = EOF_Flag;
    }
    else
    {
      token_ptr->type = VARIABLE;
    }
  }
  return tokenarray;
}

token_array_t *tokenarray;
int read_pointer = 0;

ASTNode_t *ASTNodePool;
int NodePool_Length = 0;

token_t *peek()
{
  return (tokenarray->tokenarray_ptr + read_pointer);
}

ASTNode_t *parse_primary()
{
  token_t *cur_token;
  cur_token = tokenarray->tokenarray_ptr + read_pointer;

  ASTNode_t *outputNode = ASTNodePool + NodePool_Length;
  NodePool_Length++;

  if (cur_token->type == VALUE)
  {
    outputNode->left = NULL;
    outputNode->right = NULL;
    outputNode->NodeType = NODE_VALUE;
    (outputNode->data).value = (int)strtol(cur_token->token, NULL, 10);
    read_pointer++;
    return outputNode;
  }
  else if (strstr("()", cur_token->token))
  {
    if (strstr("(", cur_token->token))
    {
      read_pointer++;
      outputNode = parse_expression();

      cur_token = tokenarray->tokenarray_ptr + read_pointer;
      if (strstr(")", cur_token->token))
      {
        read_pointer++;
        return outputNode;
      }
    }
  }
  else if (cur_token->type == VALUE_REG)
  {
    outputNode->left = NULL;
    outputNode->right = NULL;
    outputNode->NodeType = NODE_VALUE_REG;
    (outputNode->data).reg = cur_token->reg;
    read_pointer++;
    return outputNode;
  }
  else if (cur_token->type == VARIABLE)
  {
    char varname[10];
    strcpy(varname, cur_token->token);
    variable_t *new_var;
    read_pointer++;
    int var_index = -1;
    for (int i = 0; i < varpool->length; i++)
    {
      variable_t *var = varpool->varptr + i;
      if (strcmp(var->name, varname) == 0)
      {
        var_index = i;
        break;
      }
    }
    if (var_index == -1)
    {
      printf("INVALID VARIABLE. \n");
    }
    else
    {
      ASTNode_t *oNode = malloc(sizeof(ASTNode_t));
      oNode->left = NULL;
      oNode->right = NULL;
      oNode->NodeType = NODE_VALUE_REG;
      (oNode->data).reg = (varpool->varptr + var_index)->reg;
      return oNode;
    }
  }
  else
  {
    printf("RANDOM SHIT IN PARSE PRIMARY. READ POINTER: [%d] \n", read_pointer);
    printf("%s\n", cur_token->token);
    return NULL;
  }
}

ASTNode_t *parse_term()
{
  ASTNode_t *nextnode = parse_primary();
  while (strstr("*/", (tokenarray->tokenarray_ptr + read_pointer)->token))
  {
    read_pointer++;
    ASTNode_t *newNode = ASTNodePool + NodePool_Length;
    NodePool_Length++;
    newNode->left = nextnode;

    if (strstr("/", (tokenarray->tokenarray_ptr + read_pointer - 1)->token))
    {
      (newNode->data).op = OPR_DIV;
    }
    else
    {
      newNode->data.op = OPR_MUL;
    }
    newNode->right = parse_expression();
    newNode->NodeType = NODE_OPERATOR;
    nextnode = newNode;
  }

  return nextnode;
}
ASTNode_t *parse_expression()
{

  ASTNode_t *leftnode = parse_term();
  char *current_token = (tokenarray->tokenarray_ptr + read_pointer)->token;
  while (current_token != NULL && strcmp(current_token, ";") != 0 && strstr("+-", current_token))
  {
    ASTNode_t *newNode = ASTNodePool + NodePool_Length;
    NodePool_Length++;
    if (strstr("+", (tokenarray->tokenarray_ptr + read_pointer)->token))
    {
      newNode->data.op = OPR_ADD;
    }
    else
    {
      newNode->data.op = OPR_SUB;
    }
    newNode->left = leftnode;
    read_pointer++;
    newNode->right = parse_term();

    newNode->NodeType = NODE_OPERATOR;

    leftnode = newNode;
    current_token = (tokenarray->tokenarray_ptr + read_pointer)->token;
  }
  return leftnode;
}


logical_result_t *parse_logic()
{
  ASTNode_t *left_node = parse_expression();
  token_t *cur_token = tokenarray->tokenarray_ptr + read_pointer;
  logical_result_t *result = malloc(sizeof(logical_result_t));
  if (strstr(cur_token->token, ";"))
  {
    read_pointer++;
    cur_token = tokenarray->tokenarray_ptr + read_pointer;
    read_pointer++;
    if (strstr(cur_token->token, "=="))
    {
      result->Opcode = OP_CMP_JMP;
    }
    else if (strstr(cur_token->token, ">"))
    {
      result->Opcode = OP_CMP_GTR_JMP;
    }
    else if (strstr(cur_token->token, "<"))
    {
      result->Opcode = OP_CMP_LSR_JMP;
    }
  }
  else
  {
    printf("Expected ; After if ((..) \n");
  }
  ASTNode_t *right_node = parse_expression();
  
  cur_token = tokenarray->tokenarray_ptr + read_pointer;
  if (strcmp(cur_token->token, ";")==0){
    read_pointer++;
  }else {
    printf("Expected ; but got: %s \n", cur_token->token);
  }
  result->reg_right = translate(right_node);
  result->reg_left = translate(left_node);
  return result;
}

void parse_statement()
{

  token_t *current_token = (tokenarray->tokenarray_ptr + read_pointer);
  if (current_token->type == VARIABLE)
  {
    char name[10];
    strcpy(name, current_token->token);
    variable_t *new_var;
    read_pointer++;
    int var_index = -1;
    for (int i = 0; i < varpool->length; i++)
    {
      variable_t *var = varpool->varptr + i;
      if (strcmp(var->name, name) == 0)
      {
        var_index = i;
        break;
      }
    }

    if (var_index == -1)
    {
      if (varpool->length == varpool->capacity)
      {
        varpool->capacity *= 2;
        varpool->varptr = realloc(varpool->varptr, sizeof(variable_t) * (varpool->capacity));
      }
      new_var = varpool->varptr + varpool->length;
      strcpy(new_var->name, name);
      new_var->reg = last_reg - used_reg_rev;
      used_reg_rev++;
      varpool->length++;
    }
    else
    {
      new_var = varpool->varptr + var_index;
    }

    current_token = (tokenarray->tokenarray_ptr + read_pointer);
    if (strstr(current_token->token, "="))
    {
      read_pointer++;
      ASTNode_t *var_val = ASTNodePool + NodePool_Length;
      NodePool_Length++;
      var_val = parse_expression();
      uint16_t res_reg = translate(var_val);

      printf("OP_LOAD_REG [%d] [ %d] \n", res_reg, new_var->reg);
      uint16_t instruction_set = {OP_LOAD_REG, res_reg, new_var->reg};
      int offset = output_stream->length;
      uint16_t * write_ptr = output_stream->binstream + offset;
      memcpy(write_ptr, instruction_set, sizeof(instruction_set));
      output_stream->length+=3;


    }
    else
    {
    }
  }
  else if (current_token->type == KEYWORD)
  {

    if (strstr(current_token->token, "if"))
    {
      logical_result_t *result;
      read_pointer++;
      uint16_t op;
      int jmpat;
      int initlen;
      int jmp_else_index;
      current_token = tokenarray->tokenarray_ptr + read_pointer;
      if (strstr(current_token->token, "("))
      {
        read_pointer++;
        result = parse_logic();
        op = result->Opcode;

        current_token = tokenarray->tokenarray_ptr + read_pointer;
        if (strstr(current_token->token, ")"))
        {
          
          
          read_pointer++;
          
          current_token = tokenarray->tokenarray_ptr + read_pointer;
          jmpat = output_stream->length + 5;
          switch (op)
          {
          case OP_CMP_JMP:
            printf("OP_CMP_JMP [%d] [%d] [%d]\n", result->reg_left, result->reg_right, 0xABCD);
            uint16_t instruction_set = {OP_CMP_JMP, result->reg_left, result->reg_right, 0x0000};
            int offset = output_stream->length;
            uint16_t * write_ptr = output_stream->binstream + offset;
            memcpy(write_ptr, instruction_set, sizeof(instruction_set));
            output_stream->length+=4;
            
            break;
          case OP_CMP_GTR_JMP:
            printf("OP_CMP_GTR_JMP [%d] [%d] [%d]\n", result->reg_left, result->reg_right, 0xABCD);
            uint16_t instruction_set = {OP_CMP_GTR_JMP, result->reg_left, result->reg_right, 0x0000};
            int offset = output_stream->length;
            uint16_t * write_ptr = output_stream->binstream + offset;
            memcpy(write_ptr, instruction_set, sizeof(instruction_set));
            output_stream->length+=4;
            
            break;
          case OP_CMP_LSR_JMP:
            printf("OP_CMP_LSR_JMP [%d] [%d] [%d]\n", result->reg_left, result->reg_right, 0xABCD);
            uint16_t instruction_set = {OP_CMP_LSR_JMP, result->reg_left, result->reg_right, 0x0000};
            int offset = output_stream->length;
            uint16_t * write_ptr = output_stream->binstream + offset;
            memcpy(write_ptr, instruction_set, sizeof(instruction_set));
            output_stream->length+=4;
            
            break;
          default:
            printf("Unknown comparison operator used \n");
            
            printf("Got opcode: %d \n", op);
          }
          if (strstr(current_token->token, "{"))
          {
            read_pointer++;
            initlen = output_stream->length;
            parse_program(); // this will also write out the code for + if condition.
            output_stream->length += 6; // assuming if condition is 6*2 bytes long.
            jmp_else_index = output_stream->length + 1;
            printf("OP_JMP_RELP [%d] \n", 0xABCD);
            uint16_t instruction_set = {OP_JMP_RELP, 0x0000};
            int offset = output_stream->length;
            uint16_t * write_ptr = output_stream->binstream + offset;
            memcpy(write_ptr, instruction_set, sizeof(instruction_set));
            
            
            output_stream->length += 2;
            output_stream->binstream[jmpat] = output_stream->length - initlen; // changed the jump address for + if statement. Then, we write out the else part and then change the jump after if statement to jump that much...... ig it wasn't necessary to change order since i have to write out shit and come back to change anyways but its fine ig
            printf("Changed (for going to else block) jump 1* to %d bytes \n", output_stream->length-initlen);
            current_token = tokenarray->tokenarray_ptr + read_pointer;
            if (strstr(current_token->token, "}"))
            {
              read_pointer++;
              current_token = tokenarray->tokenarray_ptr + read_pointer;
              if (strstr(current_token->token, "else"))
              {
                read_pointer++;
                current_token = tokenarray->tokenarray_ptr + read_pointer;
                if (strstr(current_token->token, "{"))
                {
                  read_pointer++;
                  initlen = output_stream->length;
                  parse_program();     // this will also write out the code for + if condition.
                  output_stream->length += 6; // assuming parse program length is 6*2 bytes.....
                  output_stream->binstream[jmp_else_index] = output_stream->length - initlen; // changed the jump address for + if statement. Then, we write out the else part and then change the jump after if statement to jump that much...... ig it wasn't necessary to change order since i have to write out shit and come back to change anyways but its fine ig
                  printf("Changed jump (to jump the else blok after if ) 1 to %d bytes \n", output_stream->length-initlen);
                  current_token = tokenarray->tokenarray_ptr + read_pointer;
                  if (strstr(current_token->token, "}"))
                  {
                    read_pointer++;
                    return;
                  }else {
                    
                    printf("Expected '}' but got '%s' \n", current_token->token);
                  }
                }else {
                  printf("Expected '{' * but got '%s' \n", current_token->token);
                }
              }else {
                printf("Expected 'else' but got '%s' \n", current_token->token);
              }
            }else {
              printf("Expected } but got %s \n", current_token->token);
            }
          }
        }
        else
        {
          printf("ERROR: Expected ) instead got: %s  \n", current_token->token);

        }
      }
    }else if (strstr(current_token->token, "return")){
      read_pointer++;
      current_token = tokenarray->tokenarray_ptr + read_pointer;
      if (strstr(current_token->token, "(")){
        read_pointer++;
        ASTNode_t * return_val = parse_expression();
        uint16_t return_reg = translate(return_val);
        // use: return ( ( val ) ; ) ;
        current_token = tokenarray->tokenarray_ptr + read_pointer;
        if (strstr(current_token->token, ";")){
          read_pointer++;
          printf("OP_RETURN [%d] \n", return_reg );
          uint16_t instruction_set = {OP_RETURN, return_reg};
          int offset = output_stream->length;
          uint16_t * write_ptr = output_stream->binstream + offset;
          memcpy(write_ptr, instruction_set, sizeof(instruction_set));
          output_stream->length++;
        }else {
          printf("Expected ; but got %s \n", current_token->token);
        }


      }else {
        printf("Expected ( but got %s\n", current_token->token);
      }


    }else if (strstr(current_token->token, "while"))
    {
      logical_result_t *result;
      read_pointer++;
      uint16_t op;
      int jmpat;
      int initlen;
      int jmp_else_index;
      current_token = tokenarray->tokenarray_ptr + read_pointer;
      if (strstr(current_token->token, "("))
      {
        read_pointer++;
        result = parse_logic();
        op = result->Opcode;

        current_token = tokenarray->tokenarray_ptr + read_pointer;
        if (strstr(current_token->token, ")"))
        {
          
          read_pointer++;
          
          current_token = tokenarray->tokenarray_ptr + read_pointer;
          jmpat = output_stream->length + 5;
          switch (op)
          {
          case OP_CMP_JMP:
            printf("OP_CMP_JMP [%d] [%d] [%d]\n", result->reg_left, result->reg_right, 0xABCD);
            uint16_t instruction_set = {OP_CMP_JMP, result->reg_left, result->reg_right, 0x0000};
            int offset = output_stream->length;
            uint16_t * write_ptr = output_stream->binstream + offset;
            memcpy(write_ptr, instruction_set, sizeof(instruction_set));
            output_stream->length += 4;
            break;
          case OP_CMP_GTR_JMP:
            printf("OP_CMP_GTR_JMP [%d] [%d] [%d]\n", result->reg_left, result->reg_right, 0xABCD);
            uint16_t instruction_set = {OP_CMP_GTR_JMP, result->reg_left, result->reg_right, 0x0000};
            int offset = output_stream->length;
            uint16_t * write_ptr = output_stream->binstream + offset;
            memcpy(write_ptr, instruction_set, sizeof(instruction_set));
            output_stream->length +=4;
            break;
          case OP_CMP_LSR_JMP:
            printf("OP_CMP_LSR_JMP [%d] [%d] [%d]\n", result->reg_left, result->reg_right, 0xABCD);
            uint16_t instruction_set = {OP_CMP_LSR_JMP, result->reg_left, result->reg_right, 0x0000};
            int offset = output_stream->length;
            uint16_t * write_ptr = output_stream->binstream + offset;
            memcpy(write_ptr, instruction_set, sizeof(instruction_set));
            output_stream->length +=4;
            break;
          default:
            printf("Got unknows comparison operator. \n");
            
            printf("Got opcode: %d \n", op);
          }
          if (strstr(current_token->token, "{"))
          {
            read_pointer++;
            initlen = output_stream->length;
            parse_program(); // this will also write out the code for + if condition.
            output_stream->length += 12; // assuming if condition is 6*2 bytes long.
            jmp_else_index = output_stream->length + 1;
            printf("OP_JMP_RELN [%d] \n", 0xABCD);
            uint16_t instruction_set[] = {OP_JMP_RELN, 0x0000};
            int offset = output_stream->length;
            uint16_t * write_ptr = output_stream->binstream + offset;
            memcpy(write_ptr, instruction_set, sizeof(instruction_set));
            output_stream->length += 2;
            output_stream->binstream[jmpat] = output_stream->length - initlen; // changed the jump address for + if statement. Then, we write out the else part and then change the jump after if statement to jump that much...... ig it wasn't necessary to change order since i have to write out shit and come back to change anyways but its fine ig
            printf("Changed (for escaping while ) jump 1* to %d bytes \n", output_stream->length-initlen);
            printf("Changing - ve jump length for loop jump to %d bytes \n", output_stream->length - initlen + 4); // Need to check this.
            current_token = tokenarray->tokenarray_ptr + read_pointer;
            if (strstr(current_token->token, "}"))
            {
              read_pointer++;
              current_token = tokenarray->tokenarray_ptr + read_pointer;
            }else {
              printf("Expected } but got %s \n", current_token->token);
            }
          }
        }
        else
        {
          printf("ERROR: Expected ) instead got: %s  \n", current_token->token);

        }
      }
    }
  }

  if (strstr(";", current_token->token))
  {
    read_pointer++;
    return;
  }
}

void parse_program()
{
  token_t *cur_token = tokenarray->tokenarray_ptr + read_pointer;

  while (cur_token != NULL && strcmp(cur_token->token, "EOF") != 0 && strcmp(cur_token->token, "}") != 0)
  {
    parse_statement();
    cur_token = tokenarray->tokenarray_ptr + read_pointer;
  }
  if (strstr(cur_token->token, "}"))
  {
    return;
  }
}

void print_ast(ASTNode_t *node, int level)
{
  if (node == NULL)
  {
    printf("NULL Node \n");
    return;
  }
  for (int i = 0; i < level; i++)
  {
    printf("    "); // 4 spaces per depth level
  }

  if (node->NodeType == NODE_VALUE)
  {
    printf("[VAL: %d]\n", node->data.value);
  }
  else if (node->NodeType == NODE_OPERATOR)
  {
    char op_char = '?';
    switch (node->data.op)
    {
    case OPR_ADD:
      op_char = '+';
      break;
    case OPR_SUB:
      op_char = '-';
      break;
    case OPR_MUL:
      op_char = '*';
      break;
    case OPR_DIV:
      op_char = '/';
      break;
    }
    printf("[OP: %c]\n", op_char);
  }
  else
  {
    printf("Node type: %d \n", node->NodeType);
  }

  print_ast(node->left, level + 1);
  print_ast(node->right, level + 1);
}

int main()
{
  varpool = malloc(sizeof(var_pool_t));
  varpool->length = 0;
  varpool->capacity = 2;
  varpool->varptr = malloc(sizeof(variable_t) * 2);
  ASTNodePool = malloc(sizeof(ASTNode_t) * 4096);
  output_stream = malloc(sizeof(ByteStream_t));
  output_stream->length = 0;
  output_stream->capacity = 10;
  output_stream->binstream = malloc(sizeof(uint16_t) * 10);

  //char program[] = "i = 0 ; input = 7 ; while ( ( i ) ; < ( input ) ; ) { i = i + 3 ; } return ( i ) ; EOF";
  char * program;
  program = load_code();
  printf("Code loaded : %s \n", program);
  tokenarray = tokenizer(program);
  classifier(tokenarray);

  parse_program();

  printf("Total Variables: %d \n", varpool->length);
  for (int i = 0; i < varpool->length; i++)
  {
    printf("Variable no: [%d] \n", i + 1);
    printf("Name: [%s] :: REG: [%d] \n", (varpool->varptr + i)->name, (varpool->varptr + i)->reg);
  }

  writeBinaryStream("bsp.out", output_stream->binstream, output_stream->length);
}
