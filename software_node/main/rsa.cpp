#include "rsa.h"

const std::string RSA_PUBLIC_KEY("../rsa_public_key.pem");
const std::string RSA_PRIVATE_KEY("../rsa_private_key.pem");
const std::string RSA_PUBLIC_KEY_HW_NODE_1("../rsa_public_key_hw_node_1.pem");
const std::vector<std::string> public_keys_hw_nodes = {RSA_PUBLIC_KEY_HW_NODE_1};

std::string rsa_encrypt_message(const std::string& message, const std::string& public_key_filename)
{
    std::ifstream pub_key_file(public_key_filename);
    if (!pub_key_file)
    {
        std::cerr << "Error opening public key file: " << public_key_filename << std::endl;
        return "";
    }

    std::string pub_key_str((std::istreambuf_iterator<char>(pub_key_file)), std::istreambuf_iterator<char>());
    BIO* bio = BIO_new_mem_buf(pub_key_str.data(), pub_key_str.size());
    RSA* rsa_pubkey = RSA_new();

    if (!PEM_read_bio_RSA_PUBKEY(bio, &rsa_pubkey, nullptr, nullptr))
    {
        std::cerr << "Error reading public key from BIO" << std::endl;
        BIO_free(bio);
        return "";
    }

    int rsa_size = RSA_size(rsa_pubkey);
    std::vector<unsigned char> encrypted_message(rsa_size);

    int encrypt_len = RSA_public_encrypt(message.size(), reinterpret_cast<const unsigned char*>(message.data()), encrypted_message.data(), rsa_pubkey, RSA_PKCS1_PADDING);
    if (encrypt_len == -1)
    {
        std::cerr << "Error encrypting message: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        RSA_free(rsa_pubkey);
        BIO_free(bio);
        return "";
    }

    RSA_free(rsa_pubkey);
    BIO_free(bio);

    return std::string(reinterpret_cast<char*>(encrypted_message.data()), encrypt_len);
}

std::string rsa_decrypt_message(const std::string& encrypted_message, const std::string& private_key_filename)
{
    std::ifstream priv_key_file(private_key_filename);
    if (!priv_key_file)
    {
        std::cerr << "Error opening private key file: " << private_key_filename << std::endl;
        return "";
    }

    std::string priv_key_str((std::istreambuf_iterator<char>(priv_key_file)), std::istreambuf_iterator<char>());
    BIO* bio = BIO_new_mem_buf(priv_key_str.data(), priv_key_str.size());
    RSA* rsa_privkey = RSA_new();

    if (!PEM_read_bio_RSAPrivateKey(bio, &rsa_privkey, nullptr, nullptr))
    {
        std::cerr << "Error reading private key from BIO" << std::endl;
        BIO_free(bio);
        return "";
    }

    int rsa_size = RSA_size(rsa_privkey);
    std::vector<unsigned char> decrypted_message(rsa_size);

    int decrypt_len = RSA_private_decrypt(encrypted_message.size(), reinterpret_cast<const unsigned char*>(encrypted_message.data()), decrypted_message.data(), rsa_privkey, RSA_PKCS1_PADDING);
    if (decrypt_len == -1)
    {
        std::cerr << "Error decrypting message: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        RSA_free(rsa_privkey);
        BIO_free(bio);
        return "";
    }

    RSA_free(rsa_privkey);
    BIO_free(bio);
    
    return std::string(reinterpret_cast<char*>(decrypted_message.data()), decrypt_len);
}



