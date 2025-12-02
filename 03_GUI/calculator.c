/* calculator.c */
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 위젯들을 전역으로 선언하여 콜백 함수에서 접근 가능하게 함
GtkWidget *entry;

// 버튼 클릭 시 호출될 함수 (숫자 및 연산자 입력)
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *label = gtk_button_get_label(GTK_BUTTON(widget));
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));
    
    // 현재 텍스트 뒤에 버튼의 글자(숫자/연산자)를 이어 붙임
    gchar *new_text = g_strconcat(current_text, label, NULL);
    gtk_entry_set_text(GTK_ENTRY(entry), new_text);
    g_free(new_text);
}

// 'C' (Clear) 버튼 클릭 시 호출될 함수
static void on_clear_clicked(GtkWidget *widget, gpointer data) {
    gtk_entry_set_text(GTK_ENTRY(entry), "");
}

// '=' (Equal) 버튼 클릭 시 호출될 함수 (계산 로직)
static void on_equal_clicked(GtkWidget *widget, gpointer data) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    double num1, num2, result;
    char op;
    char result_str[100];

    // 간단한 파싱 (예: "12+34")
    // sscanf를 사용하여 숫자, 연산자, 숫자를 분리
    if (sscanf(text, "%lf%c%lf", &num1, &op, &num2) == 3) {
        switch(op) {
            case '+': result = num1 + num2; break;
            case '-': result = num1 - num2; break;
            case '*': result = num1 * num2; break;
            case '/': 
                if (num2 == 0) {
                    gtk_entry_set_text(GTK_ENTRY(entry), "Error");
                    return;
                }
                result = num1 / num2; 
                break;
            default: 
                gtk_entry_set_text(GTK_ENTRY(entry), "Error");
                return;
        }
        // 결과를 문자열로 변환하여 표시 (소수점 2자리까지)
        snprintf(result_str, sizeof(result_str), "%.2f", result);
        gtk_entry_set_text(GTK_ENTRY(entry), result_str);
    } else {
        gtk_entry_set_text(GTK_ENTRY(entry), "Error");
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    
    // 버튼 라벨 배열 (4x4 배치)
    const char *buttons[] = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "C", "0", "=", "+"
    };

    gtk_init(&argc, &argv);

    // 1. 윈도우 설정
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 250, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 2. 그리드(Grid) 레이아웃 생성
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // 3. 입력창(Entry) 생성 및 배치 (맨 윗줄)
    entry = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(entry), 1.0); // 오른쪽 정렬
    // (grid, widget, left, top, width, height)
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    // 4. 버튼 생성 및 배치
    for (int i = 0; i < 16; i++) {
        button = gtk_button_new_with_label(buttons[i]);
        
        // 버튼 종류에 따라 다른 콜백 함수 연결
        if (strcmp(buttons[i], "=") == 0) {
            g_signal_connect(button, "clicked", G_CALLBACK(on_equal_clicked), NULL);
        } else if (strcmp(buttons[i], "C") == 0) {
            g_signal_connect(button, "clicked", G_CALLBACK(on_clear_clicked), NULL);
        } else {
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
        }

        // 그리드에 부착 (i/4는 행, i%4는 열)
        // 입력창이 0번 행이므로 버튼은 1번 행부터 시작
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, (i / 4) + 1, 1, 1);
    }

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
