int getInput(char* str);
int processString(char* str, char** parsed, char** parsedpipe);
// void parseWords(char* str, char** parsed);
// int commandHandler(char** parsed);
void execArgs(char** parsed);
void execArgsPiped(char** parsed, char** parsedpipe);