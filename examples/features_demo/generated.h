#define SOME_CONSTANT (16)
// @TagA @TagB 
typedef struct MyStructA MyStructA;
struct MyStructA
{
    int a;
    float b;
    char c;
};
// @TagWithParameters(123, 456, 789) 
typedef struct MyStructB MyStructB;
struct MyStructB
{
    // @TagOnMember 
    float x;
    // @TagWithParameters(123, 456) 
    float y;
    struct
    {
        intfoo[16];
        void *bar;
        // @Tag 
        floatbaz[8][SOME_CONSTANT];
    }sub_struct;
};
// @SameAsStructs 
typedef union MyUnion MyUnion;
union MyUnion
{
    MyStructA struct_a;
    MyStructB struct_b;
};
// @Tag 
typedef enum MyEnum
{
    // @Tag1 
    MyEnum_Value1,
    // @Tag2 
    MyEnum_Value2,
    // @Tag3 
    MyEnum_Value3,
}
MyEnum;
// @Tag 
enum
{
    MyFlags_Value1 = (1<<0),
    MyFlags_Value2 = (1<<1),
    MyFlags_Value3 = (1<<2),
};
typedef unsigned int MyFlags;
