#include <config.h>
#include <spriteszx.h>
#include "lib/motorzx.h"
#include "lib/sonidoszx.h"

#pragma output STACKPTR=65000

#define GRAVEDAD 1
#define SALTO -5
#define VELOCIDAD_MAX_CAIDA 5

unsigned char mapa [] = {
1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,1,0,0,0,0,1,1,
1,0,0,0,1,0,1,0,0,1,1,
1,0,0,0,1,0,1,0,0,1,1,
1,0,0,0,1,0,1,0,0,1,1,
1,0,0,0,1,0,1,0,2,1,1,
1,0,0,0,1,0,1,1,1,1,1,
1,0,2,0,0,0,0,0,2,1,1,
1,0,0,0,0,0,0,0,0,1,1,
1,1,1,1,1,1,1,1,1,1,1,};

unsigned char pantalla [768];

void main (void)
{
    unsigned char x, y, z, frame;
    signed char velocidad_y = 0; // Velocidad vertical del personaje
    unsigned char en_suelo = 0;  // Bandera para saber si el personaje está en el suelo

    cls (7); // Borramos la pantalla
    port_out (254,0); // Ponemos el borde de color negro
    frame = 0;

    // Dibujamos el mapa
    x = 0;
    y = 0;
    for (z = 0; z < 110; ++z)
    {
        switch (mapa[z])
        {
            case 0: put_sprite_x16(sprite_negro, x, y); frame = 0; break;
            case 1: put_sprite_x16(sprite_cubo, x, y); frame = 1; break;
            case 2: put_sprite_x16(sprite_moneda, x, y); frame = 2; break;
        }
        pantalla[x + (y * 32)] = frame;
        pantalla[x + 1 + (y * 32)] = frame;
        pantalla[x + ((y + 1) * 32)] = frame;
        pantalla[x + 1 + ((y + 1) * 32)] = frame;

        ++x;
        ++x;
        if (x == 22)
        {
            x = 0;
            ++y;
            ++y;
        }
    }

    x = 2;
    y = 2;
    z = 0;
    put_sprite_x16(sprite_prota1, x, y);

    while (1)
    {
        // Comprobar si hay suelo debajo del personaje
        if (pantalla[x + ((y + 2) * 32)] == 0 && pantalla[x + 1 + ((y + 2) * 32)] == 0)
        {
            en_suelo = 0;  // No hay suelo, aplicar gravedad
        }
        else
        {
            en_suelo = 1;  // Hay suelo, desactivar gravedad
            velocidad_y = 0; // Resetear la velocidad vertical cuando está en el suelo
        }

        // Manejar salto
        if ((port_in(64510) & 1) == 0 && en_suelo) // Q para saltar
        {
            velocidad_y = SALTO; // Velocidad hacia arriba
            en_suelo = 0;
        }

        // Aplicar gravedad si no estamos en el suelo
        if (!en_suelo)
        {
            if (velocidad_y < VELOCIDAD_MAX_CAIDA)
                velocidad_y += GRAVEDAD; // Aumentar la velocidad de caída
        }

        // Actualizar posición vertical
        if (velocidad_y < 0) // Movimiento hacia arriba
        {
            if (y > 0 && pantalla[x + ((y - 1) * 32)] == 0 && pantalla[x + 1 + ((y - 1) * 32)] == 0)
            {
                put_partial1v_sprite_x16(sprite_negro, x, y + 1);
                y--;
            }
            else
            {
                velocidad_y = 0; // Si hay colisión con el techo, detener movimiento vertical
            }
        }
        else if (velocidad_y > 0) // Movimiento hacia abajo
        {
            if (y < 22 && pantalla[x + ((y + 2) * 32)] == 0 && pantalla[x + 1 + ((y + 2) * 32)] == 0)
            {
                put_partial1v_sprite_x16(sprite_negro, x, y);
                y++;
                en_suelo = 0; // El personaje está en el aire
            }
            else
            {
                velocidad_y = 0;
                en_suelo = 1; // El personaje está en el suelo
            }
        }

        // Movimiento lateral
        if ((port_in(57342) & 1) == 0 && x < 30 && pantalla[x + 2 + (y * 32)] == 0 && pantalla[x + 2 + ((y + 1) * 32)] == 0) // P
        {
            put_partial1h_sprite_x16(sprite_negro, x, y);
            z = 1;
            ++x;
        }
        if ((port_in(57342) & 2) == 0 && x > 0 && pantalla[x - 1 + (y * 32)] == 0 && pantalla[x - 1 + ((y + 1) * 32)] == 0) // O
        {
            put_partial1h_sprite_x16(sprite_negro, x + 1, y);
            z = 1;
            --x;
        }

        // Actualizar el sprite del personaje
        if (z || velocidad_y != 0)
        {
            if (frame == 0)
            {
                put_sprite_x16(sprite_prota1, x, y);
                frame = 1;
            }
            else
            {
                put_sprite_x16(sprite_prota2, x, y);
                frame = 0;
            }
            z = 0;
            sonido_andar();
        }

        // Espera para sincronizar frames
        wait_int();
        wait_int();
        wait_int();
        wait_int();
    }
}

