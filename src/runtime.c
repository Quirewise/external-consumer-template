#include <quirewise/runtime.h>

int main(void)
{
    QwRuntimeBuffer buffer = {0};
    QwRuntimeStatus status = qw_runtime_buffer_alloc(32u, &buffer);
    if (status != QW_RUNTIME_OK || !buffer.data || buffer.size != 32u) return 1;
    qw_runtime_buffer_free(&buffer);
    return 0;
}
