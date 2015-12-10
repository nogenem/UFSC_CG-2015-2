#include "Dialogs.hpp"
#include <sstream>

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
    if(m_dialog == nullptr) return -1;
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

void Object3dDialog::destroy(){
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

PolygonDialog::PolygonDialog(GtkBuilder* builder, bool isFace){
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

    // Agora com o 3D os poligonos não podem mais ser
    //  preenchidos...
    gtk_widget_hide(m_checkFilled);

    gtk_builder_connect_signals(builder, this);
}

CurveDialog::CurveDialog(GtkBuilder* builder):
    PolygonDialog(builder) {

    gtk_widget_hide(GTK_WIDGET(m_checkFilled));
    gtk_window_set_title(GTK_WINDOW(m_dialog), "Adicione uma curva");
}

Object3dDialog::Object3dDialog(GtkBuilder* builder){
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"obj3d_list_store",(char*)"dlog_add_obj3d",nullptr};

    if(!gtk_builder_add_objects_from_file(builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_add_obj3d" ) );
    m_entryName = GTK_WIDGET( gtk_builder_get_object( builder, "obj3d_name" ) );

    GtkTreeView* tree = GTK_TREE_VIEW( gtk_builder_get_object( GTK_BUILDER(builder), "obj3d_treeview" ) );
    m_model = gtk_tree_view_get_model(tree);

    gtk_builder_connect_signals(builder, this);
}

SurfaceDialog::SurfaceDialog(GtkBuilder* builder){
    GError* error = nullptr;
    // (char*) usado para tirar warnings do compilador
    char* ids[] = {(char*)"surface_comboBox",(char*)"adj_x1",(char*)"adj_y1",
        (char*)"adj_z1",(char*)"dlog_add_surface",nullptr};

    if(!gtk_builder_add_objects_from_file(builder, UI_FILE, ids, &error)){
        g_warning( "%s", error->message );
        g_free( error );
        return;
    }

    m_dialog = GTK_WIDGET( gtk_builder_get_object( builder, "dlog_add_surface" ) );
    m_entryName = GTK_WIDGET( gtk_builder_get_object( builder, "surface_name" ) );
    m_entryX = GTK_WIDGET( gtk_builder_get_object( builder, "surface_x" ) );
    m_entryY = GTK_WIDGET( gtk_builder_get_object( builder, "surface_y" ) );
    m_entryZ = GTK_WIDGET( gtk_builder_get_object( builder, "surface_z" ) );
    m_grid = GTK_WIDGET( gtk_builder_get_object( builder, "surface_grid" ) );

    m_comboBox = GTK_WIDGET( gtk_builder_get_object( builder, "cb_surface_side" ) );
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboBox), 0);
    m_btnAddSurface = GTK_WIDGET( gtk_builder_get_object( builder, "btn_add_surface" ) );

    GObject* tmpBezier = G_OBJECT(gtk_builder_get_object( builder, "rb_bezier_surface" ));
    m_rbBezier = GTK_WIDGET( tmpBezier );
    g_object_set_data(tmpBezier, "ID", GINT_TO_POINTER(ObjType::BEZIER_SURFACE));

    GObject* tmpBSpline = G_OBJECT(gtk_builder_get_object( builder, "rb_bspline_surface" ));
    m_rbBSpline = GTK_WIDGET( tmpBSpline );
    g_object_set_data(tmpBSpline, "ID", GINT_TO_POINTER(ObjType::BSPLINE_SURFACE));

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
        (char*)"adj_ang_y",(char*)"adj_ang_z",(char*)"adj_ang_a",nullptr};

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
    m_entryAngA = GTK_WIDGET( gtk_builder_get_object( builder, "entry_ang_a" ) );

    m_objCenter = GTK_WIDGET( gtk_builder_get_object( builder, "tb_obj_center" ) );
    m_pntCenter = GTK_WIDGET( gtk_builder_get_object( builder, "rb_pnt_center" ) );
}

rotateType RotateDialog::getRotateType() const {
    if(isActive(m_objCenter))
        return rotateType::OBJECT;
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

void Object3dDialog::onClickEvent(GtkBuilder* builder){
    GtkListStore *liststore = GTK_LIST_STORE(m_model);
    GtkTreeIter iter;

    PolygonDialog dlogPoly(GTK_BUILDER(builder), true);

    if(dlogPoly.run() != 1)
        return;

    Coordinates c;
    dlogPoly.getCoords(c);

    std::string name = dlogPoly.getName();
    if(name == "")
        name = "face"+std::to_string(m_faces.size()+1);

    m_faces.emplace_back(name, dlogPoly.getColor(), c);

    // Cria uma nova linha na ListStore e
    // seta com o nome da face criada
    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(liststore, &iter, 0, name.c_str(), -1);
}

void SurfaceDialog::addCoordEvent(){
    // desabilitar os botoes
    gtk_widget_set_sensitive(GTK_WIDGET(m_rbBezier), false);
    gtk_widget_set_sensitive(GTK_WIDGET(m_rbBSpline), false);
    gtk_widget_set_sensitive(GTK_WIDGET(m_comboBox), false);
    gtk_widget_set_sensitive(GTK_WIDGET(m_btnAddSurface), false);

    // terminou de preencher a 'matriz'
    if(m_i == m_max_i)
        return;

    // monta a coordenada
    std::stringstream fmt;
    fmt << "(" << getX() << ", " << getY() << ", " << getZ() << ")";

    std::string coord = fmt.str();

    // salva a coord
    m_coords.emplace_back(getX(), getY(), getZ());

    // seta o texto da label na posição 'i' e 'j'
    GtkLabel* label = GTK_LABEL(gtk_grid_get_child_at(GTK_GRID(m_grid), m_j++, m_i));
    gtk_label_set_text(label, coord.c_str());

    // verifica se chego na ultima coluna
    if(m_j == m_max_j){
        m_j = 0;
        m_i++;
    }

    // Faz o SpinButton do 'x' pegar o focus da window
    gtk_widget_grab_focus(m_entryX);
}

void SurfaceDialog::addSurfaceEvent(){
    gtk_widget_set_sensitive(GTK_WIDGET(m_rbBezier), false);
    gtk_widget_set_sensitive(GTK_WIDGET(m_rbBSpline), false);

    int num = 0;
    if(m_type == ObjType::BEZIER_SURFACE){
        num = 3;
    }else if(m_type == ObjType::BSPLINE_SURFACE){
        num = 1;
    }

    char *tmpSide = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(m_comboBox));
    std::string side(tmpSide);
    delete tmpSide;

    if(side == "Ao lado"){
        for(int col = 0; col < num; col++){
            gtk_grid_insert_column(GTK_GRID(m_grid), m_max_j);

            for(int line = 0; line < m_max_i; line++){
                gtk_grid_attach (GTK_GRID(m_grid),
                     gtk_label_new(NULL),
                     m_max_j, line, 1, 1);
                gtk_widget_show(gtk_grid_get_child_at(GTK_GRID(m_grid), m_max_j, line));
            }
            ++m_max_j;
        }
    }else if(side == "Abaixo"){
        for(int line = 0; line < num; line++){
            gtk_grid_insert_row(GTK_GRID(m_grid), m_max_i);

            for(int col = 0; col < m_max_j; col++){
                gtk_grid_attach (GTK_GRID(m_grid),
                     gtk_label_new(NULL),
                     col, m_max_i, 1, 1);
                gtk_widget_show(gtk_grid_get_child_at(GTK_GRID(m_grid), col, m_max_i));
            }
            ++m_max_i;
        }
    }
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
    double x=0.0, y=0.0, z=0.0;

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

void SurfaceDialog::setSurfaceType(ObjType type){
    if(type != ObjType::BEZIER_SURFACE &&
       type != ObjType::BSPLINE_SURFACE)
        return;

    m_type = type;
}

const Coordinates& SurfaceDialog::getCoords() const{
    int size = m_coords.size();
    if(size < (m_max_i*m_max_j))
        throw MyException("Preencha todas as coordenadas.");
    return m_coords;
}
