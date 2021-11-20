// p1.c: the parent program

#include "../headers/aux.h"

using namespace std;

int main(){
    //First check if semaphores are already exist, if thats the case then destroy them.
    //if not then ignore
    sem_unlink(P1_CRF_ENC1);
    sem_unlink(P1_CWT_ENC1); 
    sem_unlink(ENC1_CRF_P1); 
    sem_unlink(ENC1_CWT_P1);

    //First we must initialize our semaphores between P1 - ENC1
    sem_t *p1_r_enc1 = sem_open(P1_CRF_ENC1, O_CREAT, 0644, 0);
    sem_t *p1_w_enc1 = sem_open(P1_CWT_ENC1, O_CREAT, 0644, 1);
    sem_t *enc1_r_p1 = sem_open(ENC1_CRF_P1, O_CREAT, 0644, 0);
    sem_t *enc1_w_p1 = sem_open(ENC1_CWT_P1, O_CREAT, 0644, 1);
    //Create shared memory between P1 - ENC1
    void *p1_enc1_Ptr = createSharedMemory(P1_ENC1);

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
        execv("./enc1", myargs);

    }else{                      // pid > 0 we're the parent proccess
        string *terminatingMessage = new string(TERM);
        string *givenMessage = new string();
        string *takenMessage;
        char * buffer;
        while(1){
            //We must first read from user the given input
            cout << "Please, enter the message: ";
            getline(cin, *givenMessage);

            //Now that we have taken our message we need to write it inside the shared memory between P1 - ENC1
            writeMessage(p1_w_enc1, enc1_r_p1, givenMessage, p1_enc1_Ptr);
            
            sem_post(enc1_r_p1);
            //break condition if TERM has been given by P1
            if(*givenMessage == *terminatingMessage){
                break;
            }
            sem_wait(enc1_r_p1);

            //Now we must read from enc1
            buffer = readMessage(p1_r_enc1, p1_enc1_Ptr);
            takenMessage = new string(buffer);
            cout << "Message taken to P1: " << *takenMessage << endl;

            //break condition if TERM has been given by ENC1
            if(*takenMessage == *terminatingMessage){
                delete takenMessage;
                delete buffer;
                break;
            }
            delete takenMessage;
            free(buffer);

            //in order to repeat the proccess
            sem_post(p1_w_enc1);
            sem_post(enc1_w_p1);
        }
        delete terminatingMessage;
        delete givenMessage;

        return 0;
    }

    if ((ret = waitpid (pid, &status, 0)) == -1){
        cout << "parent:error" << endl;
    }
    if (ret == pid){
        cout << "Parent: Child process waited for" << endl;
    }

    //remove the shared memory object and semaphores
    shm_unlink(P1_ENC1);
    sem_unlink(P1_CRF_ENC1);
    sem_unlink(P1_CWT_ENC1); 
    sem_unlink(ENC1_CRF_P1); 
    sem_unlink(ENC1_CWT_P1);

    sem_close(p1_r_enc1);
    sem_close(p1_w_enc1);
    sem_close(enc1_r_p1);
    sem_close(enc1_w_p1);

    return 0;
}