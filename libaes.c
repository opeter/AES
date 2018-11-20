#include "libaes.h"

/* Defines if algorithm has been initialized already */
uint8_t INITIALIZED = 0;

/* AES Key Schedule */
uint32_t AES_KEY_SCHEDULE[AES_SUBKEYS][AES_SUBKEY_PARTS] = { { 0 } };

/* AES Byte Substitution Table */
#if AES_CALCULATE_LOOKUP_TABLES == 0
static const uint8_t AES_BYTE_SUB[16][16] = {
	// 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
	{ 0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76 }, // 0x0
	{ 0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0 }, // 0x1
	{ 0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15 }, // 0x2
	{ 0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75 }, // 0x3
	{ 0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84 }, // 0x4
	{ 0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF }, // 0x5
	{ 0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8 }, // 0x6
	{ 0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2 }, // 0x7
	{ 0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73 }, // 0x8
	{ 0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB }, // 0x9
	{ 0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79 }, // 0xA
	{ 0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08 }, // 0xB
	{ 0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A }, // 0xC
	{ 0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E }, // 0xD
	{ 0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF }, // 0xE
	{ 0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16 }  // 0xF
};
#else
uint8_t AES_BYTE_SUB[16][16] = { { 0 } };
#endif

/* AES Inverse Byte Substitution Table */
#if AES_CALCULATE_LOOKUP_TABLES == 0
static const uint8_t AES_INV_BYTE_SUB[16][16] = {
	// 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
	{ 0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB }, // 0x0
	{ 0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB }, // 0x1
	{ 0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E }, // 0x2
	{ 0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25 }, // 0x3
	{ 0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92 }, // 0x4
	{ 0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84 }, // 0x5
	{ 0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06 }, // 0x6
	{ 0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B }, // 0x7
	{ 0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73 }, // 0x8
	{ 0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E }, // 0x9
	{ 0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B }, // 0xA
	{ 0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4 }, // 0xB
	{ 0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F }, // 0xC
	{ 0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF }, // 0xD
	{ 0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61 }, // 0xE
	{ 0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D }  // 0xF
};
#else
uint8_t AES_INV_BYTE_SUB[16][16] = { { 0 } };
#endif

/* Multiplikation table for 0x02 in GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 0
static const uint8_t MULTIPLY_0x2[256] = {
  // 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, // 0x0
    0x20, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E, // 0x1
    0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E, // 0x2
    0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7A, 0x7C, 0x7E, // 0x3
    0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x8E, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9A, 0x9C, 0x9E, // 0x4
    0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE, 0xB0, 0xB2, 0xB4, 0xB6, 0xB8, 0xBA, 0xBC, 0xBE, // 0x5
    0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE, 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE, // 0x6
    0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE, 0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC, 0xFE, // 0x7
    0x1B, 0x19, 0x1F, 0x1D, 0x13, 0x11, 0x17, 0x15, 0x0B, 0x09, 0x0F, 0x0D, 0x03, 0x01, 0x07, 0x05, // 0x8
    0x3B, 0x39, 0x3F, 0x3D, 0x33, 0x31, 0x37, 0x35, 0x2B, 0x29, 0x2F, 0x2D, 0x23, 0x21, 0x27, 0x25, // 0x9
    0x5B, 0x59, 0x5F, 0x5D, 0x53, 0x51, 0x57, 0x55, 0x4B, 0x49, 0x4F, 0x4D, 0x43, 0x41, 0x47, 0x45, // 0xA
    0x7B, 0x79, 0x7F, 0x7D, 0x73, 0x71, 0x77, 0x75, 0x6B, 0x69, 0x6F, 0x6D, 0x63, 0x61, 0x67, 0x65, // 0xB
    0x9B, 0x99, 0x9F, 0x9D, 0x93, 0x91, 0x97, 0x95, 0x8B, 0x89, 0x8F, 0x8D, 0x83, 0x81, 0x87, 0x85, // 0xC
    0xBB, 0xB9, 0xBF, 0xBD, 0xB3, 0xB1, 0xB7, 0xB5, 0xAB, 0xA9, 0xAF, 0xAD, 0xA3, 0xA1, 0xA7, 0xA5, // 0xD
    0xDB, 0xD9, 0xDF, 0xDD, 0xD3, 0xD1, 0xD7, 0xD5, 0xCB, 0xC9, 0xCF, 0xCD, 0xC3, 0xC1, 0xC7, 0xC5, // 0xE
    0xFB, 0xF9, 0xFF, 0xFD, 0xF3, 0xF1, 0xF7, 0xF5, 0xEB, 0xE9, 0xEF, 0xED, 0xE3, 0xE1, 0xE7, 0xE5  // 0xF
};
#else
uint8_t MULTIPLY_0x2[256]  = { 0 };
#endif

/* Multiplikation table for 0x03 in GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 0
static const uint8_t MULTIPLY_0x3[256] = {
  // 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x00, 0x03, 0x06, 0x05, 0x0C, 0x0F, 0x0A, 0x09, 0x18, 0x1B, 0x1E, 0x1D, 0x14, 0x17, 0x12, 0x11, // 0x0
    0x30, 0x33, 0x36, 0x35, 0x3C, 0x3F, 0x3A, 0x39, 0x28, 0x2B, 0x2E, 0x2D, 0x24, 0x27, 0x22, 0x21, // 0x1
    0x60, 0x63, 0x66, 0x65, 0x6C, 0x6F, 0x6A, 0x69, 0x78, 0x7B, 0x7E, 0x7D, 0x74, 0x77, 0x72, 0x71, // 0x2
    0x50, 0x53, 0x56, 0x55, 0x5C, 0x5F, 0x5A, 0x59, 0x48, 0x4B, 0x4E, 0x4D, 0x44, 0x47, 0x42, 0x41, // 0x3
    0xC0, 0xC3, 0xC6, 0xC5, 0xCC, 0xCF, 0xCA, 0xC9, 0xD8, 0xDB, 0xDE, 0xDD, 0xD4, 0xD7, 0xD2, 0xD1, // 0x4
    0xF0, 0xF3, 0xF6, 0xF5, 0xFC, 0xFF, 0xFA, 0xF9, 0xE8, 0xEB, 0xEE, 0xED, 0xE4, 0xE7, 0xE2, 0xE1, // 0x5
    0xA0, 0xA3, 0xA6, 0xA5, 0xAC, 0xAF, 0xAA, 0xA9, 0xB8, 0xBB, 0xBE, 0xBD, 0xB4, 0xB7, 0xB2, 0xB1, // 0x6
    0x90, 0x93, 0x96, 0x95, 0x9C, 0x9F, 0x9A, 0x99, 0x88, 0x8B, 0x8E, 0x8D, 0x84, 0x87, 0x82, 0x81, // 0x7
    0x9B, 0x98, 0x9D, 0x9E, 0x97, 0x94, 0x91, 0x92, 0x83, 0x80, 0x85, 0x86, 0x8F, 0x8C, 0x89, 0x8A, // 0x8
    0xAB, 0xA8, 0xAD, 0xAE, 0xA7, 0xA4, 0xA1, 0xA2, 0xB3, 0xB0, 0xB5, 0xB6, 0xBF, 0xBC, 0xB9, 0xBA, // 0x9
    0xFB, 0xF8, 0xFD, 0xFE, 0xF7, 0xF4, 0xF1, 0xF2, 0xE3, 0xE0, 0xE5, 0xE6, 0xEF, 0xEC, 0xE9, 0xEA, // 0xA
    0xCB, 0xC8, 0xCD, 0xCE, 0xC7, 0xC4, 0xC1, 0xC2, 0xD3, 0xD0, 0xD5, 0xD6, 0xDF, 0xDC, 0xD9, 0xDA, // 0xB
    0x5B, 0x58, 0x5D, 0x5E, 0x57, 0x54, 0x51, 0x52, 0x43, 0x40, 0x45, 0x46, 0x4F, 0x4C, 0x49, 0x4A, // 0xC
    0x6B, 0x68, 0x6D, 0x6E, 0x67, 0x64, 0x61, 0x62, 0x73, 0x70, 0x75, 0x76, 0x7F, 0x7C, 0x79, 0x7A, // 0xD
    0x3B, 0x38, 0x3D, 0x3E, 0x37, 0x34, 0x31, 0x32, 0x23, 0x20, 0x25, 0x26, 0x2F, 0x2C, 0x29, 0x2A, // 0xE
    0x0B, 0x08, 0x0D, 0x0E, 0x07, 0x04, 0x01, 0x02, 0x13, 0x10, 0x15, 0x16, 0x1F, 0x1C, 0x19, 0x1A, // 0xF
};
#else
uint8_t MULTIPLY_0x3[256] = { 0 };
#endif

/* Multiplikation table for 0xE in GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 0
static const uint8_t MULTIPLY_0xE[256] = {
  // 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x00, 0x0E, 0x1C, 0x12, 0x38, 0x36, 0x24, 0x2A, 0x70, 0x7E, 0x6C, 0x62, 0x48, 0x46, 0x54, 0x5A, 
    0xE0, 0xEE, 0xFC, 0xF2, 0xD8, 0xD6, 0xC4, 0xCA, 0x90, 0x9E, 0x8C, 0x82, 0xA8, 0xA6, 0xB4, 0xBA, 
    0xDB, 0xD5, 0xC7, 0xC9, 0xE3, 0xED, 0xFF, 0xF1, 0xAB, 0xA5, 0xB7, 0xB9, 0x93, 0x9D, 0x8F, 0x81, 
    0x3B, 0x35, 0x27, 0x29, 0x03, 0x0D, 0x1F, 0x11, 0x4B, 0x45, 0x57, 0x59, 0x73, 0x7D, 0x6F, 0x61, 
    0xAD, 0xA3, 0xB1, 0xBF, 0x95, 0x9B, 0x89, 0x87, 0xDD, 0xD3, 0xC1, 0xCF, 0xE5, 0xEB, 0xF9, 0xF7, 
    0x4D, 0x43, 0x51, 0x5F, 0x75, 0x7B, 0x69, 0x67, 0x3D, 0x33, 0x21, 0x2F, 0x05, 0x0B, 0x19, 0x17, 
    0x76, 0x78, 0x6A, 0x64, 0x4E, 0x40, 0x52, 0x5C, 0x06, 0x08, 0x1A, 0x14, 0x3E, 0x30, 0x22, 0x2C, 
    0x96, 0x98, 0x8A, 0x84, 0xAE, 0xA0, 0xB2, 0xBC, 0xE6, 0xE8, 0xFA, 0xF4, 0xDE, 0xD0, 0xC2, 0xCC, 
    0x41, 0x4F, 0x5D, 0x53, 0x79, 0x77, 0x65, 0x6B, 0x31, 0x3F, 0x2D, 0x23, 0x09, 0x07, 0x15, 0x1B, 
    0xA1, 0xAF, 0xBD, 0xB3, 0x99, 0x97, 0x85, 0x8B, 0xD1, 0xDF, 0xCD, 0xC3, 0xE9, 0xE7, 0xF5, 0xFB, 
    0x9A, 0x94, 0x86, 0x88, 0xA2, 0xAC, 0xBE, 0xB0, 0xEA, 0xE4, 0xF6, 0xF8, 0xD2, 0xDC, 0xCE, 0xC0, 
    0x7A, 0x74, 0x66, 0x68, 0x42, 0x4C, 0x5E, 0x50, 0x0A, 0x04, 0x16, 0x18, 0x32, 0x3C, 0x2E, 0x20, 
    0xEC, 0xE2, 0xF0, 0xFE, 0xD4, 0xDA, 0xC8, 0xC6, 0x9C, 0x92, 0x80, 0x8E, 0xA4, 0xAA, 0xB8, 0xB6, 
    0x0C, 0x02, 0x10, 0x1E, 0x34, 0x3A, 0x28, 0x26, 0x7C, 0x72, 0x60, 0x6E, 0x44, 0x4A, 0x58, 0x56, 
    0x37, 0x39, 0x2B, 0x25, 0x0F, 0x01, 0x13, 0x1D, 0x47, 0x49, 0x5B, 0x55, 0x7F, 0x71, 0x63, 0x6D, 
    0xD7, 0xD9, 0xCB, 0xC5, 0xEF, 0xE1, 0xF3, 0xFD, 0xA7, 0xA9, 0xBB, 0xB5, 0x9F, 0x91, 0x83, 0x8D
};
#else
uint8_t MULTIPLY_0xE[256] = { 0 };
#endif

/* Multiplikation table for 0x9 in GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 0
static const uint8_t MULTIPLY_0x9[256] = {
  // 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x00, 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3F, 0x48, 0x41, 0x5A, 0x53, 0x6C, 0x65, 0x7E, 0x77, 
    0x90, 0x99, 0x82, 0x8B, 0xB4, 0xBD, 0xA6, 0xAF, 0xD8, 0xD1, 0xCA, 0xC3, 0xFC, 0xF5, 0xEE, 0xE7, 
    0x3B, 0x32, 0x29, 0x20, 0x1F, 0x16, 0x0D, 0x04, 0x73, 0x7A, 0x61, 0x68, 0x57, 0x5E, 0x45, 0x4C, 
    0xAB, 0xA2, 0xB9, 0xB0, 0x8F, 0x86, 0x9D, 0x94, 0xE3, 0xEA, 0xF1, 0xF8, 0xC7, 0xCE, 0xD5, 0xDC, 
    0x76, 0x7F, 0x64, 0x6D, 0x52, 0x5B, 0x40, 0x49, 0x3E, 0x37, 0x2C, 0x25, 0x1A, 0x13, 0x08, 0x01, 
    0xE6, 0xEF, 0xF4, 0xFD, 0xC2, 0xCB, 0xD0, 0xD9, 0xAE, 0xA7, 0xBC, 0xB5, 0x8A, 0x83, 0x98, 0x91, 
    0x4D, 0x44, 0x5F, 0x56, 0x69, 0x60, 0x7B, 0x72, 0x05, 0x0C, 0x17, 0x1E, 0x21, 0x28, 0x33, 0x3A, 
    0xDD, 0xD4, 0xCF, 0xC6, 0xF9, 0xF0, 0xEB, 0xE2, 0x95, 0x9C, 0x87, 0x8E, 0xB1, 0xB8, 0xA3, 0xAA, 
    0xEC, 0xE5, 0xFE, 0xF7, 0xC8, 0xC1, 0xDA, 0xD3, 0xA4, 0xAD, 0xB6, 0xBF, 0x80, 0x89, 0x92, 0x9B, 
    0x7C, 0x75, 0x6E, 0x67, 0x58, 0x51, 0x4A, 0x43, 0x34, 0x3D, 0x26, 0x2F, 0x10, 0x19, 0x02, 0x0B, 
    0xD7, 0xDE, 0xC5, 0xCC, 0xF3, 0xFA, 0xE1, 0xE8, 0x9F, 0x96, 0x8D, 0x84, 0xBB, 0xB2, 0xA9, 0xA0, 
    0x47, 0x4E, 0x55, 0x5C, 0x63, 0x6A, 0x71, 0x78, 0x0F, 0x06, 0x1D, 0x14, 0x2B, 0x22, 0x39, 0x30, 
    0x9A, 0x93, 0x88, 0x81, 0xBE, 0xB7, 0xAC, 0xA5, 0xD2, 0xDB, 0xC0, 0xC9, 0xF6, 0xFF, 0xE4, 0xED, 
    0x0A, 0x03, 0x18, 0x11, 0x2E, 0x27, 0x3C, 0x35, 0x42, 0x4B, 0x50, 0x59, 0x66, 0x6F, 0x74, 0x7D, 
    0xA1, 0xA8, 0xB3, 0xBA, 0x85, 0x8C, 0x97, 0x9E, 0xE9, 0xE0, 0xFB, 0xF2, 0xCD, 0xC4, 0xDF, 0xD6, 
    0x31, 0x38, 0x23, 0x2A, 0x15, 0x1C, 0x07, 0x0E, 0x79, 0x70, 0x6B, 0x62, 0x5D, 0x54, 0x4F, 0x46, 
};
#else
uint8_t MULTIPLY_0x9[256] = { 0 };
#endif

/* Multiplikation table for 0xB in GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 0
static const uint8_t MULTIPLY_0xB[256] = {
  // 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x00, 0x0B, 0x16, 0x1D, 0x2C, 0x27, 0x3A, 0x31, 0x58, 0x53, 0x4E, 0x45, 0x74, 0x7F, 0x62, 0x69, 
    0xB0, 0xBB, 0xA6, 0xAD, 0x9C, 0x97, 0x8A, 0x81, 0xE8, 0xE3, 0xFE, 0xF5, 0xC4, 0xCF, 0xD2, 0xD9, 
    0x7B, 0x70, 0x6D, 0x66, 0x57, 0x5C, 0x41, 0x4A, 0x23, 0x28, 0x35, 0x3E, 0x0F, 0x04, 0x19, 0x12, 
    0xCB, 0xC0, 0xDD, 0xD6, 0xE7, 0xEC, 0xF1, 0xFA, 0x93, 0x98, 0x85, 0x8E, 0xBF, 0xB4, 0xA9, 0xA2, 
    0xF6, 0xFD, 0xE0, 0xEB, 0xDA, 0xD1, 0xCC, 0xC7, 0xAE, 0xA5, 0xB8, 0xB3, 0x82, 0x89, 0x94, 0x9F, 
    0x46, 0x4D, 0x50, 0x5B, 0x6A, 0x61, 0x7C, 0x77, 0x1E, 0x15, 0x08, 0x03, 0x32, 0x39, 0x24, 0x2F, 
    0x8D, 0x86, 0x9B, 0x90, 0xA1, 0xAA, 0xB7, 0xBC, 0xD5, 0xDE, 0xC3, 0xC8, 0xF9, 0xF2, 0xEF, 0xE4, 
    0x3D, 0x36, 0x2B, 0x20, 0x11, 0x1A, 0x07, 0x0C, 0x65, 0x6E, 0x73, 0x78, 0x49, 0x42, 0x5F, 0x54, 
    0xF7, 0xFC, 0xE1, 0xEA, 0xDB, 0xD0, 0xCD, 0xC6, 0xAF, 0xA4, 0xB9, 0xB2, 0x83, 0x88, 0x95, 0x9E, 
    0x47, 0x4C, 0x51, 0x5A, 0x6B, 0x60, 0x7D, 0x76, 0x1F, 0x14, 0x09, 0x02, 0x33, 0x38, 0x25, 0x2E, 
    0x8C, 0x87, 0x9A, 0x91, 0xA0, 0xAB, 0xB6, 0xBD, 0xD4, 0xDF, 0xC2, 0xC9, 0xF8, 0xF3, 0xEE, 0xE5, 
    0x3C, 0x37, 0x2A, 0x21, 0x10, 0x1B, 0x06, 0x0D, 0x64, 0x6F, 0x72, 0x79, 0x48, 0x43, 0x5E, 0x55, 
    0x01, 0x0A, 0x17, 0x1C, 0x2D, 0x26, 0x3B, 0x30, 0x59, 0x52, 0x4F, 0x44, 0x75, 0x7E, 0x63, 0x68, 
    0xB1, 0xBA, 0xA7, 0xAC, 0x9D, 0x96, 0x8B, 0x80, 0xE9, 0xE2, 0xFF, 0xF4, 0xC5, 0xCE, 0xD3, 0xD8, 
    0x7A, 0x71, 0x6C, 0x67, 0x56, 0x5D, 0x40, 0x4B, 0x22, 0x29, 0x34, 0x3F, 0x0E, 0x05, 0x18, 0x13, 
    0xCA, 0xC1, 0xDC, 0xD7, 0xE6, 0xED, 0xF0, 0xFB, 0x92, 0x99, 0x84, 0x8F, 0xBE, 0xB5, 0xA8, 0xA3
};
#else
uint8_t MULTIPLY_0xB[256] = { 0 };
#endif

/* Multiplikation table for 0xD in GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 0
static const uint8_t MULTIPLY_0xD[256] = {
  // 0x0,  0x1,  0x2,  0x3,  0x4,  0x5,  0x6,  0x7,  0x8,  0x9,  0xA,  0xB,  0xC,  0xD,  0xE,  0xF,
    0x00, 0x0D, 0x1A, 0x17, 0x34, 0x39, 0x2E, 0x23, 0x68, 0x65, 0x72, 0x7F, 0x5C, 0x51, 0x46, 0x4B, 
    0xD0, 0xDD, 0xCA, 0xC7, 0xE4, 0xE9, 0xFE, 0xF3, 0xB8, 0xB5, 0xA2, 0xAF, 0x8C, 0x81, 0x96, 0x9B, 
    0xBB, 0xB6, 0xA1, 0xAC, 0x8F, 0x82, 0x95, 0x98, 0xD3, 0xDE, 0xC9, 0xC4, 0xE7, 0xEA, 0xFD, 0xF0, 
    0x6B, 0x66, 0x71, 0x7C, 0x5F, 0x52, 0x45, 0x48, 0x03, 0x0E, 0x19, 0x14, 0x37, 0x3A, 0x2D, 0x20, 
    0x6D, 0x60, 0x77, 0x7A, 0x59, 0x54, 0x43, 0x4E, 0x05, 0x08, 0x1F, 0x12, 0x31, 0x3C, 0x2B, 0x26, 
    0xBD, 0xB0, 0xA7, 0xAA, 0x89, 0x84, 0x93, 0x9E, 0xD5, 0xD8, 0xCF, 0xC2, 0xE1, 0xEC, 0xFB, 0xF6, 
    0xD6, 0xDB, 0xCC, 0xC1, 0xE2, 0xEF, 0xF8, 0xF5, 0xBE, 0xB3, 0xA4, 0xA9, 0x8A, 0x87, 0x90, 0x9D, 
    0x06, 0x0B, 0x1C, 0x11, 0x32, 0x3F, 0x28, 0x25, 0x6E, 0x63, 0x74, 0x79, 0x5A, 0x57, 0x40, 0x4D, 
    0xDA, 0xD7, 0xC0, 0xCD, 0xEE, 0xE3, 0xF4, 0xF9, 0xB2, 0xBF, 0xA8, 0xA5, 0x86, 0x8B, 0x9C, 0x91, 
    0x0A, 0x07, 0x10, 0x1D, 0x3E, 0x33, 0x24, 0x29, 0x62, 0x6F, 0x78, 0x75, 0x56, 0x5B, 0x4C, 0x41, 
    0x61, 0x6C, 0x7B, 0x76, 0x55, 0x58, 0x4F, 0x42, 0x09, 0x04, 0x13, 0x1E, 0x3D, 0x30, 0x27, 0x2A, 
    0xB1, 0xBC, 0xAB, 0xA6, 0x85, 0x88, 0x9F, 0x92, 0xD9, 0xD4, 0xC3, 0xCE, 0xED, 0xE0, 0xF7, 0xFA, 
    0xB7, 0xBA, 0xAD, 0xA0, 0x83, 0x8E, 0x99, 0x94, 0xDF, 0xD2, 0xC5, 0xC8, 0xEB, 0xE6, 0xF1, 0xFC, 
    0x67, 0x6A, 0x7D, 0x70, 0x53, 0x5E, 0x49, 0x44, 0x0F, 0x02, 0x15, 0x18, 0x3B, 0x36, 0x21, 0x2C, 
    0x0C, 0x01, 0x16, 0x1B, 0x38, 0x35, 0x22, 0x2F, 0x64, 0x69, 0x7E, 0x73, 0x50, 0x5D, 0x4A, 0x47, 
    0xDC, 0xD1, 0xC6, 0xCB, 0xE8, 0xE5, 0xF2, 0xFF, 0xB4, 0xB9, 0xAE, 0xA3, 0x80, 0x8D, 0x9A, 0x97 
};
#else
uint8_t MULTIPLY_0xD[256] = { 0 };
#endif

/* AES Round Coefficient Table used during Key Schedule */
#if AES_CALCULATE_LOOKUP_TABLES == 0
static const uint32_t RC[10] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
    0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000
};
#else
uint32_t RC[10] = { 0 };
#endif


/* Main AES Encryption Routine */
uint8_t
aes_encrypt(const uint8_t *key, uint8_t *state)
{
    // Initialize Key Schedule
    aes_init(key);

    /* Key Whitening */
    key_addition(AES_KEY_SCHEDULE[0], state);

    /*
     * Last round lacks MixColumn Layer so
     * we stay under AES_ROUNDS times (<)
     */
    for(int i = 1; i < AES_ROUNDS; i++)
    {
        byte_substitution(state);
        shift_rows(state);
        mix_column(state);
        key_addition(AES_KEY_SCHEDULE[i], state);
    }
    /* Last Round */
    byte_substitution(state);
    shift_rows(state);
    key_addition(AES_KEY_SCHEDULE[AES_ROUNDS], state);

    /* AES Encryption Finished */
    return 0;
}

// void
// show_state(uint8_t *state)
// {
//     for(int i = 0; i < 16; i++, state++)
//         printf("%02x", *state);
//     puts("");
//     return;
// }

/* Main AES Decryption Routine */
uint8_t
aes_decrypt(const uint8_t *key, uint8_t *state)
{
    // Initialize Key Schedule
    aes_init(key);

    /* Last round without Mix Column */
    key_addition(AES_KEY_SCHEDULE[AES_ROUNDS], state);
    inv_shift_rows(state);
    inv_byte_substitution(state);
    /* Inverse AES_ROUNDS -1 */
    for(int i = AES_ROUNDS-1; i > 0; i--)
    {
        key_addition(AES_KEY_SCHEDULE[i], state);
        inv_mix_column(state);
        inv_shift_rows(state);
        inv_byte_substitution(state);
    }

    /* Invert Key Whitening */
    key_addition(AES_KEY_SCHEDULE[0], state);

    /* AES Decryption Finished */
    return 0;
}

/* Initialize Global Key Schedule */
uint8_t
aes_init(const uint8_t *key)
{
    if(INITIALIZED == 0)
    {
        #if AES_CALCULATE_LOOKUP_TABLES == 1
        gen_rcon_table();
        gen_bytesub();
        gen_inv_bytesub();
        gen_multiplication();
        #endif
        key_schedule(key);
        INITIALIZED = 1;
    }
    return 0;
}

/* AES Key schedule */
int8_t
key_schedule(const uint8_t *key)
{
    // Fill in w0, w1, w2, w4
    uint32_t *wi = AES_KEY_SCHEDULE[0];
    for(int i = 0; i < 16; i++)
    {
        *wi = *wi << 8 | key[i];
        if(i % AES_NK == (AES_NK-1))
            wi++;
    }

    // Start actual key schedule iterations
    for(int i = 0; i < AES_WI_RUNS; i++)
    {
        uint32_t temp = *(wi-1);
        if(i % AES_NK == 0)
        {
            rotate_word(&temp);
            uint32_t sub[4] = { 0 };
            sub[0] = SUB_BYTE(temp >> 24 & 0xFF) << 24;
            sub[1] = SUB_BYTE(temp >> 16 & 0xFF) << 16;
            sub[2] = SUB_BYTE(temp >>  8 & 0xFF) << 8;
            sub[3] = SUB_BYTE(temp       & 0xFF);
            temp = sub[0] | sub[1] | sub[2] | sub[3];
            temp ^= AES_RC(i / AES_NK);
        }
        *wi = *(wi-AES_NK) ^ temp;
        wi++;
    }
    return 0;
}

/* Left-Shift-Rotate a uint32_t aka AES Word */
void
rotate_word(uint32_t *aes_word)
{
    *aes_word = (*aes_word << 8) | ((*aes_word >> 24) & 0xFF);
}

/* Retrieve the position of the left most bit */
#if AES_CALCULATE_LOOKUP_TABLES == 1
int8_t
get_shifts(uint16_t *factor)
{
    for(int i = 0; i < 16; i++)
        if(((*factor >> (16-i)) & 0x1) == 1)
            return i-1;
    return -1;
}
#endif

/* Polynomial division under GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 1
uint8_t
aes_polynomial_division(uint16_t *factor)
{
    uint16_t result = *factor;
    if(result == 0)
        return 0;
    /* We need to find out the necessary shifts
       to start reducing the polynomial *factor
       by AES_POLYNOMIAL */
    int8_t shift = get_shifts(&result);
    while (shift < 8 && shift >= 0)
    {
        result ^= (AES_POLYNOMIAL << (7 - shift));
        shift = get_shifts(&result);
    } 
    return (uint8_t) result;
}
#endif

/* Polynomial multiplication under GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 1
uint16_t
multiply_polynomial(uint8_t factor1, uint8_t factor2)
{
    /* Some sanity checks */
    if(factor1 == 0 || factor2 == 0)
        return 0;
    if(factor1 == 1)
        return factor2;
    if(factor2 == 1)
        return factor1;

    /* The actual calculation */
    uint16_t result = 0;
    for(int i = 0; i < 8; i++)
    {
        if(((factor2 >> i) & 0x1) == 1)
        {
            uint16_t intermediate = factor1;
            result ^= (intermediate <<= i);
        }
    }
    return result;
}
#endif

/* Calculate the Multiplicative of a Byte in GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 1
uint8_t
multiplicative_inverse(uint8_t input)
{
    /* 0 maps to 0x63; */
    if(input == 0)
        return 0^0x63;

    /*
     * Create logarithm chart
     */
    uint16_t poly = 1;
    uint8_t logarithm[16][16] = { { 0 } };  // 0xF * 0xF Elements
    for(int i = 1; i < 255; i++)
    {
        poly = multiply_polynomial(poly, AES_GENERATOR);
        poly = aes_polynomial_division(&poly);
        uint8_t x = poly & 0xF;
        uint8_t y = poly >> 4;
        logarithm[y][x] = i;
    }

    uint8_t x = input & 0xF;
    uint8_t y = input >> 4;
    uint8_t k = logarithm[y][x];
    k ^= 0xFF;  // substract 255 in GF(2^8)
    uint16_t result = 1;
    for(int j = 0; j < k; j++)
    {
        result = multiply_polynomial(result, AES_GENERATOR);
        result = aes_polynomial_division(&result);
    }
    uint8_t intermediate = result;
    for(int i = 1; i <= 4; i++)
        result ^= lsr8(intermediate, i);
    result ^= 0x63;
    return result;
}
#endif

/* left shift rotate a 8bit block k */
#if AES_CALCULATE_LOOKUP_TABLES == 1
uint8_t
lsr8(uint8_t k, uint8_t p)
{
    while(p--)
        k = ((k << 1) & 0x0FE) | ((k >> 7) & 0x1);
    return k;
}
#endif

/* Generate and fill global AES Byte Substitution Table */
#if AES_CALCULATE_LOOKUP_TABLES == 1
void
gen_bytesub(void)
{
    for(int i = 0; i < 256; i++)
    {
        uint8_t result = multiplicative_inverse(i);
        uint8_t x = i & 0xF;
        uint8_t y = i >> 4;
        AES_BYTE_SUB[y][x] = result;
    }
    return;
}
#endif

/* Inverse the Byte Substitution Table */
#if AES_CALCULATE_LOOKUP_TABLES == 1
void
gen_inv_bytesub(void)
{
    for(int i = 0; i < 256; i++)
    {
        uint8_t x = i & 0xF;
        uint8_t y = i >> 4;
        uint8_t inverse = AES_BYTE_SUB[y][x];
        x = inverse & 0xF;
        y = inverse >> 4;
        AES_INV_BYTE_SUB[y][x] = i;
    }
    return;
}
#endif

/* RCON Lookup Table for key Schedule */
#if AES_CALCULATE_LOOKUP_TABLES == 1
void
gen_rcon_table(void)
{
    for(int j = 0; j < AES_WI_RUNS; j++)
    {
        if(j % AES_NK == 0)
        {
            uint32_t rcon = 1 << (j / AES_NK );
            rcon = aes_polynomial_division((uint16_t *) &rcon);
            rcon <<= 24;
            RC[j / AES_NK] = rcon;
        }
    }
    return;
}
#endif

/* Exponent in GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 1
uint16_t
exp_polynomial(uint16_t factor, uint8_t exponent)
{
    if(exponent == 0)
        return 0;
    uint16_t result = factor;
    for(int i = 0; i < exponent; i++)
    {
        result = multiply_polynomial(result, exponent);
        result = aes_polynomial_division(&result);
    }
    return result;
}
#endif

/* Key Addition Layer */
int8_t
key_addition(uint32_t *key, uint8_t *state)
{
    /*
     * Take these 4x16bits from current state:
     *  ->
     * 1 0 0 0
     * 1 0 0 0
     * 1 0 0 0
     * 1 0 0 0
     */

    /*
     * key is a 4x32bit array
     * We want to have the i-th column of the word array
     * 
     * 1. 0xFF000000 0xFF000000 0xFF000000 0xFF000000
     * 2. 0x00FF0000 0x00FF0000 0x00FF0000 0x00FF0000
     * 3. 0x0000FF00 0x0000FF00 0x0000FF00 0x0000FF00
     * 4. 0x000000FF 0x000000FF 0x000000FF 0x000000FF
     * 
     */
    for(int i = 0; i < 4; i++)
    {
        /*
         * Word State is a 16 16bit integer
         * array j becomes the column offset
         */
        uint8_t *ws = state + i;

        /* GF(2^8) Add the word key to the word state */
        for(int j = 0; j < 4; j++)
        {
            /* Form the current Word Key */
            uint32_t ck = key[j];
            uint16_t wk = (ck >> (24 - (i*8)) & 0xFF);
            *ws ^= wk;
            ws = ws + 4;    /* jump to next column */
        }
    }
    return 0;
}

/* Byte Substitution Layer */
int8_t
byte_substitution(uint8_t *state)
{
    for(int i = 0; i < 16; i++, state++)
        *state = SUB_BYTE(*state);
    return 0;
}

/* Shift Row Layer */
int8_t
shift_rows(uint8_t *state)
{
    /*
        b00 b04 b08 b12
        b01 b05 b09 b13
        b02 b06 b10 b14
        b03 b07 b11 b15

        becomes

        b00 b04 b08 b12
        b05 b09 b13 b01
        b10 b14 b02 b06
        b15 b03 b07 b11
    */

    uint8_t temp = 0;

    /* First row remains untouched */
    /* noop */

    /* Second row gets shifted by one */
    // b1 b5 b9 b13 --> b5 b9 b13 b1
    temp      = state[1];
    state[1]  = state[5];
    state[5]  = state[9];
    state[9]  = state[13];
    state[13] = temp;

    /* Third row gets shifted by two */
    // b2 b6 b10 b14 --> b10 b14 b2 b6
    temp      = state[2];
    state[2]  = state[10];
    state[10] = temp;
    
    temp      = state[6];
    state[6]  = state[14];
    state[14] = temp;

    /* Fourth row gets shifted by three */
    // b3 b7 b11 b15 --> b15 b3 b7 b11
    temp      = state[3];
    state[3]  = state[15];
    state[15] = temp;

    temp      = state[7];
    state[7]  = state[15];
    state[15] = temp;

    temp      = state[11];
    state[11] = state[15];
    state[15] = temp;

    return 0;
}

/* Mix Column Layer */
int8_t
mix_column(uint8_t *state)
{
    for(int i = 0; i < 4; i++)
    {
        /*
            b0 b4  b8 b12
            b1 b5  b9 b13
            b2 b6 b10 b14
            b3 b7 b11 b15

            02 03 01 01
            01 02 03 01
            01 01 02 03
            03 01 01 02
        */

        /* Calculations require unaltered input so 
         * we have to keep track of the original and
         * new states somewhere
         */
        uint8_t temp[4] = { 0 };
        uint8_t *pstate = state + (i*4);    /* i*4 means next column */

        temp[0] =
            AES_MULTIPLY_0x2(*(pstate+0)) ^ AES_MULTIPLY_0x3(*(pstate+1)) ^
            *(pstate+2) ^ *(pstate+3);

        temp[1] =
            *(pstate+0) ^ AES_MULTIPLY_0x2(*(pstate+1)) ^
            AES_MULTIPLY_0x3(*(pstate+2)) ^ *(pstate+3);

        temp[2] =
            *(pstate+0) ^ *(pstate+1) ^ 
            AES_MULTIPLY_0x2(*(pstate+2)) ^ AES_MULTIPLY_0x3(*(pstate+3));

        temp[3] =
            AES_MULTIPLY_0x3(*(pstate+0)) ^ *(pstate+1) ^
            *(pstate+2) ^ AES_MULTIPLY_0x2(*(pstate+3));

        *(pstate+0) = temp[0];
        *(pstate+1) = temp[1];
        *(pstate+2) = temp[2];
        *(pstate+3) = temp[3];

    }
    return 0;
}

/* Generate Multiplication tables of 0x02 and 0x03 under GF(2^8) */
#if AES_CALCULATE_LOOKUP_TABLES == 1
void
gen_multiplication(void)
{
    for(int i = 0; i < 256; i++)
    {
        uint16_t result = 0;
        /* 0x2 */
        result = multiply_polynomial(0x2, i);
        MULTIPLY_0x2[i] = aes_polynomial_division(&result);

        /* 0x3 */
        result = multiply_polynomial(0x3, i);
        MULTIPLY_0x3[i] = aes_polynomial_division(&result);

        /* 0x9 */
        result = multiply_polynomial(0x9, i);
        MULTIPLY_0x9[i] = aes_polynomial_division(&result);

        /* 0xE */
        result = multiply_polynomial(0xE, i);
        MULTIPLY_0xE[i] = aes_polynomial_division(&result);

        /* 0xB */
        result = multiply_polynomial(0xB, i);
        MULTIPLY_0xB[i] = aes_polynomial_division(&result);

        /* 0xD */
        result = multiply_polynomial(0xD, i);
        MULTIPLY_0xD[i] = aes_polynomial_division(&result);
    }
    return;
}
#endif

/* Inverse Byte Substitution */
int8_t
inv_byte_substitution(uint8_t *state)
{
    for(int i = 0; i < 16; i++, state++)
        *state = INV_SUB_BYTE(*state);
    return 0;
}

/* Inverse Shift Rows */
int8_t
inv_shift_rows(uint8_t *state)
{
    /*
        b00 b04 b08 b12
        b05 b09 b13 b01
        b10 b14 b02 b06
        b15 b03 b07 b11

        becomes

        b00 b04 b08 b12
        b01 b05 b09 b13
        b02 b06 b10 b14
        b03 b07 b11 b15
    */

    uint8_t temp = 0;

    /* First row remains untouched */
    /* noop */

    /* Second row gets shifted by one */
    // b5 b9 b13 b1 --> b1 b5 b9 b13 
    temp      = state[13];
    state[13] = state[9];
    state[9]  = state[5];
    state[5]  = state[1];
    state[1]  = temp;

    /* Third row gets shifted by two */
    // b10 b14 b2 b6 --> b2 b6 b10 b14
    temp      = state[10];
    state[10] = state[2];
    state[2]  = temp;

    temp      = state[14];
    state[14] = state[6];
    state[6]  = temp;
    
    /* Fourth row gets shifted by three */
    // b15 b3 b7 b11 --> b3 b7 b11 b15
    temp      = state[15];
    state[15] = state[11];
    state[11] = temp;

    temp      = state[15];
    state[15] = state[7];
    state[7]  = temp;

    temp      = state[15];
    state[15] = state[3];
    state[3]  = temp;

    return 0;
}

/* Inverse Mix Column */
int8_t
inv_mix_column(uint8_t *state)
{
    for(int i = 0; i < 4; i++)
    {
        /*
            b0 b4  b8 b12
            b1 b5  b9 b13
            b2 b6 b10 b14
            b3 b7 b11 b15

            0e 0b 0d 09
            09 0e 0b 0d
            0d 09 0e 0b
            0b 0d 09 0e
        */

        /* Calculations require unaltered input so 
         * we have to keep track of the original and
         * new states somewhere
         */
        uint8_t temp[4] = { 0 };
        uint8_t *pstate = state + (i*4);    /* i*4 means next column */

        temp[0] =
            AES_MULTIPLY_0xE(*(pstate+0)) ^ AES_MULTIPLY_0xB(*(pstate+1)) ^
            AES_MULTIPLY_0xD(*(pstate+2)) ^ AES_MULTIPLY_0x9(*(pstate+3));

        temp[1] =
            AES_MULTIPLY_0x9(*(pstate+0)) ^ AES_MULTIPLY_0xE(*(pstate+1)) ^
            AES_MULTIPLY_0xB(*(pstate+2)) ^ AES_MULTIPLY_0xD(*(pstate+3));

        temp[2] =
            AES_MULTIPLY_0xD(*(pstate+0)) ^ AES_MULTIPLY_0x9(*(pstate+1)) ^
            AES_MULTIPLY_0xE(*(pstate+2)) ^ AES_MULTIPLY_0xB(*(pstate+3));

        temp[3] =
            AES_MULTIPLY_0xB(*(pstate+0)) ^ AES_MULTIPLY_0xD(*(pstate+1)) ^
            AES_MULTIPLY_0x9(*(pstate+2)) ^ AES_MULTIPLY_0xE(*(pstate+3));

        *(pstate+0) = temp[0];
        *(pstate+1) = temp[1];
        *(pstate+2) = temp[2];
        *(pstate+3) = temp[3];

    }
    return 0;
}