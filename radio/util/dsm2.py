from __future__ import print_function

BITLEN_DSM2 = 16


def sendByteDsm2(b):
    print("%02x:" % b, end=' ')
    lev = 0
    len = BITLEN_DSM2
    for i in range(9):
        nlev = b & 1
        if lev == nlev:
            len += BITLEN_DSM2
        else:
            print(len, end=' ')
            # _send_1(nlev ? len-5 : len+3);
            len = BITLEN_DSM2
            lev = nlev
        b = (b >> 1) | 0x80
    # _send_1(len+BITLEN_DSM2+3); // 2 stop bits
    print(len + BITLEN_DSM2)


sendByteDsm2(24)
sendByteDsm2(17)
sendByteDsm2(2)
sendByteDsm2(0x00)
sendByteDsm2(0x06)
sendByteDsm2(0)
sendByteDsm2(10)
sendByteDsm2(0)
sendByteDsm2(14)
sendByteDsm2(0)
sendByteDsm2(18)
sendByteDsm2(0)
sendByteDsm2(22)
sendByteDsm2(0)
