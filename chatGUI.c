/**
CMPSC 311: Introduction to Systems, Programming Project, Group 1
**/


#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "client.c"

#define MSG_LEN     100
#define NAME_LEN    32

// Global variables
int sockfd;
GtkWidget *display;
GtkWidget *typeText;
char name[] = "noname";
bool name_check = true;


// Function to send message on clcik of send button
void onClick(GtkWidget *widget, gpointer data) {
    const char *message = gtk_entry_get_text(GTK_ENTRY(typeText));
    if (strlen(message) > 0) {
        char fullMessage[MSG_LEN];
        if (name_check) {
            strncpy(name, message, NAME_LEN - 1);
            name[NAME_LEN - 1] = '\0'; // Ensure null termination for name
            snprintf(fullMessage, sizeof(fullMessage), "%s has joined!", name);
            fullMessage[sizeof(fullMessage) - 1] = '\0'; // Ensure null termination for fullMessage
            send_message(sockfd, fullMessage);
            name_check = false;
        } else {
            snprintf(fullMessage, sizeof(fullMessage), "%s: %s", name, message);
            fullMessage[sizeof(fullMessage) - 1] = '\0'; // Ensure null termination for fullMessage
            printf("DEBUG: fullMessage: %s\n", fullMessage); // Debug print
            send_message(sockfd, fullMessage);
        }
        gtk_entry_set_text(GTK_ENTRY(typeText), "");
    }
}

// Function to add text to the chat display
void displayMessage(GtkTextView *display, const char *text) {
    GtkTextBuffer *buf = gtk_text_view_get_buffer(display); //gets text buffer for display widget
    GtkTextIter end; //iterator for end of text buffer
    gtk_text_buffer_get_end_iter(buf, &end); //buffer points to the end
    gtk_text_buffer_insert(buf, &end, text, -1); //inputs message to buffer
    gtk_text_buffer_insert(buf, &end, "\n", -1); //inserts newline after text is displayed
}

// Function to receive message from server
// Function to receive message from server
void *getMessage(void *arg) {
    char buffer[MSG_LEN];
    int n = 1;
    while(n>0){
        struct Data data = receive_message(sockfd, MSG_LEN);
        n = data.integer;
        buffer[n] = '\0';
        strncpy(buffer, data.character, n);
        printf("DEBUG: fullMessage: %s\n", buffer); // Debug print
        displayMessage(GTK_TEXT_VIEW(display), buffer);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    sockfd = connect_server();

    /********Creation of GUI**********/
    gtk_init(&argc, &argv); //initializez GTK application

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL); //creates a window
    gtk_window_set_title(GTK_WINDOW(window), "Chatroom"); //sets title to window
    gtk_widget_set_name(window, "window"); //gives a name to whe widget
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400); //sets size of window
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL); //ensures when window closes, program terminates

   //creates a container box that will order widgest inside vertically
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *chatbox = gtk_scrolled_window_new(NULL, NULL); //creates a seprate window for chat display
    gtk_widget_set_size_request(chatbox, 400, 300); // set size of chatbox
    gtk_container_set_border_width(GTK_CONTAINER(chatbox), 5); // set border
    gtk_widget_set_name(chatbox, "chatbox"); // set a name to specfic widgets
    gtk_box_pack_start(GTK_BOX(vbox), chatbox, TRUE, TRUE, 12); //places widgets from the start of the container, also sets it so that the container expands with the chat box

    display = gtk_text_view_new(); // will display the messages
    gtk_widget_set_name(display, "Display");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(display), FALSE); //makes its so you cannot type in chat container
    gtk_container_add(GTK_CONTAINER(chatbox), display); //puts display inside the chatbox window

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);     //creates a container box that will order widgest inside horizontally
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);

    typeText = gtk_entry_new(); //new entry line for typing messages
    gtk_widget_set_name(typeText, "typeText");
    gtk_box_pack_start(GTK_BOX(hbox), typeText, TRUE, TRUE, 0);

  //  GtkTextBuffer *welcome =
  //  displayMessage(GTK_TEXT_VIEW(display), buffer);

    GtkWidget *Send = gtk_button_new_with_label("Send"); //creates button to send message
    gtk_widget_set_name(Send, "send");
    gtk_box_pack_start(GTK_BOX(hbox), Send, FALSE, FALSE, 0);
    g_signal_connect(Send, "clicked", G_CALLBACK(onClick), NULL); //will not expand with hbox


    GtkCssProvider *css_provider = gtk_css_provider_new(); // Applies CSS properties to specific widgets
    gtk_css_provider_load_from_data(css_provider,
        "#chatbox {"
        "  border: 2px solid red;"
        "  box-shadow: 0 0 5px 2px white;"
        "}"
        "#typeText {"
        "  border: 2px solid blue;"
        "  box-shadow: 0 0 5px 1px white;"
        "  background-color: deepskyblue;"
        "}"
        "#send {"
        "  border: 2px solid white;"
        "  box-shadow: 0 0 5px 1px white;"
        "}"
        "window {"
        "  background-color: darkslategrey;"
        "}"

        , -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_widget_show_all(window);

    //Welcome
    displayMessage(GTK_TEXT_VIEW(display), "ENTER NAME: \n");

    // Create thread to receive messages
    pthread_t recvThread;
    pthread_create(&recvThread, NULL, getMessage, NULL);

    gtk_main();

    return 0;
}
