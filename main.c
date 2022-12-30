#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#define clear() printf("\033[H\033[J")

const int MAX = 1000;
char** TOKEN;

void welcome();
void printDir();
void execute(char *input);
void getInput();
bool myCommands(char* input);
void error();
void addCommand(char* command);
void a();
void b();
void c();
void d();
void f();
void g();
char** splitter(const char * str, const char * delim);
bool fileOpener(char* input);
bool fileValidator(char* input);
char* fileToText(FILE* file);
void addHistory(char* input);
int mostFrequent(char* string);
void handleSigint(int sig);
int  isEmpty(const char *str);
FILE* removeEmptyLines(FILE *file);
char* removeSpaces(char *string);
void printFile(FILE *fptr);

int main() {
    signal(SIGINT, handleSigint);
    welcome();
    getInput();

    printDir();


    return 0;
}

char ** splitter(const char * str, const char * delim){
    /* count words */
    char * s = strdup(str);

    if (strtok(s, delim) == 0)
        /* no word */
        return NULL;

    int nw = 1;

    while (strtok(NULL, delim) != 0)
        nw += 1;

    strcpy(s, str); /* restore initial string modified by strtok */

    /* split */
    char ** v = malloc((nw + 1) * sizeof(char *));
    int i;

    v[0] = strdup(strtok(s, delim));

    for (i = 1; i != nw; ++i)
        v[i] = strdup(strtok(NULL, delim));

    v[i] = NULL; /* end mark */

    free(s);

    return v;
}

void getInput() {
    char* input;
    printDir();
    input = readline("\n>>: ");
    addHistory(input);
    TOKEN = splitter(input," ");
    execute(TOKEN[0]);
}

void execute(char *input) {

    char* argument_list[] = {"", "-l", NULL};
    argument_list[0] = input;
    if(!strcmp("exit",input)){
        printf("\nbye\n");
        exit(0);
    }

    else{
        pid_t child = fork();

        if(child < 0) {
            printf("error\n");
            exit(0);
        }

        else if(child == 0) {
            if(execvp(input,argument_list) == -1){
                if(!myCommands(input)){
                    error();
                }
            }
            exit(0);
        }
        else {
            wait(NULL);
            getInput();
        }

    }

}

void welcome(){
    clear();
    printf("                  ****************** welcome to my linux shell ******************                   \n");
    fflush(stdout);
    sleep(1);
    clear();
}

void printDir(){
    char dir[MAX];
    char * username;
    getcwd(dir, sizeof(dir));
    username = getenv("USER");
    printf("%s >> %s\n",username,dir);
}

void error(){
    printf("wrong command\ntry using help\n");
    fflush(stdout);
    sleep(1);
    clear();
}

bool myCommands(char* input){
    if(!strcmp(input,"exit"))
        exit(0);
    else if(!strcmp(input,"a")) {
        a();
        return true;
    }
    else if(!strcmp(input,"b")) {
        b();
        return true;
    }
    else if(!strcmp(input,"c")) {
        c();
        return true;
    }
    else if(!strcmp(input,"d")) {
        d();
        return true;
    }
    else if(!strcmp(input,"f")) {
        f();
        return true;
    }
    else if(!strcmp(input,"g")) {
        g();
        return true;
    }

    return false;
}

void g() {
    printf("g\n");
}

void f() {
    printf("f\n");
}

void d() {
    printf("d\n");
}

void c() {
    if(fileValidator(TOKEN[1])){

        FILE* file;
        file  = fopen(TOKEN[1], "r");

        printf("\n%s\n",removeSpaces(fileToText(removeEmptyLines(file))));

        fclose(file);
    }

    else {
        printf("no such file\n");
        error();
    }
    printf("********* c command *********\n");}

void b() {
    if(fileValidator(TOKEN[1])){
        FILE* file;
        file = fopen(TOKEN[1],"r");
        char* text = fileToText(file);
        fclose(file);

        char result;
        int i, len;
        int max = -1;
        int freq[MAX];
        len = strlen(text);

        for(i = 0; i < len; i++){
            freq[text[i]]++;
        }
        for(i = 0; i < len; i++){
            if(max < freq[text[i]]){
                max = freq[text[i]];
                result = text[i];
            }
        }

        if(strcmp(&result," ")) {
            if(strcmp(&result,"\n"))
                printf("\\n \n");
            else
                printf("space\n");
        }
        else {
            printf("%c\n", result);
        }
    }

    else {
        printf("no such file\n");
        error();
    }


    printf("********* b command *********\n");
}

void a() {
    if(fileValidator(TOKEN[1])){
        FILE* file;
        file = fopen(TOKEN[1],"r");
        char* text = fileToText(file);
        fclose(file);
        char** temp = splitter(text," ");
        puts(temp[0]);
    }

    else {
        printf("no such file\n");
        error();
    }
    printf("********* a command *********\n");
}

bool fileValidator(char* input){
    FILE* ptr;
    ptr = fopen(input, "r");
    if (NULL == ptr) {
        return false;
    }
    else
        return true;
}

char* fileToText(FILE* file){
    char ch;
    char text[MAX];
    int counter=0;

    while (!feof(file)) {
        ch = fgetc(file);
        text[counter] = ch;
        counter++;
    }
    fclose(file);

    char* temp = malloc(MAX);

    memcpy(temp,text, sizeof(text));
    return temp;
}

void addHistory(char* input){
    FILE* file;
    if(fileValidator("history.txt")){
        file = fopen("history.txt","a");
        fprintf(file,input);
        fprintf(file,"\n");
        fclose(file);
    }
    else{
        file = fopen("history.txt","w");
        fprintf(file,input);
        fprintf(file,"\n");
        fclose(file);
    }
}

int mostFrequent(char* string){
    int count[256] = {0}; // Assum char is ASCII
    int max = 0;
    int i;

    for(i=0; i < strlen(string) ;i++) {
        count[(unsigned char)(string[i])] ++;
    }

    for(i=0; i < 256 ;i++) {
        if (count[i] > max)
            max = count[i];
    }
    return max;
}

void handleSigint(int sig){
    printf("Caught signal %d\n", sig);//todo ^c
}

void printFile(FILE *fptr)
{
    char ch;

    while((ch = fgetc(fptr)) != EOF)
        putchar(ch);
}

int isEmpty(const char *str)
{
    char ch;

    do
    {
        ch = *(str++);

        // Check non whitespace character
        if(ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '\0')
            return 0;

    } while (ch != '\0');

    return 1;
}

FILE* removeEmptyLines(FILE *file){
    FILE* temp;
    temp = fopen("remove-blanks.tmp", "w");


    /* Exit if file not opened successfully */
    if (file == NULL || temp == NULL)
    {
        printf("Unable to open file.\n");
        printf("Please check you have read/write previleges.\n");

        exit(EXIT_FAILURE);
    }

    // Move src file pointer to beginning
    rewind(file);

    // Remove empty lines from file.
    char buffer[MAX];

    while ((fgets(buffer, MAX, file)) != NULL)
    {
        /* If current line is not empty then write to temporary file */
        if(!isEmpty(buffer))
            fputs(buffer, temp);
    }

    /* Close all open files */
    fclose(temp);
    temp = fopen("remove-blanks.tmp", "r");
    return temp;

}

char * removeSpaces(char *string)
{
    // non_space_count to keep the frequency of non space characters
    int non_space_count = 0;

    //Traverse a string and if it is non space character then, place it at index non_space_count
    for (int i = 0; string[i] != '\0'; i++)
    {
        if (string[i] != ' ')
        {
            string[non_space_count] = string[i];
            non_space_count++;//non_space_count incremented
        }
    }

    //Finally placing final character at the string end
    string[non_space_count] = '\0';
    return string;
}


