#include "core.h"

int main(int argc, char* argv[])
{
    system("CLS");
    Model* text_editor = new Model();
    Controller* ctrl = new Controller(text_editor);
    View* cur_view = new View(ctrl);

    string_t name_file;

    if (argc == 1) {
        name_file = "temp.txt";
    }
    else if (argc == 2) {
        name_file = "temp.txt";
    }
    cur_view->update_window_information();
    cur_view->update_state_line(ctrl->read_text(name_file));

    cur_view->main_loop();
    return 0;
}