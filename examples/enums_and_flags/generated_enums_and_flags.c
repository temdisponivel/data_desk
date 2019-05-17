enum TestEnum
{
ENUM_ONE,
ENUM_TWO,
ENUM_THREE,
ENUM_MAX,
};

#define FLAG_ONE (1<<0)
#define FLAG_TWO (1<<1)
#define FLAG_THREE (1<<2)
typedef unsigned int TestFlags;

enum
{
NO_NAME_ENUM_ONE,
NO_NAME_ENUM_TWO,
NO_NAME_ENUM_THREE,
NO_NAME_ENUM_MAX,
};

#define NO_NAME_FLAG_ONE (1<<0)
#define NO_NAME_FLAG_TWO (1<<1)
#define NO_NAME_FLAG_THREE (1<<2)
