/*=================== NOTES ========================
The program was tested with all of the Following
strings for each function to ensure the function
was returning the correct value.

Functions whose sole roles are to output error MESSAGES
where tested by ensuring the correct output message
was displayed for the error. See testing.txt for
example output error messages.
====================================================*/

#include "header.h"


void test_all(Program *p, FILE *fw)
{

   /*int num_instructions(FILE* fw, Program *p)*/
   if (num_instructions(fw, p) == FALSE) {
      printf("////PROGRAM ERROR///\n"
             "In Function: 'int num_instructions(FILE* fw, Program *p)',"
             "\nfailure to count instructions from file or file contains"
             "\nno instructions.\n");
      printf("\nValid LOGO programs need to contain at"
             " at least\none instruction preceeded by"
             " an opening bracket '{' and terminated"
             "\nby a closing bracket '}'.\n\n");
      exit(1);
   }

   /*int check_FD_LT_RT(Program *p)*/
   if(strsame(p->list[p->ci]->instr, "FD") ||
      strsame(p->list[p->ci]->instr, "LT") ||
      strsame(p->list[p->ci]->instr, "RT")) {
      if (check_FD_LT_RT(p) !=1) {
         printf("//////PROGRAM ERROR/////\n"
               "In Function: 'int check_FD_LT_RT(Program *p)'.\n"
               "Failure of program to increment by 1 to the "
               "next instructions.\n\n");
      }
   }

   /*int check_DO(Program *p)*/
   if(strsame(p->list[p->ci]->instr, "DO")) {
      if (check_DO(p) != 7) {
         printf("//////PROGRAM ERROR/////\n"
                "In Function: 'int check_DO(Program *p)'.\n"
                "Failure of program to increment by 6 to the "
                "next instructions.\n\n");
      }
   }

   /*OP FUNCTION ASSERTS*/
   if (strsame(p->list[p->ci]->instr, "*") ||
       strsame(p->list[p->ci]->instr, "/") ||
       strsame(p->list[p->ci]->instr, "+") ||
       strsame(p->list[p->ci]->instr, "-")) {
      assert(OP(p)==VALID);
   }

   if (strsame(p->list[p->ci]->instr, "=") ||
       strsame(p->list[p->ci]->instr, "4") ||
       strsame(p->list[p->ci]->instr, "_") ||
       strsame(p->list[p->ci]->instr, "x")) {
      assert(OP(p)==NOT_VALID);
   }

   /*VARNUM FUNCTION ASSERTS*/
   if (strsame(p->list[p->ci]->instr, "4") ||
       strsame(p->list[p->ci]->instr, "F") ||
       strsame(p->list[p->ci]->instr, "548234") ||
       strsame(p->list[p->ci]->instr, "Z")) {
      assert(VARNUM(p)==VALID);
   }

   if (strsame(p->list[p->ci]->instr, "-4") ||
       strsame(p->list[p->ci]->instr, "a") ||
       strsame(p->list[p->ci]->instr, "5.4") ||
       strsame(p->list[p->ci]->instr, "g4")) {
      assert(VARNUM(p)==NOT_VALID);
   }

   /*VAR FUNCTION ASSERTS*/
   if (strsame(p->list[p->ci]->instr, "A") ||
       strsame(p->list[p->ci]->instr, "Z") ||
       strsame(p->list[p->ci]->instr, "D") ||
       strsame(p->list[p->ci]->instr, "F")) {
      assert(VAR(p)==VALID);
   }

   if (strsame(p->list[p->ci]->instr, "AA") ||
       strsame(p->list[p->ci]->instr, "abd") ||
       strsame(p->list[p->ci]->instr, "c") ||
       strsame(p->list[p->ci]->instr, "10")) {
      assert(VAR(p)==NOT_VALID);
   }

   /*KEYWORD FUNCTION ASSERTS*/
   if (strsame(p->list[p->ci]->instr, "FD") ||
       strsame(p->list[p->ci]->instr, "LT") ||
       strsame(p->list[p->ci]->instr, "RT") ||
       strsame(p->list[p->ci]->instr, "DO") ||
       strsame(p->list[p->ci]->instr, "FROM") ||
       strsame(p->list[p->ci]->instr, "SET")) {
      assert(key_words(p)==VALID);
   }

   if (strsame(p->list[p->ci]->instr, "fd") ||
       strsame(p->list[p->ci]->instr, "FRO") ||
       strsame(p->list[p->ci]->instr, "T0") ||
       strsame(p->list[p->ci]->instr, "SD")) {
      assert(key_words(p)==NOT_VALID);
   }

   /*KEY UNCTION FUNCTION ASSERTS*/
   if((strsame(p->list[p->ci]->instr, ":=")) ||
     (strsame(p->list[p->ci]->instr, ";")) ||
     (strsame(p->list[p->ci]->instr, "{")) ||
     (strsame(p->list[p->ci]->instr, "}")))  {
      assert(key_punctuation(p)==VALID);
   }

   if (strsame(p->list[p->ci]->instr, "=") ||
       strsame(p->list[p->ci]->instr, ")") ||
       strsame(p->list[p->ci]->instr, "()") ||
       strsame(p->list[p->ci]->instr, "[") ||
       strsame(p->list[p->ci]->instr, "]") ||
       strsame(p->list[p->ci]->instr, ",") ||
       strsame(p->list[p->ci]->instr, ".")) {
      assert(key_punctuation(p)==NOT_VALID);
   }

   /*OMISSION OF A VARNUM*/
   if (strsame(p->list[p->ci]->instr, "FD") &&
       strsame(p->list[p->ci+1]->instr, "RT")){
      assert(varnum_omitted(p)==YES);
   }

   if (strsame(p->list[p->ci]->instr, "RT") &&
       strsame(p->list[p->ci+1]->instr, "SET")){
      assert(varnum_omitted(p)==YES);
   }

   if (strsame(p->list[p->ci]->instr, "FD") &&
       strsame(p->list[p->ci+1]->instr, "40")){
      assert(varnum_omitted(p)==NO);
   }

   if (strsame(p->list[p->ci]->instr, "LT") &&
       strsame(p->list[p->ci+1]->instr, "a")){
      assert(varnum_omitted(p)==NO);
   }

   if (strsame(p->list[p->ci]->instr, "FD") &&
       strsame(p->list[p->ci+1]->instr, "A")){
      assert(varnum_omitted(p)==NO);
   }

   /*VALIDATING FROM*/
   if (strsame(p->list[p->ci]->instr, "FROM")){
      assert(validate_FROM(p)==VALID);
   }

   if (strsame(p->list[p->ci]->instr, "FRO")){
      assert(validate_FROM(p)==NOT_VALID);
   }

   if (strsame(p->list[p->ci]->instr, "from")){
      assert(validate_FROM(p)==NOT_VALID);
   }

   if (strsame(p->list[p->ci]->instr, "FOR")){
      assert(validate_FROM(p)==NOT_VALID);
   }


   /*VALIDATING TO*/
   if (strsame(p->list[p->ci]->instr, "TO")){
      assert(validate_TO(p)==VALID);
   }

   if (strsame(p->list[p->ci]->instr, "T0")){
      assert(validate_TO(p)==NOT_VALID);
   }

   if (strsame(p->list[p->ci]->instr, "To")){
      assert(validate_TO(p)==NOT_VALID);
   }

   if (strsame(p->list[p->ci]->instr, "to")){
      assert(validate_TO(p)==NOT_VALID);
   }

   /*VALIDATE EQUAL*/
   if (strsame(p->list[p->ci]->instr, ":=")){
      assert(validate_equal(p)==VALID);
   }

   if (strsame(p->list[p->ci]->instr, "=")){
      assert(validate_equal(p)==NOT_VALID);
   }

   if (strsame(p->list[p->ci]->instr, ":")){
      assert(validate_equal(p)==NOT_VALID);
   }

   /*NOT VALID INSTRUCTIONS*/
   if (strsame(p->list[p->ci]->instr, "sd")){
      assert(not_valid_instr(p)== -3);
   }

   if (strsame(p->list[p->ci]->instr, "SD")){
      assert(not_valid_instr(p)== -3);
   }

   if (strsame(p->list[p->ci]->instr, "TL")){
      assert(not_valid_instr(p)== -3);
   }

   if (strsame(p->list[p->ci]->instr, "fgh")){
      assert(not_valid_instr(p)== -3);
   }

   if (strsame(p->list[p->ci]->instr, "TR")){
      assert(not_valid_instr(p)== -3);
   }

   if ((strsame(p->list[p->ci]->instr, "fd")) ||
       (strsame(p->list[p->ci]->instr, "lt")) ||
       (strsame(p->list[p->ci]->instr, "rt")) ||
       (strsame(p->list[p->ci]->instr, "do")) ||
       (strsame(p->list[p->ci]->instr, "set"))) {
      assert(not_valid_instr(p)== -2);
   }

   if((strsame(p->list[p->ci]->instr, "[")) ||
      (strsame(p->list[p->ci]->instr, "]")) ||
      (strsame(p->list[p->ci]->instr, "(")) ||
      (strsame(p->list[p->ci]->instr, "(")) ||
      (strsame(p->list[p->ci]->instr, ")"))) {
      assert(not_valid_instr(p)== -1);
   }


   /*int round_float(float x)*/
   assert(round_float(4.67)==5);
   assert(round_float(45.17)==45);
   assert(round_float(4.5)== 4);
   assert(round_float(124.00)==124);
   assert(round_float(4456.9)==4457);

}
