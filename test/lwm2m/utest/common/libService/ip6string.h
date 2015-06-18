/*
 * Copyright (c) 2014 ARM. All rights reserved.
 */
#ifndef IP6STRING_H
#define IP6STRING_H
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Print binary IPv6 address to a string.
 * String must contain enough room for full address, 40 bytes exact.
 * IPv4 tunneling addresses are not covered.
 * \param ip6addr IPv6 address.
 * \p buffer to write string to.
 */
void ip6tos(const void *ip6addr, char *p);

/**
 * Convert numeric IPv6 address string to a binary.
 * IPv4 tunneling addresses are not covered.
 * \param ip6addr IPv6 address in string format.
 * \param len Lenght of ipv6 string, maximum of 41.
 * \param dest buffer for address. MUST be 16 bytes.
 */
void stoip6(const char *ip6addr, size_t len, void *dest);
/**
 * Find out numeric IPv6 address prefix length
 * \param ip6addr  IPv6 address in string format
 * \return prefix length or 0 if it not given
 */
unsigned char sipv6_prefixlength(const char *ip6addr);

#ifdef __cplusplus
}
#endif
#endif
