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
void printRegister();
void printStack();
void print(int cnt);

struct instruction code[MAX_CODE_LENGTH];
struct instruction ir;

//global variables: program  counter (PC), base pointer (BP), stack pointer (SP), program counter (PC) and instruction register (IR).
int pc, sp = MAX_STACK_HEIGHT, bp = MAX_STACK_HEIGHT - 1;
int stack[MAX_STACK_HEIGHT] = {0};
int r[8] = {0};
int halt = 1;
int bars[MAX_CODE_LENGTH] = {0};

//infile pointer
FILE *fp;

int main(int argc, char* argv []){
    //count
    int cnt = 0;

    //open infile
    fp = fopen(argv[argc-1], "r");

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

    //print the column headers for the stack tracing
    printf("\n\nExecution of Program:\n");
    printf("\n\t\t\t\tpc\tbp\tsp\nInitial Values\t\t\t%d\t%d\t%d\n", pc, bp, sp);
    printRegister();
    printStack();

    while(halt == 1){

        fetchCycle();
        executeCycle();
        printRegister();
        printStack();

        //end of the program
        if(( (pc == 0) && (bp == 0) && (sp == 0) ))
            halt = 0;

    }

    //close files and end program
    fclose(fp);

    return 0;
    }

void fetchCycle(){
        //an instruction is fetched from the “text” segment and placed in the IR register (IR <- text[PC]) and the program counter is incremented by 1 to point to the next instruction to be executed (PC <- PC + 1).
        ir = code[pc];
        printf("%d  ", pc);
        pc++;
        return;
}

void executeCycle(){
    switch(ir.op){
            //LIT   R, 0, MLoads a constant value (literal) M into Register R
            case 1:
                printf("lit\t");
                r[ir.r] = ir.m;
                break;

            //RTN 0, 0, 0Returns from a subroutine and restore the caller environment.
            case 2:
                printf("rtn\t");
                sp = bp + 1;
                bp = stack[sp-2];
                pc = stack[sp-3];
                break;

            //LOD R, L, MLoad value into a selected register from the stack location at offset M from L lexicographical levels up
            case 3:
                printf("lod\t");
                r[ir.r] = stack[base(ir.l, bp) - ir.m];
                break;

            //STO R, L, MStore value from a selected register in the stack location at  offset M from L lexicographical levels up
            case 4:
                printf("sto\t");
                stack[base(ir.l, bp) - ir.m] = r[ir.r];
                break;

            //CAL 0, L, MCall procedure at code index M (generates new Activation Record and PC <- M)
            case 5:
                printf("cal\t");
                stack[sp-1] = base(ir.l, bp);
                stack[sp-2] = bp;
                stack[sp-3] = pc;
                bp = sp-1;
                pc = ir.m;
                break;

            //INC  0, 0, MAllocate  M  memory  words  (increment  SP  by  M).  First  three  are  reserved  to Static  Link  (SL),  Dynamic  Link  (DL),and   Return Address (RA)
            case 6:
                printf("inc\t");
                bars[sp] = 1;
                sp = sp - ir.m;
                break;

            //JMP 0, 0, MJump to instruction M (PC <- M)
            case 7:
                printf("jmp\t");
                pc = ir.m;
                break;

            //JPC
            case 8:
                printf("jpc\t");
                if(r[ir.r] == 0)
                    pc = ir.m;
                break;

            //SIO
            case 9:
                printf("sio\t");
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
                printf("neg\t");
                r[ir.r] = 0 - r[ir.r];
                break;

            //ADD
            case 11:
                printf("add\t");
                r[ir.r] = r[ir.l] + r[ir.m];
                break;

            //SUB
            case 12:
                printf("sub\t");
                r[ir.r] = r[ir.l] - r[ir.m];
                break;

            //MULI
            case 13:
                printf("mul\t");
                r[ir.r] = r[ir.l] * r[ir.m];
                break;

            //DIV
            case 14:
                printf("div\t");
                r[ir.r] = ((r[ir.l])/(r[ir.m]));
                break;

            //ODD
            case 15:
                printf("odd\t");
                r[ir.r] = r[ir.r]%2;
                break;

            //MOD
            case 16:
                printf("mod\t");
                r[ir.r] = (r[ir.l])%(r[ir.m]);
                break;

            //EQL
            case 17:
                printf("eql\t");
                if(r[ir.l] == r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //NEQ
            case 18:
                printf("neq\t");
                if(r[ir.l] != r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //LSS
            case 19:
                printf("lss\t");
                if(r[ir.l] < r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //LEQ
            case 20:
                printf("leq\t");
                if(r[ir.l] <= r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //GTR
            case 21:
                printf("gtr\t");
                if(r[ir.l] > r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;

            //GEQ
            case 22:
                printf("geq\t");
                if(r[ir.l] >= r[ir.m])
                    r[ir.r] = 1;
                else
                    r[ir.r] = 0;
                break;


            default:
                printf("err\t");
                return;

        }
        printf("  %d  %d  %d", ir.r, ir.l, ir.m);
        printf("\t\t%d\t%d\t%d\n", pc, bp, sp);
        return;
    }

int base(int L, int base){
    int b1; //Find base L levels up
    b1 = base;
    while(L > 0){
        b1 = stack[b1];
        L--;
    }
    return b1;
}

void printRegister(){
    printf("Registers: ");
    for(int i = 0; i < 8; i++)
        printf("%d ", r[i]);
    printf("\n");
}

void printStack(){
    printf("Stack: ");

    for(int i = 1; i <= MAX_STACK_HEIGHT - sp; i++){
        // Print out the bar before printing out the next AR
        if(MAX_STACK_HEIGHT-i+1!=1000 && bars[MAX_STACK_HEIGHT-i+1])
            printf("| ");
        printf("%d ", stack[MAX_STACK_HEIGHT-i]);
    }
    printf("\n\n");
    return;
}

void print(int cnt){
    int i;
    printf("Interpreted Assembly Language:\n");
    printf("\nLine\tOP\tR\tL\tM");
    for(i = 0; i < cnt; i++){
        int op = code[i].op;

        //Interpret the operation.
        switch(op){
            case 1: printf("\n%d\t", i); printf("lit\t"); break;
            case 2: printf("\n%d\t", i); printf("rtn\t"); break;
            case 3: printf("\n%d\t", i); printf("lod\t"); break;
            case 4: printf("\n%d\t", i); printf("sto\t"); break;
            case 5: printf("\n%d\t", i); printf("cal\t"); break;
            case 6: printf("\n%d\t", i); printf("inc\t"); break;
            case 7: printf("\n%d\t", i); printf("jmp\t"); break;
            case 8: printf("\n%d\t", i); printf("jpc\t"); break;
            case 9: printf("\n%d\t", i); printf("sio\t"); break;
            case 10:printf("\n%d\t", i); printf("neg\t"); break;
            case 11:printf("\n%d\t", i); printf("add\t"); break;
            case 12:printf("\n%d\t", i); printf("sub\t"); break;
            case 13:printf("\n%d\t", i); printf("mul\t"); break;
            case 14:printf("\n%d\t", i); printf("div\t"); break;
            case 15:printf("\n%d\t", i); printf("odd\t"); break;
            case 16:printf("\n%d\t", i); printf("mod\t"); break;
            case 17:printf("\n%d\t", i); printf("eql\t"); break;
            case 18:printf("\n%d\t", i); printf("neq\t"); break;
            case 19:printf("\n%d\t", i); printf("lss\t"); break;
            case 20:printf("\n%d\t", i); printf("leq\t"); break;
            case 21:printf("\n%d\t", i); printf("gtr\t"); break;
            case 22:printf("\n%d\t", i); printf("geq\t"); break;
            default: return;
        }
        printf("%d\t", code[i].r);
        printf("%d\t", code[i].l);
        printf("%d\t", code[i].m);
    }
    return;
}
