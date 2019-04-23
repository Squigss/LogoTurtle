
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <stdbool.h>

#define MAXLEN 10
#define MAXMESSAGE 200
#define MAXNESTEDLOOPS 10
#define LETTERS 26 /*26 capital letters can be VAR*/
#define VALID 1
#define NOT_VALID 0
#define YES 1
#define NO 0
#define TRUE 1
#define FALSE 0
#define ZERO 48 /*put these as an enum*/
#define ONE 49
#define NINE 57
#define DOT 46
#define ASCII 64
#define A 65  /*put these as an enum*/
#define Z 90
#define EMPTY -1
#define strsame(A,B) (strcmp(A, B)==0)

/*============SDL=======*/
#define RECTSIZE 10
#define MILLISECONDDELAY 20
#define LENGTH 300

#define WWIDTH 800
#define WHEIGHT 600
/* Font stuff*/
typedef unsigned short fntrow;
#define FNTWIDTH (sizeof(fntrow)*8)
#define FNTHEIGHT 18
#define FNTCHARS 96
#define FNT1STCHAR 32

#define SDL_8BITCOLOUR 256

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* All info required for windows / renderer & event loop */
struct SDL_Simplewin {
   SDL_bool finished;
   SDL_Window *win;
   SDL_Renderer *renderer;
   SDL_Texture *display;
   /*mine*/
   TTF_Font* font;
   SDL_Color color;
   SDL_Surface * surface;
};
typedef struct SDL_Simplewin SDL_Simplewin;
/*=========================================*/

struct instructions {
  char instr[MAXLEN];
  int line;
};
typedef struct instructions Instructs;

struct Error_messages {
   char str[MAXMESSAGE];
};
typedef struct Error_messages Error_messages;

struct DO_loops {
   int max;
   int var;
   int start;
   int end;
   int end_do_instr;
};
typedef struct DO_loops DO_loops;

struct prog {
  int ni; /*Number of instructions*/
  int ci; /*Current Instruction: refers to our position within the array*/
  Instructs **list;
  int pass;
  /*Error handling variables*/
  int errors;
  int set_errors;
  int do_errors;
  Error_messages **message;

  int do_start;
  int x0;
  int y0;
  int x; /*Coordinate x of the Turtle*/
  int y; /*Coordinate y of the Turtle*/
  int d; /*distance*/
  float angle;

  float var_val[LETTERS]; /*saves values for VARs*/
  DO_loops DO_arr[MAXNESTEDLOOPS];
  int cdl; /*tracks which Do Loop we are on when executing nested loops*/

};
typedef struct prog Program;

/*PROGRAM MEMORY FUNCTIONS*/
Program* create_mem();
bool num_instructions(FILE* fw, Program *p);
void mem_for_instrs(Program *p);
void save_instr(FILE* fw, Program *p);

/*MAIN, INSTRLIST & INSTRUCTION DEFINITIONS*/
void Prog(Program *p, SDL_Simplewin *sw);
void instrctlist(Program *p);
void instruction(Program *p);

/*INSTRUCTION DEFINITIONS*/
int check_FD_LT_RT(Program *p);
int check_DO(Program *p);
void check_SET(Program *p);

/*OTHER GRAMMAR DEFINITIONS*/
int OP(Program *p);
void POLISH(Program *p);
int VARNUM(Program *p);
int VAR(Program *p);
int key_words(Program *p);
int key_punctuation(Program *p);

/*ERROR MESSAGING FUNCTIONS*/
void validate_VARNUM(Program *p);
void validate_VAR(Program *p);
int validate_FROM(Program *p);
int validate_TO(Program *p);
int validate_equal(Program *p);
int not_valid_instr(Program *p);
int varnum_omitted(Program *p);
void check_brackets(Program *p);

/*INTERPRETER SPECIFIC FUNCTIONS*/
void Interpret(Program *p, SDL_Simplewin *sw);
void interpret_instrctlist(Program *p, SDL_Simplewin *sw);
void interpret_instruction(Program *p, SDL_Simplewin *sw);
void interpret_FD(Program *p, SDL_Simplewin *sw);
void interpret_RT_LT(Program *p);
void interpret_SET(Program *p);
int num_vars_in_SET(Program *s);
int no_ops_SET(Program *p, int top);
void interpret_DO(Program *p, SDL_Simplewin *sw);
void last_do_instruct_location(Program *p);
void DO_loop(Program *p, SDL_Simplewin *sw);
float varnum_to_int(Program *p, char *s);
int round_float(float x);

void free_prog(Program** p/*, Instructs **i*/);

/*SDL RELATED FUNCTION PROTOTYPES*/
void Neill_SDL_Init(SDL_Simplewin *sw);
void Neill_SDL_Events(SDL_Simplewin *sw);
void Neill_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b);
void Neill_SDL_RenderFillCircle(SDL_Renderer *rend, int cx, int cy, int r);
void Neill_SDL_RenderDrawCircle(SDL_Renderer *rend, int cx, int cy, int r);
void Neill_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT],
                        unsigned char chr, int ox, int oy);
void Neill_SDL_DrawString(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT],
                          char *str, int ox, int oy);
void Neill_SDL_ReadFont(fntrow fontdata[FNTCHARS][FNTHEIGHT], char *fname);
void Neill_SDL_UpdateScreen(SDL_Simplewin *sw);

void create_window(SDL_Simplewin *sw);
void display_text(SDL_Simplewin *sw, Program *p, int i);


/*TESTING FUNCTIONS*/
void test_all(Program *p, FILE *fw);
