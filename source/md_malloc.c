////////////////////////////////
// TODO(allen): Write commentary for all of this.

#define MD_IMPL_Alloc(ctx,size) MD_MALLOC_Alloc(ctx,size)

static void*
MD_MALLOC_Alloc(void *ctx, MD_u64 size)
{
    return(malloc(size));
}
