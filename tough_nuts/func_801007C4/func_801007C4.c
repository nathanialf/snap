
typedef signed char s8;
typedef unsigned char u8;
typedef signed short int s16;
typedef unsigned short int u16;
typedef signed long s32;
typedef unsigned long u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;
typedef float f32;
typedef double f64;
extern void func_80025C38(s32);
extern void func_80034A00(u8 *, s32, void (*)(void), s32, u8 *, s32);
extern void func_8003D0F0(u8 *);
extern void func_80037A10(s32, s32);
extern u8 D_80046880[];
extern u8 D_80047A30;
extern void D_800005D8(void);
extern s32 D_80040CE4;
extern u32 D_80046A68;
extern u32 D_80046A6C;
void func_801007C4(s32 arg0)
{
  func_80025C38(arg0);
  func_80034A00(D_80046880, 5, D_800005D8, arg0, &D_80047A30, 0x32);
  D_80046A68 = 0xFFFFFFFFFFFFFFFF;
  D_80046A6C = 0xFEDCBA98;
  if (D_80040CE4 == 0)
  {
    func_8003D0F0(D_80046880);
  }
  func_80037A10(0, 0);
  for (;;)
  {
  }

}
