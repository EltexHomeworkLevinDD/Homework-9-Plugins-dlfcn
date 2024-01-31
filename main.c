#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

#define EXITMSG(msg) do { \
        perror(msg); \
        exit(EXIT_FAILURE); } while(0)

// Всегда постоянные параметры
#define L_NAME_LENGTH           127   // Точная длина имени библиотеки (без учета '\0')
#define L_FUNCTION_NAME_LEN     127   // Точная длина имени функции в библиотеке
#define L_DESCRIPLION_LENGTH    127   // Точная длина описания функции
// --------------------------------

/*
В вашей библиотеке [library] должны быть:
    1) Глобальная строковая переменная размером 128 символов (127 символов без учета '\0'), под названием [library]_FUNCTION_NAME, содержащая имя функции
    2) Глобальная строковая переменная размером 128 символов (127 символов без учета '\0'), под названием [library]_DESCRIPTION, содержащая описание функции
*/

#define L_NAME_PREFIX           "./lib/lib"             // Префикс в имени библиотеки [directory]/[lib]
#define L_NAME_PREFIX_LEN       9                       // Длина префикса в имени библиотеки (длина L_NAME_PREFIX без учета '\0')
#define L_NAME_POSTFIX          ".so"                   // Постфикс в имени библиотеки [directory]/[lib]           
#define L_NAME_POSTFIX_LEN      3                       // Длина Постфикс в имени библиотеки (длина L_NAME_PREFIX без учета '\0')
#define L_FUNCTION_NAME_POSTFIX  "_FUNCTION_NAME"       // Постфикс переменной, содержащей имя функции
#define L_DESCRIPTION_NAME_POSTFIX  "_DESCRIPTION"      // Постфикс переменной, содержащей описание функции
#define MAX_L_COUNT             5                       // Максимальное количество библиотек

// Структура библиотеки
struct Library{
    void* handle;                                                   // Указатель на библиотеку
    const char short_name[L_NAME_LENGTH+1];                     // Короткое имя (без префикса и постфикса)
    int (*function)(int,int);                                       // Указатель на функцию
    const char function_name[L_FUNCTION_NAME_LEN+1];                // Имя функции в библиотеке
    const char description[L_DESCRIPLION_LENGTH+1];               // Описание функции
    const char full_name[L_NAME_PREFIX_LEN+L_NAME_LENGTH+L_NAME_POSTFIX_LEN+1];    // Полное имя библиотеки (с префиксом и постфиксом)
};

void input_data(int *value1, int *value2);

int main(){

    printf("==== Enter the libraries you are using (Enter 'q' for quit)  ====\n");
    printf("Enter name without 'lib' and '.so', max libraries count %u\n", MAX_L_COUNT);

    struct Library libraries[MAX_L_COUNT];          // Создаем массив, содержащий библиотеки
    unsigned int libraries_count = 0;               // Действительное количетсво библиотек
    memset(libraries, '\0', sizeof(libraries));     // Зануляем поля

    char lib_full_name[L_NAME_PREFIX_LEN+L_NAME_LENGTH+L_NAME_POSTFIX_LEN+1]; // Строковая переменная полного имени библиотеки
    char lib_short_name[L_NAME_LENGTH+1];               // Строковая переменная короткого имени библиотеки
    char function_name_var[L_FUNCTION_NAME_LEN + 1];    // Имя строковой переменной с именем функции
    char description_var[L_DESCRIPLION_LENGTH+1];       // Имя строковой переменной с описанием функции

    while (libraries_count < MAX_L_COUNT){ 
        // Зануляю строки
        memset(lib_short_name, '\0', sizeof(lib_short_name));
        memset(lib_full_name, '\0', sizeof(lib_full_name));
        memset(function_name_var, '\0', sizeof(function_name_var));
        memset(description_var, '\0', sizeof(description_var));
        // Считать строку
        printf("\t> ");
        if (fgets(lib_short_name, L_NAME_LENGTH+1, stdin) == NULL)   // Считываю строку
            EXITMSG("Error reading library name\n");
        // Если пользователь вышел, закрыть ввод
        if (!strcmp(lib_short_name, "q\n")){ 
            break;
        }
        int len = strcspn(lib_short_name, "\n");  // Найти позицию символа новой строки (после fgets)
        lib_short_name[len] = '\0';  // Заменить символ новой строки нулевым символом
        // Копируем строку char с коротким именем библиотеки в структуру с полем const char
        memcpy((void*)libraries[libraries_count].short_name, (void*)lib_short_name, sizeof(lib_short_name)); 
        
        // Создать строку полного имени
        strcpy(lib_full_name, L_NAME_PREFIX);
        strcat(lib_full_name, lib_short_name);
        strcat(lib_full_name, L_NAME_POSTFIX);
        // Копипуем строку char полного имени библиотеки в структуру с полем const char 
        memcpy((void*)libraries[libraries_count].full_name, (void*)lib_full_name, sizeof(lib_full_name)); 

        // Загружаем библиотеку
        libraries[libraries_count].handle = dlopen(libraries[libraries_count].full_name, RTLD_NOW);
        if (!libraries[libraries_count].handle){
            printf("Error loading the library: %s\n", dlerror());
            EXITMSG("");
        } 

        // Определяем имя переменной с именем функции
        strcat(function_name_var, lib_short_name);
        strcat(function_name_var, L_FUNCTION_NAME_POSTFIX);

        // Получили переменную с именем функции
        void* function_name_var_ptr = dlsym(libraries[libraries_count].handle, (const char*)function_name_var);
        if (function_name_var_ptr == NULL) {
            printf("Error loading the global virable FUNCTION_NAME: %s\n", dlerror());
            dlclose(libraries[libraries_count].handle);
            EXITMSG("");
        }
        // Копируем char имя функции в структуру с полем const char 
        memcpy((void*)libraries[libraries_count].function_name, (void*)function_name_var_ptr, L_FUNCTION_NAME_LEN); 

        // Получаем void указатель на функцию
        libraries[libraries_count].function = dlsym(libraries[libraries_count].handle, function_name_var_ptr);
        if (libraries[libraries_count].function == NULL) {
            printf("Error loading the function: %s\n", dlerror());
            dlclose(libraries[libraries_count].handle);
            EXITMSG("");
        }

        // Определяем имя переменной с описанем функции
        strcat(description_var, lib_short_name);
        strcat(description_var, L_DESCRIPTION_NAME_POSTFIX);

        // Получили переменную с описанем функции
        void* description_var_ptr = dlsym(libraries[libraries_count].handle, (const char*)description_var);
        if (description_var_ptr == NULL) {
            printf("Error loading the global virable DESCRIPTION: %s\n", dlerror());
            dlclose(libraries[libraries_count].handle);
            EXITMSG("");
        }
        // Копируем char описане функции в структуру с полем const char 
        memcpy((void*)libraries[libraries_count].description, (void*)description_var_ptr, L_DESCRIPLION_LENGTH); 
        libraries_count++;
    }

    printf("==== Calculator ====\n");
    printf("--- Command menu ---\n");
    printf("0 - exit             \n");
    for (int i = 0; i < libraries_count; i++){
        printf("%d - %s   (%s)\n", i+1, libraries[i].function_name, libraries[i].description);
    }
    printf("--------------------\n");


    unsigned int command = 0;
    int value1 = 0, value2 = 0;
    int work = 1;
    while(work){
        // Input command
        printf("\n");
        printf("cmd --> ");
        scanf("%u", &command);

        if (command > libraries_count){
            printf("\tWrong command\n");
        }else{
            if (command == 0){
                printf("\tExit\n");
                work = 0;
            }else{
                input_data(&value1, &value2);
                printf("\tresult: %d\n", libraries[command-1].function(value1, value2));
            }
        }
    }

    for (int i = 0; i < libraries_count; i++){
        if(dlclose(libraries[i].handle)){
            printf("Error closing the library: %s\n", dlerror());
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