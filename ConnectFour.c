#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define N 7

/*global variables*/
typedef enum game_state{P1WON, P2WON, DRAW, P1TURN, P2TURN} game_state;
game_state state;
typedef enum game_type{MULTIPLAYER,SOLO,VS} game_type;
game_type type;
int turn=1;

/*FINAL VERSION - αναπτηγμένη στρατηγiκή
In this version, the computer can sieze the winning move, block its rival, avoid threats, set traps,
and during the first 5 turns of the game will opt to drop in the columns more torward the center.*/

void init(char A[N][N]); /*fill the board with '-' at the beggining of the game*/
void print(char A[N][N]); /*print the board*/
void evaluate(char A[N][N]); /*declare victory, a draw, or change turns based on the game state and the board's layout*/
void handler(char A[N][N]); /*handle piece dropping, change of turns*/
void message(); /*print a message informing the player(s) of the game's state*/
int drop(char A[N][N],int a,char p); /*drop a new piece on the board*/
int fourinarow(char A[N][N],char p); /*check for a horizontal victory - ONLY VALID FOR THE 7-BY-7 BOARD. Δηλαδή για αυτόν που ζητάει η εκφώνηση.*/
int fourinacolumn(char A[N][N],char p); /*check for a vertical victory - ONLY VALID FOR THE 7-BY-7 BOARD*/
int fourinadiagonal(char A[N][N],char p); /*check for a diagonal vistory - again, ONLY VALID FOR THE 7-BY-7 BOARD */
void humanplays(char A[N][N], char p); /*handle input from human players*/
/*********************functions associated with the computer player****************/
void computerplays(char A[N][N],char p,char f); /*decide where to drop the piece*/
int trapcheck(char A[N][N],int c,char p,char f); /*check if the computer's impending move satisfies a threat or not */
int trapset(char A[N][N],int c,char p,char f); /*plant a trap*/
int generator(int plays[7],int len); /*generate a column for the computer based on available moves and turn number*/

/********************************main**********************************************/
int main(int argc, char *argv[]){
    
srand(time(NULL));
char board[N][N];
int choice;
init(board);
state = P1TURN;
printf("\t\t\t\tCONNECT FOUR\n");
print(board);
printf("Press:\n1 to play against the computer\n2 for a multiplayer game\n3 to see a match between two computer players\n");
scanf("%d",&choice); 
switch(choice){
               case 1: type=SOLO; break;
               case 2: type=MULTIPLAYER; break;
               case 3: type=VS; break;}
message();

while(state==P1TURN || state==P2TURN){
         handler(board);
         evaluate(board);
         print(board);
         message();
         printf("\t\t\t\t  turn %d\n\n",++turn);
         }
         
system("PAUSE");
return 0;
}
/**********************************************************************************/   
  
void evaluate(char A[N][N]){ 
     if (memchr(A,'-',N*N)==NULL) state=DRAW; /*board is full, it's a draw*/
     else if(fourinarow(A,'*') || fourinacolumn(A,'*') || fourinadiagonal(A,'*')) state=P1WON;
     else if(fourinarow(A,'+') || fourinacolumn(A,'+') || fourinadiagonal(A,'+')) state=P2WON;
     else{
     if (state==P1TURN) state=P2TURN; 
     else state=P1TURN; }
     }
     
void handler(char board[N][N]){
     char piece, fpiece;
     if (state==P1TURN){ piece='*';fpiece='+';
     switch(type){
                  case VS: 
                       computerplays(board,piece,fpiece);
                       break;
                  case SOLO:
                  case MULTIPLAYER: 
                       humanplays(board,piece);
                       break;}}
     if (state==P2TURN){ piece='+'; fpiece='*';
     switch(type){
                  case VS:
                  case SOLO: 
                       computerplays(board,piece,fpiece);
                       break;
                  case MULTIPLAYER: 
                       humanplays(board,piece);
                       break;}}
     }

int drop(char A[N][N],int column,char piece){
    if (A[0][column]!='-') return 1; /*the particular column is full*/
    int i;
    for (i=0;i<N;i++)
    if (A[i][column]!='-'){
                           A[i-1][column] = piece;
                           return 0;} /*piece dropped succesfully*/
    A[N-1][column] = piece;
    return 0; /*column is empty, piece dropped succesfully*/
    }

void computerplays(char board[N][N],char piece,char fpiece){
    char prophecy[N][N];
    memcpy(&prophecy,board,N*N); 
    int column;
    /*in the following loop the computer searches for a winning or a blocking move*/
    for (column=0;column<N;column++,memcpy(&prophecy,board,N*N)){         
    if (drop(prophecy,column,piece)==0){
                                   if(fourinarow(prophecy,piece) || fourinacolumn(prophecy,piece) || fourinadiagonal(prophecy,piece)){
                                                          drop(board,column,piece);
                                                          return;}}
                                   memcpy(&prophecy,board,N*N);
    if (drop(prophecy,column,fpiece)==0){if(fourinarow(prophecy,fpiece) || fourinacolumn(prophecy,fpiece) || fourinadiagonal(prophecy,fpiece)){
                                                          drop(board,column,piece);
                                                          return;}}                                 
                                                         }
    /*no winning/blocking move found*/
    int legalplays[7], rivaltraps[7], owntraps[7], bestplays[7]; /*an array of all possible moves, and an array of all possible moves that aren't into rival's traps, the computer's traps, and a combination of the former*/
    int number_of_legalplays=0,number_of_rivaltraps=0,number_of_owntraps=0, number_of_bestplays=0;
    for (column=0;column<N;column++){
        if (board[0][column]=='-') legalplays[number_of_legalplays++]=column;
        if (board[0][column]=='-' && trapcheck(board,column,piece,fpiece)==0) rivaltraps[number_of_rivaltraps++]=column;
        if (board[0][column]=='-' && trapset(board,column,piece,fpiece)==0) owntraps[number_of_owntraps++]=column;
        if (board[0][column]=='-' && trapcheck(board,column,piece,fpiece)==0 && trapset(board,column,piece,fpiece)==0) bestplays[number_of_bestplays++]=column;}
        if (number_of_bestplays==0){
                                    if(number_of_rivaltraps==0 && number_of_owntraps==0){
                                    drop(board,generator(legalplays,number_of_legalplays),piece);
                                    return;} /*can't play without falling into a trap or ruining my own (I'm going to lose, but I'll do so with dignity)*/
                                    if(number_of_owntraps==0){
                                    drop(board,generator(rivaltraps,number_of_rivaltraps),piece);
                                    return;} /*can't play without ruining my own trap, but maybe I can avoid the rival's*/
                                    if(number_of_rivaltraps==0){
                                    drop(board,generator(owntraps,number_of_owntraps),piece); /*<-- statement has no real purpose because the computer will lose, but I added it for completion purposes*/
                                    return;}}
        drop(board,generator(bestplays,number_of_bestplays),piece);  /*optimal move found*/         
    }
    
void humanplays(char board[N][N], char piece){
     int column;
     printf("Select a column (from 1 to 7) to drop a piece in.\n");
     scanf("%d",&column);
     if (drop(board,column-1,piece)){
     while (drop(board,column-1,piece)){
                                   printf("You can't drop a piece in this column. Pick another one.\n");
                                   scanf("%d",&column);}}
    }
     
int trapcheck(char A[N][N],int column,char piece,char fpiece){
    char test[N][N];
    memcpy(&test,A,N*N);
    if (A[1][column]!='-') return 0; /*can't satisfy a threat because the column will fill up*/
    drop(test,column,piece);
    drop(test,column,fpiece);
    if(fourinarow(test,fpiece) || fourinacolumn(test,fpiece) || fourinadiagonal(test,fpiece)) return 1; /*it's a trap!*/
    return 0;
    }
    
int trapset(char A[N][N],int column,char piece,char fpiece){
    char test[N][N];
    memcpy(&test,A,N*N);
    if (A[1][column]!='-') return 0; /*can't set a trap because the column will fill up*/
    drop(test,column,fpiece);
    drop(test,column,piece);
    if(fourinarow(test,piece) || fourinacolumn(test,piece) || fourinadiagonal(test,piece)) return 1; /*you've set a trap here*/
    return 0;
    }

int generator(int plays[7],int len){
    if (turn<5) return rand()%4+2; /*drop near the center (it's too early for threats anyway)*/
    int play = rand()%len;
    return plays[play];
}
    
    
int fourinarow(char A[N][N],char piece){
    int i;
    for (i=0;i<N;i++){
        if (A[i][3]==piece){
                           if(A[i][0]==piece && A[i][1]==piece && A[i][2]==piece) return 1;
                           if(A[i][4]==piece && A[i][5]==piece && A[i][6]==piece) return 1;
                           if(A[i][2]==piece && A[i][4]==piece && A[i][5]==piece) return 1;
                           if(A[i][1]==piece && A[i][2]==piece && A[i][4]==piece) return 1;}
        }
        return 0;    
    }
int fourinacolumn(char A[N][N],char piece){
    int i;
    for (i=0;i<N;i++){
        if (A[3][i]==piece){
                           if(A[0][i]==piece && A[1][i]==piece && A[2][i]==piece) return 1;
                           if(A[6][i]==piece && A[5][i]==piece && A[4][i]==piece) return 1;
                           if(A[2][i]==piece && A[4][i]==piece && A[5][i]==piece) return 1;
                           if(A[1][i]==piece && A[2][i]==piece && A[4][i]==piece) return 1;}
        }
        return 0;    
    }    
    
int fourinadiagonal(char A[N][N],char piece){
    
    if(A[3][3]==piece) /*4 consecutive pieces on the main diagonal*/
                       {if(A[0][0]==piece && A[1][1]==piece && A[2][2]==piece) return 1;
                       if(A[4][4]==piece && A[5][5]==piece && A[6][6]==piece) return 1;
                       if(A[1][1]==piece && A[2][2]==piece && A[4][4]==piece) return 1;
                       if(A[2][2]==piece && A[4][4]==piece && A[5][5]==piece) return 1;
                       /*4 consecutive pieces on the counter diagonal*/
                       if(A[0][6]==piece && A[1][5]==piece && A[2][4]==piece) return 1;
                       if(A[6][0]==piece && A[5][1]==piece && A[4][2]==piece) return 1;
                       if(A[2][4]==piece && A[4][2]==piece && A[5][1]==piece) return 1;
                       if(A[1][5]==piece && A[2][4]==piece && A[4][2]==piece) return 1; }                     
    if(A[3][2]==piece){ /*4 consecutive pieces on the 6-piece diagonal under the main diagonal*/
                       if(A[2][1]==piece && A[1][0]==piece && A[4][3]==piece) return 1;
                       if(A[2][1]==piece && A[4][3]==piece && A[5][4]==piece) return 1;
                       if(A[4][3]==piece && A[5][4]==piece && A[6][5]==piece) return 1;
                       /*4 consecutive pieces on the 6-piece diagonal over the main diagonal - counter*/
                       if(A[5][0]==piece && A[4][1]==piece && A[2][3]==piece) return 1;
                       if(A[1][4]==piece && A[4][1]==piece && A[2][3]==piece) return 1;
                       if(A[0][5]==piece && A[1][4]==piece && A[2][3]==piece) return 1;}                    
    if(A[3][4]==piece){ /*4 consecutive pieces on the 6-piece diagonal over the main diagonal*/
                       if(A[0][1]==piece && A[1][2]==piece && A[2][3]==piece) return 1;
                       if(A[1][2]==piece && A[2][3]==piece && A[4][5]==piece) return 1;
                       if(A[2][3]==piece && A[4][5]==piece && A[5][6]==piece) return 1;
                       /*4 consecutive pieces on the 6-piece diagonal under the main diagonal - counter*/
                       if(A[1][6]==piece && A[2][5]==piece && A[4][3]==piece) return 1;
                       if(A[2][5]==piece && A[4][3]==piece && A[5][2]==piece) return 1;
                       if(A[4][3]==piece && A[5][2]==piece && A[6][1]==piece) return 1;}     
    if(A[3][5]==piece){ /*4 consecutive pieces on the 5-piece diagonal over the main diagonal*/
                       if(A[0][2]==piece && A[2][4]==piece && A[1][3]==piece) return 1;
                       if(A[2][4]==piece && A[1][3]==piece && A[4][6]==piece) return 1;
                       /*4 consecutive pieces on the 5-piece diagonal under the main diagonal - counter*/
                       if(A[6][2]==piece && A[4][4]==piece && A[5][3]==piece) return 1;
                       if(A[2][6]==piece && A[4][4]==piece && A[5][3]==piece) return 1;}                    
    if(A[3][1]==piece){ /*4 consecutive pieces on the 5-piece diagonal under the main diagonal*/
                       if(A[2][0]==piece && A[5][3]==piece && A[4][2]==piece) return 1;
                       if(A[5][3]==piece && A[4][2]==piece && A[6][4]==piece) return 1;
                       /*4 consecutive pieces on the 5-piece diagonal over the main diagonal - counter*/
                       if(A[0][4]==piece && A[1][3]==piece && A[2][2]==piece) return 1;
                       if(A[4][0]==piece && A[1][3]==piece && A[2][2]==piece) return 1;}                       
                            
    /*and one last check for the edges of the 4-sided square that forms in the 7-by-7 board*/
    if(A[0][3]==piece && A[1][4]==piece && A[2][5]==piece && A[3][6]==piece) return 1;
    if(A[3][6]==piece && A[4][5]==piece && A[5][4]==piece && A[6][3]==piece) return 1;
    if(A[6][3]==piece && A[5][2]==piece && A[4][1]==piece && A[3][0]==piece) return 1;                                 
    if(A[3][0]==piece && A[2][1]==piece && A[1][2]==piece && A[0][3]==piece) return 1; 
    return 0;
    }                           
     
void message(){
     if (state==P1TURN) printf("It's player 1's turn.\n");
     else if (state==P2TURN) printf("It's player 2's turn.\n");
     else if (state==P1WON) printf("Player 1 won!\n");
     else if (state==P2WON) printf("Player 2 won!\n");
     else if (state==DRAW) printf("It's a draw!\n");
     }
     
void print(char A[N][N]){
     int i,j;
     printf("\t\t\t\t  ");
     for(i=0;i<N;i++){
     for(j=0;j<N;j++){
     printf("%c",A[i][j]);}
     printf("\n\t\t\t\t  ");}
     printf("\n");
     }
     
void init(char A[N][N]){
     int i,j;
     for(i=0;i<N;i++)
     for(j=0;j<N;j++)
     A[i][j] = '-';
     }                     
                       
                       
                       
 
