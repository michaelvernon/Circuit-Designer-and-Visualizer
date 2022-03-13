#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {INPUT, OUTPUT, TEMP, AND, OR, NAND, NOR, XOR, NOT, PASS, DECODER, MULTIPLEXER} type_d;

struct value {
  char name[16];
  int val; // 0 or 1
};

int numInputs = 0;
int numOutputs = 0;
int numValues = 0;
int currentDir = 0;

struct direct {
  type_d type;
  int size;
  int numInputs;
  int controlSize;
  int *params; // Variable indexes for parameters
  struct direct *next;
};

struct direct *directList;
struct value values[200];
int *outputs;

int stringToEnum(char *input){
  if (strcmp(input, "INPUT")==0){
    return INPUT;
  } else if (strcmp(input, "OUTPUT")==0){
    return OUTPUT;
  } else if (strcmp(input, "TEMP")==0){
    return TEMP;
  } else if (strcmp(input, "AND")==0){
    return AND;
  } else if (strcmp(input, "OR")==0){
    return OR;
  } else if (strcmp(input, "NAND")==0){
    return NAND;
  } else if (strcmp(input, "NOR")==0){
    return NOR;
  } else if (strcmp(input, "XOR")==0){
    return XOR;
  } else if (strcmp(input, "NOT")==0){
    return NOT;
  } else if (strcmp(input, "PASS")==0){
    return PASS;
  } else if (strcmp(input, "DECODER")==0){
    return DECODER;
  } else if (strcmp(input, "MULTIPLEXER")==0){
    return MULTIPLEXER;
  }
  return PASS;
}

int getVarIndex(char *name){
  if (strcmp(name, "0")==0){
    values[numValues].val = 0;
    numValues++;
    return numValues-1;
  }
  if (strcmp(name, "1")==0){
    values[numValues].val = 1;
    numValues++;
    return numValues-1;
  }
  for (int i=0;i<numValues;i++){
    if (strcmp(values[i].name, name)==0){
      return i;
    }
  }
  strcpy(values[numValues].name, name);
  numValues++;
  return numValues-1;
}

// Return 1 if all elements in array are 1
int checkArrayStatus(int *array, int length){
  for (int i=0;i<length;i++){
    if (array[i]==0){
      return 0;
    }
  }
  return 1;
}

// Inserts the directive the earliest it could be fully computed
void insertDirective(struct direct *direct){
  if (directList->next==NULL){
    directList->next = direct;
    return;
  }
  int inputVars = direct->numInputs;
  int *status = malloc(sizeof(int)*inputVars);
  for (int i=0;i<inputVars;i++){
    status[i] = 0;
  }
  struct direct *currentDirective = directList;
  while (currentDirective!=NULL){
    for (int i=0;i<currentDirective->size;i++){
      for (int j=0;j<inputVars;j++){
        if (strcmp(values[direct->params[j]].name,"")==0){
          status[j] = 1;
        }
        if (currentDirective->params[i] == direct->params[j]){
          status[j] = 1;
        }
      }
    }
    if (checkArrayStatus(status, inputVars)==1){
      direct->next = currentDirective->next;
      currentDirective->next = direct;
      return;
    }
    if (currentDirective->next==NULL){
      currentDirective->next = direct;
      return;
    }
    currentDirective = currentDirective->next;
  }
}

void printVariables(){
  printf("Inputs:%d Outputs:%d Total:%d", numInputs, numOutputs, numValues);
  for (int i=0;i<numValues;i++){
    printf(" Var:%d %s",i, values[i].name);
  }
  printf("\n");
}

void printDirectives(){
  struct direct *currentDirective = directList;
  while (currentDirective!=NULL){
    printf("DIRECT:%d", currentDirective->type);
    for (int i=0;i<currentDirective->size;i++){
      printf(" Var:%s", values[currentDirective->params[i]].name);
    }
    printf("\n");
    currentDirective = currentDirective->next;
  }
}

void run(int inputs[]){
  for (int i=0;i<numInputs;i++){
    values[i].val = inputs[i];
    printf("%d ", inputs[i]);
  }
  struct direct *currentDir = directList->next;
  while (currentDir!=NULL){
    if (currentDir->type==AND || currentDir->type==OR || currentDir->type==NAND || currentDir->type==NOR || currentDir->type==XOR){
      int a = values[currentDir->params[0]].val;
      int b = values[currentDir->params[1]].val;
      int result = 0;
      if (currentDir->type==AND){
        result = (a==1 && b==1) ? 1 : 0;
      } else if (currentDir->type==OR){
        result = (a==1 || b==1) ? 1 : 0;
      } else if (currentDir->type==NAND){
        result = (a==1 && b==1) ? 0 : 1;
      } else if (currentDir->type==NOR){
        result = (a==0 && b==0) ? 1 : 0;
      } else if (currentDir->type==XOR){
        result = (a!=b) ? 1 : 0;
      }
      values[currentDir->params[2]].val = result;
    } else if (currentDir->type==NOT || currentDir->type==PASS){
      int a = values[currentDir->params[0]].val;
      int result = 0;
      if (currentDir->type==NOT){
        result = (a==1) ? 0 : 1;
      } else {
        result = a;
      }
      values[currentDir->params[1]].val = result;
    } else if (currentDir->type==DECODER){
      int sum = 0;
      for (int i=0;i<currentDir->numInputs;i++){
        sum += values[currentDir->params[i]].val * (1<<(currentDir->numInputs - i - 1));
      }

      for (int i=0;i<(1<<(currentDir->numInputs));i++){
        values[currentDir->params[currentDir->numInputs+i]].val = 0;
      }
      values[currentDir->params[currentDir->numInputs + sum]].val = 1;
    } else if (currentDir->type==MULTIPLEXER){
      int sum = 0;
      int dataSize = (1<<currentDir->controlSize);
      for (int i=0;i<currentDir->controlSize;i++){
        sum += values[currentDir->params[dataSize+i]].val * (1<<(currentDir->controlSize - i - 1));
      }
      values[currentDir->params[currentDir->size-1]].val = values[currentDir->params[sum]].val;
    }
    currentDir = currentDir->next;
  }
  printf("%s", "|");
  for (int i=0;i<numOutputs;i++){
    printf(" %d", values[outputs[i]].val);
  }
  printf("\n");
}

int main(int argc, char *argv[]){
  FILE *file = fopen(argv[1], "r");
  char line[200];
  while (fgets(line, sizeof(line), file)!=NULL){
    if (line[0]!='\n'){
    char *token = strtok(line, " \n");
    int type = stringToEnum(token);
    if (type==INPUT){
      token = strtok(NULL, " \n");
      int num = atoi(token);
      numInputs = num;
      directList = malloc(sizeof(struct direct));
      directList->size = num;
      directList->params = malloc(sizeof(int)*num);
      for (int i=0;i<num;i++){
        token = strtok(NULL, " \n");
        directList->params[i] = getVarIndex(token);
      }
    } else if (type==OUTPUT){
      token = strtok(NULL, " \n");
      int num = atoi(token);
      outputs = malloc(sizeof(int)*num);
      for (int i=0;i<num;i++){
        token = strtok(NULL, " \n");
        outputs[i] = getVarIndex(token);
      }
      numOutputs = num;
    } else {
    struct direct *newDirect = malloc(sizeof(struct direct));
    newDirect->type = type;
    if (type==AND || type==OR || type==NAND || type==NOR || type==XOR){
      newDirect->size = 3;
      newDirect->numInputs = 2;
      newDirect->params = malloc(sizeof(int)*3);
      token = strtok(NULL, " \n");
      newDirect->params[0] = getVarIndex(token);
      token = strtok(NULL, " \n");
      newDirect->params[1] = getVarIndex(token);
      token = strtok(NULL, " \n");
      newDirect->params[2] = getVarIndex(token);
    } else if (type==NOT || type==PASS){
      newDirect->size = 2;
      newDirect->numInputs = 1;
      newDirect->params = malloc(sizeof(int)*2);
      token = strtok(NULL, " \n");
      newDirect->params[0] = getVarIndex(token);
      token = strtok(NULL, " \n");
      newDirect->params[1] = getVarIndex(token);
    } else if (type==DECODER || type==MULTIPLEXER){
      token = strtok(NULL, " \n");
      newDirect->size = atoi(token);
      if (type==DECODER){
        newDirect->numInputs = newDirect->size;
        newDirect->size = newDirect->size+(1<<(newDirect->size));
      } else {
        newDirect->controlSize = newDirect->size;
        newDirect->numInputs = newDirect->size+(1<<(newDirect->size));
        newDirect->size = newDirect->size+(1<<(newDirect->size)) + 1;
      }
      newDirect->params = malloc(sizeof(int)*(newDirect->size));
      for (int i=0;i<newDirect->size;i++){
        token = strtok(NULL, " \n");
        newDirect->params[i] = getVarIndex(token);
      }
    }
    insertDirective(newDirect);
    }
    }
  }
  //printVariables();
  //printDirectives();

  int *inputData;
  inputData = malloc(sizeof(int)*numInputs);
  int max = 1;
  for (int i=0;i<numInputs;i++){
    max *= 2;
  }
  for (int i=0;i<max;i++){
    for (int index=0;index<numInputs;index++){
      inputData[numInputs - index - 1] = (i >> index) & 1;
    }
    run(inputData);
  }
}
