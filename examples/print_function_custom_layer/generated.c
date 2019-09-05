typedef struct Guts Guts;
struct Guts
{
int data;
};

typedef struct Foo Foo;
struct Foo
{
int a;
float b;
char c;
Guts *guts;
double *d;
};

void
FooPrint(Foo *object)
{
    printf("{ ");
    printf("%i, ", object->a);
    printf("%f, ", object->b);
    printf("%c, ", object->c);
    printf("%i, ", object->guts->data);
    printf("}");
}

