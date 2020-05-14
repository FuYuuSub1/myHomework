#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>


char* teamlist[] = { "Salzburg", "Brugge", "Genk", "Ludogorets", "Zagreb",
"SlaviaPraha", "Viktoria", "Liverpool", "ManCity", "ManUnited",
"Chelsea", "Leicester", "Arsenal", "Wolverhampton", "Tottenham",
"Everton", "Flora", "Levadia", "Kaiju", "Paris",
"Lyon", "Marseille", "Lille", "Bayern", "Leipzig",
"Dortmund", "Leverkusen", "Monchengladbach", "Wolfsburg", "Olympiacos",
"PAOK", "Dundalk", "Juventus", "Atalanta", "Internazionale",
"Lazio", "Napoli", "Roma", "Milan", "Ajax", 
"Eindhoven", "Feyenoord", "Alkmaar", "Porto", "Benfica",
"Braga", "Sporting", "Zenit", "Celtic", "Rangers",
"Belgrade", "Barcelona", "RealMadrid", "Atletico", "Sevilla",
"Getafe", "Sociedad", "Valencia", "Bilbao", "Villarreal",
"Betis", "Basel", "Galatasaray", "Istanbul", "Shakhtar",
"Kyiv", "Crvenazvezda", "Lokomotiv" };

char* country[] = { "Austria", "Belgium", "Belgium", "Bulgaria", "Croatia",
"Czech", "Czech", "England", "England", "England",
"England", "England", "England", "England", "England",
"England", "Estonia", "Estonia", "Estonia", "France", 
"France", "France", "France", "Germany", "Germany", 
"Germany", "Germany", "Germany", "Germany", "Greece", 
"Greece", "Ireland", "Italy", "Italy", "Italy", "Italy", 
"Italy", "Italy", "Italy", "Netherlands", "Netherlands", 
"Netherlands", "Netherlands", "Portugal", "Portugal", "Portugal", 
"Portugal", "Russia", "Scotland", "Scotland", "Serbia", 
"Spain", "Spain", "Spain", "Spain", "Spain", 
"Spain", "Spain", "Spain", "Spain", "Spain", 
"Switzerland", "Turkey", "Turkey", "Ukraine", "Ukraine", 
"Serbia", "England", "Russia" };

char groupidx[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'}; 

char* mode_name(char mode);
void test(int child, int ac, char* av[]);
bool checkgroup(char* group[4]);
int searchpot(char* thatgroup, char* pot[4][8]);
bool checkpot(int pot[4]);
char* searchcountry(char* group);
bool checkcountry(char* countryidx[4]);
bool checkpotfortrial(int i, int potidx[], int groupnum);
int getgroupid(int groupid[4]);
bool ri(char* rival1, char* rival2, char* group[8][4])

char* mode_name(char mode){
    switch(mode){
        case 'T':
            return "test mode";
        case 'G':
            return "generation mode";
        case 'E':
            return "evaluation mode";
        default:
            return "";
    }
}

void test(int child, int ac, char* av[]){
    char* pot[4][8];
    char* group[8][4];
    int potidx[8][4]; 
    char* countryidx[8][4];
    int i,j,n,m;
    int c = 3;

    for(i = 0; i < 4; i++){
        printf("Child %d, pid %d: teams for pot %d are ", child, getpid(), i+1);
        for(j = 0; j < 8; j++){
            printf("%s ", av[c]);
            pot[i][j] = av[c];
            c++;
        }
        printf("\n");
    }

    for(i = 0; i < 8; i++){
        printf("Child %d, pid %d: teams for group %c are ", child, getpid(), groupidx[i]);
        for(j = 0; j < 4; j++){
            printf("%s ", av[c]);
            group[i][j] = av[c];
            c++;
        }
        printf("\n");
        if(checkgroup(group[i])){
            printf("Child %d, pid %d: Invalid grouping\n", child, getpid());
            return;
        }
        printf("Child %d, pid %d: pots for group %c are ", child, getpid(), groupidx[i]);
        for(n = 0; n < 4; n++){
            potidx[i][n] = searchpot(group[i][n], pot);
            printf("%d ", potidx[i][n]);
        }
        printf("\n");
        if(checkpot(potidx[i])){
            printf("Child %d, pid %d: Invalid grouping\n", child, getpid());
            return;
        }
        printf("Child %d, pid %d: countries for grouping %c are ", child, getpid(), groupidx[i]);
        for(n = 0; n < 4; n++){
            countryidx[i][n] = searchcountry(group[i][n]);
            printf("%s ", countryidx[i][n]);
        }
        printf("\n");
        if(checkcountry(countryidx[i])){
            printf("Child %d, pid %d: Invalid grouping\n", child, getpid());
            return;
        }
    }
    printf("Child %d, pid %d: Valid grouping\n", child, getpid());
    return;
}
bool checkgroup(char* group[4]){
    for(int i = 0; i < 4; i++){
        for(int j = i + 1; j < 4; j++){
            if(strcmp(group[i], group[j]) == 0){
                return true;
            }
        }
    }
    return false;
}

int searchpot(char* thatgroup, char* pot[4][8]){
    int i,j;
    int idx = -1;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 8; j++){
            if(!strcmp(thatgroup, pot[i][j])){
                idx = i + 1;
            }
        }
    }
    return idx;
}

bool checkpot(int pot[4]){
    int i, j;
    for(i = 0; i < 4; i++){
        if(pot[i] < 0){
            return true;
        }
        for(j = i + 1; j < 4; j++){
            if(pot[i] == pot[j]){
                return true;
            }
        }
    }
    return false;
}

char* searchcountry(char* group){
    int i;
    for(i = 0; i < 68; i++){
        if(strcmp(group, teamlist[i]) == 0){
            return country[i];
        }
    }
}

bool checkcountry(char* countryidx[4]){
    int i, j;
    for(i = 0; i < 4; i++){
        for(j = i + 1; j < 4; j++){
            if(strcmp(countryidx[i], countryidx[j]) == 0){
                return true;
            }
            if((strcmp(countryidx[i], "Ukraine") == 0 && strcmp(countryidx[j], "Russia") == 0) || (strcmp(countryidx[i], "Russia") == 0 && strcmp(countryidx[j], "Ukraine") == 0)){
                return true;
            }
        }
    }
    return false;
}

void generation(int child, int ac, char* av[]){
    int trialnum = atoi(av[33 + 2 * child]);
    int denominator = trialnum;
    int seed = atoi(av[34 + 2 * child]);
    char* pot[4][8];
    int i, j;
    int c = 3;
    int success = 0;
    srand(seed);

    for(i = 0; i < 4; i++){
        printf("Child %d, pid %d: teams for pot %d are ", child, getpid(), i+1);
        for(j = 0; j < 8; j++){
            printf("%s ", av[c]);
            pot[i][j] = av[c];
            c++;
        }
        printf("\n");
    }

    while(trialnum != 0){
        char* group[8][4];
        int groupid[8][4];
        int thatgroupid;
        int potidx[4][8];
        char* countryidx[8][4];
        int groupnum;
        int failure = 0;
        int n,m;

        for(n = 0; n < 8; n++){
            for(m = 0; m < 4; m++){
                groupid[n][m] = -1;
            }
        }
        for(n = 0; n < 8; n++){
            for(m = 0; m < 4; m++){
                do{ 
                    groupnum = rand() % 8;
                } while (!checkpotfortrial(n, potidx[m], groupnum));
                potidx[m][n] = groupnum;
                thatgroupid = getgroupid(groupid[groupnum]);
                groupid[groupnum][thatgroupid] = 0;
                group[groupnum][thatgroupid] = pot[m][n];
                countryidx[groupnum][thatgroupid] = searchcountry(pot[m][n]);
            }
        }
        for(n = 0; n < 8; n++){
            printf("Child %d, pid %d: teams for group %c are ", child, getpid(), groupidx[n]);
            for(m = 0; m < 4; m++){
                printf("%s ", group[n][m]);
            }
            printf("\n");
            if(checkcountry(countryidx[n])){
                failure++;
                printf("Child %d, pid %d: Invalid grouping\n", child, getpid());
            }
        }
        if(failure == 0){
            success++;
        }
        trialnum--;
    }
    float rate = (float)success / denominator * 100.0;
    printf("Child %d, pid %d: Seed %d with %d valid drawings out of %d (%.2f%%)\n", child, getpid(), seed, success, denominator, rate);
    return;
}

bool checkpotfortrial(int i, int potidx[], int groupnum){
    int k;
    if(i == 0) return true;
    for(k = 0; k < i; k++){
        if(potidx[k] == groupnum){
            return false;
        }
    }
    return true;    
}

int getgroupid(int groupid[4]){
    for(int i = 0; i < 4; i++){
        if(groupid[i] == -1){
            return i;
        }
    }
    return 0;
}

void evaluation(int child, int ac, char* av[], int numchild){
    int trialnum = atoi(av[33 + 2 * child]);
    int denominator = trialnum;
    int seed = atoi(av[34 + 2 * child]);
    char* pot[4][8];
    int i, j;
    int c = 3;
    int success = 0;
    int rivalmatch = 0;
    char* rival1 = av[35 + 2 * numchild];
    char* rival2 = av[36 + 2 * numchild];
    srand(seed);

    for(i = 0; i < 4; i++){
        printf("Child %d, pid %d: teams for pot %d are ", child, getpid(), i+1);
        for(j = 0; j < 8; j++){
            printf("%s ", av[c]);
            pot[i][j] = av[c];
            c++;
        }
        printf("\n");
    }

    while(trialnum != 0){
        char* group[8][4];
        int groupid[8][4];
        int thatgroupid;
        int potidx[4][8];
        char* countryidx[8][4];
        int groupnum;
        int failure = 0;
        int n,m;
        for(n = 0; i < 8; i++){
            for(m = 0; j < 4; j++){
                groupid[n][m] = -1;
            }
        }
        for(n = 0; i < 8; i++){
            for(m = 0; j < 4; j++){
                do{ 
                    groupnum = rand() % 8;
                } while (!checkpotfortrial(i, potidx[j], groupnum));
                potidx[m][n] = groupnum;
                thatgroupid = getgroupid(groupid[groupnum]);
                groupid[groupnum][thatgroupid] = 0;
                group[groupnum][thatgroupid] = pot[m][n];
                countryidx[groupnum][thatgroupid] = searchcountry(pot[m][n]);
            }
        }
        for(n = 0; n < 8; n++){
            printf("Child %d, pid %d: teams for group %c are ", child, getpid(), groupidx[n]);
            for(m = 0; m < 4; m++){
                printf("%s ", group[n][m]);
            }
            printf("\n");
            if(checkcountry(countryidx[n])){
                failure++;
                printf("Child %d, pid %d: Invalid grouping\n", child, getpid());
            }
        }
        if(failure == 0){
            success++;
            if(ri(rival1, rival2, group)){
                rivalmatch++;
            }
        }
        trialnum--;
    }
    float rate2 = (float)rivalmatch / success * 100.0;
    printf("Child %d, pid %d: Seed %d (%d) rivals %s and %s meeting %d out of %d valid drawings (%.2f%%)\n", child, getpid(), seed, denominator, rival1, rival2, rivalmatch, success, rate2);
    return;
}

bool ri(char* rival1, char* rival2, char* group[8][4]){
    int r1groupidx, r2groupidx;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 4; j++){
            if (strcmp(group[i][j], rival1) == 0){
                r1groupidx = i;
            }
            if (strcmp(group[i][j], rival2) == 0){
                r2groupidx = i;
            }
        }
    }
    if(r1groupidx == r2groupidx){
        return true;
    }else{
        return false;
    }
}

int main(int argc, char *argv[]) {
    int pid;
	int numchild = atoi(argv[1]);
	char ca = argv[2][0];
    int i;
	srand(time(NULL));

    for (i = 1; i <= numchild; i++){
        if((pid = fork()) < 0){
            printf("Invalid Fork\n");
            exit(1);
        } else if(pid > 0 && i == 1){
            printf("Parent, pid %d: %d children, %s\n", getpid(), numchild, mode_name(ca) );
        } else if(pid == 0){
            switch(ca){
                case 'T':
                    test(i, argc, argv);
                case 'G':
                    generation(i, argc, argv);
                case 'E':
                    evaluation(i, argc, argv, numchild);
            }
            exit(0);
        }
    }
    if(pid > 0){
        for(i = 0; i < numchild; i++){
            wait(0);
        }
    }
	return 0;
}