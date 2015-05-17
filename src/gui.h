/*
 * File:   gui.h
 * Author: mion00
 *
 * Created on May 14, 2015, 9:51 PM
 */

#ifndef GUI_H
#define	GUI_H

#include <stdbool.h>

typedef enum GUIMode {
    STANDARD_CLIENT,
    STANDARD_SERVER,
    LOGIN_CLIENT,
    EXIT_CLIENT,
    EXIT_SERVER,
    LOG
} GUIMode;

GUIMode modalitaGUI;

void SetGUIMode(GUIMode mode);

void updateScreen();

#endif	/* GUI_H */

