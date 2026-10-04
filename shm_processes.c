#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void DearOldDad(int SharedMem[]);
void PoorStudent(int SharedMem[]);

int main()
{
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);

    if (ShmID < 0)
    {
        printf("*** shmget error ***\n");
        exit(1);
    }

    ShmPTR = (int *)shmat(ShmID, NULL, 0);

    if ((long)ShmPTR == -1)
    {
        printf("*** shmat error ***\n");
        exit(1);
    }

    /* Shared Variables */
    ShmPTR[0] = 0; /* BankAccount */
    ShmPTR[1] = 0; /* Turn */

    pid = fork();

    if (pid < 0)
    {
        printf("*** fork error ***\n");
        exit(1);
    }
    else if (pid == 0)
    {
        PoorStudent(ShmPTR);
        exit(0);
    }
    else
    {
        DearOldDad(ShmPTR);

        wait(&status);

        shmdt((void *)ShmPTR);
        shmctl(ShmID, IPC_RMID, NULL);
    }

    return 0;
}

/* Parent Process */
void DearOldDad(int SharedMem[])
{
    int i;

    srand(time(NULL));

    for (i = 0; i < 25; i++)
    {
        sleep(rand() % 6);

        while (SharedMem[1] != 0)
            ;

        int account = SharedMem[0];

        if (account <= 100)
        {
            int balance = rand() % 101;

            if (balance % 2 == 0)
            {
                account += balance;

                printf(
                    "Dear old Dad: Deposits $%d / Balance = $%d\n",
                    balance,
                    account);
            }
            else
            {
                printf(
                    "Dear old Dad: Doesn't have any money to give\n");
            }
        }
        else
        {
            printf(
                "Dear old Dad: Thinks Student has enough Cash ($%d)\n",
                account);
        }

        SharedMem[0] = account;
        SharedMem[1] = 1;
    }
}

/* Child Process */
void PoorStudent(int SharedMem[])
{
    int i;

    srand(time(NULL) + getpid());

    for (i = 0; i < 25; i++)
    {
        sleep(rand() % 6);

        while (SharedMem[1] != 1)
            ;

        int account = SharedMem[0];

        int balance = rand() % 51;

        printf("Poor Student needs $%d\n", balance);

        if (balance <= account)
        {
            account -= balance;

            printf(
                "Poor Student: Withdraws $%d / Balance = $%d\n",
                balance,
                account);
        }
        else
        {
            printf(
                "Poor Student: Not Enough Cash ($%d)\n",
                account);
        }

        SharedMem[0] = account;
        SharedMem[1] = 0;
    }
}