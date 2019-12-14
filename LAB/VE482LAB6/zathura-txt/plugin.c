/* See LICENSE file for license and copyright information */

#include "plugin.h"

ZATHURA_PLUGIN_REGISTER_WITH_FUNCTIONS(
    "txt",
    VERSION_MAJOR, VERSION_MINOR, VERSION_REV,
    ZATHURA_PLUGIN_FUNCTIONS({.document_open = txt_document_open,
                              .document_free = txt_document_free,
                              .document_index_generate = txt_document_index_generate,
                              .page_init = txt_page_init,
                              .page_clear = txt_page_clear,
                              .page_render_cairo = txt_page_render_cairo}),
    ZATHURA_PLUGIN_MIMETYPES({"text/plain",
                              "text/css",
                              "text/csv"}))
