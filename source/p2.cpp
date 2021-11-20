// p2.c: the parent program

#include "../headers/aux.h"

using namespace std;

int main(){

    //First check if semaphores are already exist
    sem_unlink(P2_CRF_ENC2);
    sem_unlink(P2_CWT_ENC2); 
    sem_unlink(ENC2_CRF_P2); 
    sem_unlink(ENC2_CWT_P2);

    //First we must initialize our semaphores between P2 - ENC2
    sem_t *p2_r_enc2 = sem_open(P2_CRF_ENC2, O_CREAT, 0644, 0);
    sem_t *p2_w_enc2 = sem_open(P2_CWT_ENC2, O_CREAT, 0644, 1);
    sem_t *enc2_r_p2 = sem_open(ENC2_CRF_P2, O_CREAT, 0644, 0);
    sem_t *enc2_w_p2 = sem_open(ENC2_CWT_P2, O_CREAT, 0644, 1);
    //Create shared memory between P2 - ENC2
    void *p2_enc2_Ptr = createSharedMemory(P2_ENC2);

    int ret, status;
    pid_t pid; 
    // variable pid will store the 
    // value returned from fork() system call 
    pid = fork();
    if (pid == -1){             // pid == -1 means error occured

        printf("can't fork, error occured\n"); 
        exit(EXIT_FAILURE);
    }else if(pid == 0){         // pid == 0 we're the child proccess

        char *myargs [] = {NULL};
        execv("./enc2", myargs);

    }else{                      // pid > 0 we're the parent proccess
        string *terminatingMessage = new string("TERM");
        string *givenMessage = new string();
        string *takenMessage;
        char * buffer;
        while(1){
            //Now we must read from enc2
            buffer = readMessage(p2_r_enc2, p2_enc2_Ptr);
            takenMessage = new string(buffer);
            cout << "Message taken to P2: " << *takenMessage << endl;

            //break condition if TERM has been given by ENC2
            if(*takenMessage == *terminatingMessage){
                delete takenMessage;
                delete buffer;
                break;
            }
            delete takenMessage;
            free(buffer);

            //We must first read from user the given input
            cout << "Please, enter the message: ";
            getline(cin, *givenMessage);

            //Now that we have taken our message we need to write it inside the shared memory between P2 - ENC2
            writeMessage(p2_w_enc2, enc2_r_p2, givenMessage, p2_enc2_Ptr);

            sem_post(enc2_r_p2);
            //break condition if TERM has been given by P2
            if(*givenMessage == *terminatingMessage){
                break;
            }
            sem_wait(enc2_r_p2);

            //in order to rapeat the proccess
            sem_post(p2_w_enc2);
            sem_post(enc2_w_p2);
        }
        delete terminatingMessage;
        delete givenMessage;

        exit(0);
    }

    if ((ret = waitpid (pid, &status, 0)) == -1){
        printf ("parent:error\n");
    }

    if (ret == pid){
        printf ("Parent: Child process waited for.\n");
    }

    //remove the shared memory object and semaphores
    shm_unlink(P2_ENC2);
    sem_unlink(P2_CRF_ENC2);
    sem_unlink(P2_CWT_ENC2); 
    sem_unlink(ENC2_CRF_P2); 
    sem_unlink(ENC2_CWT_P2);

    sem_close(p2_r_enc2);
    sem_close(p2_w_enc2);
    sem_close(enc2_r_p2);
    sem_close(enc2_w_p2);

    return 0;
}
