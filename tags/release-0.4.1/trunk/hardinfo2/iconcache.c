/*
 *    HardInfo - Displays System Information
 *    Copyright (C) 2003-2006 Leandro A. F. Pereira <leandro@linuxmag.com.br>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, version 2.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <iconcache.h>
#include <config.h>
#include <binreloc.h>

static GHashTable *cache = NULL;

void icon_cache_init(void)
{
    if (!cache) {
        cache = g_hash_table_new(g_str_hash, g_str_equal);
    }
}

GdkPixbuf *icon_cache_get_pixbuf(const gchar *file)
{
    GdkPixbuf *icon;
    
    icon = g_hash_table_lookup(cache, file);

    if (!icon) {
        gchar *tmp = g_strdup_printf("%s/hardinfo/pixmaps/%s",
                                     gbr_find_data_dir(PREFIX),
                                     file);
        
        icon = gdk_pixbuf_new_from_file(tmp, NULL);
        g_hash_table_insert(cache, g_strdup(file), icon);
        
        g_free(tmp);
    }
    
    return icon;
}

GtkWidget *icon_cache_get_image(const gchar *file)
{
    GdkPixbuf *icon;
    
    icon = icon_cache_get_pixbuf(file);
    return gtk_image_new_from_pixbuf(icon);
}

GdkPixbuf *icon_cache_get_pixbuf_at_size(const gchar *file, gint wid, gint hei)
{
    GdkPixbuf *icon;
    
    icon = g_hash_table_lookup(cache, file);

    if (!icon) {
        gchar *tmp = g_strdup_printf("%s/hardinfo/pixmaps/%s",
                                     gbr_find_data_dir(PREFIX),
                                     file);
        
        icon = gdk_pixbuf_new_from_file_at_size(tmp, wid, hei, NULL);
        g_hash_table_insert(cache, g_strdup(file), icon);
        
        g_free(tmp);
    }
    
    return icon;
}

GtkWidget *icon_cache_get_image_at_size(const gchar *file, gint wid, gint hei)
{
    GdkPixbuf *icon;
    
    icon = icon_cache_get_pixbuf_at_size(file, wid, hei);
    return gtk_image_new_from_pixbuf(icon);
}
