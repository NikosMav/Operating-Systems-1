#include "../headers/aux.h"
#include <time.h>

using namespace std;

int main() {
    //Loading semaphore
    sem_t *chan_r_enc1 = sem_open(CHAN_CRF_ENC1, 0);
    sem_t *chan_w_enc2 = sem_open(CHAN_CWT_ENC2, 0);
    sem_t *enc2_r_chan = sem_open(ENC2_CRF_CHAN, 0);
    sem_t *chan_r_enc2 = sem_open(CHAN_CRF_ENC2, 0);
    sem_t *chan_w_enc1 = sem_open(CHAN_CWT_ENC1, 0);
    sem_t *enc1_r_chan = sem_open(ENC1_CRF_CHAN, 0);
    //Loading memories
    void* chan_enc1_Ptr = openSharedMemory(ENC1_CHAN);
    void* chan_enc2_Ptr = openSharedMemory(ENC2_CHAN);
    //Use current time as seed
    srand(time(0));

    char *messageFromEnc1;
    char *messageFromEnc2;
    int messageCheck;
    string *givenMessage;
    string* givenChecksum;
    string *copyOfGivenMessage;
    int stringLength, prob;
    char checksum[2 * MD5_DIGEST_LENGTH + 1];
    string *terminatingMessage = new string(TERM);

    while(1){
        //Read message from ENC1
        messageFromEnc1 = readMessage(chan_r_enc1, chan_enc1_Ptr);
        givenMessage = new string(messageFromEnc1);

        //Read checksum from ENC1
        stringLength = givenMessage->length() + 1;
        memcpy(checksum, messageFromEnc1 + stringLength, 2 * MD5_DIGEST_LENGTH + 1);
        givenChecksum = new string(checksum);
        free(messageFromEnc1);

        do{
            //before proceeding we must create a copy of the given string
            copyOfGivenMessage = new string(*givenMessage);

            //We must now begin the message alternating proccess
            //Take a probability
            prob = rand() % 10 + 1; //range 1 to 10
            if(prob > DEFAULT_PROB) {
                //Then we must alternate the message
                reverseStr(*copyOfGivenMessage);
            }

            //Now that we have read the message and its checksum we must convey the information to ENC2
            writeMessageandChecksum(chan_w_enc2, enc2_r_chan, copyOfGivenMessage, chan_enc2_Ptr, givenChecksum);

            //Now that message has been send, CHAN must check if message is correct
            messageCheck = readCheckMessage(chan_r_enc2, chan_enc2_Ptr);

            delete copyOfGivenMessage;

        }while(!messageCheck);  

        sem_post(enc2_r_chan);
        //break condition if TERM has been given through ENC1
        if(*givenMessage == *terminatingMessage){
            delete givenMessage;
            delete givenChecksum;
            break;
        }
        sem_wait(enc2_r_chan);

        delete givenMessage;
        delete givenChecksum;

        //Read message from ENC2
        messageFromEnc2 = readMessage(chan_r_enc2, chan_enc2_Ptr);
        givenMessage = new string(messageFromEnc2);

        //Read checksum from ENC1
        stringLength = givenMessage->length() + 1;
        memcpy(checksum, messageFromEnc2 + stringLength, 2 * MD5_DIGEST_LENGTH + 1);
        givenChecksum = new string(checksum);
        free(messageFromEnc2);

        do{
            //before proceeding we must create a copy of the given string
            copyOfGivenMessage = new string(*givenMessage);

            //We must now begin the message alternating proccess
            //Take a probability
            prob = rand() % 10 + 1; //range 1 to 10
            if(prob > DEFAULT_PROB) {
                //Then we must alternate the message
                reverseStr(*copyOfGivenMessage);
            }

            //Now that we have read the message and its checksum we must convey the information to ENC2
            writeMessageandChecksum(chan_w_enc1, enc1_r_chan, copyOfGivenMessage, chan_enc1_Ptr, givenChecksum);

            //Now that message has been send, CHAN must check if message is correct
            messageCheck = readCheckMessage(chan_r_enc1, chan_enc1_Ptr);

            delete copyOfGivenMessage;

        }while(!messageCheck);

        sem_post(enc1_r_chan);
        //break condition if TERM has been given through ENC2
        if(*givenMessage == *terminatingMessage){
            delete givenMessage;
            delete givenChecksum;
            break;
        }
        sem_wait(enc1_r_chan);

        delete givenMessage;
        delete givenChecksum;
    }
    delete terminatingMessage;

    sem_unlink(CHAN_CRF_ENC1);
    sem_unlink(CHAN_CWT_ENC2);
    sem_unlink(ENC2_CRF_CHAN);
    sem_unlink(CHAN_CRF_ENC2);
    sem_unlink(CHAN_CWT_ENC1);
    sem_unlink(ENC1_CRF_CHAN);

    sem_close(chan_r_enc1);
    sem_close(chan_w_enc2);
    sem_close(enc2_r_chan);
    sem_close(chan_r_enc2);
    sem_close(chan_w_enc1);
    sem_close(enc1_r_chan);

    shm_unlink(ENC1_CHAN);
    shm_unlink(ENC2_CHAN);

    return 0;
}