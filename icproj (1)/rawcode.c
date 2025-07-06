#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char userid[50];
    int period_dates[4][3];
    int cycle_length;
} UserData;

int * predicted;
UserData users[100];
int userCount = 0;
int addDays(int,int,int,int);
int monthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void saveToCSV()
{
    FILE *fp = fopen("period_data.csv", "w");  
    if (fp == NULL)
    {
        printf("Error opening file!\n");
        return;
    }
    fprintf(fp, "UserID,Date1,Month1,Year1,Date2,Month2,Year2,Date3,Month3,Year3,Date4,Month4,Year4,CycleLength\n");
   
    for (int i = 0; i < userCount; i++)
    {
        fprintf(fp, "%s,", users[i].userid);
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 3; k++)
            {
                fprintf(fp, "%d,", users[i].period_dates[j][k]);
            }
        }
        fprintf(fp, "%d\n", users[i].cycle_length);
    }
    fclose(fp);
}


void loadFromCSV()
{
    FILE *fp = fopen("period_data.csv", "r");
    if (fp == NULL)
    {
        return;
    }

    char line[256];
   
    fgets(line, sizeof(line), fp);
   
    userCount = 0;
    while (fscanf(fp, "%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", users[userCount].userid,
            &users[userCount].period_dates[0][0], &users[userCount].period_dates[0][1], &users[userCount].period_dates[0][2],
            &users[userCount].period_dates[1][0], &users[userCount].period_dates[1][1], &users[userCount].period_dates[1][2],
            &users[userCount].period_dates[2][0], &users[userCount].period_dates[2][1], &users[userCount].period_dates[2][2],
            &users[userCount].period_dates[3][0], &users[userCount].period_dates[3][1], &users[userCount].period_dates[3][2],
            &users[userCount].cycle_length) == 14)
    {
        userCount++;
    }
    fclose(fp);
}

int countly(int year, int month)
{
    int count = 0;
    if(month <= 2) year--;
    for (int i = 1; i <= year; i++)
    {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            count++;
        }
    }
    return count;
}

int diff(int d1, int m1, int y1, int d2, int m2, int y2)
{
    int n1 = y1 * 365 + d1;
    for (int i = 1; i < m1; i++)
        n1 += monthDays[i-1];
    n1 += countly(y1, m1);
   
    int n2 = y2 * 365 + d2;
    for (int i = 1; i < m2; i++)
        n2 += monthDays[i-1];
    n2 += countly(y2, m2);
   
    return (n2 - n1);
}

int findUser(const char* userid)
{
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].userid, userid) == 0)
        {
            return i;
        }
    }
    return -1;
}

void addUser(const char* userid)
{
    if (findUser(userid) != -1)
    {
      printf("User already exists\n");
      return;
    }

    strcpy(users[userCount].userid, userid);

    printf("Enter your previous 4 period start dates (date month year):\n");
    for (int j = 0; j < 4; j++)
    {
        printf("Period %d: ", j+1);
        for (int k = 0; k < 3; k++)
        {
            scanf("%d", &users[userCount].period_dates[j][k]);
        }
    }

    int c1 = diff(users[userCount].period_dates[0][0], users[userCount].period_dates[0][1], users[userCount].period_dates[0][2],
                  users[userCount].period_dates[1][0], users[userCount].period_dates[1][1], users[userCount].period_dates[1][2]);
    int c2 = diff(users[userCount].period_dates[1][0], users[userCount].period_dates[1][1], users[userCount].period_dates[1][2],
                  users[userCount].period_dates[2][0], users[userCount].period_dates[2][1], users[userCount].period_dates[2][2]);
    int c3 = diff(users[userCount].period_dates[2][0], users[userCount].period_dates[2][1], users[userCount].period_dates[2][2],
                  users[userCount].period_dates[3][0], users[userCount].period_dates[3][1], users[userCount].period_dates[3][2]);

    users[userCount].cycle_length = (c1 + c2 + c3) / 3;
    if(users[userCount].cycle_length>60)
    {
        printf("\033[1;37mIrregular periods! Consult a doctor.\n");
    }
    else printf("\033[1;37mCycle length calculated: %d days\n", users[userCount].cycle_length);

    userCount++;
    saveToCSV();
}

void predictPeriod(const char* userid)
{
    int index = findUser(userid);
    if (index == -1)
    {
        printf("User not found\n");
        return;
    }

    int lastPeriodDay = users[index].period_dates[3][0];
    int lastPeriodMonth = users[index].period_dates[3][1];
    int lastPeriodYear = users[index].period_dates[3][2];
    int cycleLength = users[index].cycle_length;

    predicted = (int*) malloc(3*sizeof(int));
    addDays(lastPeriodDay, lastPeriodMonth, lastPeriodYear, cycleLength);
}

int addDays(int day, int month, int year, int daysToAdd)
{
    if(daysToAdd>60)
    {
        printf("\033[1;37mIrregular oeriods, can't be predicted\n");
        return 0;
    }
    else
    {
        predicted[0]=day;
        predicted[1]=month;
        predicted[2]=year;
        while (daysToAdd > 0)
        {
            if (predicted[1] == 2 && ((predicted[2] % 4 == 0 && predicted[2] % 100 != 0) || (predicted[2] % 400 == 0)))
            {
                monthDays[1] = 29;
            }
            else
            {
                monthDays[1] = 28;
            }
            int remainingDays = monthDays[predicted[1] - 1] - predicted[0] + 1;
            if (daysToAdd >= remainingDays)
            {
                daysToAdd -= remainingDays;
                predicted[0] = 1;
                predicted[1] += 1;
                if (predicted[1] > 12)
                {
                    predicted[1] = 1;
                    predicted[2] += 1;
                }
            }
            else
            {
                predicted[0] += daysToAdd;
                daysToAdd = 0;
            }
        }
    printf("\033[1;37mNext predicted period date: %d/%d/%d\n", predicted[0], predicted[1], predicted[2]);
    free(predicted);
    }
    return 0;
}

int main()
{
    loadFromCSV();

    int choice;
    char userid[50];

    while (1)
    {
        printf("\033[1;31m \n\u2665 \033[1;36m Hello, this is Seiri!! \033[1;31m \u2665 \033[1;36m \nPress 1 to add new user\nEnter 2 to predict Period\nEnter 3 to interact with Seiri!! \nEnter 4 to Exit\nEnter choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                printf("Enter user ID: ");
                scanf("%s", userid);
                addUser(userid);
                break;
            case 2:
                printf("Enter user ID: ");
                scanf("%s", userid);
                predictPeriod(userid);
                break;
            case 3:
                int var;
                printf("Press 1 if you are happy\nPress 2 if you are sad\nPress 3 if you are frustrated\nPress 4 if you are anxious\n");
                scanf("%d", &var);
                switch(var)
                {
                    case 1:
                        printf("\033[1;37mIf you're happy and you know it clap your hands!!\n");
                        break;
                    case 2:
                        printf("\033[1;37mWhy did the scarecrow win an award?\nBecause he was outstanding in his field!\n");
                        break;
                    case 3:
                        printf("\033[1;37mLet's just hope this doesn't end up on a blooper reel someday!\n");
                        break;
                    case 4:
                        printf("\033[1;37mIt's okay to take a break, have a break, have a kitkat!\n");
                        break;
                    default:
                        printf("Invalid choice\n");
                }
               
                break;
            case 4:
                printf("\033[1;31m \u2665 \033[1;36m See you soon\033[1;31m \u2665");
                return 0;
                break;
            default:
                printf("Invalid choice\n");
                break;
        }
    }
}

	
