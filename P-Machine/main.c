//
//  main.c
//  P-Machine
//
//  Created by Weiyi Chen & Alice Zhang on 9/16/20.
//  Homework #1 (P-Machine)
//  COP 3402


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<strings.h>

#define MAX_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500

//The Instruction Set Architecture (ISA) of PM/0 has 22 instructions and the instruction format has four fields:  “OP R L M”. They are separated by one space.
typedef struct instruction{
    int op, r, l, m;
} instruction;


void fetchCycle(void);
void executeCycle(void);
int base(int L, int base);
void printStack(int flag);
void print(int cnt);

struct instruction code[MAX_CODE_LENGTH];
struct instruction ir;

//global variables: program  counter (PC), base pointer (BP), stack pointer (SP), program counter (PC) and instruction register (IR).
int pc, sp = MAX_STACK_HEIGHT, bp = sp-1;
int stack[MAX_STACK_HEIGHT] = {0};
int flag;
int r[8] = {0};
int halt = 1;

//infile, outfile
FILE *fp;
FILE *ofp;

int main(){
    //count
    int cnt = 0;
    
    //open infile and outfile
    fp = fopen("code.txt", "r");
    ofp = fopen("output.txt", "w");

    //if null print error
    if(fp == NULL){
        printf("error");
        return 0;
    }

    //reading
    while(!feof(fp)) {
        fscanf(fp, "%d", &code[cnt].op);
        fscanf(fp, "%d", &code[cnt].r);
        fscanf(fp, "%d", &code[cnt].l);
        fscanf(fp, "%d", &code[cnt].m);
        cnt++;
    }

    //call our initial printing function named "print".
    print(cnt);
    cnt = 0;

    //print the column headers for the stack tracing
    if(flag) {
        printf("\n\nExecution of Program:\n");
        printf("\nInitial Values\t\t\t\tpc\tbp\tsp\n");
    }

    fprintf(ofp, "\n\nExecution of Program:\n");
    fprintf(ofp, "\nInitial Values\t\t\t\tpc\tbp\tsp\n");
    
    while(halt == 0){
        fetchCycle();
        executeCycle();

        if(flag){
            printf("%d\t%d\t%d\t", ir.r, ir.l, ir.m);
            printf("%d\t%d\t%d\t", pc, bp, sp);
        }
        fprintf(ofp, "%d\t%d\t%d\t", ir.r, ir.l, ir.m);
        fprintf(ofp, "%d\t%d\t%d\t", pc, bp, sp);
        printStack(flag);

        if(( (pc == 0) && (bp == 0) && (sp == 0) ))
            halt = 0;

    }

    //close files and end program
    fclose(fp);
    fclose(ofp);
    return 0;
    }

void fetchCycle(){
        //an instruction is fetched from the “text” segment and placed in the IR register (IR <- text[PC]) and the program counter is incremented by 1 to point to the next instruction to be executed (PC <- PC + 1).
        ir = code[pc];
        fprintf(ofp, "%d\t", pc);
        pc++;
        return;
}

void executeCycle(){
    switch(ir.op){
            //LIT   R, 0, MLoads a constant value (literal) M into Register R
            case 1:
                fprintf(ofp, "lit\t");
                r[ir.r] = ir.m;
                break;

            //RTN 0, 0, 0Returns from a subroutine and restore the caller environment.
            case 2:
                fprintf(ofp, "rtn\t");
                sp = bp + 1;
                bp = stack[sp-2];
                pc = stack[sp-3];
                break;

            //LOD R, L, MLoad value into a selected register from the stack location at offset M from L lexicographical levels up
            case 3:
                fprintf(ofp, "lod\t");
                r[ir.r] = stack[base(ir.l, bp) - ir.m];
                break;

            //STO R, L, MStore value from a selected register in the stack location at  offset M from L lexicographical levels up
            case 4:
                fprintf(ofp, "sto\t");
                stack[base(ir.l, bp) - ir.m] = r[ir.r];
                break;

            //CAL 0, L, MCall procedure at code index M (generates new Activation Record and PC <- M)
            case 5:
                fprintf(ofp, "cal\t");
                stack[sp-1] = base(ir.l, bp);
                stack[sp-2] = bp;
                stack[sp-3] = pc;
                bp = sp-1;
                pc = ir.m;
                break;

            //INC  0, 0, MAllocate  M  memory  words  (increment  SP  by  M).  First  three  are  reserved  to Static  Link  (SL),  Dynamic  Link  (DL),and   Return Address (RA)
            case 6:
                fprintf(ofp, "inc\t");
                sp = sp - ir.m;
                break;

            //JMP 0, 0, MJump to instruction M (PC <- M)
            case 7:
                fprintf(ofp, "jmp\t");
                pc = ir.m;
                break;

            //JPC
            case 8:
                fprintf(ofp, "jpc\t");
                if(r[ir.r] == 0)
                    pc = ir.m;
                break;

            //SIO 
            case 9:
                fprintf(ofp, "sio\t");
                //print register to screen
                if(ir.m == 1)
                    printf("\nR[%d] = %d\n", ir.r, r[ir.r]);
                //read in value from screen to register
                else if(ir.m == 2){
                    printf("\nPlease type in the number you wish to store in the register. Press enter when finished.\n");
                    scanf("%d", &r[ir.r]);
                }
                //set halt flag to 0, end of program
                else if(ir.m == 3)
                    halt = 0;
                break;
            
            //NEG
            case 10:
                fprintf(ofp, "neg\t");
                r[ir.r] = 0 - r[ir.r];
                break;

            //ADD
            case 11:
                fprintf(ofp, "add\t");
                r[ir.r] = r[ir.l] + r[ir.m];
                break;

            //SUB
            case 12:
                fprintf(ofp, "sub\t");
                r[ir.r] = r[ir.l] - r[ir.m];
                break;

            //MULI
            case 13:
                fprintf(ofp, "mul\t");
                r[ir.r] = r[ir.l] * r[ir.m];
                break;

            //DIV
            case 14:
                fprintf(ofp, "div\t");
                r[ir.r] = ((r[ir.l])/(r[ir.m]));
                break;

            //ODD
            case 15:
                fprintf(ofp, "odd\t");
                r[ir.r] = r[ir.r]%2;
                break;

            //MOD
            case 16:
                fprintf(ofp, "mod\t");
                r[ir.r] = (r[ir.l])%(r[ir.m]);
                break;

            //EQL
            case 17:
                fprintf(ofp, "eql\t");
                if(r[ir.l] == r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //NEQ
            case 18:
                fprintf(ofp, "neq\t");
                if(r[ir.l] != r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //LSS
            case 19:
                fprintf(ofp, "lss\t");
                if(r[ir.l] < r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //LEQ
            case 20:
                fprintf(ofp, "leq\t");
                if(r[ir.l] <= r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //GTR
            case 21:
                fprintf(ofp, "gtr\t");
                if(r[ir.l] > r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //GEQ
            case 22:
                fprintf(ofp, "geq\t");
                if(r[ir.l] >= r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;


            default:
                fprintf(ofp, "err\t");
                return;

        }
        return;   
    }

int base(int L, int base){
    int b1; //Find base L levels up
    b1 = base;
    while(L > 0){
        b1 = stack[b1 + 1];
        L--;
    }
    return b1;
}

void printStack(int flag){
    int i;
    if(bp == 0){
        return;
    }
    else{

        for(i = 1; i <= sp; i++){
            if(flag)
                printf("%d\t", stack[i]);
            fprintf(ofp, "%d\t", stack[i]);
        }
        if(flag)
            printf("\n");
        fprintf(ofp, "\n");
        return;
    }
}

void print(int cnt){
    int i;
    fprintf(ofp, "Interpreted Assembly Language\n");
    fprintf(ofp, "\nLine\tOP\tR\tL\tM");
    for(i = 0; i < cnt; i++){
        int op = code[i].op;

        //Interpret the operation.
        switch(op){
            case 1: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "lit\t"); break;
            case 2: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "rtn\t"); break;
            case 3: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "lod\t"); break;
            case 4: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "sto\t"); break;
            case 5: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "cal\t"); break;
            case 6: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "inc\t"); break;
            case 7: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "jmp\t"); break;
            case 8: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "jpc\t"); break;
            case 9: fprintf(ofp, "\n%d\t", i); fprintf(ofp, "sio\t"); break;
            case 10:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "neg\t"); break;
            case 11:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "add\t"); break;
            case 12:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "sub\t"); break;
            case 13:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "mul\t"); break;
            case 14:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "div\t"); break;
            case 15:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "odd\t"); break;
            case 16:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "mod\t"); break;
            case 17:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "eql\t"); break;
            case 18:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "neq\t"); break;
            case 19:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "lss\t"); break;
            case 20:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "leq\t"); break;
            case 21:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "gtr\t"); break;
            case 22:fprintf(ofp, "\n%d\t", i); fprintf(ofp, "geq\t"); break;
            default: return;
        }
        fprintf(ofp, "%d\t", code[i].r);
        fprintf(ofp, "%d\t", code[i].l);
        fprintf(ofp, "%d\t", code[i].m);
    }
    return;
}
