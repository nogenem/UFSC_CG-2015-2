#include <iostream>
#include <gtk/gtk.h>
#include "MainWindow.hpp"

GtkBuilder* builder = NULL;

/* Callbacks */
extern "C"{
    void change_alg_event(GtkToggleButton *button, MainWindow* window){
        int btnId = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "ID"));
        LineClipAlgs alg = (LineClipAlgs) btnId;

        window->changeLineClipAlg(alg);
    }
    void color_choose_event(GtkColorButton *button, ObjDialog* dialog){
        dialog->onColorChangeEvent(button);
    }
    void open_file_event(GtkMenuItem *menuitem, MainWindow* window){
        window->openFile(builder);
    }
    void save_file_event(GtkMenuItem *menuitem, MainWindow* window){
        window->saveFile(builder);
    }
    void add_pnt_event(GtkMenuItem *menuitem, MainWindow* window){
        window->addPoint(builder);
    }
    void add_line_event(GtkMenuItem *menuitem, MainWindow* window){
        window->addLine(builder);
    }
    void add_poly_event(GtkMenuItem *menuitem, MainWindow* window){
        window->addPolygon(builder);
    }
    void add_bezier_curve_event(GtkMenuItem *menuitem, MainWindow* window){
        window->addBezierCurve(builder);
    }
    void add_bspline_curve_event(GtkMenuItem *menuitem, MainWindow* window){
        window->addBSplineCurve(builder);
    }
    void add_poly_coord_event(GtkWidget *button, PolygonDialog* dialog){
        dialog->onClickEvent();
    }
    void add_obj3d_event(GtkMenuItem *menuitem, MainWindow* window){
        window->addObj3D(builder);
    }
    void add_face_event(GtkWidget *button, Object3dDialog* dialog){
        dialog->onClickEvent(builder);
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
        Buttons b = (Buttons)btnId;

        switch(b){
        case Buttons::ZOOM_OUT:
        case Buttons::ZOOM_IN:
            window->zoom(b);
            break;
        case Buttons::UP:
        case Buttons::RIGHT:
        case Buttons::DOWN:
        case Buttons::LEFT:
            window->move(b);
            break;
        case Buttons::ROT_LEFT:
        case Buttons::ROT_RIGHT:
            window->rotateWindow(b);
            break;
        }
    }
    bool main_key_press_event(GtkWidget *widget, GdkEventKey *event, MainWindow *window){
        switch(event->keyval){
        case GDK_KEY_Up:
            window->move(Buttons::UP);
            return true;
        case GDK_KEY_Right:
            if (event->state & GDK_CONTROL_MASK)
                window->rotateWindow(Buttons::ROT_RIGHT);
            else
                window->move(Buttons::RIGHT);
            return true;
        case GDK_KEY_Down:
            window->move(Buttons::DOWN);
            return true;
        case GDK_KEY_Left:
            if (event->state & GDK_CONTROL_MASK)
                window->rotateWindow(Buttons::ROT_LEFT);
            else
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
        case GDK_KEY_x:
        case GDK_KEY_X:
            window->setAxis(Axes::X);
            return true;
        case GDK_KEY_y:
        case GDK_KEY_Y:
            window->setAxis(Axes::Y);
            return true;
        case GDK_KEY_z:
        case GDK_KEY_Z:
            window->setAxis(Axes::Z);
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
    void goto_obj_event(GtkMenuItem *menuitem, MainWindow* window){
        window->gotoSelectedObj();
    }
    void remove_obj_event(GtkMenuItem *menuitem, MainWindow* window){
        window->removeSelectedObj();
    }
    void translate_obj_event(GtkMenuItem *menuitem, MainWindow* window){
        window->translateSelectedObj(builder);
    }
    void scale_obj_event(GtkMenuItem *menuitem, MainWindow* window){
        window->scaleSelectedObj(builder);
    }
    void rotate_obj_event(GtkMenuItem *menuitem, MainWindow* window){
        window->rotateSelectedObj(builder);
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
    void add_poly_z_cell_edited(GtkCellRendererText *cell,
             const gchar *path_string,
             const gchar *new_text,
             PolygonDialog* dialog)
    {
        dialog->onEditCelEvent(cell, path_string, new_text, 2);
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
