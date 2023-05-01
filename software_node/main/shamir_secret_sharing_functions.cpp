/**
 * @file shamir_secret_sharing_functions_functions.cpp
 * @brief Contains necessary defines, variables definitions
 * and function definitions for using the shamir secret sharing library
 *
 * @author Danijel Camdzic
 * @date 1 May 2023
 */

#include "shamir_secret_sharing_functions.h"

/* ------------------------- FUNCTION DEFINITIONS ------------------------------------ */

/**
 * @brief Split a given password into shares using Shamir's Secret Sharing library
 * @param[in] password The password to be split into shares
 * @param[out] shares The vector to store the generated shares
 */
void sss_split_password_into_shares(const std::string& password, std::vector<sss_Share>& shares) 
{
    /* Ensure the shares vector is the correct size */
    if (shares.size() != SHAMIR_NUM_SHARES) {
        throw std::runtime_error("Shares vector size must be equal to SHAMIR_NUM_SHARES.");
    }

    /* Check if length of password exceeds the max length */
    if (password.size() > sss_MLEN) {
        throw std::runtime_error("Password length must not exceed sss_MLEN bytes");
    }

    /* Convert password to a fixed-length array */
    uint8_t password_data[sss_MLEN];
    memset(password_data, 0, sizeof(password_data));
    memcpy(password_data, password.c_str(), password.size());

    /* Split the password into shares */
    sss_create_shares(shares.data(), password_data, SHAMIR_NUM_SHARES, SHAMIR_THRESHOLD);

    /* The shares of the password are now located in the std::vector<sss_Share>& shares */
}

/**
 * @brief Recombine the password from the given shares using Shamir's Secret Sharing algorithm
 * @param[in] shares The vector containing the shares for recombination
 * @return The recombined password
 */
std::string sss_recombine_password_from_shares(const std::vector<sss_Share>& shares)
{
    /* Ensure the shares vector is the correct size */
    if (shares.size() < SHAMIR_THRESHOLD) {
        throw std::runtime_error("Shares vector size must be greater than or equal to SHAMIR_THRESHOLD.");
    }

    /* Prepare the restored data array */
    uint8_t restored_data[sss_MLEN];
    memset(restored_data, 0, sizeof(restored_data));

    /* Combine the shares to restore the original data */
    int tmp = sss_combine_shares(restored_data, shares.data(), SHAMIR_THRESHOLD);

    if (tmp != 0) {
        throw std::runtime_error("Failed to recombine shares.");
    }

    /* Convert the restored data back to a string */
    std::string restored_password(reinterpret_cast<char*>(restored_data));

    return restored_password;
}

