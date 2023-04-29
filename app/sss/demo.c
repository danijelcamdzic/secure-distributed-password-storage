#include "sss.h"
#include "randombytes.h"
#include <assert.h>
#include <string.h>

int main()
{
	uint8_t data[sss_MLEN], restored[sss_MLEN];
	sss_Share shares[5];
	size_t idx;
	int tmp;

	// Read a message to be shared
	strncpy(data, "Tyler Durden isn't real.", sizeof(data));

	// Split the secret into 5 shares (with a recombination theshold of 4)
	sss_create_shares(shares, data, 5, 4);

	// Combine some of the shares to restore the original secret
	tmp = sss_combine_shares(restored, shares, 4);
	assert(tmp == 0);
	assert(memcmp(restored, data, sss_MLEN) == 0);
}