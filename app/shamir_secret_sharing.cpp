#include "shamir_secret_sharing.h"

void shamir_secret_sharing_test(void) 
{
    uint8_t data[sss_MLEN], restored[sss_MLEN];
    sss_Share shares[5];
    size_t idx;
    int tmp;

    // Read a message to be shared
    strncpy(reinterpret_cast<char*>(data), "Tyler Durden isn't real.", sizeof(data));

    // Split the secret into 5 shares (with a recombination theshold of 4)
    sss_create_shares(shares, data, 5, 4);

    // Combine some of the shares to restore the original secret
    tmp = sss_combine_shares(restored, shares, 5);

    /* Print the messages */
    std::cout << "Original message" << std::endl;
    std::cout << data << std::endl;
    std::cout << "Restored message:" << std::endl;
    std::cout << restored << std::endl;
}