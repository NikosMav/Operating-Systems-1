// enc1.c: the parent program

#include "../headers/aux.h"

using namespace std;

int main() {
    //First check if semaphores are already exist, if thats the case then destroy them.
    //if not then ignore
    sem_unlink(ENC1_CRF_CHAN);
    sem_unlink(ENC1_CWT_CHAN); 
    sem_unlink(CHAN_CRF_ENC1); 
    sem_unlink(CHAN_CWT_ENC1);

    //First we must initialize our new semaphores between ENC1 - CHAN
    sem_t *enc1_r_chan = sem_open(ENC1_CRF_CHAN, O_CREAT, 0644, 0);
    sem_t *enc1_w_chan = sem_open(ENC1_CWT_CHAN, O_CREAT, 0644, 1);
    sem_t *chan_r_enc1 = sem_open(CHAN_CRF_ENC1, O_CREAT, 0644, 0);
    sem_t *chan_w_enc1 = sem_open(CHAN_CWT_ENC1, O_CREAT, 0644, 1);
    //Open already created semaphores
    sem_t *enc1_r_p1 = sem_open(ENC1_CRF_P1, 0);
    sem_t *enc1_w_p1 = sem_open(ENC1_CWT_P1, 0);
    sem_t *p1_r_enc1 = sem_open(P1_CRF_ENC1, 0);
    //We must now create the new shared memory segment between ENC1 - CHAN
    void *enc1_chan_Ptr = createSharedMemory(ENC1_CHAN);
    //and open shared memory between P1 - ENC1
    void *p1_enc1_Ptr = openSharedMemory(P1_ENC1);

    char *memoryPtrToMessage;
    string *givenMessageFromP1;
    string* givenMessageFromChan;
    unsigned char hash[MD5_DIGEST_LENGTH];
    unsigned char newHash[MD5_DIGEST_LENGTH];
    string* givenChecksum;
    string* newChecksum;
    string *terminatingMessage = new string(TERM);
    int stringLength, j, i;
    char *temp = (char *)malloc(sizeof(char) * (2 * MD5_DIGEST_LENGTH + sizeof(char)));

    while(1){

        //Reading message that came from P1
        memoryPtrToMessage = readMessage(enc1_r_p1, p1_enc1_Ptr);
        givenMessageFromP1 = new string(memoryPtrToMessage);

        //compute checksum of the given message
        MD5((unsigned char*)memoryPtrToMessage, strlen(memoryPtrToMessage), hash);
        free(memoryPtrToMessage);

        //Now we must write message and its checksum to new memory segment
        writeMessageandChecksum(enc1_w_chan, chan_r_enc1, givenMessageFromP1, enc1_chan_Ptr, hash);

        sem_post(chan_r_enc1);
        //Break condition if TERM has been given from P1
        if(*givenMessageFromP1 == *terminatingMessage) {
            delete givenMessageFromP1;
            break;
        }
        sem_wait(chan_r_enc1);

        while(1){
            //After that is done we must read information from shared memory between CHAN - ENC1
            memoryPtrToMessage = readMessage(enc1_r_chan, enc1_chan_Ptr);
            givenMessageFromChan = new string(memoryPtrToMessage);

            stringLength = givenMessageFromChan->length() + 1;
            givenChecksum = new string(memoryPtrToMessage + stringLength);

            //Now that we have read message and checksum we must check if message came intact
            //compute checksum of the given message from CHAN
            MD5((unsigned char*)memoryPtrToMessage, strlen(memoryPtrToMessage), newHash);
            free(memoryPtrToMessage);

            j = 0;
            for(i = 0; i < MD5_DIGEST_LENGTH; i++){
                sprintf(&temp[j], "%02x", newHash[i]);
                j += 2;
            }
            sprintf(&temp[j], "%c", '\0');
            newChecksum = new string(temp);

            //Check if both checksums are identical
            //if not then CHAN must resend the message to ENC1
            if(*newChecksum != *givenChecksum) {
                cout << "Messages are not the same" << endl;
                //Now that messages are not the same we must inform CHAN
                //Before proccess change values
                sem_post(enc1_w_chan);
                writeCheckMessage(enc1_w_chan, chan_r_enc1, enc1_chan_Ptr, 0);

                sem_post(chan_w_enc1);
            }else{
                //Now that messages are the same we must inform CHAN and break the loop
                //Before proccess change values
                sem_post(enc1_w_chan);
                writeCheckMessage(enc1_w_chan, chan_r_enc1, enc1_chan_Ptr, 1);

                break;
            }

            delete givenMessageFromChan;
            delete givenChecksum;
            delete newChecksum;
        }

        //After that enc1 read the message from CHAN it writes it to the shared memory between P1 - ENC1
        writeMessage(enc1_w_p1, p1_r_enc1, givenMessageFromChan, p1_enc1_Ptr);
        
        sem_post(p1_r_enc1);
        //Break condition if TERM has been given from CHAN
        if(*givenMessageFromChan == *terminatingMessage) {
            delete givenMessageFromChan;
            break;
        }
        sem_wait(p1_r_enc1);

        //in order to repeat the proccess
        sem_post(enc1_w_chan);
        sem_post(chan_w_enc1);

        delete givenMessageFromP1;
        delete givenMessageFromChan;
        delete givenChecksum;
        delete newChecksum;
    }
    delete terminatingMessage;
    free(temp);

    //remove the shared memory object and semaphores
    shm_unlink(ENC1_CHAN);
    sem_unlink(ENC1_CRF_CHAN);
    sem_unlink(ENC1_CWT_CHAN); 
    sem_unlink(CHAN_CRF_ENC1); 
    sem_unlink(CHAN_CWT_ENC1);

    sem_unlink(ENC1_CWT_P1);
    sem_unlink(P1_CRF_ENC1);
    sem_unlink(ENC1_CRF_P1);

    sem_close(enc1_r_chan);
    sem_close(enc1_w_chan);
    sem_close(chan_r_enc1);
    sem_close(chan_w_enc1);

    sem_close(enc1_w_p1);
    sem_close(p1_r_enc1);
    sem_close(enc1_r_p1);

    return 0;
}