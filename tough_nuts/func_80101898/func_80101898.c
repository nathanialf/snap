
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
  s32 _0;
  s32 _4;
  s32 _8;
} Foo;
extern Foo *D_8004873C;
extern Foo *D_80048740;
extern s32 D_80048820;
extern s32 func_80036010(Foo *);
extern void func_80035720(void);
extern void func_80032CFC(void *);
extern void func_80032E8C(void *);
void func_80101898(Foo *arg0)
{
  Foo *new_var2;
  int new_var;
  int new_var3;
  new_var = 2;
  D_80048820 = func_80036010(arg0);
  if ((D_8004873C != 0) && (D_8004873C->_8 == new_var))
  {
    func_80035720();
    D_8004873C->_8 = 4;
    new_var2->_8 = 3;
  }
  else
  {
    func_80032CFC(((u8 *) new_var2) + 0x28);
    func_80032E8C(((u8 *) new_var2) + 0x28);
    new_var3 = 1;
    if (new_var3)
    {
      new_var2->_8 = 2;
      D_80048740 = arg0;
      new_var2 = arg0;
    }
  }
}
