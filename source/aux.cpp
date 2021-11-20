#include "../headers/aux.h"

using namespace std;

void writeMessage(sem_t *writeSemaphore, sem_t *readSemaphore, string *message, void* shmPtr){
    //down this proccess' write semaphore
    sem_wait(writeSemaphore);

    //first write the length of the string we are about to write
    int stringLength = message->length();
    memcpy((char *)shmPtr, &stringLength, sizeof(stringLength));

    //Write to the shared memory object
    memcpy((char *)shmPtr + sizeof(stringLength), message->c_str(), stringLength);

    //add the zero to the string
    char zero_char = '\0';
    memcpy((char *)shmPtr + sizeof(stringLength) + stringLength, &zero_char, sizeof(char));

    //up other proccess' read semaphore
    sem_post(readSemaphore);
}

void *createSharedMemory(const char* name){
    //First create the shared memory
    int shm_fd;
    void* shmPtr;
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    if(shm_fd == -1){
        cout << "ERROR cannot create memory" << endl;
        return NULL;
    }

    //configure the size of the shared memory object
    ftruncate(shm_fd, SIZE);
    //memory map the shared memory object
    shmPtr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    return shmPtr;
}

void *openSharedMemory(const char* name){
    //First create the shared memory
    int shm_fd;
    void* shmPtr;
    shm_fd = shm_open(name, O_RDWR, 0666); 

    if(shm_fd == -1){
        cout << "ERROR cannot open memory" << endl;
        return NULL;
    }

    //memory map the shared memory object
    shmPtr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    return shmPtr;
}

char* readMessage(sem_t *readSemaphore, void *shmPtr){
    //downs the read semaphore
    sem_wait(readSemaphore);

    //first read what the messages' length is
    int stringLength;
    memcpy(&stringLength, (char *)shmPtr, sizeof(stringLength));

    //then read the actual message, or return message and checksum together
    //either way allocate the same space
    char* buffer = (char *)malloc(sizeof(char) * (stringLength + sizeof(char) + 2 * MD5_DIGEST_LENGTH + sizeof(char)));
    memcpy(buffer, (char *)shmPtr + sizeof(stringLength), stringLength + 1 + 2 * MD5_DIGEST_LENGTH + 1);

    return buffer;
}

void writeMessageandChecksum(sem_t *writeSemaphore, sem_t *readSemaphore, string *message, void *shmPtr, unsigned char* hash){
    //down this proccess' write semaphore
    sem_wait(writeSemaphore);

    //first write the length of the string we are about to write
    int stringLength = message->length();
    memcpy((char *)shmPtr, &stringLength, sizeof(stringLength));

    //Write message to the shared memory object
    memcpy((char *)shmPtr + sizeof(stringLength), message->c_str(), stringLength);

    //add the zero to the string
    char zero_char = '\0';
    memcpy((char *)shmPtr + sizeof(stringLength) + stringLength, &zero_char, sizeof(char));

    //Write checksum to shared memory
    char* temp = (char *)(shmPtr) + sizeof(stringLength) + stringLength + sizeof(char);
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(temp, "%02x", hash[i]);
        temp += 2;
    }

    //add the zero to the checksum string
    memcpy(temp, &zero_char, sizeof(char));

    //up other proccess' read semaphore
    sem_post(readSemaphore);
}

void writeMessageandChecksum(sem_t *writeSemaphore, sem_t *readSemaphore, string *message, void *shmPtr, string* hashInString){
    //down this proccess' write semaphore
    sem_wait(writeSemaphore);

    //first write the length of the string we are about to write
    int stringLength = message->length();
    memcpy((char *)shmPtr, &stringLength, sizeof(stringLength));

    //Write message to the shared memory object
    memcpy((char *)shmPtr + sizeof(stringLength), message->c_str(), stringLength);

    //add the zero to the string
    char zero_char = '\0';
    memcpy((char *)shmPtr + sizeof(stringLength) + stringLength, &zero_char, sizeof(char));

    //Write checksum to shared memory
    char* temp = (char *)(shmPtr) + sizeof(stringLength) + stringLength + sizeof(char);
    int checksumLength = hashInString->length();
    memcpy(temp, hashInString->c_str(), checksumLength);
    
    //add the zero to the checksum string
    memcpy(temp + checksumLength, &zero_char, sizeof(char));

    //up other proccess' read semaphore
    sem_post(readSemaphore);
}

// Function to reverse a string 
void reverseStr(string& str){ 
    int n = str.length(); 
  
    // Swap character starting from two 
    // corners 
    for (int i = 0; i < n / 2; i++) 
        swap(str[i], str[n - i - 1]); 
} 

int readCheckMessage(sem_t* readSemaphore, void *shmPtr){
    //downs the read semaphore
    sem_wait(readSemaphore);

    int checkInteger;
    memcpy(&checkInteger, (char *)shmPtr, sizeof(checkInteger));
    
    return checkInteger;
}

void writeCheckMessage(sem_t *writeSemaphore, sem_t *readSemaphore, void *shmPtr, int messageCheck){
    //down this proccess' write semaphore
    sem_wait(writeSemaphore);

    memcpy((char *)shmPtr, &messageCheck, sizeof(messageCheck));

    //up other proccess' read semaphore
    sem_post(readSemaphore);

    return;
}