typedef struct TestStruct
{
int test_integer_1;
float test_float_1;
char test_char_1;
int test_integer_2;
float test_float_2;
char test_char_2;
double *d;
}
TestStruct;

void
TestStructPrint(TestStruct *object)
{
    printf("%i", object->test_integer_1);
    printf("%f", object->test_float_1);
    printf("%c", object->test_char_1);
    printf("%i", object->test_integer_2);
    printf("%f", object->test_float_2);
    printf("%c", object->test_char_2);
}

