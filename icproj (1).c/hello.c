#include <gtk/gtk.h>

static void on_button_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    
    dialog = gtk_dialog_new_with_buttons("How to use Seiri",
                                         GTK_WINDOW(data),
                                         GTK_DIALOG_MODAL,
                                         "OK", GTK_RESPONSE_OK,
                                         NULL);

    GtkWidget *label = gtk_label_new("You can go to VScode to predict your periods and learn some facts or hear some jokes through my interactive interface based on your mood :) \n If you want to interact with me further go to VScode!!");
    gtk_box_append(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label);
    gtk_widget_set_visible(dialog, TRUE);

    g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), dialog);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *button;

    // Create a new window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Seiri");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // Create the button with the given label sucht that when we click on it it opens another dialogue box
    button = gtk_button_new_with_label("Hi, I am Seiri. I hope you are keeping well! Click to reveal. \u2665 \n ");

    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), window); //click button

    
    gtk_window_set_child(GTK_WINDOW(window), button); //window button

   
    gtk_window_present(GTK_WINDOW(window)); //visible window
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;
    //gtk application which runs on my system
    app = gtk_application_new("org.gtk.seiri", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
//run the appppppp
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
