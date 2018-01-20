#ifndef _PRE_H
#define	_PRE_H

#define PPSC  0x02
#define PPSB  0x01
#define PPSA  0x00

#define ANS_(port, pin)    ANS ## port ## pin
#define WPU_(port, pin)    WPU ## port ## pin
#define TRIS_(port, pin)   TRIS ## port ## pin
#define LAT_(port, pin)    LAT ## port ## pin
#define PORT_(port, pin)   R ## port ## pin
#define PPS_(port, pin)    R ## port ## pin ## PPS
#define IOCF_(port, pin)   IOC ## port ## F ## pin
#define IOCP_(port, pin)   IOC ## port ## P ## pin
#define IOCN_(port, pin)   IOC ## port ## N ## pin
#define PPSIN_(port, pin)  (((PPS ## port)<<3) | pin)


#define ANS(port, pin)     ANS_(port, pin)
#define WPU(port, pin)     WPU_(port, pin)
#define TRIS(port, pin)    TRIS_(port, pin)
#define LAT(port, pin)     LAT_(port, pin)
#define PORT(port, pin)    PORT_(port, pin)
#define PPS(port, pin)     PPS_(port, pin)
#define IOCF(port, pin)    IOCF_(port, pin)
#define IOCP(port, pin)    IOCP_(port, pin)
#define IOCN(port, pin)    IOCN_(port, pin)
#define PPSIN(port, pin)   PPSIN_(port, pin)


#endif
