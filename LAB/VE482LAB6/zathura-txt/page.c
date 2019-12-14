/* See LICENSE file for license and copyright information */

#include <glib.h>
#include <gtk/gtk.h>

#include "internal.h"
#include "plugin.h"

zathura_error_t
txt_page_init(zathura_page_t *page)
{
  if (page == NULL)
  {
    return ZATHURA_ERROR_INVALID_ARGUMENTS;
  }

  zathura_document_t *document = zathura_page_get_document(page);
  txt_document_t *txt_document = zathura_document_get_data(document);

  if (document == NULL || txt_document == NULL)
  {
    return ZATHURA_ERROR_UNKNOWN;
  }

  txt_document_page_meta_t *meta = girara_list_nth(txt_document->pages, zathura_page_get_index(page));
  if (meta == NULL || meta->file == NULL)
  {
    return ZATHURA_ERROR_UNKNOWN;
  }

  txt_page_t *txt_page = g_malloc0(sizeof(txt_page_t));
  if (txt_page == NULL)
  {
    return ZATHURA_ERROR_OUT_OF_MEMORY;
  }

  txt_page->file = g_strdup(meta->file);
  zathura_page_set_width(page, meta->width);
  zathura_page_set_height(page, meta->height);
  zathura_page_set_data(page, txt_page);

  return ZATHURA_ERROR_OK;
}

zathura_error_t
txt_page_clear(zathura_page_t *UNUSED(page), void *data)
{
  txt_page_t *txt_page = data;
  if (txt_page == NULL)
  {
    return ZATHURA_ERROR_OK;
  }

  g_free(txt_page->file);
  g_free(txt_page);

  return ZATHURA_ERROR_OK;
}
