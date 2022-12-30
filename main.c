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

#define clear() printf("\033[H\033[J") //clears the page

const int MAX = 1000; //maximum size of arrays in program
char** TOKEN; //list of commands

void welcome(); //welcome page
void printDir(); //prints the current directory
void execute(char *input); //executes the given command
void getInput(); //receives the input from user
bool myCommands(char* input); //additional commands
void error(); //prints an error
void a(); //cuts the first string
void b(); //most frequent char in text
void c(); //deletes all empty spaces such as space,\n,\t,...
void d(); //displays all onCommented lines
void f(); //number of text lines
void g(); //first 10 lines of text or more ...
char** splitter(const char * str, const char * delim); //splits a string base on the char you want
bool fileValidator(char* input); //validates the file base of given location
char* fileToText(FILE* file); //returns a text of file
void addHistory(char* input); //adds the commands to history.txt
void handleSigint(int sig); //handles the ctrl+c
bool isEmpty(const char *str); //returns a bool based on text given to
FILE* removeEmptyLines(FILE *file); //removes empty lines of given file
char* removeSpaces(char *string); //removes space of a text
void printFile(FILE *fptr); //prints the given file as a text

int main() {
    signal(SIGINT, handleSigint);
    welcome();
    getInput();
    printDir();

    return 0;
}

char ** splitter(const char * str, const char* delim){
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
    fprintf(stderr,"wrong command\ntry using help\n");
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
    if(fileValidator(TOKEN[1])){

        FILE* file;
        file = fopen(TOKEN[1],"r");
        char* text = fileToText(file);
        fclose(file);
        int lineNumberToShow = 10;
        int counter = 1;

        for (char c = *text; c; c=*++text) {
            printf("%c",c);
            if(c == '\n')
                counter++;
            if(counter == lineNumberToShow+1)
                break;
        }

    }

    else {
        printf("no such file\n");
        error();
    }

    printf("********* g command *********\n");
}

void f() {
    if(fileValidator(TOKEN[1])){
        FILE* file;
        int count = 0; // Line counter (result)
        file = fopen(TOKEN[1], "r");


        char* text = fileToText(file);
        fclose(file);

        for (char c = *text; c; c=*++text) {
            if(c == '\n')
                count++;
        }

        printf("%d",count+1);
    }

    else {
        printf("no such file\n");
        error();
    }

    printf("********* f command *********\n");
}

void d() {
    if(fileValidator(TOKEN[1])){
        FILE* file;
        file = fopen(TOKEN[1],"r");
        char* text = fileToText(file);

        int i=0;
        char lastchar;
        for (char c = *text; c; c=*++text) {
            int hashtag = 35;

            if((c == hashtag) && (lastchar == '\n')){
                while(c!=0) {
                    c = *++text;
                    lastchar = c;
                }
                c=*++text;
            }
            i++;
            printf("%c",c);
            lastchar = c;
        }
        fclose(file);
    }

    else {
        printf("no such file\n");
        error();
    }

    printf("********* c command *********\n");
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

void handleSigint(int sig){
    clear();
    printDir();
}

void printFile(FILE *fptr)
{
    char ch;

    while((ch = fgetc(fptr)) != EOF)
        putchar(ch);
}

bool isEmpty(const char *str)
{
    char ch;

    do
    {
        ch = *(str++);

        // Check non whitespace character
        if(ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r' && ch != '\0')
            return true;

    } while (ch != '\0');

    return false;
}

FILE* removeEmptyLines(FILE *file){
    FILE* temp;
    temp = fopen("remove-blanks.tmp", "w");

    rewind(file);

    char buffer[MAX];

    while ((fgets(buffer, MAX, file)) != NULL)
    {
        // If current line is not empty then write to temporary file
        if(!isEmpty(buffer))
            fputs(buffer, temp);
    }

    fclose(temp);
    temp = fopen("remove-blanks.tmp", "r");
    return temp;

}

char * removeSpaces(char *string){
    int non_space_count = 0;

    for (int i = 0; string[i] != '\0'; i++)
    {
        if (string[i] != ' ')
        {
            string[non_space_count] = string[i];
            non_space_count++;
        }
    }

    string[non_space_count] = '\0';
    return string;
}


