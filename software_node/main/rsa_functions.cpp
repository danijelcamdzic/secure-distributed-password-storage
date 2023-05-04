/**
 * @file rsa_functions.cpp
 * @brief Contains necessary defines, variables definitions
 * and function definitions for using the openssl library for RSA
 * cryptography
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#include "rsa_functions.h"

/* ------------------------- VARIABLE DEFINITIONS ------------------------------------ */

/* Master public and private key (RSA 2048-bit) path */
const std::string RSA_PUBLIC_KEY("../rsa_public_key.pem"); 
const std::string RSA_PRIVATE_KEY("../rsa_private_key.pem");

/* Public key (RSA 2048-bit) of the hardware nodes path */
const std::string RSA_PUBLIC_KEY_HW_NODE_1("../rsa_public_key_hw_node_1.pem");      /**< Should list all the paths to the public keys of the hardware nodes */ 

/* Vector containing the hardware nodes declaration */
const std::vector<std::string> public_keys_hw_nodes = {RSA_PUBLIC_KEY_HW_NODE_1};   /**< Should include all the above public key paths of the hardware nodes */


/* ------------------------- FUNCTION DEFINITIONS ------------------------------------ */

/**
 * @brief Encrypt a given message using RSA public key encryption
 * @param[in] message The message to be encrypted as a vector of unsigned characters
 * @param[in] public_key_filename The filename of the public key file to be used for encryption
 * @return A vector of unsigned characters containing the encrypted message or an empty vector if an error occurs
 */
std::vector<unsigned char> rsa_encrypt_message(const std::vector<unsigned char>& message, const std::string& public_key_filename)
{
    /* Open the public key file */
    std::ifstream pub_key_file(public_key_filename);
    if (!pub_key_file)
    {
        std::cerr << "Error opening public key file: " << public_key_filename << std::endl;
        return {};
    }

    /* Read the public key into a string */
    std::string pub_key_str((std::istreambuf_iterator<char>(pub_key_file)), std::istreambuf_iterator<char>());

    /* Create a BIO object from the public key string */
    BIO* bio = BIO_new_mem_buf(pub_key_str.data(), pub_key_str.size());

    /* Create a new RSA object for the public key */
    RSA* rsa_pubkey = RSA_new();

    /* Read the public key from the BIO object */
    if (!PEM_read_bio_RSA_PUBKEY(bio, &rsa_pubkey, nullptr, nullptr))
    {
        std::cerr << "Error reading public key from BIO" << std::endl;
        BIO_free(bio);
        return {};
    }

    /* Get the size of the RSA key */
    int rsa_size = RSA_size(rsa_pubkey);

    /* Create a vector to store the encrypted message */
    std::vector<unsigned char> encrypted_message(rsa_size);

    /* Encrypt the message using the public key */
    int encrypt_len = RSA_public_encrypt(message.size(), message.data(), encrypted_message.data(), rsa_pubkey, RSA_PKCS1_PADDING);

    /* Check for encryption errors */
    if (encrypt_len == -1)
    {
        std::cerr << "Error encrypting message: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        RSA_free(rsa_pubkey);
        BIO_free(bio);
        return {};
    }

    /* Free the RSA and BIO objects */
    RSA_free(rsa_pubkey);
    BIO_free(bio);

    /* Resize the encrypted message vector to the actual length of the encrypted data */
    encrypted_message.resize(encrypt_len);

    /* Return the encrypted message */
    return encrypted_message;
}

/**
 * @brief Decrypt a given encrypted message using RSA private key decryption
 * @param[in] encrypted_message The encrypted message to be decrypted as a vector of unsigned characters
 * @param[in] private_key_filename The filename of the private key file to be used for decryption
 * @return A vector of unsigned characters containing the decrypted message
 */
std::vector<unsigned char> rsa_decrypt_message(const std::vector<unsigned char>& encrypted_message, const std::string& private_key_filename)
{
    /* Open the private key file */
    std::ifstream priv_key_file(private_key_filename, std::ios::in);
    if (!priv_key_file)
    {
        std::cerr << "Error opening public key file: " << private_key_filename << std::endl;
        return {};
    }

    /* Read the private key into a string */
    std::string priv_key_str((std::istreambuf_iterator<char>(priv_key_file)), std::istreambuf_iterator<char>());

    /* Create a BIO object from the private key string */
    BIO* bio = BIO_new_mem_buf(priv_key_str.data(), priv_key_str.size());

    /* Read the private key from the BIO object */
    RSA* rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);

    /* Get the size of the RSA key */
    int keysize = RSA_size(rsa);

    /* Create a unique_ptr to manage the memory of the decryption output buffer */
    std::unique_ptr<unsigned char[]> rsa_out(new unsigned char[keysize]);

    /* Initialize the decryption output buffer to 0 */
    memset(rsa_out.get(), 0, keysize);

    /* Decrypt the message using the private key */
    int rsa_outlen = RSA_private_decrypt(
        encrypted_message.size(), encrypted_message.data(), rsa_out.get(),
        rsa, RSA_PKCS1_PADDING);

    /* Create a vector to store the decrypted message */
    std::vector<unsigned char> decrypted_message(rsa_out.get(), rsa_out.get() + rsa_outlen);

    /* Free the RSA and BIO objects */
    RSA_free(rsa);
    BIO_free(bio);

    /* Return the decrypted message */
    return decrypted_message;
}