#include "header.h"

void interpret_instrctlist(Program *p, SDL_Simplewin *sw)
{
   if(strsame(p->list[p->ci]->instr, "}")) {
      return;
   }

   interpret_instruction(p, sw);
   p->ci++;
   interpret_instrctlist(p, sw);
}

void interpret_instruction(Program *p, SDL_Simplewin *sw)
{
   interpret_RT_LT(p);
   interpret_FD(p, sw);
   interpret_SET(p);
   interpret_DO(p, sw);
}

/*=====================================
FUNCTIONS TO INTERPRET 'SET' INSTRUCTIONS
=======================================*/
void interpret_SET(Program *p)
{
   int top=0;
   int base=0; /*Top and base stack pointer*/
   float *stack;
   float m;
   float result = 0;
   int var;

   if(strsame(p->list[p->ci]->instr, "SET")) {
      p->ci++;
      var = (int)*p->list[p->ci]->instr-ASCII-1;
      p->ci++;

      /*counting the number of variables in SET*/
      while(!strsame(p->list[p->ci]->instr, ";")) {
         if (key_words(p)==VALID ||
             strsame(p->list[p->ci]->instr, "}")) {
            printf("Line %d: 'SET' cannot be executed "
                   "due to the missing an end ';'.\n\n",
                    p->list[p->ci]->line);
            p->ci--;
            return;
         }
         else {
            top++;
            p->ci++;
         }
      }
      p->ci-=top;

      /*If there are no operations to perform on
      SET, no_ops_SET function is performed and
      the reste of the code in this function ignored*/
      if (top==1 || top==2) {
         result = no_ops_SET(p, top);
         return;
      }

       /*Following code if there are operations to perform*/
      /*Creates a local stack*/
      stack=(float*)malloc(top*sizeof(float));
      if(stack == NULL) {
         printf("Stack for 'SET' not created\n");
      }
      /*Pushing and poping to the stack to get result*/
      while (!strsame(p->list[p->ci]->instr, ";")) {
         p->ci++;

        /*Pushing value onto the stack*/
        if (VARNUM(p)==VALID) {
            m = varnum_to_int(p, p->list[p->ci]->instr);
            stack[base] = m;
            base++;
         }
         else if (base >= 2){
            if(strsame(p->list[p->ci]->instr, "+")) {
               result = stack[base-2] + stack[base-1];
               base-=2;
               stack[base]=result;
               base++;
            }
            else if(strsame(p->list[p->ci]->instr, "*")) {
               result = stack[base-2] * stack[base-1];
               base-=2;
               stack[base]=result;
               base++;
            }
            else if(strsame(p->list[p->ci]->instr, "-")) {
               result = stack[base-2] - stack[base-1];
               base-=2;
               stack[base]=result;
               base++;
            }
            else if(strsame(p->list[p->ci]->instr, "/")) {
               result = stack[base-2] / stack[base-1];
               base-=2;
               stack[base]=result;
               base++;
            }
         }
         else if (base < 1){
            printf("Line %d: There are not enough "
                   "variables to perform the operation\n"
                   "for the 'SET' instruction.\n\n",
                   p->list[p->ci]->line);
            p->errors++;
         }
      }

      if (1 < base) { /*change magic number 1*/
         printf("Error in the 'SET' instruction. There "
                "might be too many\nvariable for "
                "the number of instructions.\n\n");
      }
      p->var_val[var]= result;
   }
}

int no_ops_SET(Program *p, int top)
{
   int var;
   int result = 0;

   if (top==1) {
      printf("Line %d: 'SET' instructions need to "
             "have at least one\nvariable letter or "
             "number following the ':='.\n\n",
             p->list[p->ci]->line);
      p->errors++;
   }

   else if (top == 2) {
      p->ci++;
      if (VARNUM(p)==VALID) {
         if (VAR(p)==VALID) {
            var = (int)*p->list[p->ci]->instr - (ASCII+1);
            result = p->var_val[var];
            p->ci++;
          }
          else {
             result = varnum_to_int(p, p->list[p->ci]->instr);;
             p->ci++;
          }
   }

   else {
      printf("Line %d: For the 'SET' instruction to take "
              "effect,\nthere needs to be at least 1 number "
              "or variable letter.\n\n", p->list[p->ci]->line);
     }
   }
   return result;
}


/*====================================
FUNCTIONS TO INTERPRET 'FD' INSTRUCTIONS
======================================*/
void interpret_FD(Program *p, SDL_Simplewin *sw)
{
   int distance;
   int fa=180; /*flat angle: 180 degrees*/
   float x2, y2;
   /*pi formula from: cboard.cprogramming.com*/
   const double pi = 4.0*atan(1.0);


   if(strsame(p->list[p->ci]->instr, "FD")) {
      p->ci++;
      distance = varnum_to_int(p, p->list[p->ci]->instr);

      p->x0= p->x;
      p->y0= p->y;
      /*Calculating new coordinates for Turtle*/
      x2 = p->x0 + distance* cos(p->angle*(pi/fa));
      y2 = p->y0 + distance* sin(p->angle*(pi/fa));

      p->x = round_float(x2);
      p->y = round_float(y2);


      SDL_Delay(MILLISECONDDELAY*10);
      Neill_SDL_SetDrawColour(sw, rand()%SDL_8BITCOLOUR,
                                  rand()%SDL_8BITCOLOUR,
                                  rand()%SDL_8BITCOLOUR);
      SDL_RenderDrawLine(sw->renderer, p->x0, p->y0, p->x, p->y);
      Neill_SDL_UpdateScreen(sw);
      Neill_SDL_Events(sw);
   }
}

/*===================================
INTERPRETS 'RT' AND 'LT' INSTRUCTIONS
=====================================*/
void interpret_RT_LT(Program *p)
{
   int max_angle = 360;

   if(strsame(p->list[p->ci]->instr, "RT") ||
      strsame(p->list[p->ci]->instr, "LT")) {
      p->ci++;

      /*Calculating Right turns*/
      if (strsame(p->list[p->ci-1]->instr, "RT")) {
         p->angle+=(int)varnum_to_int(p, p->list[p->ci]->instr)
                  % max_angle;
      }

      /*Calculating Left turns*/
      if (strsame(p->list[p->ci-1]->instr, "LT")) {
         p->angle-=(int)varnum_to_int(p, p->list[p->ci]->instr)
                  % max_angle;
      }
   }
}

/*===========================
INTERPRETS 'DO' INSTRUCTIONS
============================*/
void interpret_DO(Program *p, SDL_Simplewin *sw)
{
  int start, end, instr1, i;
  char var;
   if(strsame(p->list[p->ci]->instr, "DO")) {
      p->ci += 1;
      var = p->list[p->ci]->instr[0];
      p->ci += 2;
      start = varnum_to_int(p, p->list[p->ci]->instr);
      p->ci += 2;
      end = varnum_to_int(p, p->list[p->ci]->instr);
      p->ci += 2;
      instr1 = p->ci;

      for(i=start; i<=end; i++) {
         p->ci = instr1;
         p->var_val[var-'A'] = (float)i;
         interpret_instrctlist(p, sw);
      }
   }
}



/*==================
UTILITARIAN FUNCTION
====================*/
float varnum_to_int(Program *p, char *s)
{
   float number;
   int var;
   if(VAR(p)==VALID) {
      var = (int)*p->list[p->ci]->instr - (ASCII+1);
      number = p->var_val[var];
   }
   else {
      sscanf(s, "%f", &number);
   }
   return number;
}


int round_float(float x)
{
   int y = x;
   x -=y;
   if(x > 0.5) {
      return y+1;
   }
   else {
      return y;
   }
}
