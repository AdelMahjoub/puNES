/*
 * cfginput.c
 *
 *  Created on: 04/nov/2011
 *      Author: fhorse
 */

#include <string.h>
#include "cfg_file.h"
#include "sdl_gfx.h"
#include "sdlsnd.h"
#include "gtk2.h"
#include "cfgstdctrl.h"
#include "opengl.h"
#include "menu/menu_video_effect.h"

#define cfginput_enable_config(ind)\
	switch (cfgport->port.type) {\
		case CTRLDISABLED:\
		case ZAPPER:\
			gtk_widget_set_sensitive(GTK_WIDGET(button_controller[ind]), FALSE);\
			break;\
		case STDCTRL:\
			gtk_widget_set_sensitive(GTK_WIDGET(button_controller[ind]), TRUE);\
			break;\
	}

enum {
	NAME_CONTROLLER,
	VALUE_CONTROLLER,
	NUMBER_CONTROLLER,
	N_TYPE_CONTROLLERS
};

struct typesElement {
	DBWORD controller;
	DBWORD value;
	char name[20];
};

static const struct typesElement ctrlList[] = {
	{ 0, CTRLDISABLED, "Disabled"     },
	{ 0, STDCTRL,      "Standard Pad" },
	{ 0, ZAPPER,       "Zapper"       }
};

void cfg_input_controllers(void);
void cfg_input_combobox_controller_changed(GtkComboBox *combobox, _cfg_port *cfgport);
void cfg_input_configure_controllers(GtkWidget *widget, _cfg_port *cfgport);
void cfg_input_ok_clicked(GtkWidget *widget, _cfg_port *cfgport);
void cfg_input_cancel_clicked(GtkWidget *widget, _cfg_port *cfgport);
void cfg_input_window_destroy(void);
GtkWidget *cfg_input_line_select_type_controllers(BYTE number_controller, _cfg_port *cfgport);
GtkWidget *cfg_input_combobox_select_controllers(_cfg_port *cfgport);

GtkWidget *button_controller[2];

void cfgInput(void) {
	cfg_port1.id = 1;
	memcpy(&cfg_port1.port, &port1, sizeof(port1));
	cfg_port2.id = 2;
	memcpy(&cfg_port2.port, &port2, sizeof(port2));

	emu_pause(TRUE);

	button_controller[0] = button_controller[1] = NULL;

	/* ridisegno lo screen sdl ogni tot millisecondi */
	g_timeout_redraw_start();

	cfg_input_controllers();
}
void cfg_input_controllers(void) {
	GtkWidget *vbox, *mainbox;
	GtkWidget *line, *frame;

	cfg_controllers_toplevel = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_container_set_border_width(GTK_CONTAINER(cfg_controllers_toplevel), 10);
	gtk_window_set_resizable(GTK_WINDOW(cfg_controllers_toplevel), FALSE);

	gtk_window_set_title(GTK_WINDOW(cfg_controllers_toplevel), "Input Configuration");

	g_signal_connect(G_OBJECT(cfg_controllers_toplevel), "destroy",
			G_CALLBACK(cfg_input_window_destroy), NULL);

	mainbox = gtk_vbox_new(FALSE, SPACING);
	gtk_container_add(GTK_CONTAINER(cfg_controllers_toplevel), mainbox);

	frame = gtk_frame_new(" Controllers ");
	gtk_box_pack_start(GTK_BOX(mainbox), frame, FALSE, FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(frame), 10);

	vbox = gtk_vbox_new(FALSE, SPACING);
	gtk_container_add(GTK_CONTAINER(frame), vbox);

	line = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(vbox), line, FALSE, FALSE, 0);

	line = cfg_input_line_select_type_controllers(1, &cfg_port1);
	gtk_box_pack_start(GTK_BOX(vbox), line, FALSE, FALSE, 0);

	line = cfg_input_line_select_type_controllers(2, &cfg_port2);
	gtk_box_pack_start(GTK_BOX(vbox), line, FALSE, FALSE, 0);

	line = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(vbox), line, FALSE, FALSE, 0);

	line = cfg_input_ok_cancel(G_CALLBACK(cfg_input_ok_clicked),
			G_CALLBACK(cfg_input_cancel_clicked), NULL);
	gtk_container_add(GTK_CONTAINER(mainbox), line);

	gtk_widget_show_all(cfg_controllers_toplevel);
}
void cfg_input_resize_std_widget(GtkWidget *widget) {
	gtk_widget_set_size_request(GTK_WIDGET(widget), 100, -1);
}
void cfg_input_combobox_controller_changed(GtkComboBox *combobox, _cfg_port *cfgport) {
	GtkTreeIter iter;
	GtkTreeModel *model;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));

	gtk_combo_box_get_active_iter(GTK_COMBO_BOX(combobox), &iter);

	gtk_tree_model_get(model, &iter, VALUE_CONTROLLER, &cfgport->port.type, -1);

	cfginput_enable_config(cfgport->id - 1);
}
void cfg_input_configure_controllers(GtkWidget *widget, _cfg_port *cfgport) {
	switch (cfgport->port.type) {
		case CTRLDISABLED:
			break;
		case STDCTRL:
			cfg_standard_controller(cfgport);
			break;
		case ZAPPER:
			break;
	}
}
void cfg_input_ok_clicked(GtkWidget *widget, _cfg_port *cfgport) {
	if (((cfg_port1.port.type == ZAPPER) || (cfg_port2.port.type == ZAPPER)) && opengl.rotation) {
		menu_video_effect_set();
	}

	if (cfg_port1.port.type != port1.type) {
		cfg_port1.port.changed = TRUE;
	} else {
		cfg_port1.port.changed = FALSE;
	}

	if (cfg_port2.port.type != port2.type) {
		cfg_port2.port.changed = TRUE;
	} else {
		cfg_port2.port.changed = FALSE;
	}

	memcpy(&port1, &cfg_port1.port, sizeof(port1));
	memcpy(&port2, &cfg_port2.port, sizeof(port2));

	/* Faccio l'update del menu per i casi dello zapper e degli effetti */
	guiUpdate();

	cfg_file_input_save();

	input_init();

	jsQuit();
	jsInit();

	gtk_widget_destroy(cfg_controllers_toplevel);
}
void cfg_input_cancel_clicked(GtkWidget *widget, _cfg_port *cfgport) {
	gtk_widget_destroy(cfg_controllers_toplevel);
}
void cfg_input_window_destroy(void) {
	cfg_controllers_toplevel = NULL;

	if (cfg_standard_controller_toplevel != NULL) {
		gtk_widget_destroy(cfg_standard_controller_toplevel);
	}

	g_timeout_redraw_stop();
	emu_pause(FALSE);
}
GtkWidget *cfg_input_std_button(const char *description) {
	GtkWidget *button;

	button = gtk_button_new_with_label(description);
	cfg_input_resize_std_widget(button);

	return (button);
}
GtkWidget *cfg_input_ok_cancel(GCallback ok, GCallback cancel, _cfg_port *cfgport) {
	GtkWidget *buttonsbox, *button;

	buttonsbox = gtk_hbox_new(FALSE, SPACING);

	button = cfg_input_std_button("Cancel");
	gtk_box_pack_end(GTK_BOX(buttonsbox), button, FALSE, FALSE, 0);
	if (cancel != NULL) {
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(cancel), cfgport);
	}

	button = cfg_input_std_button("OK");
	gtk_box_pack_end(GTK_BOX(buttonsbox), button, FALSE, FALSE, 0);
	if (ok != NULL) {
		g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(ok), cfgport);
	}

	return (buttonsbox);
}
GtkWidget *cfg_input_line_select_type_controllers(BYTE number_controller, _cfg_port *cfgport) {
	GtkWidget *line, *label, *select;
	char description[30];
	BYTE index_button = number_controller - 1;

	sprintf(description, "  Controller %d  ", number_controller);

	line = gtk_hbox_new(FALSE, SPACING);

	label = gtk_label_new(description);
	gtk_box_pack_start(GTK_BOX(line), label, FALSE, FALSE, 0);

	select = cfg_input_combobox_select_controllers(cfgport);
	gtk_box_pack_start(GTK_BOX(line), select, FALSE, FALSE, 0);

	label = gtk_label_new("  ");
	gtk_box_pack_end(GTK_BOX(line), label, FALSE, FALSE, 0);

	button_controller[index_button] = cfg_input_std_button("Setup");
	gtk_box_pack_end(GTK_BOX(line), button_controller[index_button], FALSE, FALSE, 0);

	g_signal_connect(G_OBJECT(button_controller[index_button]), "clicked",
			G_CALLBACK(cfg_input_configure_controllers), cfgport);

	cfginput_enable_config(index_button);

	return (line);
}
GtkWidget *cfg_input_combobox_select_controllers(_cfg_port *cfgport) {
	BYTE i;
	GtkWidget *combobox;
	GtkListStore *model;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;

	combobox = gtk_combo_box_new();

	model = gtk_list_store_new(N_TYPE_CONTROLLERS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);

	for (i = 0; i < LENGTH(ctrlList); i++) {
		gtk_list_store_append(model, &iter);
		gtk_list_store_set(model, &iter, NAME_CONTROLLER, ctrlList[i].name, VALUE_CONTROLLER,
				ctrlList[i].value, NUMBER_CONTROLLER, (cfgport->id - 1), -1);
	}

	gtk_combo_box_set_model(GTK_COMBO_BOX(combobox), GTK_TREE_MODEL(model));
	g_object_unref(model);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), renderer, TRUE);
	gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combobox), renderer, "text", 0);

	gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), cfgport->port.type);

	g_signal_connect(G_OBJECT(combobox), "changed",
			G_CALLBACK(cfg_input_combobox_controller_changed), cfgport);

	return (combobox);
}
