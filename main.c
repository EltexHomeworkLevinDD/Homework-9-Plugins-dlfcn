#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>

#define EXITMSG(msg) do { \
        perror(msg); \
        exit(EXIT_FAILURE); } while(0)


#define MAX_LIBRARY_NAME_LENGTH 31 // Чистые символы, без учета '\0', лучше кратное значение + -1
#define MAX_LIBRARIES_COUNT 4

struct Library{
    void* handle;
    const char short_name[MAX_LIBRARY_NAME_LENGTH+1];
    int (*function)(int,int);
};

int main(){

    printf("==== Enter the libraries you are using (Enter 'q' for quit)  ====\n");
    printf("Enter name without 'lib' and '.so', max libraries count %u\n", MAX_LIBRARIES_COUNT);

    struct Library libraries[MAX_LIBRARIES_COUNT];  // Создаем массив, содержащий библиотеки
    unsigned int libraries_count = 0;               // Действительное количетсво библиотек
    memset(libraries, '\0', sizeof(libraries));     // Зануляем поля


    // Ввод используемых библиотек
    char input_str[MAX_LIBRARY_NAME_LENGTH+1];
    while (libraries_count < MAX_LIBRARIES_COUNT){  // Пишем
        printf("\t> ");
        // Зануляю строку
        memset(input_str, '\0', sizeof(input_str));
        // Считываю строку
        if (fgets(input_str, MAX_LIBRARY_NAME_LENGTH+1, stdin) == NULL) EXITMSG("Error reading library name\n");
        // Если пользователь вышел, закрыть ввод
        if (!strcmp(input_str, "q\n")){
            break;
        }
        // Копируем строку char с коротким именем в структуру с полем const char
        memcpy((void*)libraries[libraries_count].short_name, (void*)input_str, sizeof(input_str)); 
        
        // Создаем полное имя
        size_t len = strcspn(input_str, "\n");  // Найти позицию символа новой строки
        input_str[len] = '\0';                  // Заменить символ новой строки нулевым символом
        char prefix[] = "./lib/lib";
        char postfix[] = ".so";
        // Создать массив полного имени
        char full_name[strlen("./lib/lib") + MAX_LIBRARY_NAME_LENGTH + strlen(".so") + 1];
        // Создать полное имя
        strcpy(full_name, prefix);
        strcat(full_name, input_str);
        strcat(full_name, postfix);
        const char* const_full_name_ptr = (const char*) full_name;

        // Загружаем библиотеку
        libraries[libraries_count].handle = dlopen(full_name, RTLD_NOW);
        if (!libraries[libraries_count].handle){
            printf("Error loading the library: %s\n", dlerror());
            EXITMSG("");
        } 

        // Загружаем имя функции
        char function_name_postfix[] = "_FUNCTION_NAME"; // Постфикс переменной с именем функции
        char function_name[strlen(function_name_postfix) + MAX_LIBRARY_NAME_LENGTH + 1]; // Создаем строковую переменную для имени
        memset(function_name, '\0', sizeof(function_name)); // Зануляем строку
        strcat(function_name, input_str);
        strcat(function_name, function_name_postfix);
        const char* const_function_name = (const char*) function_name; 

        void* function_name_ptr = dlsym(libraries[libraries_count].handle, const_function_name); // Загружаем имя функции
        if (function_name_ptr == NULL) {
            printf("Error loading the global virable: %s\n", dlerror());
            dlclose(libraries[libraries_count].handle);
            EXITMSG("");
        }
        
        // Загружаем функцию
        libraries[libraries_count].function = dlsym(libraries[libraries_count].handle, function_name_ptr);
        if (function_name_ptr == NULL) {
            printf("Error loading the function: %s\n", dlerror());
            dlclose(libraries[libraries_count].handle);
            EXITMSG("");
        }

        libraries_count++;
    }

    return 0;
}
