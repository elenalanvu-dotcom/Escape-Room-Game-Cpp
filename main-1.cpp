#include "EscapeRoom.h"
#include <gtk/gtk.h>
#include <iostream>

static Game *game_engine = nullptr;

// Inventory flags tracking picked items to decouple from CLI state bugs
static bool player_has_drawer_key = false;
static bool player_has_screwdriver = false;
static bool player_has_usb_drive = false;
static bool player_has_keycard = false;
static bool drawer_is_opened = false;

// UI elements that update dynamically
static GtkWidget *inventory_list_box;
static GtkWidget *main_overlay;
static GtkWidget *window_main_game;

// Forward declaration to prevent C++ scope compilation ordering issues
static void on_view_suspects_clicked(GtkWidget *widget, gpointer data);

// Neon Amber Terminal Accent CSS Theme Rules
const gchar *terminal_css =
    "window { background-color: #050505; }"
    "label { font-family: 'Consolas', 'Courier New', monospace; color: "
    "#FF9900; font-size: 14px; text-shadow: 0px 0px 3px #FF6600; }"
    ".hitbox-btn { background-color: transparent; background-image: none; "
    "border: none; box-shadow: none; }"
    ".hitbox-btn:hover { background-color: rgba(255, 153, 0, 0.08); border: "
    "1px dashed #FF9900; }"
    "button { background-color: #0D0D0D; color: #FF9900; border: 1px solid "
    "#FF9900; font-family: 'Consolas', monospace; font-weight: bold; padding: "
    "8px; }"
    "button:hover { background-color: #FF9900; color: #0D0D0D; }"
    ".header-label { font-size: 20px; font-weight: bold; color: #FF3300; "
    "text-shadow: 0px 0px 6px #FF0000; }"
    ".story-window { background-color: #080808; border: 1px solid #FF9900; }"
    ".story-text { font-size: 16px; color: #FFB300; font-style: italic; }";

// Helper utilities for image generation scaling blocks
GtkWidget *load_and_scale_image(const char *filename, int target_w,
                                int target_h) {
  GdkPixbuf *pix = gdk_pixbuf_new_from_file(filename, NULL);
  if (!pix) {
    GtkWidget *fallback_label = gtk_label_new(
        (std::string("[ RESOURCE MISSING: ") + filename + " ]").c_str());
    return fallback_label;
  }
  GdkPixbuf *scaled =
      gdk_pixbuf_scale_simple(pix, target_w, target_h, GDK_INTERP_BILINEAR);
  GtkWidget *img = gtk_image_new_from_pixbuf(scaled);
  g_object_unref(pix);
  g_object_unref(scaled);
  return img;
}

void show_info_popup(GtkWidget *parent, const std::string &title,
                     const std::string &description) {
  GtkWidget *dialog = gtk_message_dialog_new(
      GTK_WINDOW(parent), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
      "%s", description.c_str());
  gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void rebuild_inventory_ui() {
  GList *children =
      gtk_container_get_children(GTK_CONTAINER(inventory_list_box));
  for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
    gtk_widget_destroy(GTK_WIDGET(iter->data));
  }
  g_list_free(children);

  bool empty = true;
  auto add_item_lbl = [](const std::string &name) {
    GtkWidget *lbl = gtk_label_new((" ▶ " + name).c_str());
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(inventory_list_box), lbl);
  };

  if (player_has_drawer_key) {
    add_item_lbl("Desk Drawer Key [Toolbox]");
    empty = false;
  }
  if (player_has_screwdriver) {
    add_item_lbl("Screwdriver [Toolbox]");
    empty = false;
  }
  if (player_has_usb_drive) {
    add_item_lbl("Admin Security USB [Drawer]");
    empty = false;
  }
  if (player_has_keycard) {
    add_item_lbl("Level 5 Keycard [Drawer]");
    empty = false;
  }

  if (empty) {
    GtkWidget *lbl = gtk_label_new(" [ NO CASE EVIDENCE COLLECTED ] ");
    gtk_container_add(GTK_CONTAINER(inventory_list_box), lbl);
  }
  gtk_widget_show_all(inventory_list_box);
}

// =========================================================================
// INTERACTIVE SCENE OBJECT HANDLERS
// =========================================================================

// DESK INTERACTION: Suspect Profile -> Next -> Notebook Image
void on_desk_notebook_next(GtkWidget *btn, gpointer window_ptr) {
  GtkWidget *win = GTK_WIDGET(window_ptr);
  gtk_container_remove(GTK_CONTAINER(win), gtk_bin_get_child(GTK_BIN(win)));

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_set_border_width(GTK_CONTAINER(box), 15);
  gtk_container_add(GTK_CONTAINER(win), box);

  GtkWidget *title = gtk_label_new("📄 VICTIM NOTEBOOK EXTRACT");
  gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 0);

  GtkWidget *img = load_and_scale_image("notebook.png", 580, 360);
  gtk_box_pack_start(GTK_BOX(box), img, TRUE, TRUE, 0);

  GtkWidget *close_btn = gtk_button_new_with_label("DISMISS ANALYSIS FILE");
  g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_widget_destroy),
                           win);
  gtk_box_pack_end(GTK_BOX(box), close_btn, FALSE, FALSE, 0);

  gtk_widget_show_all(win);
}

void handle_desk_inspect() {
  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win), "EVIDENCE COMPOSITE: DESK FILES");
  gtk_window_set_default_size(GTK_WINDOW(win), 600, 460);
  gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_set_border_width(GTK_CONTAINER(box), 15);
  gtk_container_add(GTK_CONTAINER(win), box);

  GtkWidget *title = gtk_label_new("📋 PRIMARY SUSPECT PROFILES REGISTER");
  gtk_box_pack_start(GTK_BOX(box), title, FALSE, FALSE, 0);

  GtkWidget *img = load_and_scale_image("suspect.png", 580, 360);
  gtk_box_pack_start(GTK_BOX(box), img, TRUE, TRUE, 0);

  GtkWidget *next_btn =
      gtk_button_new_with_label("NEXT: REVIEW RECOVERED NOTEBOOK PAGES ⏩");
  g_signal_connect(next_btn, "clicked", G_CALLBACK(on_desk_notebook_next), win);
  gtk_box_pack_end(GTK_BOX(box), next_btn, FALSE, FALSE, 0);

  gtk_widget_show_all(win);
}

// DRAWER INTERACTION
void handle_drawer_inspect() {
  if (drawer_is_opened) {
    show_info_popup(window_main_game, "DRAWER REPORT",
                    "The drawer is open. You have already gathered the Admin "
                    "USB and Keycard.");
    return;
  }

  if (!player_has_drawer_key) {
    show_info_popup(
        window_main_game, "🔒 BOX IS SEALED",
        "The drawer is locked tightly. You need a key to open it up.");
  } else {
    drawer_is_opened = true;
    player_has_usb_drive = true;
    player_has_keycard = true;
    rebuild_inventory_ui();
    show_info_popup(window_main_game, "🔓 DRAWER UNLOCKED",
                    "You slot the key from the toolbox and pop the drawer "
                    "container open!\n\n"
                    "📦 ITEMS GATHERED:\n"
                    "• Admin Security USB Drive\n"
                    "• Level 5 Authorization Keycard");
  }
}

// TOOLBOX SUBMENU SELECTION
void on_toolbox_item_collected(GtkWidget *btn, gpointer data) {
  char *item_id = (char *)data;
  if (strcmp(item_id, "wrench") == 0) {
    show_info_popup(window_main_game, "EVIDENCE REGISTERED",
                    "Bloody Wrench added to investigative case files.");
  } else if (strcmp(item_id, "screwdriver") == 0) {
    player_has_screwdriver = true;
    show_info_popup(
        window_main_game, "TOOL RECOVERED",
        "Screwdriver gathered. You can now service basic panel screws.");
  } else if (strcmp(item_id, "key") == 0) {
    player_has_drawer_key = true;
    show_info_popup(window_main_game, "KEY RECOVERED",
                    "Desk Drawer Key collected! You can now open the drawer.");
  }
  rebuild_inventory_ui();
  gtk_widget_destroy(btn);
}

// TOOLBOX INTERACTION: Marcus's Box -> Next -> Inside Contents View
void on_toolbox_inside_next(GtkWidget *btn, gpointer window_ptr) {
  GtkWidget *win = GTK_WIDGET(window_ptr);
  gtk_container_remove(GTK_CONTAINER(win), gtk_bin_get_child(GTK_BIN(win)));

  GtkWidget *fixed_canvas = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(win), fixed_canvas);

  GtkWidget *bg_img = load_and_scale_image("toolbox.png", 600, 450);
  gtk_fixed_put(GTK_FIXED(fixed_canvas), bg_img, 0, 0);

  GtkWidget *btn_wrench =
      gtk_button_new_with_label("[ Collect Bloody Wrench ]");
  g_signal_connect(btn_wrench, "clicked", G_CALLBACK(on_toolbox_item_collected),
                   (gpointer) "wrench");
  gtk_fixed_put(GTK_FIXED(fixed_canvas), btn_wrench, 60, 180);

  GtkWidget *btn_driver = gtk_button_new_with_label("[ Collect Screwdriver ]");
  g_signal_connect(btn_driver, "clicked", G_CALLBACK(on_toolbox_item_collected),
                   (gpointer) "screwdriver");
  gtk_fixed_put(GTK_FIXED(fixed_canvas), btn_driver, 320, 120);

  GtkWidget *btn_key = gtk_button_new_with_label("[ Collect Drawer Key ]");
  g_signal_connect(btn_key, "clicked", G_CALLBACK(on_toolbox_item_collected),
                   (gpointer) "key");
  gtk_fixed_put(GTK_FIXED(fixed_canvas), btn_key, 220, 280);

  GtkWidget *exit_box = gtk_button_new_with_label("CLOSE TOOLBOX");
  g_signal_connect_swapped(exit_box, "clicked", G_CALLBACK(gtk_widget_destroy),
                           win);
  gtk_fixed_put(GTK_FIXED(fixed_canvas), exit_box, 240, 400);

  gtk_widget_show_all(win);
}

void handle_toolbox_inspect() {
  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win),
                       "CRIME SCENE ANALYSIS: MAINTENANCE TOOLBOX");
  gtk_window_set_default_size(GTK_WINDOW(win), 600, 500);
  gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_set_border_width(GTK_CONTAINER(box), 15);
  gtk_container_add(GTK_CONTAINER(win), box);

  GtkWidget *img = load_and_scale_image("marcus's box.png", 580, 380);
  gtk_box_pack_start(GTK_BOX(box), img, TRUE, TRUE, 0);

  GtkWidget *next_btn = gtk_button_new_with_label("OPEN TOOLBOX CASING ⏩");
  g_signal_connect(next_btn, "clicked", G_CALLBACK(on_toolbox_inside_next),
                   win);
  gtk_box_pack_end(GTK_BOX(box), next_btn, FALSE, FALSE, 0);

  gtk_widget_show_all(win);
}

// TERMINAL INTERACTION
struct TerminalContext {
  GtkWidget *window;
  GtkWidget *password_entry;
};

void on_terminal_hint_requested(GtkWidget *btn, gpointer data) {
  show_info_popup(GTK_WIDGET(data), "SYSTEM INTEGRATION HINT",
                  "The code begins with ELEC.");
}

void on_terminal_password_submit(GtkWidget *btn, gpointer data) {
  TerminalContext *ctx = (TerminalContext *)data;
  std::string input = gtk_entry_get_text(GTK_ENTRY(ctx->password_entry));

  if (input == "ELEC2030") {
    show_info_popup(ctx->window, "NETWORK RESPONSE", "Password is correct.");

    gtk_container_remove(GTK_CONTAINER(ctx->window),
                         gtk_bin_get_child(GTK_BIN(ctx->window)));
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    gtk_container_add(GTK_CONTAINER(ctx->window), vbox);

    GtkWidget *img = load_and_scale_image("terminal.png", 650, 450);
    gtk_box_pack_start(GTK_BOX(vbox), img, TRUE, TRUE, 0);

    GtkWidget *close_btn =
        gtk_button_new_with_label("DISCONNECT FROM CONSOLE BACKPLANE");
    g_signal_connect_swapped(close_btn, "clicked",
                             G_CALLBACK(gtk_widget_destroy), ctx->window);
    gtk_box_pack_end(GTK_BOX(vbox), close_btn, FALSE, FALSE, 0);

    gtk_widget_show_all(ctx->window);
    delete ctx;
  } else {
    show_info_popup(ctx->window, "❌ SECURITY FAULT",
                    "AUTHENTICATION INVALID. Try again.");
  }
}

void handle_terminal_inspect() {
  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win),
                       "MAINFRAME SECURITY TERMINAL INTERACTION");
  gtk_window_set_default_size(GTK_WINDOW(win), 450, 250);
  gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
  gtk_container_add(GTK_CONTAINER(win), vbox);

  GtkWidget *lbl = gtk_label_new("🔐 ENTER HUB ADMINISTRATIVE CREDENTIALS:");
  gtk_box_pack_start(GTK_BOX(vbox), lbl, FALSE, FALSE, 0);

  GtkWidget *entry = gtk_entry_new();
  gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
  gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

  TerminalContext *ctx = new TerminalContext{win, entry};

  GtkWidget *btn_submit = gtk_button_new_with_label("SUBMIT CREDENTIALS");
  g_signal_connect(btn_submit, "clicked",
                   G_CALLBACK(on_terminal_password_submit), ctx);
  gtk_box_pack_start(GTK_BOX(vbox), btn_submit, FALSE, FALSE, 0);

  GtkWidget *btn_hint = gtk_button_new_with_label("REQUEST SECURITY CODE HINT");
  g_signal_connect(btn_hint, "clicked", G_CALLBACK(on_terminal_hint_requested),
                   win);
  gtk_box_pack_start(GTK_BOX(vbox), btn_hint, FALSE, FALSE, 0);

  gtk_widget_show_all(win);
}

// EXIT BLAST DOOR VERDICT PROMPT
void on_verdict_submitted(GtkWidget *btn, gpointer data) {
  char *name = (char *)data;
  GtkWidget *dialog = gtk_widget_get_ancestor(btn, GTK_TYPE_DIALOG);

  if (strcmp(name, "EDDIE") == 0) {
    show_info_popup(dialog, "🎉 CASE RESOLVED",
                    "Unlocks door. You found the killer. Congratulation!");
    gtk_widget_destroy(dialog);
    gtk_widget_destroy(window_main_game);
    gtk_main_quit();
  } else {
    show_info_popup(dialog, "🚨 FAULT: LOCKDOWN ACTIVE", "Try again");
    gtk_widget_destroy(dialog);
  }
}

void handle_exit_door_inspect() {
  GtkWidget *dialog = gtk_dialog_new_with_buttons(
      "SECURITY VERIFICATION REQUIRED", GTK_WINDOW(window_main_game),
      GTK_DIALOG_MODAL, NULL);
  gtk_window_set_default_size(GTK_WINDOW(dialog), 450, 200);
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
  gtk_container_add(GTK_CONTAINER(content), vbox);

  GtkWidget *lbl = gtk_label_new("\nSelect the killer:\n");
  gtk_box_pack_start(GTK_BOX(vbox), lbl, FALSE, FALSE, 5);

  GtkWidget *btn_clara = gtk_button_new_with_label("[ Clara ]");
  g_signal_connect(btn_clara, "clicked", G_CALLBACK(on_verdict_submitted),
                   (gpointer) "CLARA");
  gtk_box_pack_start(GTK_BOX(vbox), btn_clara, FALSE, FALSE, 2);

  GtkWidget *btn_marcus = gtk_button_new_with_label("[ Marcus ]");
  g_signal_connect(btn_marcus, "clicked", G_CALLBACK(on_verdict_submitted),
                   (gpointer) "MARCUS");
  gtk_box_pack_start(GTK_BOX(vbox), btn_marcus, FALSE, FALSE, 2);

  GtkWidget *btn_eddie = gtk_button_new_with_label("[ Eddie ]");
  g_signal_connect(btn_eddie, "clicked", G_CALLBACK(on_verdict_submitted),
                   (gpointer) "EDDIE");
  gtk_box_pack_start(GTK_BOX(vbox), btn_eddie, FALSE, FALSE, 2);

  gtk_widget_show_all(dialog);
  gtk_dialog_run(GTK_DIALOG(dialog));
}

// Master Interactive Router
static void on_hitbox_clicked_router(GtkWidget *button, gpointer user_data) {
  char *obj_id = (char *)user_data;
  if (strcmp(obj_id, "desk") == 0)
    handle_desk_inspect();
  else if (strcmp(obj_id, "cabinet") == 0)
    handle_drawer_inspect();
  else if (strcmp(obj_id, "toolbox") == 0)
    handle_toolbox_inspect();
  else if (strcmp(obj_id, "terminal") == 0)
    handle_terminal_inspect();
  else if (strcmp(obj_id, "exit_door") == 0)
    handle_exit_door_inspect();
}

static void on_view_suspects_clicked(GtkWidget *widget, gpointer data) {
  GtkWidget *suspect_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(suspect_win),
                       "CASE DOSSIER: SUSPECT REGISTRY");
  gtk_window_set_default_size(GTK_WINDOW(suspect_win), 700, 450);
  gtk_window_set_position(GTK_WINDOW(suspect_win), GTK_WIN_POS_CENTER);

  GdkPixbuf *pix = gdk_pixbuf_new_from_file("suspect.png", NULL);
  if (!pix) {
    GtkWidget *lbl = gtk_label_new(
        "\n [ FILE NOT FOUND: Place 'suspect.png' in project folder ] \n");
    gtk_container_add(GTK_CONTAINER(suspect_win), lbl);
  } else {
    GdkPixbuf *scaled_pix =
        gdk_pixbuf_scale_simple(pix, 700, 450, GDK_INTERP_BILINEAR);
    GtkWidget *img = gtk_image_new_from_pixbuf(scaled_pix);
    gtk_container_add(GTK_CONTAINER(suspect_win), img);
    g_object_unref(pix);
    g_object_unref(scaled_pix);
  }
  gtk_widget_show_all(suspect_win);
}

void create_transparent_hitbox(GtkWidget *fixed_container, const char *obj_id,
                               int x, int y, int w, int h) {
  GtkWidget *btn = gtk_button_new();
  gtk_widget_set_size_request(btn, w, h);
  gtk_style_context_add_class(gtk_widget_get_style_context(btn), "hitbox-btn");
  g_signal_connect(btn, "clicked", G_CALLBACK(on_hitbox_clicked_router),
                   (gpointer)obj_id);
  gtk_fixed_put(GTK_FIXED(fixed_container), btn, x, y);
}

// =========================================================================
// SEQUENTIAL PRE-GAME NARRATIVE FRAME ENGINE
// =========================================================================
static int story_index = 0;
static GtkWidget *story_label_widget;
static GtkWidget *story_win_widget;

const char *story_frames[] = {
    "“...ugh...”\n\n“My head...”\n\n“Why does it hurt so much...?”\n\n“Where "
    "am I...?”\n\n“Why can’t I remember anything...?””",
    "“That door... locked?”\n\n“What is this place...?”\n\n“Wait...””",
    "“I remember something...”\n\n“A scream...”\n\n“Blood...”\n\n“I saw "
    "someone die.”",
    "“Professor Raymond...”\n\n“My professor...”\n\n“He was working on "
    "something secret...”\n\n“Something he said nobody should ever "
    "control...”\n\n“An artificial intelligence system...”\n\n“A system that "
    "could predict human thinking...”\n\n“He said it became "
    "unstable...”\n\n“He said someone inside the lab wanted to steal it...”",
    "“There were three people in the room...”\n\n“Clara... Marcus... "
    "Eddie...”\n\n“They were arguing... and going outside”\n\n“The professor "
    "looked terrified...”\n\n“Then suddenly—”\n\n“No...”",
    "“Someone attacked me from behind...”\n\n“After that... everything went "
    "dark.\n\n“Why am I still here...?”\n\n“Did the killer lock me inside this "
    "room?”\n\n“There has to be a way out...”\n\n“Maybe... maybe the answers "
    "are somewhere in this room.”\n\n“I need to remember what "
    "happened.”\n\n“And I need to find out who killed my professor.”"};

void on_story_next_clicked(GtkWidget *btn, gpointer data) {
  story_index++;
  if (story_index < 6) {
    gtk_label_set_text(GTK_LABEL(story_label_widget),
                       story_frames[story_index]);
  } else {
    gtk_widget_destroy(story_win_widget);
    gtk_widget_show_all(window_main_game);
  }
}

void play_cinematic_prolog() {
  story_win_widget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(story_win_widget),
                       "MEMORY RECOVERY CHANNELS ONLINE...");
  gtk_window_set_default_size(GTK_WINDOW(story_win_widget), 550, 360);
  gtk_window_set_position(GTK_WINDOW(story_win_widget), GTK_WIN_POS_CENTER);
  gtk_style_context_add_class(gtk_widget_get_style_context(story_win_widget),
                              "story-window");

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_container_set_border_width(GTK_CONTAINER(box), 30);
  gtk_container_add(GTK_CONTAINER(story_win_widget), box);

  story_label_widget = gtk_label_new(story_frames[0]);
  gtk_label_set_justify(GTK_LABEL(story_label_widget), GTK_JUSTIFY_LEFT);
  gtk_style_context_add_class(gtk_widget_get_style_context(story_label_widget),
                              "story-text");
  gtk_box_pack_start(GTK_BOX(box), story_label_widget, TRUE, TRUE, 0);

  GtkWidget *next_btn = gtk_button_new_with_label("ADVANCE RETRIEVAL LOG ⏩");
  g_signal_connect(next_btn, "clicked", G_CALLBACK(on_story_next_clicked),
                   NULL);
  gtk_box_pack_end(GTK_BOX(box), next_btn, FALSE, FALSE, 0);

  gtk_widget_show_all(story_win_widget);
}

// =========================================================================
// MAIN ENTRY ENVIRONMENT
// =========================================================================
int main(int argc, char *argv[]) {
  game_engine = new Game();
  gtk_init(&argc, &argv);

  GtkCssProvider *cssProvider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(cssProvider, terminal_css, -1, NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                            GTK_STYLE_PROVIDER(cssProvider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);

  window_main_game = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window_main_game),
                       "CRIME SCENE ROOM INVESTIGATOR");
  gtk_window_set_default_size(GTK_WINDOW(window_main_game), 1024, 768);
  gtk_window_set_position(GTK_WINDOW(window_main_game), GTK_WIN_POS_CENTER);
  g_signal_connect(window_main_game, "destroy", G_CALLBACK(gtk_main_quit),
                   NULL);

  main_overlay = gtk_overlay_new();
  gtk_container_add(GTK_CONTAINER(window_main_game), main_overlay);

  GtkWidget *bg_scene = load_and_scale_image("background.png", 1024, 768);
  gtk_container_add(GTK_CONTAINER(main_overlay), bg_scene);

  GtkWidget *canvas = gtk_fixed_new();
  gtk_overlay_add_overlay(GTK_OVERLAY(main_overlay), canvas);

  // Object tracking hitbox mapping positions (Power panel has been deleted)
  create_transparent_hitbox(canvas, "desk", 80, 420, 240, 180);
  create_transparent_hitbox(canvas, "toolbox", 240, 310, 90,
                            50); // Toolbox is now next to the drawer cabinet
  create_transparent_hitbox(canvas, "cabinet", 340, 280, 100, 250);
  create_transparent_hitbox(canvas, "terminal", 720, 500, 280, 240);
  create_transparent_hitbox(canvas, "exit_door", 540, 180, 200, 250);

  GtkWidget *sus_toggle = gtk_button_new_with_label("📋 REVIEW SUSPECT LIST");
  gtk_widget_set_size_request(sus_toggle, 200, 40);
  g_signal_connect(sus_toggle, "clicked", G_CALLBACK(on_view_suspects_clicked),
                   NULL);
  gtk_fixed_put(GTK_FIXED(canvas), sus_toggle, 790, 20);

  // Sidebar floating container: Collected Evidence Matrix
  GtkWidget *hud_grid = gtk_grid_new();
  gtk_container_set_border_width(GTK_CONTAINER(hud_grid), 25);
  gtk_widget_set_valign(hud_grid, GTK_ALIGN_END);
  gtk_widget_set_halign(hud_grid, GTK_ALIGN_END);
  gtk_overlay_add_overlay(GTK_OVERLAY(main_overlay), hud_grid);

  GtkWidget *vbox_inv = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *inv_title = gtk_label_new("🎒 COLLECTED EVIDENCE CASEFILES");
  gtk_box_pack_start(GTK_BOX(vbox_inv), inv_title, FALSE, FALSE, 0);

  inventory_list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  gtk_widget_set_size_request(inventory_list_box, 260, 110);
  gtk_box_pack_start(GTK_BOX(vbox_inv), inventory_list_box, TRUE, TRUE, 0);
  gtk_grid_attach(GTK_GRID(hud_grid), vbox_inv, 0, 0, 1, 1);

  rebuild_inventory_ui();

  // Launch memory story prologs
  play_cinematic_prolog();

  gtk_main();
  delete game_engine;
  return 0;
}