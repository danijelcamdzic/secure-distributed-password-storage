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
        return {};
    }

    std::string pub_key_str((std::istreambuf_iterator<char>(pub_key_file)), std::istreambuf_iterator<char>());
    BIO* bio = BIO_new_mem_buf(pub_key_str.data(), pub_key_str.size());
    RSA* rsa_pubkey = RSA_new();

    if (!PEM_read_bio_RSA_PUBKEY(bio, &rsa_pubkey, nullptr, nullptr))
    {
        std::cerr << "Error reading public key from BIO" << std::endl;
        BIO_free(bio);
        return {};
    }

    int rsa_size = RSA_size(rsa_pubkey);
    std::vector<unsigned char> encrypted_message(rsa_size);

    int encrypt_len = RSA_public_encrypt(message.size(), reinterpret_cast<const unsigned char*>(message.data()), encrypted_message.data(), rsa_pubkey, RSA_PKCS1_PADDING);
    if (encrypt_len == -1)
    {
        std::cerr << "Error encrypting message: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        RSA_free(rsa_pubkey);
        BIO_free(bio);
        return {};
    }

    RSA_free(rsa_pubkey);
    BIO_free(bio);

    encrypted_message.resize(encrypt_len);

    std::string encrypted_message_str(encrypted_message.begin(), encrypted_message.end());

    return encrypted_message_str;
}

std::vector<unsigned char> rsa_decrypt_message(const std::vector<unsigned char>& encrypted_message, const std::string& private_key_filename)
{
    FILE *fp = fopen(private_key_filename.c_str(), "r");

    RSA *rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);

    fclose(fp);

    int keysize = RSA_size(rsa);

    std::unique_ptr<unsigned char[]> rsa_out(new unsigned char[keysize]);

    memset(rsa_out.get(), 0, keysize);

    int rsa_outlen = RSA_private_decrypt(
            encrypted_message.size(), encrypted_message.data(), rsa_out.get(),
            rsa, RSA_PKCS1_PADDING);

    std::vector<unsigned char> decrypted_message(rsa_out.get(), rsa_out.get() + rsa_outlen);

    RSA_free(rsa);

    return decrypted_message;
}



