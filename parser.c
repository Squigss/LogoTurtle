/*================NOTES========================
The parser and interpreter have been split out
into 2 files. Some functions are used by both
the parser and interpreter. All of these shared
functions are contained within this file, the
parser file, as these common functions relate
to pure grammar.

The parser and interpreter are run through twice.
The first time checking for any errors and if no
errors are found the SDL screen prints the Turtle

! DO instructions. For example: "DO A FROM 1 TO 5"
will be executed 4 times
===============================================*/
#include "header.h"

int main(int argc, char** argv)
{
   Program* program;
   FILE* fw;
   SDL_Simplewin sw;


   if(argc != 2) {
      printf("Enter at least 1 file name\n");
   }

   fw = fopen(argv[1], "r");
   if(!fw){
      fprintf(stderr, "Can't open %s\n", argv[1]);
      exit(1);
   }

   /*Creates memory for program*/
   program = create_mem();
   num_instructions(fw, program);
   mem_for_instrs(program);
   fclose(fw);

   /*inserts instructions into memory*/
   fw = fopen(argv[1], "r");
   if(!fw) {
      fprintf(stderr, "Can't open %s\n", argv[1]);
      exit(1);
   }
   save_instr(fw, program);
   fclose(fw);


   Neill_SDL_Init(&sw);
   test_all(program, fw);
   Prog(program, &sw);


   SDL_DestroyTexture(sw.display);
   SDL_DestroyRenderer(sw.renderer);
   SDL_DestroyWindow(sw.win);
   SDL_Quit();
   atexit(SDL_Quit);

   free_prog(&program);

   return 0;
}

/*====================================
MEMORY & INSTRUCTION STORAGE FUNCTIONS
======================================*/
/*Initialises 'Program' structure*/
Program* create_mem()
{
   Program* p;
   Instructs* i;
   int j = 0;

   i =(Instructs*)malloc(1 * sizeof(Instructs));
   if(i == NULL) {
      printf("No memory has been allocated to "
             "save the instructions\n");
   }

   p = (Program*)malloc(1 * sizeof(Program));
   if(p == NULL) {
      printf("No memory has been allocated for "
             "the program\n");
   }

   /*ci= Current instruction counter,
   ni= Number of instructions in program*/
   p->ci= 0;
   p->ni= 0;
   p->errors= 0;
   p->set_errors= 0;
   p->do_errors= 0;
   /*Turtle coordinates(starts at center of screen)
   and d (distance) and angle to calcuate new
   coordinates.*/
   p->x0=0;
   p->y0=0;
   p->x= WWIDTH/2;
   p->y= WHEIGHT/2;
   p->d= 0;
   p->angle= 0;

   /*array to hold value of variable numbers*/
   for(j=0; j<LETTERS; j++) {
      p->var_val[j]=0;
   }

   return p;
}

/*Counts the number of instructions in program*/
bool num_instructions(FILE* fw, Program *p)
{
   char temp[MAXLEN*2];

   while(fscanf(fw, "%s", temp)==1) {
      p->ni +=1;
      if(strlen(temp) >= MAXLEN) {
         printf("\n'%s' is an invalid number.\n"
                "Numbers need to be smaller than "
                "100 million.\n\n", temp);
         exit(1);
      }
      printf(" ");
   }
   /*Something strange happening here but
   couldn't find how to fix it on time.*/
   printf("\n");

   if (p->ni==0) {
      p->errors++;
      return FALSE;
   }
   return TRUE;
}

/*Creates memory for each instruction*/
void mem_for_instrs(Program *p)
{
   int i;

   p->list=(Instructs**)malloc
           ((p->ni+1)*sizeof(Instructs*));
   if(p->list == NULL) {
      printf("Pointer array not created\n");
   }

   /*1 additional space for instructions than
   necessary is allocated to account for situations
   where the closing bracket is missing.
   N.B. The program inserts a closing bracket if
   there is one missing so the recursion works.*/
   for (i=0; i < (p->ni+1) ; i++) {
      p->list[i]=(Instructs*)malloc
                 (MAXLEN * sizeof(Instructs));
      if(p->list[i] == NULL) {
         printf("Empty word array not created\n");
      }
   }
}

/*Saves each instruction and the line number
that instruction is on in the text file*/
void save_instr(FILE* fw, Program *p)
{
   int i=0;
   char new_line;
   int line_num=1;
   char str[MAXLEN];

   /*The instructions get saved each as seperate
   strings so that lines can be jumped when writing
   the LOGO program to make it more readable.
   Saving the line is just for error messages*/
   while(fscanf(fw, "%s%c", str, &new_line)!=EOF) {
      strcpy(p->list[i]->instr, str);
      p->list[i]->line = line_num;
      i++;
      if(new_line=='\n') {
         line_num++;
      }
   }
   /*Assigns a line num to the additional
   instruction space at the end in case
   a closing bracket needs to be inserted*/
   p->list[i]->line = line_num+1;
}

/*===================================
CHECKS VALIDITY OF THE WHOLE PROGRAM
=====================================*/
/*Formal Grammar: <MAIN>::="{"<INSTRLIST>*/
void Prog(Program *p, SDL_Simplewin *sw)
{
   if(!strsame(p->list[p->ci]->instr, "{")) {
      p->errors++;
      printf("No begining instruction. Start "
             "program with '{'.\n\n");
   }

   if(!strsame(p->list[p->ni-1]->instr, "}")) {
      p->errors++;
      printf("No closing bracket. A '}' has been "
             "added for you at the end of the "
             "program.\n\n");
     /*Inserts closing bracket if missing at the
     end of the LOGO program to ensure recursion
     in the instruclist function works properly*/
     p->ni++;
     strcpy(p->list[p->ni-1]->instr, "}");
   }
   check_brackets(p);
   instrctlist(p);

   if (p->errors == 0) {
     p->ci=0;
     interpret_instrctlist(p, sw);
   }
   else {
      /*I wanted to free the memory here as we are Exiting
      but I could not find a way to do it with a segmentation
      fault*/
      exit(1);
   }
}

/*==============================================
CHECKS EACH INSTRUCTION IN THE INSTRUCTION LIST
================================================*/
/*Formal Grammar::=<INSTRUCTION><INSTRCTLIST>|"}"*/
void instrctlist(Program *p)
{
   if (p->ci == p->ni) {
      return;
   }
   else if(strsame(p->list[p->ci]->instr, "}")) {
      return;
   }
   instruction(p);
   p->ci++;
   instrctlist(p);
}

void instruction(Program *p)
{
   check_FD_LT_RT(p);
   check_DO(p);
   check_SET(p);
   not_valid_instr(p);
}

/*=========================
 DEFINITION OF INSTRUCTIONS
===========================*/
/*Formal Grammar: <FD>::="FD" <VARNUM>*/
/*Formal Grammar: <LT>::="LT" <VARNUM>*/
/*Formal Grammar: <RT>::="RT" <VARNUM>*/
int check_FD_LT_RT(Program *p)
{
   /*Count is used for testing only.
   count should = 1 at end of function.*/
   int count = 0;

   if(strsame(p->list[p->ci]->instr, "FD") ||
      strsame(p->list[p->ci]->instr, "LT") ||
      strsame(p->list[p->ci]->instr, "RT")) {
      p->ci++;
      count++;
      validate_VARNUM(p);

      /*checking there are no stray characters after
      the instruction*/
      if (!strsame(p->list[p->ci+1]->instr, "}")) {

         p->ci++;
         if (key_words(p)==NO) {
            p->errors++;
            printf("Line %d: '%s' is not valid. The "
                "instruction '%s %s' needs to be followed "
                "by:\n - another instruction ('FD', 'DO', "
                "'RT', 'LT' or 'SET') OR\n - by a closing "
                "closing bracket '}'\n\n",
                p->list[p->ci-1]->line, p->list[p->ci]->instr,
                p->list[p->ci-2]->instr, p->list[p->ci-1]->instr);
        }
        p->ci--;
     }
     return count;
   }
   return count;
}

/*Formal Grammar: <DO>::="DO" <VAR> "FROM"
<VARNUM> "TO <VARNUM> "{" <INSTRCTLIST>*/
int check_DO(Program *p)
{
  /*Count is used for testing only.
  count should = 6 at end of function.*/
   int count = 0;

   if(strsame(p->list[p->ci]->instr, "DO")) {
      p->ci += 1;
      count++;
      validate_VAR(p);
      p->ci += 1;
      count++;
      validate_FROM(p);
      p->ci += 1;
      count++;
      validate_VARNUM(p);
      p->ci += 1;
      count++;
      validate_TO(p);
      p->ci += 1;
      count++;
      validate_VARNUM(p);
      p->ci += 1;
      count++;

      /*Checks if followed by an opening braket*/
      if (!strsame(p->list[p->ci]->instr, "{")) {
         p->errors++;
         printf("Line %d: Missing an opening '{' "
                "following the 'DO' statement.\n\n",
                 p->list[p->ci-1]->line);
      }
      else {
         p->ci++;
         count++;
      }
      instrctlist(p);
      return count;
   }
   return count;
}

/*Formal Grammar: <SET>::="SET"<VAR>":="<POLISH>*/
void check_SET(Program *p)
{
   if(strsame(p->list[p->ci]->instr, "SET")) {
      p->ci++;
      validate_VAR(p);
      p->ci++;
      validate_equal(p);
      p->ci++;
      POLISH(p);
   }
}

/*===============
OTHER DEFINITIONS
=================*/
/*Formal Grammar: <OP> ::="+"|"-"|"*"|"/"*/
int OP(Program *p)
{
   if((strsame(p->list[p->ci]->instr, "+")) ||
      (strsame(p->list[p->ci]->instr, "-")) ||
      (strsame(p->list[p->ci]->instr, "*")) ||
      (strsame(p->list[p->ci]->instr, "/"))) {
      return VALID;
   }
   return NOT_VALID;
}

/*Formal Grammar: <POLISH>::=<OP>
<POLISH> | <VARNUM><POLISH> | ";"*/
void POLISH(Program *p)
{
   if(strsame(p->list[p->ci]->instr, ";")) {
      return;
   }

   else if (key_punctuation(p)==VALID ||
            key_words(p)==VALID) {
      p->errors++;
      p->set_errors++;
      printf("Line %d: The instruction 'SET', "
             "line %d is missing an ending ';'."
             "\n\n", p->list[p->ci]->line,
             p->list[p->ci]->line);
      p->ci--;
      return;
   }

   else if(VARNUM(p)==VALID || OP(p)==VALID) {
      p->ci++;
   }
   else if (OP(p)==NOT_VALID) {
      p->errors++;
      p->set_errors++;
      printf("Line %d: '%s' is not a valid operator."
             "\nThe only valid operators are: '+', "
             "'-', '/' or '*'.\n\n",
             p->list[p->ci]->line,
             p->list[p->ci]->instr);
      p->ci++;
   }
   POLISH(p);

}

/*Formal grammar: <VARNUM> ::= number | <VAR>*/
int VARNUM(Program *p)
{
   int i, digit, str_len;

   str_len= strlen(p->list[p->ci]->instr);

   if (VAR(p)==VALID) {
      return VALID;
   }
   else {
      for (i=0; i<str_len; i++) {
         digit = (int)p->list[p->ci]->instr[i];
         if ((digit < ZERO || NINE < digit) &&
             digit!=DOT) {
            return NOT_VALID;
         }
      }
   }
   return VALID;
}

/*Formal Grammar: <VAR>::=A to Z letters*/
int VAR(Program *p)
{
   int letter;
   letter = p->list[p->ci]->instr[0];

   if ((strlen(p->list[p->ci]->instr)==1) &&
       (letter >= A && letter <= Z)) {
      return VALID;
   }
   return NOT_VALID;
}

int key_words(Program *p)
{
   if((strsame(p->list[p->ci]->instr, "FD")) ||
     (strsame(p->list[p->ci]->instr, "LT")) ||
     (strsame(p->list[p->ci]->instr, "RT")) ||
     (strsame(p->list[p->ci]->instr, "DO")) ||
     (strsame(p->list[p->ci]->instr, "SET")) ||
     (strsame(p->list[p->ci]->instr, "DO")) ||
     (strsame(p->list[p->ci]->instr, "FROM"))) {
      return YES;
   }
   return NO;
}

int key_punctuation(Program *p)
{
   if((strsame(p->list[p->ci]->instr, ":=")) ||
     (strsame(p->list[p->ci]->instr, ";")) ||
     (strsame(p->list[p->ci]->instr, "{")) ||
     (strsame(p->list[p->ci]->instr, "}"))) {
      return YES;
   }
   return NO;
}

/*==============
ERROR MESSAGING
================*/
void validate_VARNUM(Program *p)
{
   if(VARNUM(p)==NOT_VALID) {
      p->errors++;
      p->set_errors++;
      p->do_errors++;
      printf("Line %d: '%s' is not a valid "
             "number or letter.\nVariables are a "
             "single uppercase letter or a "
             "positive integer number."
             "\n\n", p->list[p->ci]->line,
             p->list[p->ci]->instr);
      varnum_omitted(p);
   }
}

void validate_VAR(Program *p)
{
   if (VAR(p)==NOT_VALID) {
      p->errors++;
      p->set_errors++;
      p->do_errors++;
      printf("Line %d: '%s' is an invalid letter. "
             "Enter a single uppercase letter.\n\n",
             p->list[p->ci]->line,
             p->list[p->ci]->instr);
      varnum_omitted(p);
   }
}

int varnum_omitted(Program *p)
{
   char str[MAXLEN];

   strcpy(str, p->list[p->ci]->instr);
   if (key_punctuation(p)==YES || key_words(p)==YES) {
      p->errors++;
      p->set_errors++;
      p->do_errors++;
      printf("Line %d: There might be missing a "
             "variable number or letter after\n"
             "instruction '%s' or the instruction "
             "and variable number or letter\nmay have "
             "been inverted. Make sure instructions "
             "'SET', 'DO', 'FD', 'LT', 'RT'\nare "
             "followed by a letter or number.\n\n",
             p->list[p->ci-1]->line,
             p->list[p->ci-1]->instr);
      p->ci--;
      return NO;
   }
   return YES;
}

int validate_FROM(Program *p)
{
   if(!strsame(p->list[p->ci]->instr, "FROM")) {
      p->errors++;
      printf("Line %d: did you mean 'FROM' instead "
             "of '%s?'.\n\n", p->list[p->ci]->line,
             p->list[p->ci]->instr);
      return NOT_VALID;
   }
   return VALID;
}

int validate_TO(Program *p)
{
   if(!strsame(p->list[p->ci]->instr, "TO")) {
      if(VARNUM(p)==VALID) {
         p->errors++;
         p->do_errors++;
         printf("Line %d: it looks like the instruction 'TO'"
                " is missing from the 'DO' statement.\n\n",
                p->list[p->ci]->line);
         p->ci--;
         return NOT_VALID;
      }
      else {
        p->errors++;
        printf("Line %d: did you mean 'TO' instead "
             "of '%s'?.\n\n", p->list[p->ci]->line,
              p->list[p->ci]->instr);
        return NOT_VALID;
      }
   }
   return VALID;
}

int validate_equal(Program *p)
{
   int current_inst= p->ci;

   while (key_punctuation(p)==NOT_VALID) {
      p->ci++;
   }

   /*SET: Makes sure there is only 1 VAR before the :=*/
   if (strsame(p->list[p->ci]->instr, ":=") &&
      (p->ci != current_inst)) {
      p->errors++;
      p->set_errors++;
      printf("Line %d: Error before the sign ':='.\n"
             "You can only set one variable letter "
             "at a time.\n\n", p->list[p->ci]->line);
      return NOT_VALID;
   }

   else if (strsame(p->list[p->ci]->instr, ";") ||
            strsame(p->list[p->ci]->instr, "}") ||
            strsame(p->list[p->ci]->instr, "{")) {
      p->errors++;
      p->set_errors++;
      printf("Line %d: You need a ':=' to do a SET "
            "instruction.\n\n", p->list[p->ci]->line);
      p->ci= current_inst;
      return NOT_VALID;
   }
   return VALID;
}

int not_valid_instr(Program *p)
{
   int punctuation_err = -1;
   int lowercase_err = -2;
   int invalid_instr_err = -3;

   if (VARNUM(p)==VALID) {
      return VALID;
   }
   else if (key_punctuation(p)==YES ||
            key_words(p)==YES || OP(p)==VALID) {
      return VALID;
   }
   else if((strsame(p->list[p->ci]->instr, "[")) ||
          (strsame(p->list[p->ci]->instr, "]")) ||
          (strsame(p->list[p->ci]->instr, "(")) ||
          (strsame(p->list[p->ci]->instr, ")"))) {
      p->errors++;
      printf("Line %d: '%s' is an invalid character. "
             "Did you mean '{' or '}' instead?\n\n",
           p->list[p->ci]->line, p->list[p->ci]->instr);
      return punctuation_err;
   }
   else if ((strsame(p->list[p->ci]->instr, "fd")) ||
            (strsame(p->list[p->ci]->instr, "lt")) ||
            (strsame(p->list[p->ci]->instr, "rt")) ||
            (strsame(p->list[p->ci]->instr, "do")) ||
            (strsame(p->list[p->ci]->instr, "set"))) {
      p->errors++;
      printf("Line %d: instruction '%s' needs to be "
             "written in uppercase letters.\n\n",
             p->list[p->ci]->line, p->list[p->ci]->instr);
      return lowercase_err;
   }
   else {
      p->errors++;
      printf("Line %d: Only FD, LT, RT, DO and "
             "SET are valid instructions.\nDid "
             "you mean one of those instead of "
             "'%s'?.\n\n", p->list[p->ci]->line,
             p->list[p->ci]->instr);
      return invalid_instr_err;
   }
   return VALID;
}

void check_brackets(Program *p)
{
   int i = 0;
   int open_brackets=0;
   int close_brackets=0;

   for (i=0; i<p->ni; i++) {
      if (strsame(p->list[i]->instr, "{")) {
         open_brackets++;
   }
      else if (strsame(p->list[i]->instr, "}")) {
         close_brackets++;
      }
   }

   if (close_brackets < open_brackets) {
      p->errors++;
      printf("FATAL ERROR! Missing closing brackets.\n\n");
      exit(1);
   }
   else if (open_brackets < close_brackets) {
      p->errors++;
      printf("missing opening brackets.\n\n");
   }
}

/*==========
FREES MEMORY
============*/
void free_prog(Program** p)
{
   Program* free_prog = *p;
   int i = 0;

   for (i=0; i<free_prog->ni; i++) {
      free(free_prog->list[i]->instr);
   }


   for (i=0; i<free_prog->errors; i++) {
      free(free_prog->message[i]->str);
   }
   free(free_prog->list);
   free(free_prog->message);
   free(free_prog);
}
