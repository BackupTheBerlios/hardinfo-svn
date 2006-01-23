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

static void
get_glx_info(DisplayInfo *di)
{
    gchar *output;
    if (g_spawn_command_line_sync("glxinfo", &output, NULL, NULL, NULL)) {
	gchar **output_lines, **old;

	output_lines = g_strsplit(output, "\n", 0);
	g_free(output);

	old = output_lines;
	while (*(++output_lines)) {
	    if (strstr(*output_lines, "OpenGL")) {
		gchar **tmp = g_strsplit(*output_lines, ":", 0);

		tmp[1] = g_strchug(tmp[1]);

		get_str("OpenGL vendor str", di->ogl_vendor);
		get_str("OpenGL renderer str", di->ogl_renderer);
		get_str("OpenGL version str", di->ogl_version);

		g_strfreev(tmp);
	    }
	}

	g_strfreev(old);

	if (!di->ogl_vendor)
	    di->ogl_vendor = "Unknown";
	if (!di->ogl_renderer)
	    di->ogl_renderer = "Unknown";
	if (!di->ogl_version)
	    di->ogl_version = "Unknown";
    } else {
	di->ogl_vendor = di->ogl_renderer = di->ogl_version = "Unknown";
    }

}

static void
get_x11_info(DisplayInfo *di)
{
    gchar *output;
    
    if (g_spawn_command_line_sync("xdpyinfo", &output, NULL, NULL, NULL)) {
	gchar **output_lines, **old;

	output_lines = g_strsplit(output, "\n", 0);
	g_free(output);

	old = output_lines;
	while (*(output_lines++)) {
            gchar **tmp = g_strsplit(*output_lines, ":", 0);

            if (tmp[1] && tmp[0]) {
              tmp[1] = g_strchug(tmp[1]);

              get_str("vendor string", di->vendor);
              get_str("X.Org version", di->version);
              get_str("XFree86 version", di->version);

              if (g_str_has_prefix(tmp[0], "number of extensions")) {
                int n;
                
                di->extensions = "";
                
                for (n = atoi(tmp[1]); n; n--) {
                  di->extensions = g_strconcat(di->extensions, 
                                               g_strstrip(*(++output_lines)),
                                               "=\n",
                                               NULL);
                }
                g_strfreev(tmp);
                
                break;
              }
            }

            g_strfreev(tmp);
	}

	g_strfreev(old);
    }
    
    GdkScreen *screen = gdk_screen_get_default();
    
    if (screen && GDK_IS_SCREEN(screen)) {
        gint n_monitors = gdk_screen_get_n_monitors(screen);
        gint i;
        
        di->monitors = "";
        for (i = 0; i < n_monitors; i++) {
            GdkRectangle rect;
            
            gdk_screen_get_monitor_geometry(screen, i, &rect);
            
            di->monitors = g_strdup_printf("%sMonitor %d=%dx%d pixels\n",
                                           di->monitors, i, rect.width, rect.height);
        }
      } else {
          di->monitors = "";
      }
}

static DisplayInfo *
computer_get_display(void)
{
    DisplayInfo *di = g_new0(DisplayInfo, 1);
    
    GdkScreen *screen = gdk_screen_get_default();

    di->width = gdk_screen_get_width(screen);
    di->height = gdk_screen_get_height(screen);

    get_glx_info(di);
    get_x11_info(di);

    return di;
}
