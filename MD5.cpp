/**
 *  This is a demo on MD5 algorithm.
 *  This demo convert a simple string to MD5 code
 *
 *  Note:
 *    Complie with c++11 standard
 *
 *  Todo:
 *    1. Performance improvement, not copying the original string
 *    2. File support, allowing reading binary from files
 *
 *  Author: Tommy Leo
 *   Email: tommyleoex@gmail.com
 *    Date: Oct 7 2016
 */

#include <iostream>
#include <iomanip>
#include <functional>
#include <cstring>
#include <string>
#include <cmath>

typedef unsigned long long UINT64;
typedef unsigned int UINT32;
typedef unsigned char UINT8;

using namespace std;

class MD5 {
public:
    MD5() : result{0} {
        //  Calculate the 64-element table
        for (int i = 0; i < 64; i++) {
            T[i] = 4294967296 * abs(sin(i + 1));
        }
    }

    void getMD5(const char *message, UINT32 size) {
        //  initialize result
        result[0] = initialMD5[0];
        result[1] = initialMD5[1];
        result[2] = initialMD5[2];
        result[3] = initialMD5[3];

        //  calculate number of zeros to append
        UINT32 zeroNum = 64 - (size + 1 + 8) % 64;
        //  allocate a new space, then do padding
        char *appendedMessage = new char[size + 1 + zeroNum + 8 + 1];
        //  copy the original message to the new space
        memcpy(appendedMessage, message, size);
        //  append 1
        appendedMessage[size] = 0x80;
        //  append zeros
        memset(appendedMessage + size + 1, 0, zeroNum);
        //  append original message's size
        UINT64 length = (UINT64)size * 8;
        memcpy(appendedMessage + size + 1 + zeroNum, &length, 8);

        //  outter loop, for each 512-bit block, loop in char
        for (UINT32 i = 0; i < size + 1 + zeroNum + 8; i += 64) {
            //  load A, B, C and D
            UINT32 A = result[0];
            UINT32 B = result[1];
            UINT32 C = result[2];
            UINT32 D = result[3];

            //  temporarily store the result of F, G, H, I function
            UINT32 tempF = 0;
            //  temporarily store the number of sub-block
            UINT32 tempBlockNum = 0;

            //  64 operations on each 512-bit block
            for (UINT32 op = 0; op < 64; op++) {
                //  first kind of operation
                if (op < 16) {
                    tempF = F(B, C, D);
                    tempBlockNum = op;
                }
                //  second kind of operation
                else if (op < 32) {
                    tempF = G(B, C, D);
                    tempBlockNum = (5 * op + 1) % 16;
                }
                //  third kind of operation
                else if (op < 48) {
                    tempF = H(B, C, D);
                    tempBlockNum = (3 * op + 5) % 16;
                }
                //  fourth kind of operation
                else if (op < 64) {
                    tempF = I(B, C, D);
                    tempBlockNum = (7 * op) % 16;
                }

                //  core transformation
                A = B + leftRotate(A + tempF + T[op] + *(UINT32*)(appendedMessage + i + tempBlockNum * 4),
                                   s[op]);

                //  right rotate
                UINT32 tempD = D;
                D = C;
                C = B;
                B = A;
                A = tempD;
            }
            //  add to result
            result[0] += A;
            result[1] += B;
            result[2] += C;
            result[3] += D;
        }

        //  swap, convert little endian to big endian
        for (int i = 0; i < 4; i++) {
            UINT32 tmp = result[i];
            memcpy(result + i, ((char *)&tmp) + 3, 1);
            memcpy((char *)(result + i) + 1, ((char *)&tmp) + 2, 1);
            memcpy((char *)(result + i) + 2, ((char *)&tmp) + 1, 1);
            memcpy((char *)(result + i) + 3, ((char *)&tmp), 1);
        }

        delete appendedMessage;
    }


    //  store the MD5 result
    UINT32 result[4];

private:
    //  abs(sin(i)) table
    UINT32 T[64];
    //  shift order
    const UINT32 s[64] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
    };
    //  magic initialization constants
    const UINT32 initialMD5[4] = {
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476
    };

    //  4 auxiliary functions
    UINT32 F(UINT32 B, UINT32 C, UINT32 D) {
        return (B & C) | (~B & D);
    }
    UINT32 G(UINT32 B, UINT32 C, UINT32 D) {
        return (B & D) | (C & ~D);
    }
    UINT32 H(UINT32 B, UINT32 C, UINT32 D) {
        return B ^ C ^ D;
    }
    UINT32 I(UINT32 B, UINT32 C, UINT32 D) {
        return C ^ (B | ~D);
    }

    //  left rotate
    UINT32 leftRotate(UINT32 num, UINT32 bit) {
        return (num << bit) | (num >> (32 - bit));
    }
};


int main(void) {
    string strs[] = {
        "",
        "a",
        "abc",
        "message digest",
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
        "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    };
    MD5 md5;
    for (string str: strs) {
        md5.getMD5(str.c_str(), str.size());
        cout << "----------------------------------------\n";
        cout << "Original message: " << str << endl;
        cout << "MD5: "
             << hex << setw(8) << setfill('0')
             << md5.result[0] << " " << md5.result[1]
             << " " << md5.result[2] << " "  << md5.result[3] << endl;
    }
    return 0;
}
