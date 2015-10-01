#include "Dialogs.hpp"

FileDialog::FileDialog(GtkBuilder* builder, bool toSave){
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_file",(char*)"obj_filter",nullptr};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_file" ) );
    if(toSave){
        gtk_window_set_title (GTK_WINDOW(m_dialog), "Salve o arquivo...");
        gtk_file_chooser_set_action(GTK_FILE_CHOOSER(m_dialog), GTK_FILE_CHOOSER_ACTION_SAVE);
        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(m_dialog), true);
        gtk_file_chooser_set_create_folders(GTK_FILE_CHOOSER(m_dialog), true);
        gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(m_dialog), "untitled.obj");
    }
}

int Dialog::run(){
    if(m_dialog == nullptr) return 0;
    return gtk_dialog_run(GTK_DIALOG(m_dialog));
}

void Dialog::destroy(){
    if(m_dialog != nullptr){
        gtk_widget_destroy(m_dialog);
        m_dialog = nullptr;
    }
}

void PolygonDialog::destroy(){
    if(m_model != nullptr){
        gtk_list_store_clear(GTK_LIST_STORE(m_model));
        m_model = nullptr;
    }
}

PointDialog::PointDialog(GtkBuilder* builder){
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_add_pnt",(char*)"adj_x1",
        (char*)"adj_y1",(char*)"adj_z1",nullptr};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_add_pnt" ) );
    m_entryName = GTK_WIDGET( gtk_builder_get_object( builder, "point_name" ) );
    m_entryX = GTK_WIDGET( gtk_builder_get_object( builder, "point_x" ) );
    m_entryY = GTK_WIDGET( gtk_builder_get_object( builder, "point_y" ) );
    m_entryZ = GTK_WIDGET( gtk_builder_get_object( builder, "point_z" ) );

    gtk_builder_connect_signals(builder, this);
}

LineDialog::LineDialog(GtkBuilder* builder){
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = { (char*)"dlog_add_line",(char*)"adj_x1",
                    (char*)"adj_y1",(char*)"adj_z1",
                    (char*)"adj_x2",(char*)"adj_y2",
                    (char*)"adj_z2",nullptr };

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_add_line" ) );
    m_entryName = GTK_WIDGET( gtk_builder_get_object( builder, "line_name" ) );
    m_entryX1 = GTK_WIDGET( gtk_builder_get_object( builder, "line_x1" ) );
    m_entryY1 = GTK_WIDGET( gtk_builder_get_object( builder, "line_y1" ) );
    m_entryZ1 = GTK_WIDGET( gtk_builder_get_object( builder, "line_z1" ) );
    m_entryX2 = GTK_WIDGET( gtk_builder_get_object( builder, "line_x2" ) );
    m_entryY2 = GTK_WIDGET( gtk_builder_get_object( builder, "line_y2" ) );
    m_entryZ2 = GTK_WIDGET( gtk_builder_get_object( builder, "line_z2" ) );

    gtk_builder_connect_signals(builder, this);
}

PolygonDialog::PolygonDialog(GtkBuilder* builder){
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"poly_list_store",(char*)"adj_x1",(char*)"adj_y1",
        (char*)"adj_z1",(char*)"dlog_add_poly",nullptr};

    if(!gtk_builder_add_objects_from_file(builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_add_poly" ) );
    m_entryName = GTK_WIDGET( gtk_builder_get_object( builder, "poly_name" ) );
    m_entryX = GTK_WIDGET( gtk_builder_get_object( builder, "poly_x" ) );
    m_entryY = GTK_WIDGET( gtk_builder_get_object( builder, "poly_y" ) );
    m_entryZ = GTK_WIDGET( gtk_builder_get_object( builder, "poly_z" ) );
    m_checkFilled = GTK_WIDGET( gtk_builder_get_object( builder, "poly_filled" ) );

    GtkTreeView* tree = GTK_TREE_VIEW( gtk_builder_get_object( GTK_BUILDER(builder), "poly_treeview" ) );
    m_model = gtk_tree_view_get_model(tree);

    gtk_builder_connect_signals(builder, this);
}

TranslateDialog::TranslateDialog(GtkBuilder* builder){
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_translate",(char*)"adj_x1",
        (char*)"adj_y1",(char*)"adj_z1",nullptr};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_translate" ) );
    m_entryDX = GTK_WIDGET( gtk_builder_get_object( builder, "entry_dx" ) );
    m_entryDY = GTK_WIDGET( gtk_builder_get_object( builder, "entry_dy" ) );
    m_entryDZ = GTK_WIDGET( gtk_builder_get_object( builder, "entry_dz" ) );
}

ScaleDialog::ScaleDialog(GtkBuilder* builder){
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_scale",(char*)"adj_x1",
        (char*)"adj_y1",(char*)"adj_z1",nullptr};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_scale" ) );
    m_entrySX = GTK_WIDGET( gtk_builder_get_object( builder, "entry_sx" ) );
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_entrySX), 1.0);
    m_entrySY = GTK_WIDGET( gtk_builder_get_object( builder, "entry_sy" ) );
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_entrySY), 1.0);
    m_entrySZ = GTK_WIDGET( gtk_builder_get_object( builder, "entry_sz" ) );
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_entrySZ), 1.0);
}

RotateDialog::RotateDialog(GtkBuilder* builder){
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"dlog_rotate",(char*)"adj_x1",
        (char*)"adj_y1",(char*)"adj_z1",(char*)"adj_ang_x",
        (char*)"adj_ang_y",(char*)"adj_ang_z",nullptr};

    if(!gtk_builder_add_objects_from_file (builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_rotate" ) );
    m_entryCX = GTK_WIDGET( gtk_builder_get_object( builder, "entry_cx" ) );
    m_entryCY = GTK_WIDGET( gtk_builder_get_object( builder, "entry_cy" ) );
    m_entryCZ = GTK_WIDGET( gtk_builder_get_object( builder, "entry_cz" ) );
    m_entryAngX = GTK_WIDGET( gtk_builder_get_object( builder, "entry_ang_x" ) );
    m_entryAngY = GTK_WIDGET( gtk_builder_get_object( builder, "entry_ang_y" ) );
    m_entryAngZ = GTK_WIDGET( gtk_builder_get_object( builder, "entry_ang_z" ) );

    m_objCenter = GTK_WIDGET( gtk_builder_get_object( builder, "tb_obj_center" ) );
    m_worldCenter = GTK_WIDGET( gtk_builder_get_object( builder, "rb_world_center" ) );
    m_pntCenter = GTK_WIDGET( gtk_builder_get_object( builder, "rb_pnt_center" ) );
}

rotateType RotateDialog::getRotateType() const {
    if(isActive(m_objCenter))
        return rotateType::OBJECT;
    else if(isActive(m_worldCenter))
        return rotateType::WORLD;
    else
        return rotateType::POINT;
}

void PolygonDialog::onClickEvent(){
    GtkListStore *liststore = GTK_LIST_STORE(m_model);
    GtkTreeIter iter;

    double x = gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryX));
    double y = gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryY));
    double z = gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryZ));

    // Cria uma nova linha na ListStore e
    // seta os valores de 'x', 'y' e 'z'
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, x, 1, y, 2, z, -1);

    // Seta os SpinButtons para 0 denovo
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_entryX), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_entryY), 0.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_entryZ), 0.0);

    // Faz o SpinButton do 'x' pegar o focus da window
    gtk_widget_grab_focus(m_entryX);
}

void PolygonDialog::onEditCelEvent(GtkCellRendererText *cell,
                            const gchar *path_string,
                            const gchar *new_text, int column){
    GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
    GtkTreeIter iter;

    gtk_tree_model_get_iter (m_model, &iter, path);
    gtk_list_store_set (GTK_LIST_STORE (m_model), &iter, column,
                        atof(new_text), -1);
}

void PolygonDialog::getCoords(Coordinates& coords) const {
    GtkTreeIter iter;
    gboolean valid;
    double x, y, z;

    valid = gtk_tree_model_get_iter_first (m_model,
                                            &iter);

    if(!valid)
        throw MyException("Adicione pelo menos uma coordenada.\n");

    while(valid){
        gtk_tree_model_get (m_model, &iter,
                       0, &x, 1, &y, 2, &z, -1);
        coords.emplace_back(x, y, z);

        valid = gtk_tree_model_iter_next (m_model,
                                     &iter);
    }
}

CurveDialog::CurveDialog(GtkBuilder* builder):
    PolygonDialog(builder) {

    gtk_widget_hide(GTK_WIDGET(m_checkFilled));
    gtk_window_set_title(GTK_WINDOW(m_dialog), "Adicione uma curva");
}
