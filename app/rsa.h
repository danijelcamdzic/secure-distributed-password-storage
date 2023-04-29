#ifndef RSA_H
#define RSA_H

#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

std::string encrypt_message(const std::string& message, const std::string& public_key_filename);
std::string decrypt_message(const std::string& encrypted_message, const std::string& private_key_filename);

#endif