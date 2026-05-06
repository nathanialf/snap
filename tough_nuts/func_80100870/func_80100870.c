
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
extern u8 D_800007C4[];
extern u8 D_80045870[];
extern u8 D_80045B20[];
extern u8 D_800486A8[];
extern u64 D_800456A8;
extern int D_80045A58;
extern void func_8003D8A0(s32);
extern void func_80034E30(void);
extern void func_80034A00(u8 *, s32, u8 *, u8 *, u8 *, s32);
extern void func_8003D0F0(u8 *);
void func_80100870(void)
{
  volatile int new_var;
  D_800456A8 = 0xFEDCBA98ULL;
  func_8003D8A0(0x04900000);
  func_80034E30();
  func_80034A00(D_80045870, 1, D_800007C4, D_800486A8, D_80045B20, (0, 0x7F & 0xFFFFu));
 D_80045A58 = (new_var = 0xFEDCBA98ULL); func_8003D0F0(D_80045870); if (D_800486A8) { }
  if (!D_80045870)
  {
  }
}
