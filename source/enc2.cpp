// enc2.c: the parent program

#include "../headers/aux.h"

using namespace std;

int main() {
    //First check if semaphores are already exist, if thats the case then destroy them.
    //if not then ignore
    sem_unlink(ENC2_CRF_CHAN);
    sem_unlink(ENC2_CWT_CHAN); 
    sem_unlink(CHAN_CRF_ENC2); 
    sem_unlink(CHAN_CWT_ENC2);

    //First we must initialize our new semaphores between ENC2 - CHAN
    sem_t *enc2_r_chan = sem_open(ENC2_CRF_CHAN, O_CREAT, 0644, 0);
    sem_t *enc2_w_chan = sem_open(ENC2_CWT_CHAN, O_CREAT, 0644, 1);
    sem_t *chan_r_enc2 = sem_open(CHAN_CRF_ENC2, O_CREAT, 0644, 0);
    sem_t *chan_w_enc2 = sem_open(CHAN_CWT_ENC2, O_CREAT, 0644, 1);
    //Open already created semaphores
    sem_t *enc2_w_p2 = sem_open(ENC2_CWT_P2, 0);
    sem_t *p2_r_enc2 = sem_open(P2_CRF_ENC2, 0);
    sem_t *enc2_r_p2 = sem_open(ENC2_CRF_P2, 0);
    //We must now create the new shared memory segment between ENC2 - CHAN
    void *enc2_chan_Ptr = createSharedMemory(ENC2_CHAN);
    //and open the shared memory between P2 - ENC2
    void *p2_enc2_Ptr = openSharedMemory(P2_ENC2);

    char *memoryPtrToMessage;
    string *givenMessageFromP2;
    string* givenMessageFromChan;
    unsigned char hash[MD5_DIGEST_LENGTH];
    unsigned char newHash[MD5_DIGEST_LENGTH];
    string* givenChecksum;
    string* newChecksum;
    string *terminatingMessage = new string(TERM);
    int stringLength, j, i;
    char *temp = (char *)malloc(sizeof(char) * (2 * MD5_DIGEST_LENGTH + sizeof(char)));

    while(1){

        while(1){
            //First we must read information from shared memory between CHAN - enc2
            memoryPtrToMessage = readMessage(enc2_r_chan, enc2_chan_Ptr);
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
            //if not then CHAN must resend the message to ENC2
            if(*newChecksum != *givenChecksum) {
                cout << "Messages are not the same" << endl;
                //Now that messages are not the same we must inform CHAN
                writeCheckMessage(enc2_w_chan, chan_r_enc2, enc2_chan_Ptr, 0);
                //After proccess change values
                sem_post(enc2_w_chan);

                sem_post(chan_w_enc2);
            }else{
                //Now that messages are the same we must inform CHAN and break the loop
                writeCheckMessage(enc2_w_chan, chan_r_enc2, enc2_chan_Ptr, 1);
                //After proccess change values
                sem_post(enc2_w_chan);

                break;
            }

            delete givenMessageFromChan;
            delete givenChecksum;
            delete newChecksum;
        }

        //After that enc2 read the message from CHAN it writes it to the shared memory between p2 - enc2
        writeMessage(enc2_w_p2, p2_r_enc2, givenMessageFromChan, p2_enc2_Ptr);

        sem_post(p2_r_enc2);
        //Break condition if TERM has been given from CHAN
        if(*givenMessageFromChan == *terminatingMessage) {
            delete givenMessageFromChan;
            delete givenChecksum;
            delete newChecksum;
            break;
        }
        sem_wait(p2_r_enc2);

        //Reading message that came from p2
        memoryPtrToMessage = readMessage(enc2_r_p2, p2_enc2_Ptr);
        givenMessageFromP2 = new string(memoryPtrToMessage);

        //compute checksum of the given message
        MD5((unsigned char*)memoryPtrToMessage, strlen(memoryPtrToMessage), hash);
        free(memoryPtrToMessage);

        //Now we must write message and its checksum to new memory segment
        writeMessageandChecksum(enc2_w_chan, chan_r_enc2, givenMessageFromP2, enc2_chan_Ptr, hash);

        sem_post(chan_r_enc2);
        //Break condition if TERM has been given from p2
        if(*givenMessageFromP2 == *terminatingMessage) {
            delete givenMessageFromP2;
            delete givenMessageFromChan;
            delete givenChecksum;
            delete newChecksum;
            break;
        }
        sem_wait(chan_r_enc2);

        //in order to rapeat the proccess
        sem_post(enc2_w_chan);
        sem_post(chan_w_enc2);

        delete givenMessageFromP2;
        delete givenMessageFromChan;
        delete givenChecksum;
        delete newChecksum;
    }
    delete terminatingMessage;
    free(temp);

    //remove the shared memory object and semaphores
    shm_unlink(ENC2_CHAN);
    sem_unlink(ENC2_CRF_CHAN);
    sem_unlink(ENC2_CWT_CHAN); 
    sem_unlink(CHAN_CRF_ENC2); 
    sem_unlink(CHAN_CWT_ENC2);
    
    sem_unlink(ENC2_CWT_P2);
    sem_unlink(P2_CRF_ENC2);
    sem_unlink(ENC2_CRF_P2);

    sem_close(enc2_r_chan);
    sem_close(enc2_w_chan);
    sem_close(chan_r_enc2);
    sem_close(chan_w_enc2);

    sem_close(enc2_w_p2);
    sem_close(p2_r_enc2);
    sem_close(enc2_r_p2);

    return 0;
}