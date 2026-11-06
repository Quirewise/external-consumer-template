#include <quirewise/runtime.h>

int main(void)
{
    QwRuntimeBuffer buffer = {0};
    QwRuntimeStatus status = qw_runtime_buffer_alloc(32u, &buffer);
    if (status != QW_RUNTIME_OK || !buffer.data || buffer.size != 32u) return 1;
    qw_runtime_buffer_free(&buffer);
    if (qw_runtime_abi_version() == 0u) return 2;
    if (!qw_runtime_status_string(QW_RUNTIME_OK)) return 3;
    if (!qw_runtime_status_string((QwRuntimeStatus)9999)) return 4;
    return 0;
}
