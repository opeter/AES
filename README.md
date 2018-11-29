# Advanced Encryption Standard

As a self-imposed programming challenge I started to have a closer look into the topic of Cryptography. I wanted to know much more about the (probably) most important cipher in the world: The Advanced Encryption Standard.

The following articles will describe some of the history and mathematical background but mostly they will cover the adventure of writing my own implementation of an AES library in C, which can be found here: https://gogs.gfuzz.de/oliver.peter/AES/releases

# Blog articles
[Overview : Advanced Encryption Standard (AES, 2001)](https://www.gfuzz.de/index.php?/archives/16-Overview-Advanced-Encryption-Standard-AES,-2001.html)
[AES (1/3): An Introduction to Galois Fields](https://www.gfuzz.de/index.php?/archives/13-AES-13-An-Introduction-to-Galois-Fields.html)
[AES (2/3): A Description of AES Lookup Tables](https://www.gfuzz.de/index.php?/archives/15-AES-23-A-Description-of-AES-Lookup-Tables.html)
[AES (3/3): Encryption and Decryption](https://www.gfuzz.de/index.php?/archives/14-AES-33-Encryption-and-Decryption.html)

# Public library funcions
A key has to be an array of 16x 8bit elements.
```c
static const uint8_t key[16] =
{
    0x2b, 0x7e, 0x15, 0x16,
    0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88,
    0x09, 0xcf, 0x4f, 0x3c
};
```
An input state has to be an array of 16x 8bit elements
```c
static const uint8_t state[16] =
{
    0xae, 0x2d, 0x8a, 0x57,
    0x1e, 0x03, 0xac, 0x9c,
    0x9e, 0xb7, 0x6f, 0xac,
    0x45, 0xaf, 0x8e, 0x51
};
```
Inputs where taken from the original AES paper, Appendix A.

Library functions have to be initialized (lookup tables and key schedule) by providing a pointer to the secret key
```c
uint8_t aes_init(const uint8_t *key);
```

An input state, for both enciphering and deciphering, has to be a pointer to a 128bit block
```c
uint8_t aes_encrypt(uint8_t *state);
uint8_t aes_decrypt(uint8_t *state);
```
#License
Educational purpose only.