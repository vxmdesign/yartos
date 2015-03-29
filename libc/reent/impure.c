#include <wchar.h>
#include <reent.h>
static struct _reent __ATTRIBUTE_IMPURE_PTR__ impure_data = _REENT_INIT(impure_data);
struct _reent *__ATTRIBUTE_IMPURE_PTR__ _impure_ptr = &impure_data;
