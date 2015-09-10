#ifndef DIALOGS_HPP
#define DIALOGS_HPP

#include <gtk/gtk.h>
#include <string>
#include "Objects.hpp"
#include "MyException.hpp"

#define UI_FILE "window.glade"

enum class rotateType { OBJECT, WORLD, POINT };

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
            { return gtk_entry_get_text(GTK_ENTRY(_entryName)); }
        const GdkRGBA& getColor() const { return _color; }

        // Events
        void onColorChangeEvent(GtkColorButton* btn)
            {gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(btn), &_color);}

    protected:
        GtkWidget* _entryName = nullptr;
        GdkRGBA _color{};
};

class PointDialog : public ObjDialog
{
    public:
        PointDialog(GtkBuilder* builder);
        double getX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryX)); }
        double getY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryY)); }

    private:
        GtkWidget *_entryX = nullptr,
                  *_entryY = nullptr;
};

class LineDialog : public ObjDialog
{
    public:
        LineDialog(GtkBuilder* builder);
        double getX1() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryX1)); }
        double getY1() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryY1)); }
        double getX2() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryX2)); }
        double getY2() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryY2)); }

    private:
        GtkWidget *_entryX1 = nullptr, *_entryY1 = nullptr,
            *_entryX2 = nullptr, *_entryY2 = nullptr;
};

class PolygonDialog : public ObjDialog
{
    public:
        PolygonDialog(GtkBuilder* builder);
        ~PolygonDialog(){ destroy(); }
        void destroy();

        void getCoords(Coordinates& coords) const;
        bool shouldBeFilled() const
            { return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(_checkFilled)); }

        // Events
        void onClickEvent();//Adiciona coordenada
        void onEditCelEvent(GtkCellRendererText *cell,
                            const gchar *path_string,
                            const gchar *new_text, int column);//Edita coordenada ja adicionada

    private:
        GtkTreeModel *_model = nullptr;
        GtkWidget *_entryX = nullptr, *_entryY = nullptr,
                  *_checkFilled = nullptr;
};

class TranslateDialog : public Dialog
{
    public:
        TranslateDialog(GtkBuilder* builder);
        double getDX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryDX)); }
        double getDY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryDY)); }

    private:
        GtkWidget *_entryDX = nullptr,
                  *_entryDY = nullptr;
};

class ScaleDialog : public Dialog
{
    public:
        ScaleDialog(GtkBuilder* builder);
        double getSX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entrySX)); }
        double getSY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entrySY)); }

    private:
        GtkWidget *_entrySX = nullptr,
                  *_entrySY = nullptr;
};

class RotateDialog : public Dialog
{
    public:
        RotateDialog(GtkBuilder* builder);

        double getAngulo() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryAngulo)); }
        double getCX() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryCX)); }
        double getCY() const
            { return gtk_spin_button_get_value(GTK_SPIN_BUTTON(_entryCY)); }
        rotateType getRotateType() const; // Retorna o tipo de rotacao escolhido pelo usuario

    private:
        bool isActive(GtkWidget *widget) const
            { return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)); }

    private:
        GtkWidget *_entryAngulo = nullptr, *_entryCX = nullptr,
                  *_entryCY = nullptr, *_objCenter = nullptr,
                  *_worldCenter = nullptr, *_pntCenter = nullptr;
};

#endif // DIALOGS_HPP
