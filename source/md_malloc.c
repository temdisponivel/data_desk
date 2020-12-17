////////////////////////////////
// TODO(allen): Write commentary for all of this.

#define MD_IMPL_Alloc(ctx,size) MD_MALLOC_Alloc(ctx,size)

static void*
MD_MALLOC_Alloc(void *ctx, MD_u64 size)
{
    MD_Assert(ctx == (void*)1);
    return(malloc(size));
}

#define MD_IMPL_GetCtx() MD_MALLOC_GetCtx()

static void*
MD_MALLOC_GetCtx(void)
{
    return((void*)1);
}
