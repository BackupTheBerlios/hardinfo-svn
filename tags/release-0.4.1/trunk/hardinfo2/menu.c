/*
 * HardInfo
 * Copyright(C) 2003-2006 Leandro A. F. Pereira.
 *
 * menu.c is based on UI Manager tutorial by Ryan McDougall
 * Copyright(C) 2005 Ryan McDougall.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <gtk/gtk.h>
#include <menu.h>
#include <config.h>

#include <stock.h>
#include <binreloc.h>

#include <callbacks.h>

static GtkActionEntry entries[] = 
{
  { "FileMenuAction", NULL, "_File" },                  /* name, stock id, label */
  { "ViewMenuAction", NULL, "_View" },
  { "HelpMenuAction", NULL, "_Help" },
  { "MainMenuBarAction", NULL, "" },
  
  { "ReportAction", HI_STOCK_REPORT,                    /* name, stock id */
    "Generate _Report", "<control>R",                   /* label, accelerator */
    "Creates a report in HTML",                         /* tooltip */ 
    G_CALLBACK(cb_generate_report) },
  
  { "RefreshAction", GTK_STOCK_REFRESH,
    "_Refresh", "F5",
    NULL,
    G_CALLBACK(cb_refresh) },

  { "AboutAction", GTK_STOCK_ABOUT,
    "_About\342\200\246", NULL,    
    "Displays program version information",
    G_CALLBACK(cb_about) },

  { "QuitAction", GTK_STOCK_QUIT,
    "_Quit", "<control>Q",    
    NULL,
    G_CALLBACK(cb_quit) }
};

static GtkToggleActionEntry toggle_entries[] =
{
  { "SidePaneAction", NULL,
    "_Side Pane", NULL,    
    "Toggles side pane visibility",
    G_CALLBACK(cb_side_pane) },
  { "ToolbarAction", NULL,
    "_Toolbar", NULL,    
    NULL,
    G_CALLBACK(cb_toolbar) },
};

/* Implement a handler for GtkUIManager's "add_widget" signal. The UI manager
 * will emit this signal whenever it needs you to place a new widget it has. */
static void
menu_add_widget(GtkUIManager *ui, GtkWidget *widget, GtkContainer *container)
{
    gtk_box_pack_start(GTK_BOX(container), widget, FALSE, FALSE, 0);
    gtk_widget_show(widget);
}

void menu_init(Shell *shell)
{
    GtkWidget           *menu_box;              /* Packing box for the menu and toolbars */
    GtkActionGroup      *action_group;          /* Packing group for our Actions */
    GtkUIManager        *menu_manager;          /* The magic widget! */
    GError              *error;                 /* For reporting exceptions or errors */
    gchar               *uidefs_path;
    
    /* Create our objects */
    menu_box = shell->vbox;
    action_group = gtk_action_group_new("HardInfo");
    menu_manager = gtk_ui_manager_new();
    
    shell->action_group = action_group;
    shell->ui_manager   = menu_manager;
    
    /* Pack up our objects:
     * menu_box -> window
     * actions -> action_group
     * action_group -> menu_manager */
    gtk_action_group_add_actions(action_group, entries, G_N_ELEMENTS(entries),
                                 NULL);
    gtk_action_group_add_toggle_actions(action_group, toggle_entries,
                                        G_N_ELEMENTS(toggle_entries), NULL);
    gtk_ui_manager_insert_action_group(menu_manager, action_group, 0);
    
    /* Read in the UI from our XML file */
    error = NULL;
    uidefs_path = g_strdup_printf("%s/hardinfo/uidefs.xml",
                                  gbr_find_data_dir(PREFIX));
    gtk_ui_manager_add_ui_from_file(menu_manager, uidefs_path, &error);
    g_free(uidefs_path);
    
    if (error) {
        g_error("building menus failed: %s", error->message);
        g_error_free(error);
    }
    
    /* Connect up important signals */
    /* This signal is necessary in order to place widgets from the UI manager
     * into the menu_box */
    g_signal_connect(menu_manager, "add_widget", 
                     G_CALLBACK(menu_add_widget),
                     menu_box);
 
    /* Show the window and run the main loop, we're done! */
    gtk_widget_show(menu_box);

    gtk_toolbar_set_style(GTK_TOOLBAR(gtk_ui_manager_get_widget(shell->ui_manager, "/MainMenuBarAction")),
                          GTK_TOOLBAR_BOTH_HORIZ);
}
