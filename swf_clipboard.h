#include "swf_client.h"
#include <freerdp/client/cliprdr.h>

BOOL swf_cliprdr_init(swfContext* swfc, CliprdrClientContext* cliprdr);
void swf_cliprdr_uninit(swfContext* wfc, CliprdrClientContext* cliprdr);

UINT cliprdr_send_format_data_request(CliprdrClientContext* cliprdr, UINT32 format);