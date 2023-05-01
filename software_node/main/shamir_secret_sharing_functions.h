/**
 * @file shamir_secret_sharing_functions.h
 * @brief Contains necessary defines, variables declarations
 * and function protypes for using the shamir secret sharing library
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#ifndef SHAMIR_SECRET_SHARING_FUNCTIONS_H
#define SHAMIR_SECRET_SHARING_FUNCTIONS_H

#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <string.h>

/* Shamir secret sharing library includes (from the "sss" folder )*/
#include "sss.h"
#include "randombytes.h"

/* Number of shares and threshold defines */
#define SHAMIR_NUM_SHARES   1                   /**< Should be equal to the number of hardware nodes that these shares are being sent to */     
#define SHAMIR_THRESHOLD    1                   /**< Must be less than or equal to SHAMIR_NUM_SHARES and represents the reconstruction threshold */

/* Function prototypes */
void sss_split_password_into_shares(const std::string& password, std::vector<sss_Share>& shares);
std::string sss_recombine_password_from_shares(const std::vector<sss_Share>& shares);

#endif /* SHAMIR_SECRET_SHARING_FUNCTIONS_H */