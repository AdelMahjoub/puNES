/*
 * menu_video_tv_aspect_ratio.c
 *
 *  Created on: 16/dic/2011
 *      Author: fhorse
 */

#include "menu_video_tv_aspect_ratio.h"
#include "cfg_file.h"
#include "opengl.h"

enum {
	MTVASPECTRATIO,
	NUMCHKS
};

static GtkWidget *check[NUMCHKS];

void menu_video_tv_aspect_ratio(GtkWidget *video, GtkAccelGroup *accel_group) {
	check[MTVASPECTRATIO] = gtk_check_menu_item_new_with_mnemonic("Aspect Ra_tio");

	gtk_menu_shell_append(GTK_MENU_SHELL(video), check[MTVASPECTRATIO]);

	g_signal_connect_swapped(G_OBJECT(check[MTVASPECTRATIO]), "activate",
	        G_CALLBACK(menu_video_tv_aspect_ratio_set), NULL);
}
void menu_video_tv_aspect_ratio_check(void) {
	if (gfx.opengl) {
		gtk_widget_set_sensitive(check[MTVASPECTRATIO], TRUE);
	} else {
		gtk_widget_set_sensitive(check[MTVASPECTRATIO], FALSE);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MTVASPECTRATIO]), FALSE);
		return;
	}

	if (cfg->tv_aspect_ratio == TRUE) {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MTVASPECTRATIO]), TRUE);
	} else {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check[MTVASPECTRATIO]), FALSE);
	}
}
void menu_video_tv_aspect_ratio_set(void) {
	if (gui_in_update) {
		return;
	}

	cfg->tv_aspect_ratio = !cfg->tv_aspect_ratio;

	gfx_set_screen(NO_CHANGE, NO_CHANGE, NO_CHANGE, NO_CHANGE, TRUE);
}
