#ifndef AUX
#define AUX

#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <openssl/md5.h>
#include <sys/wait.h>

#define SIZE 512

#define P1_CRF_ENC1 "/p1_can_read_from_enc1"
#define P1_CWT_ENC1 "/p1_can_write_to_enc1"
#define ENC1_CRF_P1 "/enc1_can_read_from_p1"
#define ENC1_CWT_P1 "/enc1_can_write_to_p1"

#define ENC1_CRF_CHAN "/enc1_can_read_from_chan"
#define ENC1_CWT_CHAN "/enc1_can_write_to_chan"
#define CHAN_CRF_ENC1 "/chan_can_read_from_enc1"
#define CHAN_CWT_ENC1 "/chan_can_write_to_enc1"

#define P2_CRF_ENC2 "/p2_can_read_from_enc2"
#define P2_CWT_ENC2 "/p2_can_write_to_enc2"
#define ENC2_CRF_P2 "/enc2_can_read_from_p2"
#define ENC2_CWT_P2 "/enc2_can_write_to_p2"

#define ENC2_CRF_CHAN "/enc2_can_read_from_chan"
#define ENC2_CWT_CHAN "/enc2_can_write_to_chan"
#define CHAN_CRF_ENC2 "/chan_can_read_from_enc2"
#define CHAN_CWT_ENC2 "/chan_can_write_to_enc2"

#define P1_ENC1 "P1ENC1"
#define ENC1_CHAN "ENC1CHAN"
#define P2_ENC2 "P2ENC2"
#define ENC2_CHAN "ENC2CHAN"

#define TERM "TERM"
#define DEFAULT_PROB 5

using namespace std;

void writeMessage(sem_t *writeSemaphore, sem_t *readSemaphore, string *message, void *shmPtr);
void writeChecksum(sem_t writeSemaphore, unsigned char* hash, void *smhPtr);
void *createSharedMemory(const char* name);
void *openSharedMemory(const char* name);
char* readMessage(sem_t *readSemaphore, void *shmPtr);
void writeMessageandChecksum(sem_t *writeSemaphore, sem_t *readSemaphore, string *message, void *shmPtr, unsigned char* hash);
void writeMessageandChecksum(sem_t *writeSemaphore, sem_t *readSemaphore, string *message, void *shmPtr, string* hashInString);
void reverseStr(string& str);
int readCheckMessage(sem_t* readSemaphore, void *shmPtr);
void writeCheckMessage(sem_t *writeSemaphore, sem_t *readSemaphore, void *shmPtr, int messageCheck);

#endif