#ifndef RSA_H
#define RSA_H

#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

extern const std::string RSA_PUBLIC_KEY;
extern const std::string RSA_PRIVATE_KEY;
extern const std::string RSA_PUBLIC_KEY_HW_NODE_1;
extern const std::vector<std::string> public_keys_hw_nodes;

std::string rsa_encrypt_message(const std::string& message, const std::string& public_key_filename);
std::vector<unsigned char> rsa_decrypt_message(const std::vector<unsigned char>& encrypted_message, const std::string& private_key_filename);

#endif