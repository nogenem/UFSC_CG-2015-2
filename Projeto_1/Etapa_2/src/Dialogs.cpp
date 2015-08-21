#include "Dialogs.hpp"

int Dialog::run(){
    if(_dialog == NULL) return 0;
    return gtk_dialog_run(GTK_DIALOG(_dialog));
}

void Dialog::destroy(){
    if(_dialog != NULL){
        gtk_widget_destroy(_dialog);
        _dialog = NULL;
    }
}

void PolygonDialog::destroy(){
    if(_model != NULL){
        gtk_list_store_clear(GTK_LIST_STORE(_model));
        _model = NULL;
    }
}

PointDialog::PointDialog(GtkBuilder* builder){
    GError* error = NULL;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_add_pnt",(char*)"adj_x1",
        (char*)"adj_y1",NULL};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    _dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_add_pnt" ) );
    _entryName = GTK_WIDGET( gtk_builder_get_object( builder, "point_name" ) );
    _entryX = GTK_WIDGET( gtk_builder_get_object( builder, "point_x" ) );
    _entryY = GTK_WIDGET( gtk_builder_get_object( builder, "point_y" ) );
}

LineDialog::LineDialog(GtkBuilder* builder){
    GError* error = NULL;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_add_line",(char*)"adj_x1",
        (char*)"adj_y1",(char*)"adj_x2",(char*)"adj_y2",NULL};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    _dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_add_line" ) );
    _entryName = GTK_WIDGET( gtk_builder_get_object( builder, "line_name" ) );
    _entryX1 = GTK_WIDGET( gtk_builder_get_object( builder, "line_x1" ) );
    _entryY1 = GTK_WIDGET( gtk_builder_get_object( builder, "line_y1" ) );
    _entryX2 = GTK_WIDGET( gtk_builder_get_object( builder, "line_x2" ) );
    _entryY2 = GTK_WIDGET( gtk_builder_get_object( builder, "line_y2" ) );
}

PolygonDialog::PolygonDialog(GtkBuilder* builder){
    GError* error = NULL;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"poly_list_store",(char*)"adj_x1",(char*)"adj_y1",
        (char*)"dlog_add_poly",NULL};

    if(!gtk_builder_add_objects_from_file(builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    _dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_add_poly" ) );
    _entryName = GTK_WIDGET( gtk_builder_get_object( builder, "poly_name" ) );
    _entryX = GTK_WIDGET( gtk_builder_get_object( builder, "poly_x" ) );
    _entryY = GTK_WIDGET( gtk_builder_get_object( builder, "poly_y" ) );
    _checkFilled = GTK_WIDGET( gtk_builder_get_object( builder, "poly_filled" ) );

    GtkTreeView* tree = GTK_TREE_VIEW( gtk_builder_get_object( GTK_BUILDER(builder), "poly_treeview" ) );
    _model = gtk_tree_view_get_model(tree);

    gtk_builder_connect_signals(builder, this);
}

TranslateDialog::TranslateDialog(GtkBuilder* builder){
    GError* error = NULL;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_translate",(char*)"adj_x1",
        (char*)"adj_y1",NULL};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    _dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_translate" ) );
    _entryDX = GTK_WIDGET( gtk_builder_get_object( builder, "entry_dx" ) );
    _entryDY = GTK_WIDGET( gtk_builder_get_object( builder, "entry_dy" ) );
}

ScaleDialog::ScaleDialog(GtkBuilder* builder){
    GError* error = NULL;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_scale",(char*)"adj_x1",
        (char*)"adj_y1",NULL};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    _dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_scale" ) );
    _entrySX = GTK_WIDGET( gtk_builder_get_object( builder, "entry_sx" ) );
    _entrySY = GTK_WIDGET( gtk_builder_get_object( builder, "entry_sy" ) );
}

RotateDialog::RotateDialog(GtkBuilder* builder){
    GError* error = NULL;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_rotate",(char*)"adj_x1",
        (char*)"adj_y1",(char*)"adj_angulo",NULL};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    _dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_rotate" ) );
    _entryCX = GTK_WIDGET( gtk_builder_get_object( builder, "entry_cx" ) );
    _entryCY = GTK_WIDGET( gtk_builder_get_object( builder, "entry_cy" ) );
    _entryAngulo = GTK_WIDGET( gtk_builder_get_object( builder, "entry_angulo" ) );

    _objCenter = GTK_WIDGET( gtk_builder_get_object( builder, "tb_obj_center" ) );
    _worldCenter = GTK_WIDGET( gtk_builder_get_object( builder, "rb_world_center" ) );
    _pntCenter = GTK_WIDGET( gtk_builder_get_object( builder, "rb_pnt_center" ) );
}

rotateType RotateDialog::getRotateType(){
    if(isActive(_objCenter))
        return rotateType::OBJECT;
    else if(isActive(_worldCenter))
        return rotateType::WORLD;
    else
        return rotateType::POINT;
}

void PolygonDialog::onClickEvent(){
    GtkListStore *liststore = GTK_LIST_STORE(_model);
    GtkTreeIter iter;

    double x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryX));
    double y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryY));

    // Cria uma nova linha na ListStore e
    // seta os valores de 'x' e 'y'
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, x, 1, y, -1);

    // Seta os SpinButtons para 0 denovo
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(_entryX), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(_entryY), 0.0);

    // Faz o SpinButton do 'x' pegar o focus da window
    gtk_widget_grab_focus(_entryX);
}

void PolygonDialog::onEditCelEvent(GtkCellRendererText *cell,
                            const gchar *path_string,
                            const gchar *new_text, int column){
    GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter iter;

    gtk_tree_model_get_iter (_model, &iter, path);
    gtk_list_store_set (GTK_LIST_STORE (_model), &iter, column,
                        atof(new_text), -1);
}

void PolygonDialog::getCoords(Coordinates& coords){
    GtkTreeIter iter;
    gboolean valid;
    double x, y;

    valid = gtk_tree_model_get_iter_first (_model,
                                            &iter);

    if(!valid)
        throw "Adicione pelo menos uma coordenada.\n";

    while(valid){
        gtk_tree_model_get (_model, &iter,
                       0, &x, 1, &y, -1);
        coords.emplace_back(x, y);

        valid = gtk_tree_model_iter_next (_model,
                                     &iter);
    }
}
