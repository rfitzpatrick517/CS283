#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int count_words(char *, int, int);
//add additional prototypes here
int reverse_string(char *, int);
int print_words(char *, int);


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions

    if (len <= 0) {
        return -2; // Buffer length must be at least 0
    }

    // Creating pointers
    char *buff_pointer = buff;
    char *user_pointer = user_str;
    
    int count = 0; // counts number of characters copied
    int consec_whitespace = 0; // Handles cases where multiple whitespace characters are in user string

    while (*user_pointer == ' ' || *user_pointer == '\t') {
        user_pointer++; // skips leading whitespace
    }

    while (*user_pointer != '\0') { // loops until end of user string
        if (*user_pointer == ' '  || *user_pointer == '\t') { // checks for whitespace (space or tab)
            if (consec_whitespace == 0 && count < len) {
                *buff_pointer = ' '; // Adds space to buffer
                buff_pointer++;
                count++;
                consec_whitespace = 1;
            }
        } else {
            if (count < len) {
                *buff_pointer = *user_pointer;
                buff_pointer++;
                count++;
                consec_whitespace = 0;
            } else {
                return -1; // User supplied string is too large
            }
        }
        user_pointer++; 
    }
    
    // Created this statement when debugging due to an issue of the buffer having an extra space at the end
    if (buff_pointer > buff && *(buff_pointer - 1) == ' ') {
        buff_pointer--;
        count--;
    }  

    // Remainder of buffer is filled with '.'
    while (count < len) {
        *buff_pointer = '.';
        buff_pointer++;
        count++;
    }


    return count; // Returns length of the user supplied string
}

// NOTE: added brackets around buffer so it would pass tests
void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    printf("]\n");
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int count = 0;
    int inside_word = 0;

    // iterates until we have hit string length
    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) == ' ') { // count is only increased for non-whitespace (word)
            inside_word = 0;
        } else {
            if (!inside_word) {
                count++;
                inside_word = 1;
            }
        }
    }

    return count; // returns number of words in user string
}

int reverse_string(char *buff, int str_len) {
    char *start = buff; // pointer to beginning of str
    char *end = buff + str_len - 1; // pointer to end of str
    char temp;

    if (buff == NULL || str_len <= 0) {
        return -1;
    }

    // Swapping characters from start and end and going towards the center
    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }

    printf("Reversed String: %.*s\n", str_len, buff); // %.*s prints a substring with a desired length (str_len), from buff
    return 0;
}

int print_words(char *buff, int str_len) {
    if (buff == NULL || str_len <= 0) {
        return -1;
    }
    
    printf("Word Print\n----------\n"); // print header
    
    int word_index = 1; // keeps track of word order
    char *start = buff; // pointer to start of current word

    // iterates through buffer
    for (int i = 0; i <= str_len; i++) {
        if (*(buff + i) == ' ' || i == str_len) { // a word ends when there is a space or we are at the end of a string
            if (start < buff + i) {
                int word_len = buff + i - start;
                printf("%d. %.*s (%d)\n", word_index++, word_len, start, word_len);
            }
            start = buff + i + 1; // move to start of next word
        }
    }

    return 0;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      This is safe because the condition "argc < 2" first checks if there are less than 2 arguments passed. If this 
    //      is true, then the second condition of the or statement (*argv[1] != '-') would never be evaluated, since the or itself is already true.
    //      The only case where argv[1] doesn't exist would be if argc < 2, but since we explicitly check for that, we don't have to worry about argv[1] not 
    //      existing for the second condition of the or statement.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    // The if statement checks if the program has been given at least 3 arguments, which makes sense given the instructions which states the arguments
    // should be the following: stringfun -[h|c|r|w|x] "sample string" [other_options]. This if statement checks that if there isn't enough arguments,
    // then we will use the usage function and exit the program with error code 1, which is used for command line problems.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 2
    buff = (char *)malloc(BUFFER_SZ);
    if (buff == NULL) {
        exit(2);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        case 'r':
            rc = reverse_string(buff, user_str_len);
            if (rc < 0){
                printf("Error reversing string, rc = %d", rc);
                free(buff);
                exit(2);
            }
            break;

        case 'w':
            rc = print_words(buff, user_str_len);
            if (rc < 0){
                printf("Error printing words, rc = %d", rc);
                free(buff);
                exit(2);
            }
            break;

        case 'x':
            if (argc != 5) {
                printf("Error: -x flag requires 3 arguments.\n");
                free(buff);
                exit(1);
            }
            printf("Not Implemented!");
            free(buff);
            exit(0);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          Using a length parameter alllows us to accommodate for buffers of different sizes, making
// the code more adaptable for other uses. Passing the length explicitly also makes it so we don't need the use of
// functions like strlen(), which sometimes isn't allowed for certain reasons. Passing the length explicitly also allows us to 
// check for possible errors, such as if the given length is longer than the buffer, etc.