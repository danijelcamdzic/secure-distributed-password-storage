/**
 * @file RSA_Decrypt.c
 * @brief Contains implementation of RSA decrypt algorithm
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

#include "RSA/RSA.h"

/* Private RSA key */
const unsigned char *private_key = (const unsigned char *)"-----BEGIN PRIVATE KEY-----\n"
                                                        "MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQC240GJf9WuE/vW\n"
                                                        "z7JMinulBsOQZ8JBgmxrZPDRDg/8JpeINU5r58p/0rEfrQv4xTbiMa4/zeAoDv0M\n"
                                                        "X4eLT3yAuoKjnPxPPiWI1XgjvzNsJQ330RlIWSvSVIXb/yRFOh4GMhDxE7FlDciQ\n"
                                                        "1tCov4Q4aDAPf2mt84UEeIg2G1g/TUe7Pyp4a01RepzaWjvS9DGbp9EX7P7orYJd\n"
                                                        "d/XHpzw8wu+sTlp1EzfPf5kPAu5DeJgZYh1x1zTzVezpQzMaA1/WyzybpknPESVi\n"
                                                        "rJewIjp5TflWvn7tAiY5gNecmL7UiIQeNoGITKkmHd0mTubWgYvNHcUDaFW6OhNc\n"
                                                        "xayqOZanAgMBAAECggEAGLcm2NwhFmtg8gC+NGsqlny5hRRgmNXKrSDGYponPYjs\n"
                                                        "RYi0AQ/MD/DQkFqkbt7IB1LLckKc87eYKZ88rsOegbmy2fA8hhZR9ZH+WxEFyVb1\n"
                                                        "8khHGxM3QYS7/6UVU72s57gbDgP+nzd4+V9Dx5T022KedsRd6xd+dxeK4N+v0gNM\n"
                                                        "aqaSyzCxWwHkUN1g8nZgIqe/UM0N/cpGsiF4tUZSb+m8oUtXVz4Phz6ismaQv0yH\n"
                                                        "B/nRJkpjdI7VAeSnQOoBQy1EvO37cs5gk2+T+FaHM3qQlXGYzPoyG1+mLs/Zarpd\n"
                                                        "vgNNWEfRhVkEpysI7dqbbv56bnEKn5cVH3m6j5YeAQKBgQDf17VAdL+YTnkI7GQu\n"
                                                        "uK1jz/+ESCcdlSFVIFryBuCEPra3E57J6vkzwg41Us6tW3qOsikqgLiGZAUIDReH\n"
                                                        "PgXQqvoVpygCSpux8LTMVvrAwOi/qO/hIPVGIg5wx6L5Ot58rqnAI1j879O0apRA\n"
                                                        "cCJT1eWFRAefUnMxNxnfZi3EJwKBgQDRKVgUPc2u7l7h/TZhH7EUQT7WRFNbFmCX\n"
                                                        "8YNLR8ojR7YbkMRH1jJBGafAZKnJJeOH+OqkbvIx8+9S8m/kWg5WlHm6MLXD/0VG\n"
                                                        "YN8C3fx1jsxuzkCXK8EOQV2pLxWPfjrsvGz1oLF8ixSqfbpM4o9u4IW6/X/v/BYP\n"
                                                        "1tBCY3SpgQKBgQDRTYHNo10DrVy/W0rR5R08F2F074Kjxq5ESty/Mm79OSbgSRhQ\n"
                                                        "9cXO/8UJV0SqFL+kbjLP60yfXSbXQGafrv8A/t6ZIJPokJrE564m463UTo1TqUOj\n"
                                                        "3o155p5aFlHteX5QV4Gih2rOF/J84Kt/FVsDyR20XshB0XakrkEjAwgIQQKBgQCk\n"
                                                        "ZB9TAWRIihdO/UohOdxetA0kMN1m6QaQRYTQxSLNac5qifSLFYG4AucC4ttDOiFD\n"
                                                        "vFMHXB/FPRkrk1p4GoPMbrPhr+kcm+ShtP9ybHDBILPynu7LIduBTcTEdTo35pl/\n"
                                                        "eXUV9O+qmFFs0pZ7WVIsbhENb7J/Nx1L46UZmLuXgQKBgQDfMuU4K1jb8PCno5Uk\n"
                                                        "Dsrpp60GPyflskervoSqENy5yWNY7D2VEdFHRkz1BIRyTBXmsccrjPt+Co73aea2\n"
                                                        "n4CPCoQir7jkVTeKnZB8mZyZKOBYPGYeo46Bx4yTRvTZ9SuGmdtmvaTC6z1475Wx\n"
                                                        "zOnlXq5SzkJViJXh2CtNAthK4Q==\n"
                                                        "-----END PRIVATE KEY-----\n";

size_t RSA_Decrypt(const char* text, size_t length, const unsigned char* rsa_private_key)
{
    // RNG (Random number generator init)
    int ret = 0;
    mbedtls_entropy_context entropy;
    mbedtls_entropy_init(&entropy);             // Initialize the entropy context
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *personalization = "wefoierncniuerhfiowenjfkwebufu";

    mbedtls_ctr_drbg_init(&ctr_drbg);           // Initialize the CTR-DRBG context

    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const unsigned char *)personalization,
                                strlen(personalization));   // Seed the random number generator
    if (ret != 0)
    {
       RSA_Print("Error occured during the function mbedtls_ctr_drbg_seed");

        return 0;
    }

    /* Creating rsa context + Importing pub key */
    ret = 0;
    mbedtls_pk_context pk;
    mbedtls_pk_init(&pk);                       // Initialize the private key context

    /*
     * Read the RSA private key
     */
    if ((ret = mbedtls_pk_parse_key(&pk, rsa_private_key, strlen((const char *)rsa_private_key) + 1, NULL, 0, mbedtls_ctr_drbg_random, &ctr_drbg)) != 0)
    {
       RSA_Print("Failed! mbedtls_pk_parse_public_key returned an error");
    }

    unsigned char result[MBEDTLS_MPI_MAX_SIZE];
    size_t olen = 0;

    /*
     * Calculate the RSA decryption of the data.
     */
    RSA_Print("Generating the decrypted value: \n");
    fflush(stdout);

    const unsigned char *to_decrypt = (unsigned char *)text;
    if ((ret = mbedtls_pk_decrypt(&pk, to_decrypt, length, result, &olen, sizeof(result),
                                  mbedtls_ctr_drbg_random, &ctr_drbg)) != 0) // Perform RSA decryption
    {
        RSA_Print("Failed! mbedtls_pk_decrypt returned an error");
    }

    /* Print the result of the decrypted string */
    if (olen < sizeof(result)) {
        result[olen] = '\0';
    } else {
       RSA_Print("Decrypted data is too large for the result buffer");
    }

    RSA_Print("Decrypted string:");
    RSA_Print((char *)result);

    /* Free space */
    mbedtls_pk_free(&pk);                       // Free the public key context
    mbedtls_entropy_free(&entropy);             // Free the entropy context
    mbedtls_ctr_drbg_free(&ctr_drbg);           // Free the CTR-DRBG context

    memcpy(RSA_ENCRYPTED_BUFFER, result, olen);

    return olen;
}

