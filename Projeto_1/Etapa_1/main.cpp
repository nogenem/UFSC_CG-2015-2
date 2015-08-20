#include <iostream>
#include <gtk/gtk.h>
#include "MainWindow.hpp"

GtkBuilder* builder = NULL;

/* Callbacks */
extern "C"{
    void add_pnt_event(GtkMenuItem *menuitem, MainWindow* window){
        window->addPoint(builder);
    }
    void add_line_event(GtkMenuItem *menuitem, MainWindow* window){
        window->addLine(builder);
    }
    void add_poly_event(GtkMenuItem *menuitem, MainWindow* window){
        window->addPolygon(builder);
    }
    void add_poly_coord_event(GtkWidget *button, PolygonDialog* dialog){
        dialog->onClickEvent();
    }
    void help_event(GtkMenuItem *menuitem, MainWindow* window){
        window->showHelpDialog();
    }
    bool on_draw_event(GtkWidget *widget, cairo_t *cr, MainWindow* window){
        window->onDraw(cr);
        return true;
    }
    void main_btns_event(GtkWidget *button, MainWindow* window){
        int btnId = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "ID"));

        switch(btnId){
        case Buttons::ZOOM_OUT:
        case Buttons::ZOOM_IN:
            window->zoom((Buttons)btnId);
            break;
        case Buttons::UP:
        case Buttons::RIGHT:
        case Buttons::DOWN:
        case Buttons::LEFT:
            window->move((Buttons)btnId);
            break;
        }
    }
    bool main_key_press_event(GtkWidget *widget, GdkEventKey *event, MainWindow *window){
        switch(event->keyval){
        case GDK_KEY_Up:
            window->move(Buttons::UP);
            return true;
        case GDK_KEY_Right:
            window->move(Buttons::RIGHT);
            return true;
        case GDK_KEY_Down:
            window->move(Buttons::DOWN);
            return true;
        case GDK_KEY_Left:
            window->move(Buttons::LEFT);
            return true;
        case GDK_KEY_plus:
        case GDK_KEY_KP_Add:
            window->zoom(Buttons::ZOOM_IN);
            return true;
        case GDK_KEY_minus:
        case GDK_KEY_KP_Subtract:
            window->zoom(Buttons::ZOOM_OUT);
            return true;
        default:
            return false;
        }
    }
    bool show_pop_up_event(GtkWidget *widget,
               GdkEvent *event,
               MainWindow *window){

        if(event->type == GDK_BUTTON_PRESS &&
           event->button.button == 3)
            window->showPopUp(event);

        return false;
    }
    void remove_obj_event(GtkMenuItem *menuitem, MainWindow* window){
        window->removeSelectedObj();
    }
    void add_poly_x_cell_edited (GtkCellRendererText *cell,
             const gchar *path_string,
             const gchar *new_text,
             PolygonDialog* dialog)
    {
        dialog->onEditCelEvent(cell, path_string, new_text, 0);
    }
    void add_poly_y_cell_edited (GtkCellRendererText *cell,
             const gchar *path_string,
             const gchar *new_text,
             PolygonDialog* dialog)
    {
        dialog->onEditCelEvent(cell, path_string, new_text, 1);
    }
}

int main(int argc, char ** argv)
{
    /* Init GTK+ */
    gtk_init( &argc, &argv );

    builder = gtk_builder_new();
    MainWindow* window = new MainWindow(GTK_BUILDER(builder));
    gtk_builder_connect_signals( GTK_BUILDER(builder), window );

    /* Start main loop */
    gtk_main();
    return 0;
}
