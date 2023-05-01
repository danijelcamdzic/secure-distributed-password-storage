/**
 * @file rsa_functions.h
 * @brief Contains necessary defines, variables declarations
 * and function protypes for using the openssl library for RSA
 * cryptography
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#ifndef RSA_FUNCTIONS_H
#define RSA_FUNCTIONS_H

#include <iostream>
#include <thread>
#include <fstream>
#include <vector>

/* Opensll library for RSA cryptography */
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

/* Master public and private key (RSA 2048-bit) path declarations */
extern const std::string RSA_PUBLIC_KEY;
extern const std::string RSA_PRIVATE_KEY;

/* Public key (RSA 2048-bit) of the hardware nodes path declaration */
extern const std::string RSA_PUBLIC_KEY_HW_NODE_1;                      /**< Should list all the paths to the public keys of the hardware nodes */  

/* Vector containing the hardware nodes declaration */
extern const std::vector<std::string> public_keys_hw_nodes;

/* Function prototypes */
std::vector<unsigned char> rsa_encrypt_message(const std::vector<unsigned char>& message, const std::string& public_key_filename);
std::vector<unsigned char> rsa_decrypt_message(const std::vector<unsigned char>& encrypted_message, const std::string& private_key_filename);

#endif /* RSA_FUNCTIONS_H */