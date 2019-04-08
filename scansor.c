#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

void
TokenScan (
	FILE *in, 
	FILE *out,
	FILE *inHTML,
	FILE *outHTML
	);
	
void token (
	FILE *in, 
	FILE *out,
	FILE *inHTML,
	FILE *outHTML
	);

void tokeRelace (
    char *in
    );
    
void 
WritOneLine (
	char *buff,
	FILE *outHTML
	);
static 
void 
insertSort (
    char *ararylist[],
    int arraySzie
    );
    
static 
int 
queryKey (
    char *ararylist[],
    int arraySzie,
    char *key
    );
static void testInsertSork();
static void testQuery();


static char oneLineBuffer[2048];
//static int oneLineBufferSize = 0;
//static int oneLineBufferPointer = 0;
static char stringBuilder[2048];

static int reamder = 0;


static char tempBuffer[1024];
static unsigned int pointer = 0;
static unsigned int size = 0;

static void initHtmlHeader(FILE *in, FILE *out);
static void initHtmltail(FILE *in, FILE *out);


//
// This Maroc is a bad define.
//

#define WRITE(buff, size) do {				\
		fwrite(buff, 1, size, outHTML);		\
 } while(0);
		
void WriteType (
	int type,
	char *buffer,
	char *outbuffer
	);

#define CTYPE_MAROC	    (0)
#define CTYPE_KEYWD	    (1)
#define CTYPE_NUMBER	(2)
#define CTYPE_OPREATOR	(3)
#define CTYPE_STRING	(4)
#define CTYPE_COMMENT   (5)

//
//  log(n)
//
static char *C_KEY_WORDS[] = {
    "auto",     "break",    "case",     "char",     "const",    "continue",     "default",  "do",
    "double",   "else",     "enum",     "extern",   "float",    "for",          "go",       "if",
    "int",      "long",     "register", "return",   "short",    "signed",       "sizeof",   "static",
    "struct",   "switch",   "typedef",  "union",    "unsigned", "void",         "volatile", "while",
    
//
// for c11 extends.
//
#ifdef C11_KEY_DEFINE

//
// C11 INCLUDE C99
//
#define C99_KEY_DEFINE
    "_Alignas",    "_Alignof",    "_Static_assert",    "_Noreturn",    "_Thread_local",
#endif
//
// for c99 extends.
//
#ifdef C99_KEY_DEFINE
    "inline",   "restrict", "_Bool",    "_Complex", "_Imaginary", 
#endif

//
// for MicroSolfWare C Compiler extends.
//
#ifdef MSC_EXTENDS_KEY_WORDS
    "__asm",
#endif 

//
// for LLVM Clang extends.
// 
#ifdef GNU_GCC_EXTENDS_KEY_WORDS
#endif

//
// for GNU GCC extends.
//
#ifdef CLANG_CC_EXTENDS_KEY_WORDS
#endif 

//
// for user costomer Key Words.
//
#ifdef USER_COUSTOMER_KEY_WORDS
    "printf"
#endif 
};

static 
void 
insertSort (
    char *ararylist[],
    int arraySzie
    )
{
    int i;
    int j;
    char *tmp;
    assert(ararylist);
    
    for (i = 0; i < arraySzie; i++) {
        tmp= ararylist[i];
#ifdef  DBG
        printf("%d need insert int\n", j);
#endif //DBG
        for (j = i; j > 0; j--) {
            if (strcmp(tmp, ararylist[j - 1]) < 0) {
                ararylist[j] = ararylist[j - 1];
            } else {
                break;
            }
        }
#ifdef  DBG
        printf("%d insert int\n", j);
#endif //DBG
        ararylist[j] = tmp;
    }
    
}

static void testInsertSork()
{
    int i;
    
    insertSort(C_KEY_WORDS, sizeof(C_KEY_WORDS) / sizeof(char *));
    
    printf("%d key words\n\n", sizeof(C_KEY_WORDS) / sizeof(char *));
    
    for (i = 0; i < sizeof(C_KEY_WORDS) / sizeof(char *); i++) {
        printf("%s\n", C_KEY_WORDS[i]);
    }
    return ;
}

//
// queryKey function return values.
//
#define STATUS_FIND     (1)
#define STATUS_NOFIND   (0)

static 
int 
queryKey (
    char *ararylist[],
    int arraySzie,
    char *key
    )
{
    int middle;
    int left;
    int right;
    int result;
    
    assert(ararylist);
    assert(key);
    
    result = 0;
    left = 0;
    right = arraySzie - 1;
    
    while (left <= right) {
        middle = (left + right) / 2;
#ifdef DBG
        printf("left : %d, right : %d, middle :  %d\n", left, right, middle);
#endif 
        result = strcmp(key, ararylist[middle]);
        if (result == 0) {
            return STATUS_FIND;
        } else if (result < 0) {
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }
    return STATUS_NOFIND;
}
static void testQuery()
{
    char *key = "int";
    if (queryKey(C_KEY_WORDS, sizeof(C_KEY_WORDS) / sizeof(char *), key)) {
        printf("find key %s\n", key);
    } else {
        printf("not find key %s\n", key);
    }
    key = "Int";
    if (queryKey(C_KEY_WORDS, sizeof(C_KEY_WORDS) / sizeof(char *), key)) {
        printf("find key %s\n", key);
    } else {
        printf("not find key %s\n", key);
    }
    key = "while";
    if (queryKey(C_KEY_WORDS, sizeof(C_KEY_WORDS) / sizeof(char *), key)) {
        printf("find key %s\n", key);
    } else {
        printf("not find key %s\n", key);
    }
}


int main(int argc, char *argv[])
{
	FILE *in;
 	FILE *out;
	FILE *inHTML;
	FILE *outHTML;
	
	char outName[256];
	char htmlName[256];
	
	if (argc != 2) {
		printf("Please input the file name\n");
		return 0;
	}
	in = fopen(argv[1], "r");
	
	
	if (in == NULL) {
		printf("open FILE %s False.\n", argv[1]);
		return 0;
	}
	inHTML = fopen("token.html", "r");
	
	if (inHTML == NULL) {
		printf("open FILE %s False.\n", "token.html");
		return 0;
	}
	
	outName[0] = 0;
	htmlName[0] = 0;
	
	strcat(outName, argv[1]);
	strcat(htmlName, argv[1]);
	
	strcat(outName, ".txt");
	strcat(htmlName, ".html");
	
	
	out = fopen(outName, "w");
	
	if (out == NULL) {
		printf("open FILE %s False.\n", outName);
		return 0;
	}
	outHTML = fopen(htmlName, "w");
	if (outHTML == NULL) {
		printf("open FILE %s False.\n", htmlName);
		return 0;
	}
    
    //
    // init the key words.
    //
    insertSort(C_KEY_WORDS, sizeof(C_KEY_WORDS) / sizeof(char *));
    
    
	initHtmlHeader(inHTML, outHTML);
	
	token(in, out, inHTML, outHTML);
	
	initHtmltail(inHTML, outHTML);
	
	fclose(in);
	fclose(out);
	fclose(inHTML);
	fclose(outHTML);
	
	
	return 0;
}

void token (
	FILE *in, 
	FILE *out,
	FILE *inHTML,
	FILE *outHTML
	)
{
	assert(in && out && inHTML && outHTML);
	TokenScan(in, out, inHTML, outHTML);
};

char tokenL[2];

//
// add a char number counter.
//

#define MAX_SIZE_BUFFER	(1024)
void
TokenScan (
	FILE *in, 
	FILE *out,
	FILE *inHTML,
	FILE *outHTML
	)
{
	int buffer_one_ptr;
	int buffer_two_ptr;
	char *bufferOne;
	char *bufferTwo;
	unsigned int sizeOfRead;
	int machine_status;
	int lineNo;
	char inputc;
    
    int sumCharNo = 0;
	//
	// bufferOne for read in
	// bufferTwo for save one token
	//
	

    
    

    
    
    
	bufferOne = (char *)malloc(MAX_SIZE_BUFFER);
	bufferTwo = (char *)malloc(MAX_SIZE_BUFFER);
	
	assert(bufferOne && bufferTwo);
	machine_status = 0;
	buffer_two_ptr = 0;
	lineNo = 0;
    
//
//  This var use for refernece.
//
//  static char oneLineBuffer[1024];
//  static int oneLineBufferSize = 0;
//  static int oneLineBufferPointer = 0;
//  static char stringBuilder[1024];
//
	oneLineBuffer[0] = 0;
    stringBuilder[0] = 0;
	do {
        
		sizeOfRead = fread(bufferOne, 1, MAX_SIZE_BUFFER, in);
		buffer_one_ptr = 0;
		
		while (buffer_one_ptr < sizeOfRead) {
			inputc = bufferOne[buffer_one_ptr];
			switch (machine_status) {
				case 0 : {
					if (isalpha((int)inputc & 0xff) || inputc == '_') {
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						machine_status = 1;
                        sumCharNo++;
                        
					} else if (isdigit((int)inputc & 0xff)) {
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						machine_status = 2;
                        sumCharNo++;
                        
					} else if (inputc == '/') {
						machine_status = 3;
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        sumCharNo++;
					} else if (inputc == '\'') {
						machine_status = 5;
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        sumCharNo++;
					} else if (inputc == '\"') {
						machine_status = 4;
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        sumCharNo++;
					} else if (inputc == '#') {
						machine_status = 6;
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        sumCharNo++;
					} else if (inputc == '(') {
                        
                        WriteType(CTYPE_OPREATOR, "(", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "lineno :%d, find token : %c\n", lineNo ,'(');
						buffer_one_ptr++;
                        
						sumCharNo++;

					} else if (inputc == ')') {
                        WriteType(CTYPE_OPREATOR, ")", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "line %d, find token : %c\n", lineNo, ')');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '[') {
                        WriteType(CTYPE_OPREATOR, "[", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
                        
						fprintf(out, "lineNo %d, find token : %c\n", lineNo ,'[');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == ']') {
                        WriteType(CTYPE_OPREATOR, "]", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "lineNo %d, find token : %c\n", lineNo ,']');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '-') {
                        WriteType(CTYPE_OPREATOR, "-", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "lineNo %d, find token : %c\n", lineNo,'-');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '.') {
                        WriteType(CTYPE_OPREATOR, ".", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "lineNo %d, find token : %c\n", lineNo ,'.');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '!') {
                        WriteType(CTYPE_OPREATOR, "!", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '!');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '~') {
                        WriteType(CTYPE_OPREATOR, "~", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '~');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '<') {
                        WriteType(CTYPE_OPREATOR, "&lt;", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '<');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '>') {
                        WriteType(CTYPE_OPREATOR, "&gt;", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", ':');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == ':') {
                        WriteType(CTYPE_OPREATOR, ":", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", ':');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '\?') {
                        WriteType(CTYPE_OPREATOR, "?", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '?');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '+') {
                        WriteType(CTYPE_OPREATOR, "+", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '+');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '*') {
                        WriteType(CTYPE_OPREATOR, "*", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '*');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '%') {
                        WriteType(CTYPE_OPREATOR, "%", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '%');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '=') {
                        WriteType(CTYPE_OPREATOR, "=", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '=');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '^') {
                        WriteType(CTYPE_OPREATOR, "^", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '^');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '&') {
                        WriteType(CTYPE_OPREATOR, "&amp;", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '&');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '|') {
                        WriteType(CTYPE_OPREATOR, "|", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '|');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '\t') {
                        reamder = 4 - (sumCharNo) % 4;
                        sumCharNo += reamder;
                        
                        while (reamder > 0) {
                            strcat(oneLineBuffer, "&nbsp;");
                            reamder--;
                        }
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %s\n", "table key");
						buffer_one_ptr++;
					} else if (inputc == '{') {
                        WriteType(CTYPE_OPREATOR, "{", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %c\n", '{');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '}') {
                        WriteType(CTYPE_OPREATOR, "}", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
                        
						fprintf(out, "find token : %c\n", '}');
						buffer_one_ptr++;
                        
                        sumCharNo++;
					} else if (inputc == '\n') {
                        WritOneLine(oneLineBuffer, outHTML);
                        tokeRelace(oneLineBuffer);
                        
						lineNo++;
						buffer_one_ptr++;
                        
                        sumCharNo = 0;
					} else if (inputc == ' ') {
                        //WriteType(CTYPE_OPREATOR, "&nbsp;", stringBuilder);
                        strcat(oneLineBuffer, "&nbsp;");
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %s\n", "space");
						buffer_one_ptr++;
                        
                        sumCharNo++;
                    } else if (inputc == ';') {
                        WriteType(CTYPE_OPREATOR, ";", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %s\n", ";");
						buffer_one_ptr++;
                        
                        sumCharNo++;
                    } else if (inputc == ',') {
                        WriteType(CTYPE_OPREATOR, ",", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token : %s\n", ";");
						buffer_one_ptr++;
                        
                        sumCharNo++;
                    } else {
                        strcat(oneLineBuffer, "&nbsp;");
                        
						buffer_one_ptr++;
                        
                        sumCharNo++;
					}
					break;
				}
				case 1 : {
					if (isalpha((int)inputc & 0xff) || isdigit((int)inputc & 0xff) || inputc == '_') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        
                        sumCharNo++;
					} else {
						bufferTwo[buffer_two_ptr] = 0;
                        if(queryKey(C_KEY_WORDS, sizeof(C_KEY_WORDS) / sizeof(char *), bufferTwo)) {
                            
                            WriteType(CTYPE_KEYWD, bufferTwo, stringBuilder);
                            strcat(oneLineBuffer, stringBuilder);
                            stringBuilder[0] = 0;
                            
                            fprintf(out, "find token system define identfiler %s\n", bufferTwo);
                        } else {
                            strcat(oneLineBuffer, bufferTwo);
                            stringBuilder[0] = 0;
                            fprintf(out, "find token user define identfiler %s\n", bufferTwo);
                        }
						machine_status = 0;
						buffer_two_ptr = 0;
					}
					break;
					
				}
				case 2 : {
					if (isdigit((int)inputc & 0xff)|| isalpha((int)inputc & 0xff) || inputc =='.') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        
                        sumCharNo++;
					} else {
						bufferTwo[buffer_two_ptr] = 0;
                        
                        WriteType(CTYPE_NUMBER, bufferTwo, stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
                        
						fprintf(out, "find token number %s\n", bufferTwo);
						machine_status = 0;
						buffer_two_ptr = 0;
					}
					break;
				}
				case 3 : {
					if (inputc == '/') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						machine_status = 7;
                        
                        sumCharNo++;
					} else if (inputc == '*') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						machine_status = 8;
                        
                        sumCharNo++;
					} else {
                        WriteType(CTYPE_OPREATOR, "/", stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						fprintf(out, "find token %c\n", '/');
						machine_status = 0;
						buffer_two_ptr = 0;
					}
					break;
				}
				case 4 : {
					if (inputc == '\"') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        
                        sumCharNo++;
                        
                        if (buffer_two_ptr >= 2 && bufferTwo[buffer_two_ptr-2] == '\\') {
                            //
                            // nothing to do.
                            //
                        } else {
                            bufferTwo[buffer_two_ptr] = 0;
                            
                            WriteType(CTYPE_STRING, bufferTwo, stringBuilder);
                            strcat(oneLineBuffer, stringBuilder);
                            stringBuilder[0] = 0;
                            
                            fprintf(out, "find token string %s\n", bufferTwo);
                            machine_status = 0;
                            buffer_two_ptr = 0;
                        }
					} else if (inputc == '\n') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						bufferTwo[buffer_two_ptr] = 0;
						lineNo++;
						if (buffer_two_ptr >= 2 && bufferTwo[buffer_two_ptr-2] == '\\') {
							fprintf(out, "find token no full string %s\n", bufferTwo);
						} else {
							machine_status = 0;
							fprintf(out, "find token no finished string %s\n", bufferTwo);
						}
                        
                        WriteType(CTYPE_STRING, bufferTwo, stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        
                        tokeRelace(oneLineBuffer);
                        
                        stringBuilder[0] = 0;
                        WritOneLine(oneLineBuffer, outHTML);
                        
                        
						buffer_two_ptr = 0;
                        
                        sumCharNo = 0;
					} else {
                        if (inputc == '<') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'l';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '>') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'g';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == ' ') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'n';
                            bufferTwo[buffer_two_ptr + 2] = 'b';
                            bufferTwo[buffer_two_ptr + 3] = 's';
                            bufferTwo[buffer_two_ptr + 4] = 'p';
                            bufferTwo[buffer_two_ptr + 5] = ';';
                            buffer_two_ptr += 6;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '\t') {
                            buffer_one_ptr++;
                            bufferTwo[buffer_two_ptr] = 0;
                            reamder = 4 - (sumCharNo) % 4;
                            sumCharNo += reamder;
                            while (reamder > 0) {
                                strcat(bufferTwo, "&nbsp;");
                                reamder--;
                                buffer_two_ptr += 6;
                            }
                        } else {
                            bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                            
                            sumCharNo++;
                        }
					}
					break;
				}
				case 5 : {
					if (inputc == '\'') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        if (buffer_two_ptr >= 2 && bufferTwo[buffer_two_ptr-2] == '\\') {
                            //
                            // nothing to do.
                            //
                        } else {
                            bufferTwo[buffer_two_ptr] = 0;
                            
                            WriteType(CTYPE_STRING, bufferTwo, stringBuilder);
                            strcat(oneLineBuffer, stringBuilder);
                            stringBuilder[0] = 0;
                            
                            fprintf(out, "find token little string %s\n", bufferTwo);
                            machine_status = 0;
                            buffer_two_ptr = 0;
                        }
                        
                        sumCharNo++;
					} else if (inputc == '\n') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						bufferTwo[buffer_two_ptr] = 0;
						lineNo++;
						if (buffer_two_ptr >= 2 && bufferTwo[buffer_two_ptr-2] == '\\') {
							fprintf(out, "find token no full little string %s\n", bufferTwo);
						} else {
							machine_status = 0;
							fprintf(out, "find token no finished little string %s\n", bufferTwo);
						}
                        
                        WriteType(CTYPE_STRING, bufferTwo, stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
                        tokeRelace(oneLineBuffer);
                        
                        WritOneLine(oneLineBuffer, outHTML);
                        
						buffer_two_ptr = 0;
                        
                        sumCharNo = 0;
					} else {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
                        
                        if (inputc == '<') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'l';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '>') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'g';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == ' ') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'n';
                            bufferTwo[buffer_two_ptr + 2] = 'b';
                            bufferTwo[buffer_two_ptr + 3] = 's';
                            bufferTwo[buffer_two_ptr + 4] = 'p';
                            bufferTwo[buffer_two_ptr + 5] = ';';
                            buffer_two_ptr += 6;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '\t') {
                            buffer_one_ptr++;
                            bufferTwo[buffer_two_ptr] = 0;
                            reamder = 4 - (sumCharNo) % 4;
                            sumCharNo += reamder;
                            while (reamder > 0) {
                                strcat(bufferTwo, "&nbsp;");
                                reamder--;
                                buffer_two_ptr += 6;
                            }
                        } else {
                            bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        
                            sumCharNo++;
                        }
					}
					break;
				}
				case 6 : {
					if (inputc == '\n') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						bufferTwo[buffer_two_ptr] = 0;
						lineNo++;
						if (buffer_two_ptr >= 2 && bufferTwo[buffer_two_ptr-2] == '\\') {
							fprintf(out, "find token mutil line marco %s\n", bufferTwo);
						} else {
							machine_status = 0;
							fprintf(out, "find token single line marco %s\n", bufferTwo);
						}
                        
                        WriteType(CTYPE_MAROC, bufferTwo, stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
                        tokeRelace(oneLineBuffer);
                        
                        WritOneLine(oneLineBuffer, outHTML);
                        
						buffer_two_ptr = 0;
                        
                        sumCharNo = 0;
					} else {
                        if (inputc == '<') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'l';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '>') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'g';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == ' ') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'n';
                            bufferTwo[buffer_two_ptr + 2] = 'b';
                            bufferTwo[buffer_two_ptr + 3] = 's';
                            bufferTwo[buffer_two_ptr + 4] = 'p';
                            bufferTwo[buffer_two_ptr + 5] = ';';
                            buffer_two_ptr += 6;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '\t') {
                            buffer_one_ptr++;
                            bufferTwo[buffer_two_ptr] = 0;
                            reamder = 4 - (sumCharNo) % 4;
                            sumCharNo += reamder;
                            while (reamder > 0) {
                                strcat(bufferTwo, "&nbsp;");
                                reamder--;
                                buffer_two_ptr += 6;
                            }
                        } else {
                            assert(buffer_two_ptr < MAX_SIZE_BUFFER);
                            bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                            
                            sumCharNo++;
                        }
					}
					
					break;
				}
				case 7 : {
					if (inputc == '\n') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						bufferTwo[buffer_two_ptr] = 0;
						lineNo++;
						fprintf(out, "find token single line comment %s\n", bufferTwo);
                        
                        
                        WriteType(CTYPE_COMMENT, bufferTwo, stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
                        tokeRelace(oneLineBuffer);
                        
                        WritOneLine(oneLineBuffer, outHTML);
                        
                        
						buffer_two_ptr = 0;
						machine_status = 0;
                        
                        sumCharNo = 0;
					} else {
                        
                         if (inputc == '<') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'l';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '>') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'g';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == ' ') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'n';
                            bufferTwo[buffer_two_ptr + 2] = 'b';
                            bufferTwo[buffer_two_ptr + 3] = 's';
                            bufferTwo[buffer_two_ptr + 4] = 'p';
                            bufferTwo[buffer_two_ptr + 5] = ';';
                            buffer_two_ptr += 6;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '\t') {
                            buffer_one_ptr++;
                            
                            bufferTwo[buffer_two_ptr] = 0;
                            reamder = 4 - (sumCharNo) % 4;
                            sumCharNo += reamder;
                            while (reamder > 0) {
                                strcat(bufferTwo, "&nbsp;");
                                reamder--;
                                buffer_two_ptr += 6;
                            }
                        } else {
                            assert(buffer_two_ptr < MAX_SIZE_BUFFER);
                            bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                            
                            sumCharNo++;
                        }
					}
					break;
				}
				case 8 : {
					if (inputc == '*') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						machine_status = 9;
                        
                        sumCharNo++;
                        
					} else if (inputc == '\n') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						bufferTwo[buffer_two_ptr] = 0;
						lineNo++;
						fprintf(out, "find token mutil line comment %s\n", bufferTwo);
                        
                        WriteType(CTYPE_COMMENT, bufferTwo, stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
                        tokeRelace(oneLineBuffer);
                        
                        WritOneLine(oneLineBuffer, outHTML);
                        
						buffer_two_ptr = 0;
                        
                        sumCharNo = 0;
					} else {
                        if (inputc == '<') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'l';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '>') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'g';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == ' ') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'n';
                            bufferTwo[buffer_two_ptr + 2] = 'b';
                            bufferTwo[buffer_two_ptr + 3] = 's';
                            bufferTwo[buffer_two_ptr + 4] = 'p';
                            bufferTwo[buffer_two_ptr + 5] = ';';
                            buffer_two_ptr += 6;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '\t') {
                            buffer_one_ptr++;
                            bufferTwo[buffer_two_ptr] = 0;
                            reamder = 4 - (sumCharNo) % 4;
                            sumCharNo += reamder;
                            while (reamder > 0) {
                                strcat(bufferTwo, "&nbsp;");
                                reamder--;
                                buffer_two_ptr += 6;
                            }
                        } else {
                            assert(buffer_two_ptr < MAX_SIZE_BUFFER);
                            bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                            
                            sumCharNo++;
                        }
					}
					break;
				}
				case 9 : {
					if (inputc == '/') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						bufferTwo[buffer_two_ptr] = 0;
						fprintf(out, "find token mutil line comment %s\n", bufferTwo);
                        
                        WriteType(CTYPE_COMMENT, bufferTwo, stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
						buffer_two_ptr = 0;
						machine_status = 0;
                        
                        sumCharNo++;
					} else if (inputc == '*') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                        
                        sumCharNo++;
					} else if (inputc == '\n') {
						assert(buffer_two_ptr < MAX_SIZE_BUFFER);
						bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
						bufferTwo[buffer_two_ptr] = 0;
						lineNo++;
						fprintf(out, "find token mutil line comment %s\n", bufferTwo);
                        
                        WriteType(CTYPE_COMMENT, bufferTwo, stringBuilder);
                        strcat(oneLineBuffer, stringBuilder);
                        stringBuilder[0] = 0;
                        
                        tokeRelace(oneLineBuffer);
                        
                        WritOneLine(oneLineBuffer, outHTML);
                        
						buffer_two_ptr = 0;
						machine_status = 8;
                        
                        sumCharNo = 0;
					} else {
                        if (inputc == '<') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'l';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '>') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'g';
                            bufferTwo[buffer_two_ptr + 2] = 't';
                            bufferTwo[buffer_two_ptr + 3] = ';';
                            buffer_two_ptr += 4;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == ' ') {
                            bufferTwo[buffer_two_ptr    ] = '&';
                            bufferTwo[buffer_two_ptr + 1] = 'n';
                            bufferTwo[buffer_two_ptr + 2] = 'b';
                            bufferTwo[buffer_two_ptr + 3] = 's';
                            bufferTwo[buffer_two_ptr + 4] = 'p';
                            bufferTwo[buffer_two_ptr + 5] = ';';
                            buffer_two_ptr += 6;
                            buffer_one_ptr++;
                            
                            sumCharNo++;
                        } else if (inputc == '\t') {
                            buffer_one_ptr++;
                            bufferTwo[buffer_two_ptr] = 0;
                            reamder = 4 - (sumCharNo) % 4;
                            sumCharNo += reamder;
                            while (reamder > 0) {
                                strcat(bufferTwo, "&nbsp;");
                                reamder--;
                                buffer_two_ptr += 6;
                            }
                        } else {
                            assert(buffer_two_ptr < MAX_SIZE_BUFFER);
                            bufferTwo[buffer_two_ptr++] = bufferOne[buffer_one_ptr++];
                            machine_status = 8;
                            
                            sumCharNo++;
                        }
					}
					
				}
			}
		}
	} while (sizeOfRead == MAX_SIZE_BUFFER);
	
	free(bufferOne);
	free(bufferTwo);
	printf("LineNo : %d\n", lineNo + 1);
	
}
static void initHtmlHeader(FILE *in, FILE *out)
{
	int flag;
	
	flag = 0;
	
	do {
		pointer = 0;
		size = fread(tempBuffer, 1, 1024, in);
		while (pointer < size) {
			if (tempBuffer[pointer] == '$') {
				flag = 1;
				break;
			}
			pointer++;
		}
		if (flag) {
			fwrite(tempBuffer, 1, pointer, out);
			break;
		} else {
			fwrite(tempBuffer, 1, size, out);
		}
		
	} while (size == 1024);
	
	
	//
	// if we don't find the $ signal. than assert false.
	//
	assert(flag);
	
}
static void initHtmltail(FILE *in, FILE *out)
{
	fwrite(&tempBuffer[pointer + 1], 1, size - pointer - 1, out);
	
	do {
		size = fread(tempBuffer, 1, 1024, in);
		fwrite(tempBuffer, 1, size, out);
	} while (size == 1024);
	
}
//
//#define CTYPE_MAROC		(0)
//#define CTYPE_KEYWD		(1)
//#define CTYPE_NUMBER		(2)
//#define CTYPE_OPREATOR	(3)
//#define CTYPE_STRING		(4)
//#define CTYPE_COMMENT     (5)
//
void WriteType (
	int type,
	char *buffer,
	char *outbuffer
	)
{
    assert(buffer != outbuffer);
    
	switch (type) {
		case CTYPE_MAROC : {
			sprintf(outbuffer, "%s%s%s", "<span class=\"maroc\">", buffer, "</span>");
			break;
		}
		case CTYPE_KEYWD : {
			sprintf(outbuffer, "%s%s%s", "<span class=\"key\">", buffer, "</span>");
			break;
		}
		case CTYPE_NUMBER : {
			sprintf(outbuffer, "%s%s%s", "<span class=\"number\">", buffer, "</span>");
			break;
		}
		case CTYPE_OPREATOR : {
			sprintf(outbuffer, "%s%s%s", "<span class=\"op\">", buffer, "</span>");
			break;
		}
		case CTYPE_STRING : {
			sprintf(outbuffer, "%s%s%s", "<span class=\"string\">", buffer, "</span>");
			break;
		}
        case CTYPE_COMMENT : {
			sprintf(outbuffer, "%s%s%s", "<span class=\"comment\">", buffer, "</span>");
			break;
        }
		default:
			break;
	}
		
}
void 
WritOneLine (
	char *buff,
	FILE *outHTML
	)
{
	char one[] = "    <div class=\"online\">";
	char two[] = "    </div>\n";
	WRITE(one, sizeof(one) - 1);
	WRITE(buff, strlen(buff));
	WRITE(two, sizeof(two) - 1);
    
    buff[0] = 0;
	
}

void tokeRelace (
    char *in
    )
{
    int i;
    int size;
    int j;
    
    
    char *p;
    
    
    return ;
    
    j = 0;
    p = (char *)malloc(2048);
    
    assert(p);
    
    p[0] = 0;
    
    
    size = strlen(in);
    
    
    strcat(p, in);
    
    in[0] = 0;
    
    
    for (i = 0; i < size; i++) {
        if (p[i] == '<') {
            in[j    ] = '&';
            in[j + 1] = 'l';
            in[j + 2] = 't';
            in[j + 3] = ';';
            j += 4;
        } else if (p[i] == '>') {
            in[j    ] = '&';
            in[j + 1] = 'g';
            in[j + 2] = 't';
            in[j + 3] = ';';
            j += 4;
        } /*else if (p[i] == '\"') {
            in[j    ] = '&';
            in[j + 1] = 'q';
            in[j + 2] = 'u';
            in[j + 3] = 'o';
            in[j + 4] = 't';
            j += 5;
        }*/ else {
            in[j++] = p[i];
        }
        i++;
    }
    in[j] = 0;
}
