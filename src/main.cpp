#include <Arduino.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include "LGFX_ESP32_2432S028.h"
#include <lvgl.h>

LGFX gfx;

// --- Variabili Globali ---
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[320 * 20];

// Oggetti UI
lv_obj_t * screen_main;
lv_obj_t * screen_terminal;
lv_obj_t * screen_sensors;

lv_obj_t * ui_textarea;      // Schermata 1
lv_obj_t * label_temp_val;   // Schermata 2
lv_obj_t * label_hum_val;    // Schermata 2
lv_obj_t * label_counter;    // Schermata 1
int counter = 0;

// --- Callback Navigazione ---
static void start_terminal_cb(lv_event_t * e) { lv_scr_load(screen_terminal); }
static void start_sensors_cb(lv_event_t * e) { lv_scr_load(screen_sensors); }

// --- Callback Contatore ---
static void btn_inc_cb(lv_event_t * e) {
    counter++;
    lv_label_set_text_fmt(label_counter, "Conteggio: %d", counter);
}

// --- Funzioni di Creazione Schermate ---

void create_screen_main() {
    screen_main = lv_obj_create(NULL);
    
    lv_obj_t * btn1 = lv_btn_create(screen_main);
    lv_obj_set_size(btn1, 200, 50);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);
    lv_obj_add_event_cb(btn1, start_terminal_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * l1 = lv_label_create(btn1);
    lv_label_set_text(l1, "Apri Terminale");
    lv_obj_center(l1);

    lv_obj_t * btn2 = lv_btn_create(screen_main);
    lv_obj_set_size(btn2, 200, 50);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_event_cb(btn2, start_sensors_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * l2 = lv_label_create(btn2);
    lv_label_set_text(l2, "Apri Sensori");
    lv_obj_center(l2);
}

void create_screen_terminal() {
    screen_terminal = lv_obj_create(NULL);
    
    // Bottone + Label (Riga in alto)
    lv_obj_t * btn = lv_btn_create(screen_terminal);
    lv_obj_set_size(btn, 100, 40);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(btn, btn_inc_cb, LV_EVENT_CLICKED, NULL);
    lv_label_set_text(lv_label_create(btn), "+1");

    label_counter = lv_label_create(screen_terminal);
    lv_label_set_text(label_counter, "Conteggio: 0");
    lv_obj_align(label_counter, LV_ALIGN_TOP_RIGHT, -20, 20);

    // TextArea
    ui_textarea = lv_textarea_create(screen_terminal);
    lv_obj_set_size(ui_textarea, 300, 160);
    lv_obj_align(ui_textarea, LV_ALIGN_BOTTOM_MID, 0, -10);
}

void create_screen_sensors() {
    screen_sensors = lv_obj_create(NULL);
    // Impostiamo uno sfondo leggermente diverso per la pagina se vuoi
    lv_obj_set_style_bg_color(screen_sensors, lv_palette_lighten(LV_PALETTE_GREY, 4), 0);

    // --- RIGA TEMPERATURA ---
    lv_obj_t * l_t = lv_label_create(screen_sensors);
    lv_label_set_text(l_t, "Temperatura");
    lv_obj_align(l_t, LV_ALIGN_TOP_LEFT, 20, 50);
    lv_obj_set_style_text_font(l_t, &lv_font_montserrat_14, 0);

    // Contenitore per il valore Temperatura (il "box" grigio)
    lv_obj_t * cont_temp = lv_obj_create(screen_sensors);
    lv_obj_set_size(cont_temp, 120, 45);
    lv_obj_align(cont_temp, LV_ALIGN_TOP_RIGHT, -20, 35);
    
    // Stile del Box
    lv_obj_set_style_bg_color(cont_temp, lv_palette_lighten(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_border_width(cont_temp, 2, 0);
    lv_obj_set_style_border_color(cont_temp, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_radius(cont_temp, 10, 0); // Angoli arrotondati
    lv_obj_clear_flag(cont_temp, LV_OBJ_FLAG_SCROLLABLE); // Evita scrollbar interne

    label_temp_val = lv_label_create(cont_temp);
    lv_label_set_text(label_temp_val, "--.- C");
    lv_obj_center(label_temp_val); // Centra il testo nel box
    lv_obj_set_style_text_font(label_temp_val, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(label_temp_val, lv_palette_main(LV_PALETTE_RED), 0);

    // --- RIGA UMIDITÀ ---
    lv_obj_t * l_h = lv_label_create(screen_sensors);
    lv_label_set_text(l_h, "Umidita'");
    lv_obj_align(l_h, LV_ALIGN_TOP_LEFT, 20, 120);
    lv_obj_set_style_text_font(l_h, &lv_font_montserrat_14, 0);

    // Contenitore per il valore Umidità
    lv_obj_t * cont_hum = lv_obj_create(screen_sensors);
    lv_obj_set_size(cont_hum, 120, 45);
    lv_obj_align(cont_hum, LV_ALIGN_TOP_RIGHT, -20, 105);
    
    // Stile del Box (uguale a sopra)
    lv_obj_set_style_bg_color(cont_hum, lv_palette_lighten(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_border_width(cont_hum, 2, 0);
    lv_obj_set_style_border_color(cont_hum, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_set_style_radius(cont_hum, 10, 0);
    lv_obj_clear_flag(cont_hum, LV_OBJ_FLAG_SCROLLABLE);

    label_hum_val = lv_label_create(cont_hum);
    lv_label_set_text(label_hum_val, "-- %");
    lv_obj_center(label_hum_val);
    lv_obj_set_style_text_font(label_hum_val, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_color(label_hum_val, lv_palette_main(LV_PALETTE_BLUE), 0);
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx.startWrite(); // Blocca il bus SPI per il display
    gfx.setAddrWindow(area->x1, area->y1, w, h);
    // Torniamo alla formula magica del primo test
    gfx.pushPixels((uint16_t *)&color_p->full, w * h, true);
    gfx.endWrite(); // Rilascia il bus SPI

    lv_disp_flush_ready(disp);
}
void my_touch_read(lv_indev_drv_t *d, lv_indev_data_t *data) {
    uint16_t x, y;
    if(gfx.getTouch(&x, &y)) { data->state=LV_INDEV_STATE_PR; data->point.x=x; data->point.y=y; }
    else data->state=LV_INDEV_STATE_REL;
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(10);
    gfx.init(); gfx.setRotation(1); gfx.setBrightness(128);
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, 320 * 20);
    static lv_disp_drv_t d_drv; lv_disp_drv_init(&d_drv);
    d_drv.hor_res=320; d_drv.ver_res=240; d_drv.flush_cb=my_disp_flush; d_drv.draw_buf=&draw_buf;
    lv_disp_drv_register(&d_drv);
    static lv_indev_drv_t i_drv; lv_indev_drv_init(&i_drv);
    i_drv.type=LV_INDEV_TYPE_POINTER; i_drv.read_cb=my_touch_read;
    lv_indev_drv_register(&i_drv);

    create_screen_main();
    create_screen_terminal();
    create_screen_sensors();

    lv_scr_load(screen_main); // Carica la home all'avvio
}

void loop() {
    lv_timer_handler();
    if (Serial.available()) {
        String msg = Serial.readStringUntil('\n');
        msg.trim();
        if (msg.length() > 0) {
            // Parsing logica "t=" o "h="
            if (msg.startsWith("t=")) {
                lv_label_set_text(label_temp_val, (msg.substring(2) + " C").c_str());
            } else if (msg.startsWith("h=")) {
                lv_label_set_text(label_hum_val, (msg.substring(2) + " %").c_str());
            } else {
                // Se siamo nella schermata terminale, scriviamo lì
                if(lv_scr_act() == screen_terminal) {
                    lv_textarea_add_text(ui_textarea, (msg + "\n").c_str());
                }
            }
        }
    }
    delay(5);
}