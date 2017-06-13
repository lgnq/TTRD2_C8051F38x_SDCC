#include "main.h"

void key_init(void)
{
    P3MDOUT = 0x00;
    P3SKIP  = 0xFF;
}

unsigned char key_read(void)
{
    static unsigned char key_state = KEY_STATE_0, key_time = 0, key_temp = N_KEY;
    unsigned char key_value, key_press, key_return = N_KEY;

    key_value = KEY_INPUT;

    if (key_value == 0)
    {
        //LED_G = 0;
        key_press = 0;
    }
    else
    {
        //LED_G = 1;
        key_press = 1;
    }

    switch (key_state)
    {
        case KEY_STATE_0:
            if (key_press)
            {
                key_temp = key_value;
                key_state = KEY_STATE_1;
            }
        break;

        case KEY_STATE_1:
            if ((key_press) && (key_temp == key_value))
            {
                key_time = 0;
                key_state = KEY_STATE_2;
            }
            else
                key_state = KEY_STATE_0;
        break;

        case KEY_STATE_2:
            if (!key_press)
            {
                key_return = key_temp;
                key_state = KEY_STATE_0;
            }
            else
            {
                if (key_temp == key_value)
                    key_time++;
                else
                    key_state = KEY_STATE_0;

                if (key_time >= 250)
                {
                    key_return = key_temp | 0x80;
                    key_state = KEY_STATE_3;
                }
            }
        break;

        case KEY_STATE_3:
            if (!key_press)
                key_state = KEY_STATE_0;
        break;

        default:
            key_state = KEY_STATE_0;
        break;
    }

    return key_return;
}

void key_update(void)
{
    unsigned char key;

    key = key_read();

    if (key != N_KEY)
    {
        switch (key)
        {
            case KEY_MODE:
                UART2_BUF_O_Write_String_To_Buffer("KEY_MODE\n");
                reset_heartbeat_flag = 1;
                break;
            case KEY_ENTER:
                UART2_BUF_O_Write_String_To_Buffer("KEY_ENTER\n");
                break;
            case KEY_UP:
                UART2_BUF_O_Write_String_To_Buffer("KEY_UP\n");
                break;
            case KEY_DOWN:
                UART2_BUF_O_Write_String_To_Buffer("KEY_DOWN\n");
                break;
            case KEY_RIGHT:
                UART2_BUF_O_Write_String_To_Buffer("KEY_RIGHT\n");
                break;
            case KEY_LEFT:
                UART2_BUF_O_Write_String_To_Buffer("KEY_LEFT\n");
                break;

            case KEY_L_MODE:
                UART2_BUF_O_Write_String_To_Buffer("KEY_L_MODE\n");
                break;
            case KEY_L_ENTER:
                UART2_BUF_O_Write_String_To_Buffer("KEY_L_ENTER\n");
                break;
            case KEY_L_UP:
                UART2_BUF_O_Write_String_To_Buffer("KEY_L_UP\n");
                break;
            case KEY_L_DOWN:
                UART2_BUF_O_Write_String_To_Buffer("KEY_L_DOWN\n");
                break;
            case KEY_L_RIGHT:
                UART2_BUF_O_Write_String_To_Buffer("KEY_L_RIGHT\n");
                break;
            case KEY_L_LEFT:
                UART2_BUF_O_Write_String_To_Buffer("KEY_L_LEFT\n");
                break;

            default:
                UART2_BUF_O_Write_String_To_Buffer("UNKNOW_KEY\n");
                break;
        }
    }
}

