// === the fixed point macros ========================================
#ifndef FIX15_H
#define FIX15_H

typedef signed int fix15 ;
#define multfix15(a,b) ((fix15)((((signed long long)(a))*((signed long long)(b)))>>15))
#define float2fix15(a) ((fix15)((a)*32768.0)) // 2^15
#define fix2float15(a) ((float)(a)/32768.0)
#define absfix15(a) abs(a) 
#define int2fix15(a) ((fix15)(a << 15))
#define fix2int15(a) ((int)(a >> 15))
#define char2fix15(a) (fix15)(((fix15)(a)) << 15)
#define divfix(a,b) (fix15)(div_s64s64( (((signed long long)(a)) << 15), ((signed long long)(b))))
#define float2fix(a) ((fix15)((a) * 32768.0))
#define fix2float(a) ((float)(a) / 32768.0)
#define sqrtfix(a) (float2fix(sqrt(fix2float(a))))
#define fix15abs(a) ((a) < 0 ? -(a) : (a))

#endif