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
 * @brief Sets the RSA Print function.
 * @param print Function pointer to the user-defined RSA Print function.
 */
void RSA_SetPrint(RSA_PrintPtr print) 
{
    RSA_Print = print;
}

