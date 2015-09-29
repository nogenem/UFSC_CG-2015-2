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

class MainWindow
{
    public:
        MainWindow(GtkBuilder* builder);
        virtual ~MainWindow() { delete _world; delete _viewport; }

        // Events
        void openFile(GtkBuilder* builder);
        void saveFile(GtkBuilder* builder);

        void addPoint(GtkBuilder* builder);
        void addLine(GtkBuilder* builder);
        void addPolygon(GtkBuilder* builder);
        void addBezierCurve(GtkBuilder* builder);
        void addBSplineCurve(GtkBuilder* builder);

        void zoom(Buttons id);
        void move(Buttons id);
        void rotateWindow(Buttons id);

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
        GtkWidget *_mainWindow = nullptr, *_step = nullptr,
                  *_log = nullptr, *_popUp = nullptr,
                  *_logScroll = nullptr;

        GtkWidget *_helpDialog = nullptr;
        GtkTreeModel* _mainModel = nullptr;
        GtkTreeSelection* _treeSelection = nullptr;

        Viewport *_viewport = nullptr;
        World *_world = nullptr;
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

    _mainWindow = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "main_window" ) );

    // Pega o tamanho da area de desenho e manda para o Viewport
    // Como a area eh fixa, pode-se pegar o tamanho requisitado
    GtkWidget* area = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "drawing_area" ) );

    GtkRequisition min_size;
    gtk_widget_get_preferred_size(area, &min_size, nullptr);

    _world = new World();
    _viewport = new Viewport(min_size.width, min_size.height, _world);
    //_viewport = new Viewport(1000, 1000, _world);

    _step = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "entry_step" ) );

    // Pega o modelo e a classe de seleção da Treeview
    // Eles serão usados para manipular os dados da ListStore (Add/Remove)
    GtkTreeView* tree = GTK_TREE_VIEW( gtk_builder_get_object( GTK_BUILDER(builder), "main_treeview" ) );
    _mainModel = gtk_tree_view_get_model(tree);
    _treeSelection = gtk_tree_view_get_selection(tree);

    _popUp = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "main_pop_up_menu" ) );

    _log = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "tv_log" ) );
    _logScroll = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "log_scroll" ) );

    _helpDialog = GTK_WIDGET( gtk_builder_get_object( GTK_BUILDER(builder), "dlog_help" ) );

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

    gtk_widget_show( _mainWindow );
}

void MainWindow::changeLineClipAlg(LineClipAlgs alg){
    _viewport->changeLineClipAlg(alg);
    gtk_widget_queue_draw(_mainWindow);
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
                    _world->addObj(obj);
                    _viewport->transformAndClipObj(obj);
                    addObjOnListStore(obj->getName(), obj->getTypeName().c_str());

                    gtk_widget_queue_draw(_mainWindow);
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
            w.writeObjs(_world);

            log("Arquivo salvo.\n");
        }catch(MyException& e){
            log(e.what());
            showErrorDialog(e.what());
        }
    }
}

void MainWindow::showErrorDialog(const char* msg){
    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(_mainWindow),
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_CLOSE,
                                 msg);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

void MainWindow::showHelpDialog(){
    gtk_dialog_run (GTK_DIALOG (_helpDialog));
    gtk_widget_hide(_helpDialog);
}

void MainWindow::log(const char* msg){
    GtkTextBuffer *buffer;
    GtkTextIter end;

    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(_log));
    gtk_text_buffer_get_end_iter (buffer, &end);

    gtk_text_buffer_insert (buffer, &end, msg, -1);

    // Scroll para o final do Textview
    GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW(_logScroll));
    gtk_adjustment_set_value(adj, (gtk_adjustment_get_upper(adj) -
                                   gtk_adjustment_get_page_size(adj)));
}

void MainWindow::addObjOnListStore(const std::string& name, const char* type){
    GtkListStore *liststore = GTK_LIST_STORE(_mainModel);
    GtkTreeIter iter;

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, name.c_str(), 1, type, -1);
}

void MainWindow::showPopUp(GdkEvent *event){
    gtk_menu_popup(GTK_MENU(_popUp), nullptr, nullptr, nullptr, nullptr,
                    event->button.button, event->button.time);

}

bool MainWindow::getSelectedObjName(std::string &name, GtkTreeIter *iter){
    if(!gtk_tree_selection_get_selected(GTK_TREE_SELECTION(_treeSelection),
                                    nullptr, iter))
        return false;

    char *_name;
    gtk_tree_model_get(_mainModel, iter, 0, &_name, -1);

    name = _name;
    delete _name;

    return true;
}

void MainWindow::onDraw(cairo_t* cr){
    _viewport->drawObjs(cr);
}

void MainWindow::gotoSelectedObj(){
    GtkTreeIter iter;
    std::string name;

    if(!getSelectedObjName(name, &iter))
        return;

    _viewport->gotoObj(name);
    gtk_widget_queue_draw(_mainWindow);
}

void MainWindow::removeSelectedObj(){
    GtkTreeIter iter;
    std::string name;

    if(!getSelectedObjName(name, &iter))
        return;

    try{
        _world->removeObj(name);
        gtk_list_store_remove(GTK_LIST_STORE(_mainModel), &iter);

        gtk_widget_queue_draw(_mainWindow);
        log("Objeto removido.\n");
    }catch(MyException& e){
        log(e.what());
    }
}

void MainWindow::zoom(Buttons id){
    double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(_step));
    try{
        switch(id){
        case Buttons::ZOOM_OUT:
            _viewport->zoomWindow(value);
            break;
        case Buttons::ZOOM_IN:
            _viewport->zoomWindow(-value);
            break;
        default:
            break;
        }
        gtk_widget_queue_draw(_mainWindow);
    }catch(MyException& e){
        log(e.what());
    }
}

void MainWindow::move(Buttons id){
    double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(_step));
    switch(id){
    case Buttons::UP:
        _viewport->moveWindow(0,value);
        break;
    case Buttons::RIGHT:
        _viewport->moveWindow(value,0);
        break;
    case Buttons::DOWN:
        _viewport->moveWindow(0,-value);
        break;
    case Buttons::LEFT:
        _viewport->moveWindow(-value,0);
        break;
    default:
        break;
    }
    gtk_widget_queue_draw(_mainWindow);
}

void MainWindow::rotateWindow(Buttons id){
    double value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(_step));
    switch(id){
    case Buttons::ROT_LEFT:
        _viewport->rotateWindow(value);
        break;
    case Buttons::ROT_RIGHT:
        _viewport->rotateWindow(-value);
        break;
    default:
        break;
    }
    gtk_widget_queue_draw(_mainWindow);
}

void MainWindow::addPoint(GtkBuilder* builder){
    PointDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Coordinate c(dialog.getX(),dialog.getY());

                Object* obj = _world->addPoint(
                                    dialog.getName(), dialog.getColor(),c);
                _viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Point");

                gtk_widget_queue_draw(_mainWindow);
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
                c.emplace_back(dialog.getX1(), dialog.getY1());
                c.emplace_back(dialog.getX2(), dialog.getY2());

                Object* obj = _world->addLine(
                                    dialog.getName(), dialog.getColor(), c);
                _viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Line");

                gtk_widget_queue_draw(_mainWindow);
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

                Object* obj = _world->addPolygon(dialog.getName(), dialog.getColor(),
                                            dialog.shouldBeFilled(), c);
                _viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Polygon");

                gtk_widget_queue_draw(_mainWindow);
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

void MainWindow::addBezierCurve(GtkBuilder* builder){
    CurveDialog dialog(GTK_BUILDER(builder));
    bool finish = false;

    while(!finish){
        if(dialog.run() == 1){
            try{
                Coordinates c;
                dialog.getCoords(c);

                Object* obj = _world->addBezierCurve(dialog.getName(),
                                            dialog.getColor(), c);
                _viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Curve");

                gtk_widget_queue_draw(_mainWindow);
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

                Object* obj = _world->addBSplineCurve(dialog.getName(),
                                            dialog.getColor(), c);
                _viewport->transformAndClipObj(obj);
                addObjOnListStore(dialog.getName(), "Curve");

                gtk_widget_queue_draw(_mainWindow);
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
                Object* obj = _world->translateObj(name, dialog.getDX(), dialog.getDY());
                _viewport->transformAndClipObj(obj);

                gtk_widget_queue_draw(_mainWindow);
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
                Object* obj = _world->scaleObj(name, dialog.getSX(), dialog.getSY());
                _viewport->transformAndClipObj(obj);

                gtk_widget_queue_draw(_mainWindow);
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
                Object* obj = _world->rotateObj(name, dialog.getAngulo(),
                                    Coordinate(dialog.getCX(), dialog.getCY()),
                                        dialog.getRotateType());
                _viewport->transformAndClipObj(obj);

                gtk_widget_queue_draw(_mainWindow);
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
