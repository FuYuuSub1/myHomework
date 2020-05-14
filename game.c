#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

char rank[] = {'3','4','5','6','7','8','9','T','J','Q','K','A','2'};
char suit[] = {'D','C','H','S'};

int p2c_fd [60][2];
int c2p_fd [60][2];
int child[60];

char* hold = "hold";
char* get = "get";
char* pass = "pass";
char* okay = "okay";
char* finish = "finish";
char* yes = "yes";
char* no = "no";
char* begin = "begin";
char* play = "play ";
char* toplay = "toplay ";
char* restart = "restart ";
char* done = "done";
char* d3 = "diamond3";

void run_player(int childnum);
bool comparecard(char before[2], char thatcard[2]);
bool checkfinish(int thatchild);
bool checkbuf(char buf[30], char* command);
bool duplicate(int thatcard, char deck[60][2]);

void run_player(int childnum){
    char buf[30];
    int i;
    int n;
    int cardnum;
    char deck[60][2];
    int used_card[60] = {0};


    while((n = read(p2c_fd[childnum][0], buf, 20))> 0 ){
        buf[n] = 0;
        if(n == 2){
            deck[cardnum][0] = buf[0];
            deck[cardnum][1] = buf[1];
            cardnum++;
            write(c2p_fd[childnum][1], get, 3);
        }else if(checkbuf(buf, hold)){
            printf("Child %d, pid %d: I hold %d cards\n", childnum + 1, getpid(), cardnum);
            printf("Child %d, pid %d: ", childnum + 1, getpid());
            for(i = 0; i < cardnum; i++){
                printf("%c%c ", deck[i][0], deck[i][1]);
            }
            printf("\n");
            write(c2p_fd[childnum][1], okay, 4);
        }else if(checkbuf(buf, d3)){
            bool flag = true;
            for(i = 0; i < cardnum; i++){
                if(deck[i][0] == 'D' && deck[i][i] == '3'){
                    write(c2p_fd[childnum][1], yes, 3);
                    flag = false;
                    break;
                }
            }
            if(flag) write(c2p_fd[childnum][1] , no, 2);
        }else if(checkbuf(buf, begin) && buf[4] == begin[4]){
            char command[7];
            printf("Child %d, pid %d: hold ", childnum + 1, getpid());
            for(i = 0; i < cardnum; i++){
                printf("%c%c ", deck[i][0], deck[i][1]);
            }
            printf("\n");
            strcpy(command, play);
            for(i = 0; i < cardnum; i++){
                if(deck[i][0] == 'D' && deck[i][1] == '3'){
                    used_card[i] = 1;
                    printf("Child %d, pid %d: play D3\n", childnum + 1, getpid());
                    command[5] = deck[i][0];
                    command[6] = deck[i][1];
                    break;
                }
            }
            write(c2p_fd[childnum][1] , command, 7);
        } else if(checkbuf(buf, toplay) && buf[4] == toplay[4] && buf[5] == toplay[5]){
            char before[2];
            before[0] = buf[7];
            before[1] = buf[8];
            bool flag = false;
            int count = 0;
            char command[7];
            printf("Child %d, pid %d: hold", childnum + 1, getpid());
            for(i = 0; i < cardnum; i++){
                if(used_card[i] == 0){
                    printf("%c%c ", deck[i][0], deck[i][1]);
                }                
            }
            printf("\n");
            for(i = 0; i < cardnum; i++){
                if(used_card[i] == 0){
                    if(comparecard(before, deck[i])){
                        if(!flag){
                            flag = true;
                            count = i;
                        }else{
                            if(!comparecard(deck[i], deck[count])){
                                count = i;
                            }
                        }
                    }
                }
            }
            if(flag){
                printf("Child %d, pid %d: play %c%c\n", childnum + 1, getpid(), deck[count][0], deck[count][1]);
                strcpy(command, play);
                command[5] = deck[count][0];
                command[6] = deck[count][1];
                used_card[count] = 1;
                write(c2p_fd[childnum][1], command, 7);
            }else{
                printf("Child %d, pid %d: pass\n", childnum + 1, getpid());
                write(c2p_fd[childnum][1], pass, 4);
            }
        }else if(checkbuf(buf, finish) && buf[4] == finish[4] && buf[5] == finish[5]){
            bool flag = true;
            for(i = 0; i < cardnum; i++){
                if(used_card[i] == 0){
                    flag = false;
                }
            }
            if(flag){
                write(c2p_fd[childnum][1], finish, 6);
            }else{
                write(c2p_fd[childnum][1], no, 2);
            }
        }else if(checkbuf(buf, restart) && buf[4] == restart[4] && buf[5] == restart[5] && buf[6] == restart[6]){
            bool flag = true;
            char command[7];
            printf("Child %d, pid %d: hold ", childnum + 1, getpid());
            int smallcard = 0;
            for(i = 0; i < cardnum; i++){
                if(used_card[i] == 0){
                    printf("%c%c ", deck[i][0], deck[i][1]);
                }
            }
            printf("\n");
            for(i = 0; i < cardnum; i++){
                if(flag){
                    smallcard = i;
                    flag = false;
                }else{
                    if(!comparecard(deck[smallcard], deck[i])){
                        deck[smallcard][0] = deck[i][0];
                        deck[smallcard][1] = deck[i][1];
                    }
                }
            }
            printf("Child %d, pid %d: play %c%c \n", childnum + 1, getpid(), deck[smallcard][0], deck[smallcard][1]);
            used_card[smallcard] = 1;
            strcpy(command, play);
            command[5] = deck[smallcard][0];
            command[6] = deck[smallcard][1];
            write(c2p_fd[childnum][1], command, 7);
        }else if(checkbuf(buf, done)){
            return;
        }
    }
    return;
}

bool comparecard(char before[2], char thatcard[2]){
    int k, suit1, suit2, rank1, rank2;
    for(k = 0; k < 4; k++){
        if(before[0] == suit[k]) suit1 = k;
        if(thatcard[0] == suit[k]) suit2 = k;
    }
    for(k = 0; k < 13; k++){
        if(before[1] == rank[k]) rank1 = k;
        if(thatcard[1] == rank[k]) rank2 = k; 
    }
    if(rank1 < rank2){
        return false;
    }else if(rank1 == rank2){
        if(suit1 > suit2){
            return false;
        }else{
            return true;
        }
    }else{
        return true;
    }
}

bool checkbuf(char buf[30], char* command){
    return buf[0] == hold[0] && buf[1] == hold[1] && buf[2] == hold[2] && buf[3] == hold[3];
}

bool duplicate(int thatcard, char deck[60][2]){
    for(int i = 1; i < thatcard; i++){
        if(deck[thatcard][0]==deck[i][0] && deck[thatcard][1]==deck[i][1]){
            return true;
        }
    }
    return false;
}
int main(int argc, char* argv[]){
    int childnum = atoi(argv[1]);
    int i,j;
    int pid; 
    char deck[60][2];
    int thatcard;
    int rankorsuit;
    for(i = 0; i < childnum; i++){
        if(pipe(p2c_fd[i]) < 0){
            printf("Pipe creation error\n");
            exit(1);
        }
        if(pipe(c2p_fd[i]) < 0){
            printf("Pipe creation error\n");
            exit(1);
        }
    }
    for(i = 1; i <= childnum; i++){
        if((pid = fork()) < 0){
            printf("Invalid Fork\n");
            exit(1);
        }else if (pid > 0){
            child[i] = pid;
            close(p2c_fd[i][0]);
            close(c2p_fd[i][1]);
        }else if (pid == 0){
            close(p2c_fd[childnum][1]);
            close(c2p_fd[childnum][0]);
            run_player(i);
        }
    }

    char ch;

    if(pid > 0){
        printf("Parent: there are %d players, ", childnum);
        for(i = 0; i < childnum; i++){
            printf("%d ", child[i+1]);
        }
        printf("\n");
        while(!feof(stdin)){
            ch = fgetc(stdin);
            if (ch == ' '){
                thatcard++;
                rankorsuit = 0;
            } else{
                deck[thatcard][rankorsuit] = ch;
                rankorsuit++;
                if(duplicate(thatcard, deck)){
                    printf("Parent: duplcated %c%c discarded\n", deck[thatcard][0],deck[thatcard][1]);
                    thatcard--;
                }
            }
        }
        char buf[30];
        int idx;
        int n;
        for(i = 0; i < (thatcard + 1); i++){
            idx = i % childnum;
            write(p2c_fd[idx][1], deck[i], 2);
            while(true){
                if((n = read(c2p_fd[i][0], buf, 30)) > 0){
                    buf[n] = 0;
                    if(checkbuf(buf, okay)){
                        break;
                    }
                }
            }
        }
        int first;
        bool flag = false;
        for(i = 0; i < childnum; i++){
            write(p2c_fd[i][1], d3, 8);
            while(true){
                if((n = read(c2p_fd[i][0], buf, 30)) > 0){
                    buf[n] = 0;
                    if(buf[0] == yes[0] && buf[1] == yes[1] && buf[2] == yes[2]){
                        flag = true;
                        first = i;
                        break;
                    }else if(buf[0] == no[0] && buf[1] == no[1]){
                        break;
                    }
                }
            }
            if(flag) break;
        }

        write(p2c_fd[first][1], begin, 5);
        char usedcard[2];
        while(true){
            if((n = read(c2p_fd[i][0], buf, 30)) > 0){
                buf[n] = 0;
                if(checkbuf(buf, play)){
                    if(buf[5] == 'D' && buf[6] == '3'){
                        usedcard[0] = buf[5];
                        usedcard[1] = buf[6];
                        printf("Parent: child %d plays %c%c\n", first + 1, usedcard[0],usedcard[1]);
                        break;
                    }
                }
            }
        }

        int endchild = 0;
        int current = first;
        int endchildqueue[60] = {0};
        char command[9];
        int passchild = 0;
        int count = 0;
        int ranklist[60];
        int stillplay = childnum;
        int wordcount = 9;

        strcpy(command, toplay);

        for(i = 0; i < childnum; i++){
            ranklist[i] = childnum + 1;
        }

        while(endchild < (childnum - 1)){
            for(i = 0; i < childnum; i++){
                current = current++ % childnum;
                if(endchildqueue[current] != 1){
                    write(p2c_fd[current][1], command, 9);
                    while(true){
                        if((n = read(c2p_fd[current][0], buf, 30)) > 0){
                            buf[n] = 0;
                            if(checkbuf(buf, play)){                
                                if(count != endchild){
                                    stillplay--;
                                    count = endchild;
                                }
                                usedcard[0] = buf[5];
                                usedcard[1] = buf[6];
                                printf("Parent: child %d plays %c%c\n", current + 1, usedcard[0], usedcard[1]);
                                strcpy(command, toplay);
                                command[7] = usedcard[0];
                                command[8] = usedcard[1];
                                wordcount = 9;
                                
                                if(checkfinish(current)){
                                    printf("Parent: child %d finishes\n", current + 1);
                                    endchildqueue[current]++;
                                    ranklist[endchild] = current;
                                    endchild++;
                                }
                                break;
                            } else if(checkbuf(buf, pass)){
                                printf("Parent: child &d passes\n", current + 1);
                                passchild++;
                                if(passchild == (stillplay - 1)){
                                    if(passchild != endchild){
                                        stillplay--;
                                        count = endchild;
                                    }
                                    strcpy(command, restart);
                                    wordcount = 7;
                                    passchild = 0;
                                    break;
                                }
                                break;
                            }                  
                        }
                    }
                }
                if(endchild == (childnum-1)){
                    break;
                }
            }
        }
        for(i = 0; i < childnum -1; i++){
            printf("Parent: child %d rank %d\n", ranklist[i], i + 1);
        }
        for(i = 0; i < childnum; i++){
            write(p2c_fd[i][1], done, 4);
            if(endchildqueue[i] == 0){
                printf("Parent: child %d is loser\n", i + 1);
            }
        }
        for(i = 0; i < childnum; i++){
            wait(NULL);
        }
    }
    return 0;
}

bool checkfinish(int thatchild){
    char buf[30];
    int n;
    write(p2c_fd[thatchild][1], finish, 6);
    while(true){
        if((n = read(c2p_fd[thatchild][0], buf, 30)) > 0){
            buf[n] = 0;
            if(buf[0] == finish[0] && buf[1] == finish[1] && buf[2] == finish[2] && buf[3] == finish[3] && buf[4] == finish[4] && buf[5] == finish[5]){
                return true;
            }else{
                return false;
            }
        }
    }
}