/* simple_gui.c */
#include <gtk/gtk.h>

// 윈도우의 X 버튼을 눌렀을 때 호출될 함수 (프로그램 종료)
static void on_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    
    // 1. GTK 환경 초기화
    gtk_init(&argc, &argv);

    // 2. 윈도우 위젯 생성
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    // 3. 윈도우 속성 설정 (제목, 크기)
    gtk_window_set_title(GTK_WINDOW(window), "Linux GUI Lab");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // 4. 시그널 연결 (종료 이벤트)
    // "destroy" 시그널이 발생하면 on_destroy 함수를 실행하라
    g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

    // 5. 위젯을 화면에 표시
    gtk_widget_show_all(window);

    // 6. 메인 루프 진입 (이벤트 대기)
    gtk_main();

    return 0;
}
