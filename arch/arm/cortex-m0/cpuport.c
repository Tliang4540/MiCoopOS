
void *_mc_stack_init(void (*func)(void *), void *param, void *stack, unsigned int stack_size)
{
    unsigned int *sp = (unsigned int*)((unsigned int)stack + stack_size);

    *(--sp) = (unsigned int)func;
    sp -= 8;
    *sp = (unsigned int)param;

    return sp;
}
