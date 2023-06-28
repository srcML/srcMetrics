/**
 * @file bliterals.h
 * @brief Defines binary literals for C.
 *
 * All n-bit binary literals up to n=8 have unique macro definition.
 * Use function-like macros for n=16, n=32, and n=64.
 * These declarations may deprecate when C23 defines built-in binary literals.
 *
 * @author Yavuz Koroglu
 */
#ifndef BLITERALS_H
    #define BLITERALS_H /* Binary Literals (may become deprecated after C23) */
    #include <stdint.h>

    /**
     * @def B_0
     *   1'b0
     */
    #define B_0 0x0U
    /**
     * @def B_1
     *   1'b1
     */
    #define B_1 0x1U

    /**
     * @def B_00
     *   2'b00
     */
    #define B_00 0x0U
    /**
     * @def B_01
     *   2'b01
     */
    #define B_01 0x1U
    /**
     * @def B_10
     *   2'b10
     */
    #define B_10 0x2U
    /**
     * @def B_11
     *   2'b11
     */
    #define B_11 0x3U

    /**
     * @def B_000
     *   3'b000
     */
    #define B_000 0x0U
    /**
     * @def B_001
     *   3'b001
     */
    #define B_001 0x1U
    /**
     * @def B_010
     *   3'b010
     */
    #define B_010 0x2U
    /**
     * @def B_011
     *   3'b011
     */
    #define B_011 0x3U
    /**
     * @def B_100
     *   3'b100
     */
    #define B_100 0x4U
    /**
     * @def B_101
     *   3'b101
     */
    #define B_101 0x5U
    /**
     * @def B_110
     *   3'b110
     */
    #define B_110 0x6U
    /**
     * @def B_111
     *   3'b111
     */
    #define B_111 0x7U

    /**
     * @def B_0000
     *   4'b0000
     */
    #define B_0000 0x0U
    #define B_0001 0x1U
    #define B_0010 0x2U
    #define B_0011 0x3U
    #define B_0100 0x4U
    #define B_0101 0x5U
    #define B_0110 0x6U
    #define B_0111 0x7U
    #define B_1000 0x8U
    #define B_1001 0x9U
    #define B_1010 0xAU
    #define B_1011 0xBU
    #define B_1100 0xCU
    #define B_1101 0xDU
    #define B_1110 0xEU
    #define B_1111 0xFU

    #define B_00000 0x00U
    #define B_00001 0x01U
    #define B_00010 0x02U
    #define B_00011 0x03U
    #define B_00100 0x04U
    #define B_00101 0x05U
    #define B_00110 0x06U
    #define B_00111 0x07U
    #define B_01000 0x08U
    #define B_01001 0x09U
    #define B_01010 0x0AU
    #define B_01011 0x0BU
    #define B_01100 0x0CU
    #define B_01101 0x0DU
    #define B_01110 0x0EU
    #define B_01111 0x0FU
    #define B_10000 0x10U
    #define B_10001 0x11U
    #define B_10010 0x12U
    #define B_10011 0x13U
    #define B_10100 0x14U
    #define B_10101 0x15U
    #define B_10110 0x16U
    #define B_10111 0x17U
    #define B_11000 0x18U
    #define B_11001 0x19U
    #define B_11010 0x1AU
    #define B_11011 0x1BU
    #define B_11100 0x1CU
    #define B_11101 0x1DU
    #define B_11110 0x1EU
    #define B_11111 0x1FU

    #define B_000000 0x00U
    #define B_000001 0x01U
    #define B_000010 0x02U
    #define B_000011 0x03U
    #define B_000100 0x04U
    #define B_000101 0x05U
    #define B_000110 0x06U
    #define B_000111 0x07U
    #define B_001000 0x08U
    #define B_001001 0x09U
    #define B_001010 0x0AU
    #define B_001011 0x0BU
    #define B_001100 0x0CU
    #define B_001101 0x0DU
    #define B_001110 0x0EU
    #define B_001111 0x0FU
    #define B_010000 0x10U
    #define B_010001 0x11U
    #define B_010010 0x12U
    #define B_010011 0x13U
    #define B_010100 0x14U
    #define B_010101 0x15U
    #define B_010110 0x16U
    #define B_010111 0x17U
    #define B_011000 0x18U
    #define B_011001 0x19U
    #define B_011010 0x1AU
    #define B_011011 0x1BU
    #define B_011100 0x1CU
    #define B_011101 0x1DU
    #define B_011110 0x1EU
    #define B_011111 0x1FU
    #define B_100000 0x20U
    #define B_100001 0x21U
    #define B_100010 0x22U
    #define B_100011 0x23U
    #define B_100100 0x24U
    #define B_100101 0x25U
    #define B_100110 0x26U
    #define B_100111 0x27U
    #define B_101000 0x28U
    #define B_101001 0x29U
    #define B_101010 0x2AU
    #define B_101011 0x2BU
    #define B_101100 0x2CU
    #define B_101101 0x2DU
    #define B_101110 0x2EU
    #define B_101111 0x2FU
    #define B_110000 0x30U
    #define B_110001 0x31U
    #define B_110010 0x32U
    #define B_110011 0x33U
    #define B_110100 0x34U
    #define B_110101 0x35U
    #define B_110110 0x36U
    #define B_110111 0x37U
    #define B_111000 0x38U
    #define B_111001 0x39U
    #define B_111010 0x3AU
    #define B_111011 0x3BU
    #define B_111100 0x3CU
    #define B_111101 0x3DU
    #define B_111110 0x3EU
    #define B_111111 0x3FU

    #define B_0000000 0x00U
    #define B_0000001 0x01U
    #define B_0000010 0x02U
    #define B_0000011 0x03U
    #define B_0000100 0x04U
    #define B_0000101 0x05U
    #define B_0000110 0x06U
    #define B_0000111 0x07U
    #define B_0001000 0x08U
    #define B_0001001 0x09U
    #define B_0001010 0x0AU
    #define B_0001011 0x0BU
    #define B_0001100 0x0CU
    #define B_0001101 0x0DU
    #define B_0001110 0x0EU
    #define B_0001111 0x0FU
    #define B_0010000 0x10U
    #define B_0010001 0x11U
    #define B_0010010 0x12U
    #define B_0010011 0x13U
    #define B_0010100 0x14U
    #define B_0010101 0x15U
    #define B_0010110 0x16U
    #define B_0010111 0x17U
    #define B_0011000 0x18U
    #define B_0011001 0x19U
    #define B_0011010 0x1AU
    #define B_0011011 0x1BU
    #define B_0011100 0x1CU
    #define B_0011101 0x1DU
    #define B_0011110 0x1EU
    #define B_0011111 0x1FU
    #define B_0100000 0x20U
    #define B_0100001 0x21U
    #define B_0100010 0x22U
    #define B_0100011 0x23U
    #define B_0100100 0x24U
    #define B_0100101 0x25U
    #define B_0100110 0x26U
    #define B_0100111 0x27U
    #define B_0101000 0x28U
    #define B_0101001 0x29U
    #define B_0101010 0x2AU
    #define B_0101011 0x2BU
    #define B_0101100 0x2CU
    #define B_0101101 0x2DU
    #define B_0101110 0x2EU
    #define B_0101111 0x2FU
    #define B_0110000 0x30U
    #define B_0110001 0x31U
    #define B_0110010 0x32U
    #define B_0110011 0x33U
    #define B_0110100 0x34U
    #define B_0110101 0x35U
    #define B_0110110 0x36U
    #define B_0110111 0x37U
    #define B_0111000 0x38U
    #define B_0111001 0x39U
    #define B_0111010 0x3AU
    #define B_0111011 0x3BU
    #define B_0111100 0x3CU
    #define B_0111101 0x3DU
    #define B_0111110 0x3EU
    #define B_0111111 0x3FU
    #define B_1000000 0x40U
    #define B_1000001 0x41U
    #define B_1000010 0x42U
    #define B_1000011 0x43U
    #define B_1000100 0x44U
    #define B_1000101 0x45U
    #define B_1000110 0x46U
    #define B_1000111 0x47U
    #define B_1001000 0x48U
    #define B_1001001 0x49U
    #define B_1001010 0x4AU
    #define B_1001011 0x4BU
    #define B_1001100 0x4CU
    #define B_1001101 0x4DU
    #define B_1001110 0x4EU
    #define B_1001111 0x4FU
    #define B_1010000 0x50U
    #define B_1010001 0x51U
    #define B_1010010 0x52U
    #define B_1010011 0x53U
    #define B_1010100 0x54U
    #define B_1010101 0x55U
    #define B_1010110 0x56U
    #define B_1010111 0x57U
    #define B_1011000 0x58U
    #define B_1011001 0x59U
    #define B_1011010 0x5AU
    #define B_1011011 0x5BU
    #define B_1011100 0x5CU
    #define B_1011101 0x5DU
    #define B_1011110 0x5EU
    #define B_1011111 0x5FU
    #define B_1100000 0x60U
    #define B_1100001 0x61U
    #define B_1100010 0x62U
    #define B_1100011 0x63U
    #define B_1100100 0x64U
    #define B_1100101 0x65U
    #define B_1100110 0x66U
    #define B_1100111 0x67U
    #define B_1101000 0x68U
    #define B_1101001 0x69U
    #define B_1101010 0x6AU
    #define B_1101011 0x6BU
    #define B_1101100 0x6CU
    #define B_1101101 0x6DU
    #define B_1101110 0x6EU
    #define B_1101111 0x6FU
    #define B_1110000 0x70U
    #define B_1110001 0x71U
    #define B_1110010 0x72U
    #define B_1110011 0x73U
    #define B_1110100 0x74U
    #define B_1110101 0x75U
    #define B_1110110 0x76U
    #define B_1110111 0x77U
    #define B_1111000 0x78U
    #define B_1111001 0x79U
    #define B_1111010 0x7AU
    #define B_1111011 0x7BU
    #define B_1111100 0x7CU
    #define B_1111101 0x7DU
    #define B_1111110 0x7EU
    #define B_1111111 0x7FU

    #define B_00000000 0x00U
    #define B_00000001 0x01U
    #define B_00000010 0x02U
    #define B_00000011 0x03U
    #define B_00000100 0x04U
    #define B_00000101 0x05U
    #define B_00000110 0x06U
    #define B_00000111 0x07U
    #define B_00001000 0x08U
    #define B_00001001 0x09U
    #define B_00001010 0x0AU
    #define B_00001011 0x0BU
    #define B_00001100 0x0CU
    #define B_00001101 0x0DU
    #define B_00001110 0x0EU
    #define B_00001111 0x0FU
    #define B_00010000 0x10U
    #define B_00010001 0x11U
    #define B_00010010 0x12U
    #define B_00010011 0x13U
    #define B_00010100 0x14U
    #define B_00010101 0x15U
    #define B_00010110 0x16U
    #define B_00010111 0x17U
    #define B_00011000 0x18U
    #define B_00011001 0x19U
    #define B_00011010 0x1AU
    #define B_00011011 0x1BU
    #define B_00011100 0x1CU
    #define B_00011101 0x1DU
    #define B_00011110 0x1EU
    #define B_00011111 0x1FU
    #define B_00100000 0x20U
    #define B_00100001 0x21U
    #define B_00100010 0x22U
    #define B_00100011 0x23U
    #define B_00100100 0x24U
    #define B_00100101 0x25U
    #define B_00100110 0x26U
    #define B_00100111 0x27U
    #define B_00101000 0x28U
    #define B_00101001 0x29U
    #define B_00101010 0x2AU
    #define B_00101011 0x2BU
    #define B_00101100 0x2CU
    #define B_00101101 0x2DU
    #define B_00101110 0x2EU
    #define B_00101111 0x2FU
    #define B_00110000 0x30U
    #define B_00110001 0x31U
    #define B_00110010 0x32U
    #define B_00110011 0x33U
    #define B_00110100 0x34U
    #define B_00110101 0x35U
    #define B_00110110 0x36U
    #define B_00110111 0x37U
    #define B_00111000 0x38U
    #define B_00111001 0x39U
    #define B_00111010 0x3AU
    #define B_00111011 0x3BU
    #define B_00111100 0x3CU
    #define B_00111101 0x3DU
    #define B_00111110 0x3EU
    #define B_00111111 0x3FU
    #define B_01000000 0x40U
    #define B_01000001 0x41U
    #define B_01000010 0x42U
    #define B_01000011 0x43U
    #define B_01000100 0x44U
    #define B_01000101 0x45U
    #define B_01000110 0x46U
    #define B_01000111 0x47U
    #define B_01001000 0x48U
    #define B_01001001 0x49U
    #define B_01001010 0x4AU
    #define B_01001011 0x4BU
    #define B_01001100 0x4CU
    #define B_01001101 0x4DU
    #define B_01001110 0x4EU
    #define B_01001111 0x4FU
    #define B_01010000 0x50U
    #define B_01010001 0x51U
    #define B_01010010 0x52U
    #define B_01010011 0x53U
    #define B_01010100 0x54U
    #define B_01010101 0x55U
    #define B_01010110 0x56U
    #define B_01010111 0x57U
    #define B_01011000 0x58U
    #define B_01011001 0x59U
    #define B_01011010 0x5AU
    #define B_01011011 0x5BU
    #define B_01011100 0x5CU
    #define B_01011101 0x5DU
    #define B_01011110 0x5EU
    #define B_01011111 0x5FU
    #define B_01100000 0x60U
    #define B_01100001 0x61U
    #define B_01100010 0x62U
    #define B_01100011 0x63U
    #define B_01100100 0x64U
    #define B_01100101 0x65U
    #define B_01100110 0x66U
    #define B_01100111 0x67U
    #define B_01101000 0x68U
    #define B_01101001 0x69U
    #define B_01101010 0x6AU
    #define B_01101011 0x6BU
    #define B_01101100 0x6CU
    #define B_01101101 0x6DU
    #define B_01101110 0x6EU
    #define B_01101111 0x6FU
    #define B_01110000 0x70U
    #define B_01110001 0x71U
    #define B_01110010 0x72U
    #define B_01110011 0x73U
    #define B_01110100 0x74U
    #define B_01110101 0x75U
    #define B_01110110 0x76U
    #define B_01110111 0x77U
    #define B_01111000 0x78U
    #define B_01111001 0x79U
    #define B_01111010 0x7AU
    #define B_01111011 0x7BU
    #define B_01111100 0x7CU
    #define B_01111101 0x7DU
    #define B_01111110 0x7EU
    #define B_01111111 0x7FU
    #define B_10000000 0x80U
    #define B_10000001 0x81U
    #define B_10000010 0x82U
    #define B_10000011 0x83U
    #define B_10000100 0x84U
    #define B_10000101 0x85U
    #define B_10000110 0x86U
    #define B_10000111 0x87U
    #define B_10001000 0x88U
    #define B_10001001 0x89U
    #define B_10001010 0x8AU
    #define B_10001011 0x8BU
    #define B_10001100 0x8CU
    #define B_10001101 0x8DU
    #define B_10001110 0x8EU
    #define B_10001111 0x8FU
    #define B_10010000 0x90U
    #define B_10010001 0x91U
    #define B_10010010 0x92U
    #define B_10010011 0x93U
    #define B_10010100 0x94U
    #define B_10010101 0x95U
    #define B_10010110 0x96U
    #define B_10010111 0x97U
    #define B_10011000 0x98U
    #define B_10011001 0x99U
    #define B_10011010 0x9AU
    #define B_10011011 0x9BU
    #define B_10011100 0x9CU
    #define B_10011101 0x9DU
    #define B_10011110 0x9EU
    #define B_10011111 0x9FU
    #define B_10100000 0xA0U
    #define B_10100001 0xA1U
    #define B_10100010 0xA2U
    #define B_10100011 0xA3U
    #define B_10100100 0xA4U
    #define B_10100101 0xA5U
    #define B_10100110 0xA6U
    #define B_10100111 0xA7U
    #define B_10101000 0xA8U
    #define B_10101001 0xA9U
    #define B_10101010 0xAAU
    #define B_10101011 0xABU
    #define B_10101100 0xACU
    #define B_10101101 0xADU
    #define B_10101110 0xAEU
    #define B_10101111 0xAFU
    #define B_10110000 0xB0U
    #define B_10110001 0xB1U
    #define B_10110010 0xB2U
    #define B_10110011 0xB3U
    #define B_10110100 0xB4U
    #define B_10110101 0xB5U
    #define B_10110110 0xB6U
    #define B_10110111 0xB7U
    #define B_10111000 0xB8U
    #define B_10111001 0xB9U
    #define B_10111010 0xBAU
    #define B_10111011 0xBBU
    #define B_10111100 0xBCU
    #define B_10111101 0xBDU
    #define B_10111110 0xBEU
    #define B_10111111 0xBFU
    #define B_11000000 0xC0U
    #define B_11000001 0xC1U
    #define B_11000010 0xC2U
    #define B_11000011 0xC3U
    #define B_11000100 0xC4U
    #define B_11000101 0xC5U
    #define B_11000110 0xC6U
    #define B_11000111 0xC7U
    #define B_11001000 0xC8U
    #define B_11001001 0xC9U
    #define B_11001010 0xCAU
    #define B_11001011 0xCBU
    #define B_11001100 0xCCU
    #define B_11001101 0xCDU
    #define B_11001110 0xCEU
    #define B_11001111 0xCFU
    #define B_11010000 0xD0U
    #define B_11010001 0xD1U
    #define B_11010010 0xD2U
    #define B_11010011 0xD3U
    #define B_11010100 0xD4U
    #define B_11010101 0xD5U
    #define B_11010110 0xD6U
    #define B_11010111 0xD7U
    #define B_11011000 0xD8U
    #define B_11011001 0xD9U
    #define B_11011010 0xDAU
    #define B_11011011 0xDBU
    #define B_11011100 0xDCU
    #define B_11011101 0xDDU
    #define B_11011110 0xDEU
    #define B_11011111 0xDFU
    #define B_11100000 0xE0U
    #define B_11100001 0xE1U
    #define B_11100010 0xE2U
    #define B_11100011 0xE3U
    #define B_11100100 0xE4U
    #define B_11100101 0xE5U
    #define B_11100110 0xE6U
    #define B_11100111 0xE7U
    #define B_11101000 0xE8U
    #define B_11101001 0xE9U
    #define B_11101010 0xEAU
    #define B_11101011 0xEBU
    #define B_11101100 0xECU
    #define B_11101101 0xEDU
    #define B_11101110 0xEEU
    #define B_11101111 0xEFU
    #define B_11110000 0xF0U
    #define B_11110001 0xF1U
    #define B_11110010 0xF2U
    #define B_11110011 0xF3U
    #define B_11110100 0xF4U
    #define B_11110101 0xF5U
    #define B_11110110 0xF6U
    #define B_11110111 0xF7U
    #define B_11111000 0xF8U
    #define B_11111001 0xF9U
    #define B_11111010 0xFAU
    #define B_11111011 0xFBU
    #define B_11111100 0xFCU
    #define B_11111101 0xFDU
    #define B_11111110 0xFEU
    #define B_11111111 0xFFU

    #define B2(high,low)                        (((uint_fast16_t)high << 8U) | low)
    #define B4(hh,h,l,ll)                       (((uint_fast32_t)B2(hh,h) << 16U) | B2(l,ll))
    #define B8(hhhh,hhh,hh,h,l,ll,lll,llll)     (((uint_fast64_t)B4(hhhh,hhh,hh,h) << 32U) | B4(l,ll,lll,llll))
#endif
