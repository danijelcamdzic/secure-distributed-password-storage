/**
 * @file RSA_Encrypt.c
 * @brief Contains implementation of RSA encrypt algorithm
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#include "RSA/RSA.h"

/* Public RSA key */
const unsigned char *key = (const unsigned char *)"-----BEGIN PUBLIC KEY-----\n"
                                                "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAtuNBiX/VrhP71s+yTIp7\n"
                                                "pQbDkGfCQYJsa2Tw0Q4P/CaXiDVOa+fKf9KxH60L+MU24jGuP83gKA79DF+Hi098\n"
                                                "gLqCo5z8Tz4liNV4I78zbCUN99EZSFkr0lSF2/8kRToeBjIQ8ROxZQ3IkNbQqL+E\n"
                                                "OGgwD39prfOFBHiINhtYP01Huz8qeGtNUXqc2lo70vQxm6fRF+z+6K2CXXf1x6c8\n"
                                                "PMLvrE5adRM3z3+ZDwLuQ3iYGWIdcdc081Xs6UMzGgNf1ss8m6ZJzxElYqyXsCI6\n"
                                                "eU35Vr5+7QImOYDXnJi+1IiEHjaBiEypJh3dJk7m1oGLzR3FA2hVujoTXMWsqjmW\n"
                                                "pwIDAQAB\n"
                                                "-----END PUBLIC KEY-----\n";

size_t RSA_Encrypt(const char* text)
{
    char* TAG = "RSA_Encrypt";                  // Declare and initialize TAG for logging purposes

    /* RNG (Random number generator init) */
    int ret = 0;
    mbedtls_entropy_context entropy;
    mbedtls_entropy_init(&entropy);             // Initialize the entropy context
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *personalization = "ewkjfhiuercuieabkuaeuwrhukf";

    mbedtls_ctr_drbg_init(&ctr_drbg);           // Initialize the CTR-DRBG context

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const unsigned char *)personalization,
                                strlen(personalization));   // Seed the random number generator
    if (ret != 0)
    {
        ESP_LOGI(TAG, "Error occured during the function mbedtls_ctr_drbg_seed");

        return 0;                               // Return 0 if seeding failed
    }

    /* Creating rsa context + Importing pub key */
    ret = 0;
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);                       // Initialize the public key context

    /*
     * Read the RSA public key
     */
    if ((ret = mbedtls_pk_parse_public_key(&pk, key, strlen((const char *)key) + 1)) != 0)
    {
        ESP_LOGI(TAG," Failed! mbedtls_pk_parse_public_key returned: -0x%04x\n", -ret);
    };

    /* Encrypting data */
    const unsigned char *to_encrypt = (const unsigned char *)text;
    size_t to_encrypt_len = strlen((const char *)to_encrypt); // Get the length of the text to encrypt

    /* unsigned char buf[MBEDTLS_MPI_MAX_SIZE]; */
    size_t olen = 0;                            // Initialize the output length to 0

    /*
     * Calculate the RSA encryption of the data.
     */
    ESP_LOGI(TAG,"Generating the encrypted value: \n");
    fflush(stdout);

    if ((ret = mbedtls_pk_encrypt(&pk, to_encrypt, to_encrypt_len,
                                  buf, &olen, sizeof(buf),
                                  mbedtls_ctr_drbg_random, &ctr_drbg)) != 0) // Perform RSA encryption
    {
        ESP_LOGI(TAG, "Failed! mbedtls_pk_encrypt returned: -0x%04x\n", -ret);
    }

    /* Print the encrypted value in readable form */
    for (size_t i = 0; i < olen; i++)
    {
        mbedtls_printf("%02X%s", buf[i],
                       (i + 1) % 16 == 0 ? "\r\n" : " "); // Print the encrypted data in hexadecimal format
    }

    /* Free space */
    mbedtls_pk_free(&pk);                       // Free the public key context
    mbedtls_entropy_free(&entropy);             // Free the entropy context
    mbedtls_ctr_drbg_free(&ctr_drbg);           // Free the CTR-DRBG context

    /* Return the length of encrypted data */
    return olen;                                
}

