#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

#define led_blue 11
#define led_red 13
#define led_green 12
#define button 5

bool semaforo_on = false;
int estado_atual = 0;
bool leds_em_transicao = false;

void set_init() {
    gpio_init(led_blue);
    gpio_init(led_red);
    gpio_init(led_green);

    gpio_set_dir(led_blue, GPIO_OUT);
    gpio_set_dir(led_red, GPIO_OUT);
    gpio_set_dir(led_green, GPIO_OUT);

    gpio_put(led_blue, false);
    gpio_put(led_red, false);
    gpio_put(led_green, false);

    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_down(button);
}

void mudar_semaforo(bool b, bool r, bool g) {
    gpio_put(led_blue, b);
    gpio_put(led_red, r);
    gpio_put(led_green, g);
}

bool timer_callback(repeating_timer_t *rt) {
    if (!semaforo_on || leds_em_transicao) {
        return true; // Não faz nada se os LEDs estão em transição ou se o semáforo estiver desligado
    }

    // Mudar para o próximo estado do semáforo
    estado_atual++;
    if (estado_atual >= 3) {
        estado_atual = 0; // Volta ao primeiro estado
    }

    // Atualizar LEDs conforme o estado atual
    mudar_semaforo(
        (estado_atual == 0), // Blue
        (estado_atual == 1), // Red
        (estado_atual == 2)  // Green
    );

    return true; // Continua o temporizador
}

void handle_button_press() {
    if (gpio_get(button) && !leds_em_transicao) {
        sleep_ms(400); // Debounce do botão

        // Inicia ou para o semáforo
        semaforo_on = !semaforo_on;
        if (semaforo_on) {
            // Começa o semáforo
            estado_atual = 0;
            mudar_semaforo(true, false, false); // Inicia com o primeiro LED aceso
        } else {
            // Desliga todos os LEDs
            mudar_semaforo(false, false, false);
        }
    }
}

int main() {
    set_init();
    stdio_init_all();

    repeating_timer_t timer;
    
    // Configura o temporizador para chamar a função de callback a cada 3 segundos
    add_repeating_timer_ms(3000, timer_callback, NULL, &timer);

    while (true) {
        handle_button_press(); // Verifica o pressionamento do botão

        // Aguarda para evitar a sobrecarga do processador
       
    }
}

