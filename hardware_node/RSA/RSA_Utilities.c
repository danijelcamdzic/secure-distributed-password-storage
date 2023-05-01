/**
 * @file RSA_Utilities.c
 * @brief Contains implementation of RSA utility functions
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

/* RSA library */
#include "RSA/RSA.h"

/* ------------------------- VARIABLE DEFINITIONS ------------------------------------ */

/* Print function handle */
RSA_PrintPtr RSA_Print = NULL;

/* ------------------------- FUNCTION DEFINITIONS ------------------------------------ */

/**
 * @brief Sets the RSA Print function.
 * @param print Function pointer to the user-defined RSA Print function.
 */
void RSA_SetPrint(RSA_PrintPtr print) 
{
    RSA_Print = print;
}

