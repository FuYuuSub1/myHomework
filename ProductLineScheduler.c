#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>

struct order {
    char orderid[5];
    int dueyear;
    int duemonth;
    int dueday;
    int quantity;
    char producttype[9];
    int priority;
};

int fd[2][2];

char* c[3] = { "PX","PY","PZ" };
char finish[] = "finish";
char ord[] = "ord";
char next[] = "next";
char ok[] = "ok";
char reject[] = "reject";
char end[] = "end";
void sendok(int number);
void checkok(int number);
void date2numeric(char* date, int* date_arr);
int leapyear(int* date);
int getperiodlength(int* start, int* end);
bool smaller(int d1[3], int y, int m, int d);
bool checkdue(int due[3], int start[3], int end[3]);
double power(double a, int b);
double sqerror(int a, int b, int c);

const int daysinmonth[12] = { 31, 28, 31, 30, 31, 30,
31, 31, 30, 31, 30, 31 };

char* PX = "Plant_X";
char* PY = "Plant_Y";
char* PZ = "Plant_Z";
char* done = "done";

void date2numeric(char* date, int date_arr[3]) {
    char* t;
    t = strtok(date, "-");
    date_arr[0] = atoi(t);
    t = strtok(NULL, "-");
    date_arr[1] = atoi(t);
    t = strtok(NULL, "-");
    date_arr[2] = atoi(t);
}

int leapyear(int* date) {
    int y = date[0];
    if (date[1] <= 2) y--;
    return (y / 4 - y / 100 + y / 400);
}


bool smaller(int d1[3], int y, int m, int d) {
    if (d1[0] < y) return true;
    if (d1[0] == y && d1[1] < m) return true;
    if (d1[0] == y && d1[1] == m && d1[2] < d) return true;
    return false;
}

//check the due date is within the timeslot
bool checkdue(int due[3], int start[3], int end[3]) {
    int i = 0;
    if (!smaller(due, start[0], start[1], start[2]) &&
        smaller(due, end[0], end[1], end[2])) {
        return true;
    }
    return false;
}

//calculate the day difference
int getperiodlength(int* start, int* end) {
    int i;
    int old = start[0] * 365 + start[2];
    int new = end[0] * 365 + end[2];
    for (i = 0; i < start[1] - 1; i++) {
        old += daysinmonth[i];
    }
    for (i = 0; i < end[1] - 1; i++) {
        new += daysinmonth[i];
    }
    old += leapyear(start);
    new += leapyear(end);
    return (new - old + 1);
}

double power(double a, int b) {
    double r = a;
    int x;
    for (x = 0; x < b - 1; x++) {
        r = r * a;
    }
    return r;
}

double sqerror(int a, int b, int c) {
    double mean = (a + b + c) / 3.0;
    double sqe = power((a - mean), 2) + power((b - mean), 2) + power((c - mean), 2);
    return sqe;
}


int main(int argc, char* argv[]) {
    struct order order_arr[10000];
    int periodstart[3] = { 0 };
    int periodend[3] = { 0 };
    int daydiff;
    int orderindex = 0;
    char** X = calloc(daydiff, sizeof(char));
    char** Y = calloc(daydiff, sizeof(char));
    char** Z = calloc(daydiff, sizeof(char));

    printf("\t~~WELCOME TO PLS~~\n\n");
    bool running = true;
    while (running) {
        char* token;
        char command[100];
        int i;
        int actionlength;
        printf("Please enter:\n");
        fgets(command, sizeof(command), stdin);
        token = strtok(command, " ");
        if (strcmp(token, "addPEIOD") == 0) {
            char* startdate;
            char* enddate;
            startdate = strtok(NULL, " ");
            enddate = strtok(NULL, " \n");
            date2numeric(startdate, periodstart);
            date2numeric(enddate, periodend);
            daydiff = getperiodlength(periodstart, periodend);
            int k;
            for (k = 0; k < daydiff; k++) {
                X[k] = calloc(5, sizeof(char));
                X[k] = 0;
                Y[k] = calloc(5, sizeof(char));
                Y[k] = 0;
                Z[k] = calloc(5, sizeof(char));
                Z[k] = 0;
            }

        }
        else if (strcmp(token, "addORDER") == 0) {
            char* odr;
            char* duedate;
            char* amountstr;
            int amount;
            char* ptype;
            int duedatenum[3];
            odr = strtok(NULL, " ");
            duedate = strtok(NULL, " ");
            amountstr = strtok(NULL, " ");
            ptype = strtok(NULL, "\n");
            amount = atoi(amountstr);
            date2numeric(duedate, duedatenum);
            if (checkdue(duedatenum, periodstart, periodend)) {
                strcpy(order_arr[orderindex].orderid, odr);
                order_arr[orderindex].dueyear = duedatenum[0];
                order_arr[orderindex].duemonth = duedatenum[1];
                order_arr[orderindex].dueday = duedatenum[2];
                order_arr[orderindex].quantity = amount;
                strcpy(order_arr[orderindex].producttype, ptype);
                orderindex++;
            }
            else {
                printf("Due day is out of the range.\n");
            }

        }
        else if (strcmp(token, "addBATCH") == 0) {
            char* filename;
            filename = strtok(NULL, "\n");
            char c;
            char str[60];
            FILE* fp;
            int count = 0;
            //for storage
            char* call;
            char* odr;
            char* duedate;
            char* amountstr;
            int amount;
            char* ptype;
            int duedatenum[3];

            //counting lines
            fp = fopen(filename, "r"); // read mode
            if (fp == NULL) {
                perror("Error while opening the file.\n");
                exit(EXIT_FAILURE);
            }
            while (fgets(str, 60, fp) != NULL) {

                call = strtok(str, " ");
                odr = strtok(NULL, " ");
                duedate = strtok(NULL, " ");
                amountstr = strtok(NULL, " ");
                ptype = strtok(NULL, "\n");
                amount = atoi(amountstr);

                date2numeric(duedate, duedatenum);

                if (checkdue(duedatenum, periodstart, periodend)) {
                    strcpy(order_arr[orderindex].orderid, odr);
                    order_arr[orderindex].dueyear = duedatenum[0];
                    order_arr[orderindex].duemonth = duedatenum[1];
                    order_arr[orderindex].dueday = duedatenum[2];
                    order_arr[orderindex].quantity = amount;
                    strcpy(order_arr[orderindex].producttype, ptype);

                    orderindex++;

                }
                else {
                    printf("Due day is out of the range.\n");
                }

            }
            // Close the file 
            fclose(fp);
        }
        else if (strcmp(token, "runPLS") == 0) {

            int pid, r;

            char buf[80];

            if (pipe(fd[0]) < 0) { //for parent send message
                printf("Pipe creation error\n");
                exit(1);
            }
            if (pipe(fd[1]) < 0) { //for child send message
                printf("Pipe creation error\n");
                exit(1);
            }
            pid = fork();
            if (pid < 0) {
                printf("Fork failed\n");
                exit(1);
            }
            else if (pid > 0) {  //parent
                close(fd[0][0]);
                close(fd[1][1]);
                //here do the allocation
                char* filename;
                char* mode;
                mode = strtok(NULL, " ");
                strtok(NULL," ");
                strtok(NULL," ");
                strtok(NULL," ");
                filename = strtok(NULL,"\n");
                char fn[50];
                strcpy(fn,filename);
                write(fd[0][1], fn, 50);
                checkok(1);

                if (strcmp(mode, "FCFS") == 0) {
                    //FCFS
                    //sooner due, do first
                    //sort due date of all orders
                    //for each order,
                    //find how many days of X Y Z from due date
                    //find a combination of XYZ satisfies the quantity where XYZ < due, mean square error minimum
                    //allocate period
                

                    //sort due date
                    int i, j, k;
                    for (i = 0; i < orderindex - 1; i++) {
                        for (j = i + 1; j < orderindex; j++) {
                            if (order_arr[i].dueyear > order_arr[j].dueyear) {
                                struct order temp = order_arr[i];
                                order_arr[i] = order_arr[j];
                                order_arr[j] = temp;
                            }
                            else if (order_arr[i].dueyear == order_arr[j].dueyear && order_arr[i].duemonth > order_arr[j].duemonth) {
                                struct order temp = order_arr[i];
                                order_arr[i] = order_arr[j];
                                order_arr[j] = temp;
                            }
                            else if (order_arr[i].dueyear == order_arr[j].dueyear && order_arr[i].duemonth == order_arr[j].duemonth
                                && order_arr[i].dueday > order_arr[j].dueday) {
                                struct order temp = order_arr[i];
                                order_arr[i] = order_arr[j];
                                order_arr[j] = temp;
                            }
                        }
                    }
                    
    
                    for (i = 0; i < orderindex; i++) {
                        
                        int remainingX, remainingY, remainingZ;
                        int thisorderdue[3];
                        thisorderdue[0] = order_arr[i].dueyear;
                        thisorderdue[1] = order_arr[i].duemonth;
                        thisorderdue[2] = order_arr[i].dueday;
                        int duediff = getperiodlength(periodstart, thisorderdue);
                        int remainder = 0;
                        int countx, county, countz;
                        for (k = 0; k < duediff; k++) {
                            if (X[k] == 0) {
                                remainingX = duediff - k;
                                break;
                            }
                        }
                        for (k = 0; k < duediff; k++) {
                            if (Y[k] == 0) {
                                remainingY = duediff - k;
                                break;
                            }
                        }
                        for (k = 0; k < duediff; k++) {
                            if (Z[k] == 0) {
                                remainingZ = duediff - k;
                                break;
                            }
                        }


                        
                        int finalX = 100, finalY = 0, finalZ = 100;
                        int n, m, o;
                        if (order_arr[i].quantity < 300) {
                            finalX = 1; finalY = 0; finalZ = 0;
                        }
                        else {
                            for (n = 0; n < remainingX; n++) {
                                for (m = 0; m < remainingY; m++) {
                                    for (o = 0; o < remainingZ; o++) {
                                        if (order_arr[i].quantity % 100 == 0) {
                                            if (n * 300 + m * 400 + o * 500 == order_arr[i].quantity) {
                                                if (sqerror(n, m, o) < sqerror(finalX, finalY, finalZ)) {
                                                    finalX = n; finalY = m; finalZ = o;
                                                }
                                            }
                                        }
                                        else {
                                            if (n * 300 + m * 400 + o * 500 == (order_arr[i].quantity / 100 + 1) * 100) {
                                                if (sqerror(n, m, o) < sqerror(finalX, finalY, finalZ)) {
                                                    finalX = n; finalY = m; finalZ = o;
                                                    remainder = (order_arr[i].quantity / 100 + 1) * 100 - order_arr[i].quantity;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        
                        int quantity;
                        int startyear;
                        int startmonth;
                        int startday;
                        char chararr[20];
                        bool printed = false;
                        if ((remainingX + finalX) != remainingX) {
                            printed = true;
                            write(fd[0][1], ord, 3);
                            checkok(1);
                            strcpy(chararr,c[0]);
                            write(fd[0][1], chararr, 20);
                            checkok(1);
                            write(fd[0][1], &periodstart[0], sizeof(periodstart[0]));
                            checkok(1);
                            write(fd[0][1], &periodstart[1], sizeof(periodstart[1]));
                            checkok(1);
                            write(fd[0][1], &periodstart[2], sizeof(periodstart[2]));
                            checkok(1);
                            write(fd[0][1], &periodend[0], sizeof(periodend[0]));
                            checkok(1);
                            write(fd[0][1], &periodend[1], sizeof(periodend[1]));
                            checkok(1);
                            write(fd[0][1], &periodend[2], sizeof(periodend[2]));
                            checkok(1);

                            write(fd[0][1], order_arr[i].orderid, 5);
                            checkok(1);

                            write(fd[0][1], order_arr[i].producttype, 9);
                            checkok(1);

                            write(fd[0][1], &order_arr[i].dueyear, sizeof(order_arr[i].dueyear));
                            checkok(1);

                            write(fd[0][1], &order_arr[i].duemonth, sizeof(order_arr[i].duemonth));
                            checkok(1);

                            write(fd[0][1], &order_arr[i].dueday, sizeof(order_arr[i].dueday));
                            checkok(1);

                            int counter = 0;
                            for (n = remainingX; n < remainingX + finalX; n++) {
                                counter += 1;
                                X[n] = order_arr[i].orderid;
                                write(fd[0][1], next, 4);
                                checkok(1);
                                startday = periodstart[2] + countx;
                                countx++;
                                startyear = periodstart[0];
                                startmonth = periodstart[1];
                                while (startday > daysinmonth[periodstart[1] - 1]) {
                                    startday -= daysinmonth[periodstart[1] - 1];
                                    startmonth += 1;
                                    if (startmonth > 12) {
                                        startmonth = 1;
                                        startyear += 1;
                                    }
                                }
                                write(fd[0][1], &startyear, sizeof(startyear));
                                checkok(1);

                                write(fd[0][1], &startmonth, sizeof(startmonth));
                                checkok(1);
                                write(fd[0][1], &startday, sizeof(startday));
                                checkok(1);
                                quantity = 300 - remainder;
                                remainder = 0;
                                write(fd[0][1], &quantity, sizeof(quantity));
                                checkok(1);


                            }
                            
                            write(fd[0][1], end, 3);
                            checkok(1);
                        }
                        
                        if ((remainingY + finalY) != remainingY) {
                            printed = true;
                            write(fd[0][1], ord, 3);
                            checkok(1);
                            strcpy(chararr,c[1]);
                            write(fd[0][1], chararr, 20);
                            checkok(1);
                            write(fd[0][1], &periodstart[0], sizeof(periodstart[0]));
                            checkok(1);
                            write(fd[0][1], &periodstart[1], sizeof(periodstart[1]));
                            checkok(1);
                            write(fd[0][1], &periodstart[2], sizeof(periodstart[2]));
                            checkok(1);
                            write(fd[0][1], &periodend[0], sizeof(periodend[0]));
                            checkok(1);
                            write(fd[0][1], &periodend[1], sizeof(periodend[1]));
                            checkok(1);
                            write(fd[0][1], &periodend[2], sizeof(periodend[2]));
                            checkok(1);
                            
                            write(fd[0][1], order_arr[i].orderid, 5);
                            checkok(1);
                            
                            write(fd[0][1], order_arr[i].producttype, 9);
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].dueyear, sizeof(order_arr[i].dueyear));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].duemonth, sizeof(order_arr[i].duemonth));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].dueday, sizeof(order_arr[i].dueday));
                            checkok(1);
                            
							
                            for (m = remainingY; m < remainingY + finalY; m++) {
                                Y[m] = order_arr[i].orderid;
                                write(fd[0][1], next, 4);
                                checkok(1);
                                startday = periodstart[2] + county;
                                county++;
                                startyear = periodstart[0];
                                startmonth = periodstart[1];
                                while (startday > daysinmonth[periodstart[1] - 1]) {
                                    startday -= daysinmonth[periodstart[1] - 1];
                                    startmonth += 1;
                                    if (startmonth > 12) {
                                        startmonth = 1;
                                        startyear += 1;
                                    }
                                }
                                write(fd[0][1], &startyear, sizeof(startyear));
                                checkok(1);
                                write(fd[0][1], &startmonth, sizeof(startmonth));
                                checkok(1);
                                write(fd[0][1], &startday, sizeof(startday));
                                checkok(1);
                                quantity = 400 - remainder;
                                remainder = 0;
                                write(fd[0][1], &quantity, sizeof(quantity));
                                checkok(1);
                            }
                            write(fd[0][1], end, 3);
                            checkok(1);
                        }
                        if ((remainingZ + finalZ) != remainingZ) {
                            printed = true;
                            write(fd[0][1], ord, 3);
                            checkok(1);
                            strcpy(chararr,c[2]);
                            write(fd[0][1], chararr, 20);
                            checkok(1);
                            write(fd[0][1], &periodstart[0], sizeof(periodstart[0]));
                            checkok(1);
                            write(fd[0][1], &periodstart[1], sizeof(periodstart[1]));
                            checkok(1);
                            write(fd[0][1], &periodstart[2], sizeof(periodstart[2]));
                            checkok(1);
                            write(fd[0][1], &periodend[0], sizeof(periodend[0]));
                            checkok(1);
                            write(fd[0][1], &periodend[1], sizeof(periodend[1]));
                            checkok(1);
                            write(fd[0][1], &periodend[2], sizeof(periodend[2]));
                            checkok(1);
                            
                            write(fd[0][1], order_arr[i].orderid, 5);
                            checkok(1);
                            
                            write(fd[0][1], order_arr[i].producttype, 9);
                            checkok(1);
                           
                            write(fd[0][1], &order_arr[i].dueyear, sizeof(order_arr[i].dueyear));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].duemonth, sizeof(order_arr[i].duemonth));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].dueday, sizeof(order_arr[i].dueday));
                            checkok(1);
                            
							
                            for (o = remainingZ; o < remainingZ + finalZ; o++) {
                                Z[o] = order_arr[i].orderid;
                                startday = periodstart[2] + countz;
                                countz++;
                                startyear = periodstart[0];
                                startmonth = periodstart[1];
                                while (startday > daysinmonth[periodstart[1] - 1]) {
                                    startday -= daysinmonth[periodstart[1] - 1];
                                    startmonth += 1;
                                    if (startmonth > 12) {
                                        startmonth = 1;
                                        startyear += 1;
                                    }
                                }
                                write(fd[0][1], next, 4);
                                checkok(1);
                                write(fd[0][1], &startyear, sizeof(startyear));
                                checkok(1);
                                write(fd[0][1], &startmonth, sizeof(startmonth));
                                checkok(1);
                                write(fd[0][1], &startday, sizeof(startday));
                                checkok(1);
                                quantity = 500 - remainder;
                                remainder = 0;
                                write(fd[0][1], &quantity, sizeof(quantity));
                                checkok(1);

                            }
                           
                            write(fd[0][1], end, 3);
                            checkok(1);
                        }
                        if (printed = false) {
                            write(fd[0][1], ord, 3);
                            checkok(1);
                            strcpy(chararr, c[0]);
                            write(fd[0][1], chararr, 20);
                            checkok(1);
                            write(fd[0][1], &periodstart[0], sizeof(periodstart[0]));
                            checkok(1);
                            write(fd[0][1], &periodstart[1], sizeof(periodstart[1]));
                            checkok(1);
                            write(fd[0][1], &periodstart[2], sizeof(periodstart[2]));
                            checkok(1);
                            write(fd[0][1], &periodend[0], sizeof(periodend[0]));
                            checkok(1);
                            write(fd[0][1], &periodend[1], sizeof(periodend[1]));
                            checkok(1);
                            write(fd[0][1], &periodend[2], sizeof(periodend[2]));
                            checkok(1);
                         
                            write(fd[0][1], order_arr[i].orderid, 5);
                            checkok(1);
                          
                            write(fd[0][1], order_arr[i].producttype, 9);
                            checkok(1);
                          
                            write(fd[0][1], &order_arr[i].dueyear, sizeof(order_arr[i].dueyear));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].duemonth, sizeof(order_arr[i].duemonth));
                            checkok(1);
                           
                            write(fd[0][1], &order_arr[i].dueday, sizeof(order_arr[i].dueday));
                            checkok(1);
                           
                            write(fd[0][1], reject, 6);
                        }
                    }
                }
                else if (strcmp(mode, "SJF") == 0) {

                    //SJF
                    //smaller product amount, do first
                    //sort product amount of all orders (if same amount, compare dueday)
                    //for each order
                    //find how many days of X Y Z from due date
                    //find a combination of XYZ satisfies the quantity where XYZ < due, mean square error minimum
                    //allocate period
                    int j;
                    for (i = 0; i < orderindex - 1; i++) {
                        for (j = i + 1; j < orderindex; j++) {
                            if (order_arr[i].quantity > order_arr[j].quantity) {
                                struct order temp = order_arr[i];
                                order_arr[i] = order_arr[j];
                                order_arr[j] = temp;
                            }
                            else if (order_arr[i].quantity == order_arr[j].quantity) {
                                if (order_arr[i].dueyear > order_arr[j].dueyear) {
                                    struct order temp = order_arr[i];
                                    order_arr[i] = order_arr[j];
                                    order_arr[j] = temp;
                                }
                                else if (order_arr[i].dueyear == order_arr[j].dueyear && order_arr[i].duemonth > order_arr[j].duemonth) {
                                    struct order temp = order_arr[i];
                                    order_arr[i] = order_arr[j];
                                    order_arr[j] = temp;
                                }
                                else if (order_arr[i].dueyear == order_arr[j].dueyear && order_arr[i].duemonth == order_arr[j].duemonth
                                    && order_arr[i].dueday > order_arr[j].dueday) {
                                    struct order temp = order_arr[i];
                                    order_arr[i] = order_arr[j];
                                    order_arr[j] = temp;
                                }
                            }
                            else {}
                        }
                    }

                    for (i = 0; i < orderindex; i++) {
                        int remainingX, remainingY, remainingZ;
                        int* thisorderdue = calloc(3,sizeof(int));
                        thisorderdue[0] = order_arr[i].dueyear;
                        thisorderdue[1] = order_arr[i].duemonth;
                        thisorderdue[2] = order_arr[i].dueday;
                        int duediff = getperiodlength(periodstart, thisorderdue);
                        int k;
                        int count1,count2,count3;
                        for (k = 0; k < duediff; k++) {
                            if (X[k] == 0) {
                                remainingX = duediff - k;
                                break;
                            }
                        }
                        for (k = 0; k < duediff; k++) {
                            if (Y[k] == 0) {
                                remainingY = duediff - k;
                                break;
                            }
                        }
                        for (k = 0; k < duediff; k++) {
                            if (Z[k] == 0) {
                                remainingZ = duediff - k;
                                break;
                            }
                        }
                        int remainder = 0;
                        int finalX = 100, finalY = 0, finalZ = 100;
                        if (order_arr[i].quantity < 300) {
                            finalX = 1; finalY = 0; finalZ = 0;
                        }
                        else {
                            int n, m, o;
                            for (n = 0; n < remainingX; n++) {
                                for (m = 0; m < remainingY; m++) {
                                    for (o = 0; o < remainingZ; o++) {
                                        if (order_arr[i].quantity % 100 == 0) {
                                            if (n * 300 + m * 400 + o * 500 == order_arr[i].quantity) {
                                                if (sqerror(n, m, o) < sqerror(finalX, finalY, finalZ)) {
                                                    finalX = n; finalY = m; finalZ = o;
                                                }
                                            }
                                        }
                                        else {
                                            if (n * 300 + m * 400 + o * 500 == (order_arr[i].quantity / 100 + 1) * 100) {
                                                if (sqerror(n, m, o) < sqerror(finalX, finalY, finalZ)) {
                                                    finalX = n; finalY = m; finalZ = o;
                                                    remainder = (order_arr[i].quantity / 100 + 1) * 100 - order_arr[i].quantity;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        int quantity;
                        int startyear;
                        int startmonth;
                        int startday;
                        char chararr[20];
                        bool printed = false;
                        if ((remainingX + finalX) != remainingX) {
                            printed = true;
                            write(fd[0][1], ord, 3);
                            checkok(1);
                            strcpy(chararr,c[0]);
                            write(fd[0][1], chararr, 20);
                            checkok(1);
                            write(fd[0][1], &periodstart[0], sizeof(periodstart[0]));
                            checkok(1);
                            write(fd[0][1], &periodstart[1], sizeof(periodstart[1]));
                            checkok(1);
                            write(fd[0][1], &periodstart[2], sizeof(periodstart[2]));
                            checkok(1);
                            write(fd[0][1], &periodend[0], sizeof(periodend[0]));
                            checkok(1);
                            write(fd[0][1], &periodend[1], sizeof(periodend[1]));
                            checkok(1);
                            write(fd[0][1], &periodend[2], sizeof(periodend[2]));
                            checkok(1);

                            write(fd[0][1], order_arr[i].orderid, 5);
                            checkok(1);

                            write(fd[0][1], order_arr[i].producttype, 9);
                            checkok(1);

                            write(fd[0][1], &order_arr[i].dueyear, sizeof(order_arr[i].dueyear));
                            checkok(1);

                            write(fd[0][1], &order_arr[i].duemonth, sizeof(order_arr[i].duemonth));
                            checkok(1);

                            write(fd[0][1], &order_arr[i].dueday, sizeof(order_arr[i].dueday));
                            checkok(1);

                            int counter = 0;
                            int n, m ,o;
                            for (n = remainingX; n < remainingX + finalX; n++) {
                                counter += 1;
                                X[n] = order_arr[i].orderid;
                                write(fd[0][1], next, 4);
                                checkok(1);
                                startday = periodstart[2] + count1;
                                count1++;
                                startyear = periodstart[0];
                                startmonth = periodstart[1];
                                while (startday > daysinmonth[periodstart[1] - 1]) {
                                    startday -= daysinmonth[periodstart[1] - 1];
                                    startmonth += 1;
                                    if (startmonth > 12) {
                                        startmonth = 1;
                                        startyear += 1;
                                    }
                                }
                                write(fd[0][1], &startyear, sizeof(startyear));
                                checkok(1);

                                write(fd[0][1], &startmonth, sizeof(startmonth));
                                checkok(1);
                                write(fd[0][1], &startday, sizeof(startday));
                                checkok(1);
                                quantity = 300 - remainder;
                                remainder = 0;
                                write(fd[0][1], &quantity, sizeof(quantity));
                                checkok(1);


                            }
                            
                            write(fd[0][1], end, 3);
                            checkok(1);
                        }
                        int m, o;
                        if ((remainingY + finalY) != remainingY) {
                            printed = true;
                            write(fd[0][1], ord, 3);
                            checkok(1);
                            strcpy(chararr,c[1]);
                            write(fd[0][1], chararr, 20);
                            checkok(1);
                            write(fd[0][1], &periodstart[0], sizeof(periodstart[0]));
                            checkok(1);
                            write(fd[0][1], &periodstart[1], sizeof(periodstart[1]));
                            checkok(1);
                            write(fd[0][1], &periodstart[2], sizeof(periodstart[2]));
                            checkok(1);
                            write(fd[0][1], &periodend[0], sizeof(periodend[0]));
                            checkok(1);
                            write(fd[0][1], &periodend[1], sizeof(periodend[1]));
                            checkok(1);
                            write(fd[0][1], &periodend[2], sizeof(periodend[2]));
                            checkok(1);
                            
                            write(fd[0][1], order_arr[i].orderid, 5);
                            checkok(1);
                            
                            write(fd[0][1], order_arr[i].producttype, 9);
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].dueyear, sizeof(order_arr[i].dueyear));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].duemonth, sizeof(order_arr[i].duemonth));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].dueday, sizeof(order_arr[i].dueday));
                            checkok(1);
                            
							
                            for (m = remainingY; m < remainingY + finalY; m++) {
                                Y[m] = order_arr[i].orderid;
                                write(fd[0][1], next, 4);
                                checkok(1);
                                startday = periodstart[2] + count2;
                                count2++;
                                startyear = periodstart[0];
                                startmonth = periodstart[1];
                                while (startday > daysinmonth[periodstart[1] - 1]) {
                                    startday -= daysinmonth[periodstart[1] - 1];
                                    startmonth += 1;
                                    if (startmonth > 12) {
                                        startmonth = 1;
                                        startyear += 1;
                                    }
                                }
                                write(fd[0][1], &startyear, sizeof(startyear));
                                checkok(1);
                                write(fd[0][1], &startmonth, sizeof(startmonth));
                                checkok(1);
                                write(fd[0][1], &startday, sizeof(startday));
                                checkok(1);
                                quantity = 400 - remainder;
                                remainder = 0;
                                write(fd[0][1], &quantity, sizeof(quantity));
                                checkok(1);
                            }
                            write(fd[0][1], end, 3);
                            checkok(1);
                        }
                        if ((remainingZ + finalZ) != remainingZ) {
                            printed = true;
                            write(fd[0][1], ord, 3);
                            checkok(1);
                            strcpy(chararr,c[2]);
                            write(fd[0][1], chararr, 20);
                            checkok(1);
                            write(fd[0][1], &periodstart[0], sizeof(periodstart[0]));
                            checkok(1);
                            write(fd[0][1], &periodstart[1], sizeof(periodstart[1]));
                            checkok(1);
                            write(fd[0][1], &periodstart[2], sizeof(periodstart[2]));
                            checkok(1);
                            write(fd[0][1], &periodend[0], sizeof(periodend[0]));
                            checkok(1);
                            write(fd[0][1], &periodend[1], sizeof(periodend[1]));
                            checkok(1);
                            write(fd[0][1], &periodend[2], sizeof(periodend[2]));
                            checkok(1);
                            
                            write(fd[0][1], order_arr[i].orderid, 5);
                            checkok(1);
                            
                            write(fd[0][1], order_arr[i].producttype, 9);
                            checkok(1);
                           
                            write(fd[0][1], &order_arr[i].dueyear, sizeof(order_arr[i].dueyear));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].duemonth, sizeof(order_arr[i].duemonth));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].dueday, sizeof(order_arr[i].dueday));
                            checkok(1);
                            
							
                            for (o = remainingZ; o < remainingZ + finalZ; o++) {
                                Z[o] = order_arr[i].orderid;
                                startday = periodstart[2] + count3;
                                count3++;
                                startyear = periodstart[0];
                                startmonth = periodstart[1];
                                while (startday > daysinmonth[periodstart[1] - 1]) {
                                    startday -= daysinmonth[periodstart[1] - 1];
                                    startmonth += 1;
                                    if (startmonth > 12) {
                                        startmonth = 1;
                                        startyear += 1;
                                    }
                                }
                                write(fd[0][1], next, 4);
                                checkok(1);
                                write(fd[0][1], &startyear, sizeof(startyear));
                                checkok(1);
                                write(fd[0][1], &startmonth, sizeof(startmonth));
                                checkok(1);
                                write(fd[0][1], &startday, sizeof(startday));
                                checkok(1);
                                quantity = 500 - remainder;
                                remainder = 0;
                                write(fd[0][1], &quantity, sizeof(quantity));
                                checkok(1);

                            }
                           
                            write(fd[0][1], end, 3);
                            checkok(1);
                        }
                        if (printed = false) {
                            write(fd[0][1], ord, 3);
                            checkok(1);
                            strcpy(chararr, c[0]);
                            write(fd[0][1], chararr, 20);
                            checkok(1);
                            write(fd[0][1], &periodstart[0], sizeof(periodstart[0]));
                            checkok(1);
                            write(fd[0][1], &periodstart[1], sizeof(periodstart[1]));
                            checkok(1);
                            write(fd[0][1], &periodstart[2], sizeof(periodstart[2]));
                            checkok(1);
                            write(fd[0][1], &periodend[0], sizeof(periodend[0]));
                            checkok(1);
                            write(fd[0][1], &periodend[1], sizeof(periodend[1]));
                            checkok(1);
                            write(fd[0][1], &periodend[2], sizeof(periodend[2]));
                            checkok(1);
                         
                            write(fd[0][1], order_arr[i].orderid, 5);
                            checkok(1);
                          
                            write(fd[0][1], order_arr[i].producttype, 9);
                            checkok(1);
                          
                            write(fd[0][1], &order_arr[i].dueyear, sizeof(order_arr[i].dueyear));
                            checkok(1);
                            
                            write(fd[0][1], &order_arr[i].duemonth, sizeof(order_arr[i].duemonth));
                            checkok(1);
                           
                            write(fd[0][1], &order_arr[i].dueday, sizeof(order_arr[i].dueday));
                            checkok(1);
                           
                            write(fd[0][1], reject, 6);
                        }
                        
                    }
                }
                
                write(fd[0][1], finish, 6);
                wait();



            }

            else { //child
           /*parameter:
           date, productname, ordernumber, plant, quantity, duedate
           */

           // here print the output form
                close(fd[1][0]);
                close(fd[0][1]);
                int r,r2;
                int yyyy, mm, dd;
                char *productname = calloc(10,sizeof(char));
                char *ordernumber = calloc(6,sizeof(char));
                char *plant = calloc(4,sizeof(char));
                int quan;
                int dyyyy, dmm, ddd;
                int sy, sm, sd;
                int ey, em, ed;
                int count = 100;
                char buf2[80];
                char name[100];
                int n;
            
                while ((n = read(fd[0][0], buf2, 80)) > 0) {
                    buf2[n] = 0;
                    strncpy(name, buf2, n + 1);
                    break;
                }
                FILE *outfile = fopen(name,"w");
                sendok(1);
                while (true) {
                    char buf[80];
                    if (count == 100) {
                        if ((r = read(fd[0][0], buf, 80)) > 0) {
                            buf[r] = 0;
                            if (buf[0] == ord[0] && buf[1] == ord[1] && buf[2] == ord[2]) {
                                sendok(1);
                                count = 0;
                            }
                            if (buf[0] == end[0] && buf[1] == end[1] && buf[2] == end[2]) {
                                sendok(1);
                                count = 100;
                                
                            }
                            if (buf[0] == next[0] && buf[1] == next[1] && buf[2] == next[2]) {
                                sendok(1);
                                count = 12;
                            }
                            else if (buf[0] == finish[0] && buf[1] == finish[1] && buf[2] == finish[2] && buf[3] == finish[3] && buf[4] == finish[4] && buf[5] == finish[5]) {
                                break;
                            }
                            if (buf[0] == reject[0] && buf[1] == reject[1] && buf[2] == reject[2] && buf[3] == reject[3] && buf[4] == reject[4] && buf[5] == reject[5]) {
                                fprintf(outfile, "----REJECTED----\n");
                                break;
                            }
                        }
                    }
                    else if (count == 0) {
                        if ((r = read(fd[0][0], buf, 80)) > 0) {
                            buf[r] = 0;
                            strncpy(plant, &buf[0], r + 1);
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 1) {
                        if (read(fd[0][0], &sy, sizeof(sy)) > 0) {
                            count += 1;
                            sendok(1);
                        }
                    }
                    else if (count == 2) {
                        if (read(fd[0][0], &sm, sizeof(sm)) > 0) {
                            sendok(1);
                            count += 1;
                        }

                    }
                    else if (count == 3) {
                        if (read(fd[0][0], &sd, sizeof(sd)) > 0) {
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 4) {
                        if (read(fd[0][0], &ey, sizeof(ey)) > 0) {
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 5) {
                        if (read(fd[0][0], &em, sizeof(em)) > 0) {
                            sendok(1);
                            count += 1;
                        }
                        sendok(1);
                    }
                    else if (count == 6) {
                        if (read(fd[0][0], &ed, sizeof(ed)) > 0) {
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 7) {
                        if ((r = read(fd[0][0], buf, 80)) > 0) {
                            buf[r] = 0;
                            strncpy(ordernumber, &buf[0], r + 1);
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 8) {
                        if ((r = read(fd[0][0], buf, 80)) > 0) {
                            buf[r] = 0;
                            strncpy(productname, &buf[0], r + 1);
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 9) {
                        if (read(fd[0][0], &dyyyy, sizeof(dyyyy)) > 0) {
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 10) {
                        if (read(fd[0][0], &dmm, sizeof(dmm)) > 0) {
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 11) {
                        if (read(fd[0][0], &ddd, sizeof(ddd)) > 0) {
                            sendok(1);
                            count = 100;
                            fprintf(outfile, "\n%s   %s   %d-%d-%d\n", productname, ordernumber, dyyyy, dmm, ddd);
                            fprintf(outfile, "Plant: %s\n", plant);
                            fprintf(outfile, "Date   Quentity   Due_date\n");
                        }

                    }
                    else if (count == 12) {
                        if (read(fd[0][0], &yyyy, sizeof(yyyy)) > 0) {
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 13) {
                        if (read(fd[0][0], &mm, sizeof(mm)) > 0) {
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 14) {
                        if (read(fd[0][0], &dd, sizeof(dd)) > 0) {
                            sendok(1);
                            count += 1;
                        }
                    }
                    else if (count == 15) {
                        if (read(fd[0][0], &quan, sizeof(quan)) > 0) {
                            sendok(1);
                            count = 100;
                            fprintf(outfile, "%d-%d-%d   %d    %d-%d-%d\n", yyyy, mm, dd, quan, dyyyy, dmm, ddd);
                        }
                    }
					


                }
                fclose(outfile);
				exit(0);
            }
        }



        else if (strcmp(token, "exitPLS") == 0) {
            running = 0;
        }
        else if (strcmp(token, "printORDER") == 0) {
            int i;
            for (i = 0; i < orderindex; i++) {
                printf("%s %d %d %d %d %s\n", order_arr[i].orderid, order_arr[i].dueyear, order_arr[i].duemonth, order_arr[i].dueday, order_arr[i].quantity, order_arr[orderindex].producttype);
            }
        }
        else {
            printf("Invalid Command\n");
        }
    }

    return 0;
}

void checkok(int number) {
    char buf[20];
    int num;
    while (true) { /* read from pipe */
        if ((num = read(fd[number][0], buf, 20)) > 0) {
            buf[num] = 0;
            if (buf[0] == ok[0] && buf [1] == ok[1]) {
                return;
            }
        }
    }
}
void sendok(int number) {
    write(fd[number][1], ok, 2);
    return;
}
