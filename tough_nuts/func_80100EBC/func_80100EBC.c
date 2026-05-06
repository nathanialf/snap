
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
typedef struct 
{
  s32 data[20];
} Block50;
extern Block50 D_80048758;
extern Block50 D_800487A8;
extern u32 D_8004882C;
extern u32 D_800487F8;
extern void func_80035740(void);
extern void func_80035660(s32);
void func_80100EBC(void)
{
  Block50 *new_var2;
  u32 new_var;
  D_80048758 = D_800487A8;
  new_var2 = &D_800487A8;
  if (((((&D_800487A8) && (&D_800487A8)) ^ 0) && new_var2) != 0)
  {
  }
  func_80035740();
  func_80035660(new_var = ((((u32) D_8004882C) << 4) >> 31) & 0xFFu);
  D_800487F8 = 0;
}
