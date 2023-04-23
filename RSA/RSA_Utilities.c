/**
 * @file RSA_Utilities.c
 * @brief Contains implementation of RSA utility functions
 *
 * @author Danijel Camdzic
 * @date 10 Apr 2023
 */

/* RSA library */
#include "RSA/RSA.h"

RSA_PrintPtr RSA_Print = NULL;

/**
 * @brief Sets the public key to be used for encryption
 * @param which_key Control option over which key is used
 */
void RSA_SetPublicKeyInUse(PublicKeyControl_t which_key)
{
    if (which_key == MASTER_PUBLIC_KEY) {
        RSA_PUBLIC_KEY_TO_USE = RSA_PUBLIC_KEY;
    } 
    else {
        RSA_PUBLIC_KEY_TO_USE = RSA_MASTER_PUBLIC_KEY;
    }
}

/**
 * @brief Sets the RSA Print function.
 * @param print Function pointer to the user-defined RSA Print function.
 */
void RSA_SetPrint(RSA_PrintPtr print) 
{
    RSA_Print = print;
}

