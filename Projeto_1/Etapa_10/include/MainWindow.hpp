#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <gtk/gtk.h>
#include <string>

#include "Viewport.hpp"
#include "Dialogs.hpp"
#include "Objects.hpp"
#include "World.hpp"
#include "FileHandlers.hpp"
#include "MyException.hpp"

#define UI_FILE "window.glade"

enum class Buttons { ZOOM_OUT, ZOOM_IN, UP, RIGHT, DOWN, LEFT, ROT_LEFT, ROT_RIGHT };
enum class Axes { X, Y, Z };

class MainWindow
{
    public:
        MainWindow(GtkBuilder* builder);
        virtual ~MainWindow() { delete m_world; delete m_viewport; }

        // Events
        void openFile(GtkBuilder* builder);
        void saveFile(GtkBuilder* builder);

        void addPoint(GtkBuilder* builder);
        void addLine(GtkBuilder* builder);
        void addPolygon(GtkBuilder* builder);
        void addBezierCurve(GtkBuilder* builder);
        void addBSplineCurve(GtkBuilder* builder);
        void addObj3D(GtkBuilder* builder);
        void addSurface(GtkBuilder* builder);

        void zoom(Buttons id);
        void move(Buttons id);
        void rotateWindow(Buttons id);

        void setAxis(Axes axis);

        void onDraw(cairo_t* cr);
        void showPopUp(GdkEvent *event);
        void gotoSelectedObj();
        void removeSelectedObj();
        void translateSelectedObj(GtkBuilder* builder);
        void scaleSelectedObj(GtkBuilder* builder);
        void rotateSelectedObj(GtkBuilder* builder);
        void showHelpDialog();
        void changeLineClipAlg(LineClipAlgs alg);

    private:
        // Seta o parametro 'name' e 'inter' para
        // apontar para o obj selecionado;
        // Retorna TRUE caso tenha algum obj selecionado
        bool getSelectedObjName(std::string &name, GtkTreeIter *iter);
        void showErrorDialog(const char* msg);
        // Escreve no TextView de logs
        void log(const char* msg);
        // Adiciona os dados de um objeto na ListStore
        void addObjOnListStore(const std::string& name, const char* type);

    private:
        GtkWidget *m_mainWindow = nullptr, *m_step = nullptr,
                  *m_log = nullptr, *m_popUp = nullptr,
                  *m_logScroll = nullptr, *m_axes;

        GtkWidget *m_helpDialog = nullptr;
        GtkTreeModel* m_mainModel = nullptr;
        GtkTreeSelection* m_treeSelection = nullptr;

        Viewport *m_viewport = nullptr;
        World *m_world = nullptr;
};

MainWindow::MainWindow(GtkBuilder* builder) {
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"main_list_store",(char*)"adj_step",
        (char*)"main_window",(char*)"main_pop_up_menu",(char*)"adj_log",
        (char*)"dlog_help", nullptr};

    if(!gtk_builder_add_objects_from_file (GTK_BUILDER(builder), UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_mainWindow = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "main_window" ) );

    // Pega o tamanho da area de desenho e manda para o Viewport
    // Como a area eh fixa, pode-se pegar o tamanho requisitado
    GtkWidget* area = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "drawing_area" ) );

    GtkRequisition min_size;
    gtk_widget_get_preferred_size(area, &min_size, nullptr);

    m_world = new World();
    m_viewport = new Viewport(min_size.width, min_size.height, m_world);
    //_viewport = new Viewport(1000, 1000, _world);

    m_step = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "entry_step" ) );

    // Pega o modelo e a classe de seleção da Treeview
    // Eles serão usados para manipular os dados da ListStore (Add/Remove)
    GtkTreeView* tree = GTK_TREE_VIEW( gtk_builder_get_object( GTK_BUILDER(builder), "main_treeview" ) );
    m_mainModel = gtk_tree_view_get_model(tree);
    m_treeSelection = gtk_tree_view_get_selection(tree);

    m_popUp = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "main_pop_up_menu" ) );

    m_log = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "tv_log" ) );
    m_logScroll = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "log_scroll" ) );

    m_helpDialog = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "dlog_help" ) );

    m_axes = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "comboBox_axes" ) );
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_axes), 2);

    // Salva o id do botao para poder ser usado depois apenas 1 função
    // para manipula-los
    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "btn_zoom_out")),
                       "ID", GINT_TO_POINTER(Buttons::ZOOM_OUT));
    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "btn_zoom_in")),
                       "ID", GINT_TO_POINTER(Buttons::ZOOM_IN));
    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "btn_up")),
                       "ID", GINT_TO_POINTER(Buttons::UP));
    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "btn_right")),
                       "ID", GINT_TO_POINTER(Buttons::RIGHT));
    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "btn_down")),
                       "ID", GINT_TO_POINTER(Buttons::DOWN));
    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "btn_left")),
                       "ID", GINT_TO_POINTER(Buttons::LEFT));
    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "btn_rot_left")),
                       "ID", GINT_TO_POINTER(Buttons::ROT_LEFT));
    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "btn_rot_right")),
                       "ID", GINT_TO_POINTER(Buttons::ROT_RIGHT));

    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "rb_CS_alg")),
                       "ID", GINT_TO_POINTER(LineClipAlgs::CS));
    g_object_set_data(G_OBJECT(gtk_builder_get_object(GTK_BUILDER(builder), "rb_LB_alg")),
                       "ID", GINT_TO_POINTER(LineClipAlgs::LB));

    gtk_widget_show( m_mainWindow );
}

void MainWindow::setAxis(Axes axis){
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_axes), (int)axis);
}

void MainWindow::changeLineClipAlg(LineClipAlgs alg){
    m_viewport->changeLineClipAlg(alg);
    gtk_widget_queue_draw(m_mainWindow);
}

void MainWindow::openFile(GtkBuilder* builder){
    FileDialog dialog(GTK_BUILDER(builder));

    if(dialog.run() == 1){
        char* filename = dialog.getFileName();

        if(filename == nullptr)
            return;

        std::string file(filename);
        delete filename;
        try{
            ObjReader r(file);
            for(auto obj : r.getObjs()){
                try{
                    m_world->addObj(obj);
                    m_viewport->transformAndClipObj(obj);
                    addObjOnListStore(obj->getName(), obj->getTypeName().c_str());

                    gtk_widget_queue_draw(m_mainWindow);
                }catch(MyException& e){
                    log(e.what());
                    delete obj;
                }
            }

            log("Arquivo carregado.\n");
        }catch(MyException& e){
            log(e.what());
            showErrorDialog(e.what());
        }
    }
}

void MainWindow::saveFile(GtkBuilder* builder){
    FileDialog dialog(GTK_BUILDER(builder), true);

    if(dialog.run() == 1){
        char* filename = dialog.getFileName();

        if(filename == nullptr)
            return;

        std::string file(filename);
        delete filename;
        try{
            ObjWriter w(file);
            w.writeObjs(m_world);

            log("Arquivo salvo.\n");
        }catch(MyException& e){
            log(e.what());
            showErrorDialog(e.what());
        }
    }
}

void MainWindow::showErrorDialog(const char* msg){
    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(m_mainWindow),
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_CLOSE,
                                 msg);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

void MainWindow::showHelpDialog(){
    gtk_dialog_run (GTK_DIALOG (m_helpDialog));
    gtk_widget_hide(m_helpDialog);
}

void MainWindow::log(const char* msg){
    GtkTextBuffer *buffer;
    GtkTextIter end;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(m_log));
    gtk_text_buffer_get_end_iter (buffer, &end);

    gtk_text_buffer_insert (buffer, &end, msg, -1);

    // Scroll para o final do Textview
    GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW(m_logScroll));
    gtk_adjustment_set_value(adj, (gtk_adjustment_get_upper(adj) -
                                   gtk_adjustment_get_page_size(adj)));
}

void MainWindow::addObjOnListStore(const std::string& name, const char* type){
    GtkListStore *liststore = GTK_LIST_STORE(m_mainModel);
    GtkTreeIter iter;

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, name.c_str(), 1, type, -1);
}

void MainWindow::showPopUp(GdkEvent *event){
    gtk_menu_popup(GTK_MENU(m_popUp), nullptr, nullptr, nullptr, nullptr,
                    event->button.button, event->button.time);

}

bool MainWindow::getSelectedObjName(std::string &name, GtkTreeIter *iter){
    if(!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(m_treeSelection),
                                    nullptr, iter))
        return false;

    char *_name;
    gtk_tree_model_get(m_mainModel, iter, 0, &_name, -1);

    name = _name;
    delete _name;

    return true;
}

void MainWindow::onDraw(cairo_t* cr){
    m_viewport->drawObjs(cr);
}

void MainWindow::gotoSelectedObj(){
    GtkTreeIter iter;
    std::string name;

    if(!getSelectedObjName(name, &iter))
        return;

    m_viewport->gotoObj(name);
    gtk_widget_queue_draw(m_mainWindow);
}

void MainWindow::removeSelectedObj(){
    GtkTreeIter iter;
    std::string name;

    if(!getSelectedObjName(name, &iter))
        return;

    try{
        m_world->removeObj(name);
        gtk_list_store_remove(GTK_LIST_STORE(m_mainModel), &iter);

        gtk_widget_queue_draw(m_mainWindow);
        log("Objeto removido.\n");
    }catch(MyException& e){
        log(e.what());
    }
}

void MainWindow::zoom(Buttons id){
    double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_step));
    try{
        switch(id){
        case Buttons::ZOOM_OUT:
            m_viewport->zoomWindow(value);
            break;
        case Buttons::ZOOM_IN:
            m_viewport->zoomWindow(-value);
            break;
        default:
            break;
        }
        gtk_widget_queue_draw(m_mainWindow);
    }catch(MyException& e){
        log(e.what());
    }
}

void MainWindow::move(Buttons id){
    double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_step));
    switch(id){
    case Buttons::UP:
        m_viewport->moveWindow(0,value);
        break;
    case Buttons::RIGHT:
        m_viewport->moveWindow(value,0);
        break;
    case Buttons::DOWN:
        m_viewport->moveWindow(0,-value);
        break;
    case Buttons::LEFT:
        m_viewport->moveWindow(-value,0);
        break;
    default:
        break;
    }
    gtk_widget_queue_draw(m_mainWindow);
}

void MainWindow::rotateWindow(Buttons id){
    double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_step));
    char *tmpAxis = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(m_axes));
    std::string axis(tmpAxis);
    delete tmpAxis;

    switch(id){
    case Buttons::ROT_LEFT:
        m_viewport->rotateWindow(value, axis);
        break;
    case Buttons::ROT_RIGHT:
        m_viewport->rotateWindow(-value, axis);
        break;
    default:
        break;
    }
    gtk_widget_queue_draw(m_mainWindow);
}

void MainWindow::addPoint(GtkBuilder* builder){
    PointDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Coordinate c(dialog.getX(),dialog.getY(),dialog.getZ());

                Object* obj = m_world->addPoint(
                                    dialog.getName(), dialog.getColor(), c);
                m_viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Point");

                gtk_widget_queue_draw(m_mainWindow);
                log("Novo ponto adicionado.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else
            finish = true;
    }
}

void MainWindow::addLine(GtkBuilder* builder){
    LineDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Coordinates c;
                c.emplace_back(dialog.getX1(), dialog.getY1(), dialog.getZ1());
                c.emplace_back(dialog.getX2(), dialog.getY2(), dialog.getZ2());

                Object* obj = m_world->addLine(
                                    dialog.getName(), dialog.getColor(), c);
                m_viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Line");

                gtk_widget_queue_draw(m_mainWindow);
                log("Nova reta adicionada.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else
            finish = true;
    }
}

void MainWindow::addPolygon(GtkBuilder* builder){
    PolygonDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Coordinates c;
                dialog.getCoords(c);

                Object* obj = m_world->addPolygon(dialog.getName(), dialog.getColor(),
                                            dialog.shouldBeFilled(), c);
                m_viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Polygon");

                gtk_widget_queue_draw(m_mainWindow);
                log("Novo poligono adicionado.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else
            finish = true;
    }
}

void MainWindow::addObj3D(GtkBuilder* builder){
    Object3dDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    while(!finish){
        if(dialog.run() >= 0){//Nao intendo porque disso...
            try{
                Object* obj = m_world->addObj3D(dialog.getName(), dialog.getFaces());

                m_viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "3D Object");

                gtk_widget_queue_draw(m_mainWindow);
                log("Novo Objeto 3D adicionado.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else{
            finish = true;
        }
    }
}

void MainWindow::addSurface(GtkBuilder* builder){
    SurfaceDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    while(!finish){
        if(dialog.run() == 1){
            try{
                auto& c = dialog.getCoords();
                Object* obj = m_world->addSurface(dialog.getName(), dialog.getColor(),
                                                  dialog.getSurfaceType(), dialog.getMaxLines(),
                                                  dialog.getMaxCols(), c);
                m_viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), obj->getTypeName().c_str());

                gtk_widget_queue_draw(m_mainWindow);
                log("Nova superficie adicionada.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else
            finish = true;
    }
}

void MainWindow::addBezierCurve(GtkBuilder* builder){
    CurveDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Coordinates c;
                dialog.getCoords(c);

                Object* obj = m_world->addBezierCurve(dialog.getName(),
                                            dialog.getColor(), c);
                m_viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Curve");

                gtk_widget_queue_draw(m_mainWindow);
                log("Nova curva adicionada.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else
            finish = true;
    }
}

void MainWindow::addBSplineCurve(GtkBuilder* builder){
    CurveDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Coordinates c;
                dialog.getCoords(c);

                Object* obj = m_world->addBSplineCurve(dialog.getName(),
                                            dialog.getColor(), c);
                m_viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Curve");

                gtk_widget_queue_draw(m_mainWindow);
                log("Nova curva adicionada.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else
            finish = true;
    }
}

void MainWindow::translateSelectedObj(GtkBuilder* builder){
    TranslateDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    std::string name;
    GtkTreeIter iter;

    if(!getSelectedObjName(name, &iter))
        return;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Object* obj = m_world->translateObj(name, dialog.getDX(),
                                                    dialog.getDY(), dialog.getDZ());
                m_viewport->transformAndClipObj(obj);

                gtk_widget_queue_draw(m_mainWindow);
                log("Objeto transladado.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else
            finish = true;
    }
}

void MainWindow::scaleSelectedObj(GtkBuilder* builder){
    ScaleDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    std::string name;
    GtkTreeIter iter;

    if(!getSelectedObjName(name, &iter))
        return;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Object* obj = m_world->scaleObj(name, dialog.getSX(),
                                                dialog.getSY(), dialog.getSZ());
                m_viewport->transformAndClipObj(obj);

                gtk_widget_queue_draw(m_mainWindow);
                log("Objeto escalonado.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else
            finish = true;
    }
}

void MainWindow::rotateSelectedObj(GtkBuilder* builder){
    RotateDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    std::string name;
    GtkTreeIter iter;

    if(!getSelectedObjName(name, &iter))
        return;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Coordinate c(dialog.getCX(), dialog.getCY(), dialog.getCZ());
                Object* obj = m_world->rotateObj(name, dialog.getAnguloX(),
                                            dialog.getAnguloY(), dialog.getAnguloZ(),
                                            dialog.getAnguloA(), c, dialog.getRotateType());
                m_viewport->transformAndClipObj(obj);

                gtk_widget_queue_draw(m_mainWindow);
                log("Objeto rotacionado.\n");
                finish = true;
            }catch(MyException& e){
                log(e.what());
                showErrorDialog(e.what());
            }
        }else
            finish = true;
    }
}

#endif // MAINWINDOW_HPP
