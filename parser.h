int getInput(char* str);
// void parseWords(char* str, char** parsed);
// int commandHandler(char** parsed);
void execArgs(char** parsed);
void execArgsPiped(char** parsed, char** parsedpipe);
void execOutRedirection(char** cmd, char** ouput_f);
void execInRedirection(char** cmd, char** input_f);
void execErrRedirection(char** cmd, char** ouput_f);
int checkSymbol(char* word);
void parseWords(char* str, char** parsed1);
// void processString(char* str, char** parsed, char** parsedpipe);