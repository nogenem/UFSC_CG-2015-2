#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include <gtk/gtk.h>
#include <string>
#include "Objects.hpp"
#include "MyException.hpp"

#define UI_FILE "window.glade"

enum class rotateType { OBJECT, POINT };

class Dialog
{
    public:
        Dialog(){}
        ~Dialog(){ destroy(); }
        int run();
        virtual void destroy();

    protected:
        GtkWidget *m_dialog = nullptr;
};

class FileDialog : public Dialog
{
    public:
        FileDialog(GtkBuilder* builder, bool toSave=false);
        char* const getFileName() const
            { return gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(m_dialog)); }
};

class ObjDialog : public Dialog
{
    public:
        ObjDialog() {}
        std::string const getName() const
            { return gtk_entry_get_text(GTK_ENTRY(m_entryName)); }
        const GdkRGBA& getColor() const { return m_color; }

        // Events
        void onColorChangeEvent(GtkColorButton* btn)
            {gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(btn), &m_color);}

    protected:
        GtkWidget* m_entryName = nullptr;
        GdkRGBA m_color{};
};

class PointDialog : public ObjDialog
{
    public:
        PointDialog(GtkBuilder* builder);
        double getX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryX)); }
        double getY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryY)); }
        double getZ() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryZ)); }

    private:
        GtkWidget *m_entryX = nullptr,
                  *m_entryY = nullptr,
                  *m_entryZ = nullptr;
};

class LineDialog : public ObjDialog
{
    public:
        LineDialog(GtkBuilder* builder);
        double getX1() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryX1)); }
        double getY1() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryY1)); }
        double getZ1() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryZ1)); }
        double getX2() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryX2)); }
        double getY2() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryY2)); }
        double getZ2() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryZ2)); }

    private:
        GtkWidget *m_entryX1 = nullptr, *m_entryY1 = nullptr, *m_entryZ1 = nullptr,
            *m_entryX2 = nullptr, *m_entryY2 = nullptr, *m_entryZ2 = nullptr;
};

class PolygonDialog : public ObjDialog
{
    public:
        PolygonDialog(GtkBuilder* builder, bool isFace=false);
        ~PolygonDialog(){ destroy(); }
        void destroy();

        void getCoords(Coordinates& coords) const;
        bool shouldBeFilled() const
            { return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_checkFilled)); }

        // Events
        virtual void onClickEvent();//Adiciona coordenada
        void onEditCelEvent(GtkCellRendererText *cell,
                            const gchar *path_string,
                            const gchar *new_text, int column);//Edita coordenada ja adicionada

    protected:
        GtkTreeModel *m_model = nullptr;
        GtkWidget *m_entryX = nullptr, *m_entryY = nullptr, *m_entryZ = nullptr,
                  *m_checkFilled = nullptr;
};

class CurveDialog : public PolygonDialog
{
    public:
        CurveDialog(GtkBuilder* builder);
        ~CurveDialog(){ destroy(); }
};

class Object3dDialog : public ObjDialog
{
    public:
        Object3dDialog(GtkBuilder* builder);
        ~Object3dDialog(){ destroy(); }
        void destroy();

        FaceList& getFaces(){ return m_faces; }

        // Events
        void onClickEvent(GtkBuilder* builder);//Adiciona face

    protected:
        GtkTreeModel *m_model = nullptr;

        FaceList m_faces;
};

class SurfaceDialog : public ObjDialog
{
    public:
        SurfaceDialog(GtkBuilder* builder);
        ~SurfaceDialog(){ destroy(); }

        double getX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryX)); }
        double getY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryY)); }
        double getZ() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryZ)); }

        ObjType getSurfaceType(){ return m_type; }
        int getMaxLines(){ return m_max_i; }
        int getMaxCols(){ return m_max_j; }

        void setSurfaceType(ObjType type);

        const Coordinates& getCoords() const;

        // Events
        void addCoordEvent();//Adiciona coordenada
        void addSurfaceEvent();//Adiciona mais linhas/colunas a 'matriz'

    protected:
        GtkWidget *m_entryX = nullptr, *m_entryY = nullptr, *m_entryZ = nullptr,
                *m_comboBox = nullptr, *m_grid = nullptr;
        GtkWidget *m_btnAddSurface=nullptr, *m_rbBezier=nullptr, *m_rbBSpline=nullptr;

        ObjType m_type = ObjType::BEZIER_SURFACE;

        int m_i = 0, m_j = 0, m_max_i = 4, m_max_j = 4;
        Coordinates m_coords;
};

class TranslateDialog : public Dialog
{
    public:
        TranslateDialog(GtkBuilder* builder);
        double getDX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryDX)); }
        double getDY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryDY)); }
        double getDZ() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryDZ)); }

    private:
        GtkWidget *m_entryDX = nullptr,
                  *m_entryDY = nullptr,
                  *m_entryDZ = nullptr;
};

class ScaleDialog : public Dialog
{
    public:
        ScaleDialog(GtkBuilder* builder);
        double getSX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entrySX)); }
        double getSY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entrySY)); }
        double getSZ() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entrySZ)); }

    private:
        GtkWidget *m_entrySX = nullptr,
                  *m_entrySY = nullptr,
                  *m_entrySZ = nullptr;
};

class RotateDialog : public Dialog
{
    public:
        RotateDialog(GtkBuilder* builder);

        double getAnguloX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryAngX)); }
        double getAnguloY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryAngY)); }
        double getAnguloZ() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryAngZ)); }
        double getAnguloA() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryAngA)); }
        double getCX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryCX)); }
        double getCY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryCY)); }
        double getCZ() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_entryCZ)); }
        rotateType getRotateType() const; // Retorna o tipo de rotacao escolhido pelo usuario

    private:
        bool isActive(GtkWidget *widget) const
            { return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)); }

    private:
        GtkWidget *m_entryAngX = nullptr, *m_entryAngY = nullptr, *m_entryAngZ = nullptr,
            *m_entryCX = nullptr, *m_entryCY = nullptr, *m_entryCZ = nullptr,
            *m_objCenter = nullptr, *m_pntCenter = nullptr, *m_entryAngA = nullptr;
};

#endif // DIALOGS_HPP
