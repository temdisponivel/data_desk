
typedef struct Foo Foo;
struct Foo
{
    int an_integer;
    float a_float;
    char a_char;
    double *ptr_to_double;
};

void
FooPrint(Foo *object)
{
    printf("%i", object->an_integer);
    printf("%f", object->a_float);
    printf("%c", object->a_char);
}

