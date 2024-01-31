#include "stdio.h"
#include "lib/CALC/CALC.h"

#define CALCULATOR_CMD_EXIT         0
#define CALCULATOR_CMD_ADD          1
#define CALCULATOR_CMD_SUBSTRACT    2
#define CALCULATOR_CMD_MULTIPLY     3
#define CALCULATOR_CMD_DIVIDE       4

void input_data(int *value1, int *value2);

int main(){

    printf("==== Calculator ====\n");
    printf("--- Command menu ---\n");
    printf("0 - exit             \n");
    printf("1 - add       (a + b)\n");
    printf("2 - substract (a - b)\n");
    printf("3 - multiply  (a * b)\n");
    printf("4 - divide    (a / b)\n");
    printf("--------------------\n");

    unsigned int command = 0;
    int value1 = 0, value2 = 0;
    int work = 1;
    while(work){
        // Input command
        printf("\n");
        printf("cmd --> ");
        scanf("%u", &command);

        // === Processing
        switch (command)
        {
        case CALCULATOR_CMD_ADD:
            input_data(&value1, &value2);
            printf("\tresult: %d\n", CALC_add(value1, value2));
            break;
        case CALCULATOR_CMD_SUBSTRACT:
            input_data(&value1, &value2);
            printf("\tresult: %d\n", CALC_substract(value1, value2));
            break;
        case CALCULATOR_CMD_MULTIPLY:
            input_data(&value1, &value2);
            printf("\tresult: %d\n", CALC_multiply(value1, value2));
            break;
        case CALCULATOR_CMD_DIVIDE:
            input_data(&value1, &value2);
            printf("\tresult: %d\n", CALC_divide(value1, value2));
            break;
        case CALCULATOR_CMD_EXIT:
            printf("\tExit\n");
            work = 0;
            break;
        default:
            printf("\tWrong command\n");
            break;
        }
    }

    return 0;
}

void input_data(int *value1, int *value2){
    // For proper processing of non-integers
    float fvalue = 0;

    printf("\ta: ");
    scanf("%f", &fvalue);
    *value1 = (int)fvalue;

    printf("\tb: ");
    scanf("%f", &fvalue);
    *value2 = (int)fvalue;
}