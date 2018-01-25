#pragma once

#include <stdint.h>
#include <stdio.h>

/**
   I'm using 128 bit ECB mode AES only.
 */

// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb 4
// The number of 32 bit words in a key.
#define Nk 4
// Key length in bytes [128 bit]
#define KEYLEN 16
// The number of rounds in AES Cipher.
#define Nr 10


// Only use ECB mode 
//void AES128_ECB_encrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
//void AES128_ECB_decrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
//void decrypt(char buf[], int len, char key[], char code[]);
//int encrypt(char buf[], int len, char key[], char code[]);
void AES128_ECB_decrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
void AES128_ECB_encrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
  



