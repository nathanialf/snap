
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
typedef struct Foo
{
  u8 _0[0x70];
  s32 idx;
} Foo;
extern Foo *D_80048800[3];
extern Foo *D_8004880C;
extern Foo *D_80048810;
extern Foo *D_8004881C;
extern s32 D_80048838;
extern Foo *func_80035270(void);
extern Foo *func_8003D820(void);
extern Foo *func_80036010(void);
s32 func_80100A64(Foo *arg0)
{
  Foo *a;
  Foo *b;
  Foo *p;
  s32 i;
  if (D_8004880C != 0)
  {
    return 1;
  }
  if (D_80048810 != 0)
  {
    return 0;
  }
  a = func_80035270();
  b = func_8003D820();
  if (arg0->idx != (-1))
  {
    p = D_80048800[arg0->idx];
    if (((p != 0) && (b != p)) && (a != p))
    {
      D_80048810 = (D_8004880C = p);
      D_80048838 = 0;
      D_8004881C = func_80036010();
      return 1;
    }
  }
  for (i = 0; i < 3; i++)
  {
    p = D_80048800[i];
    if (((p != 0) && (b != p)) && (a != p))
    {
      D_8004880C = p;
      D_80048838 = 0;
      D_8004881C = func_80036010();
      return 1;
    }
  }

  return 0;
}
