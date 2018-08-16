/*
    SuperCollider Qt IDE
    Copyright (c) 2018 SuperCollider Team
    https://supercollider.github.io/

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "main.hpp"
#include <QBrush>

using namespace ScIDE;

void Main::setAppPaletteFromSettings() {
    const QTextCharFormat *format = &mSettings->getThemeVal("text");

    // QPalette::Window = general background color.
    QColor window = format->background().color();

    // QPalette::WindowText = general foreground color.
    QColor window_text = format->foreground().color();

    // QPalette::Button = background color of buttons.
    QColor button;

    // QPalette::Light = shadow for disabled text.
    QColor disabled_shadow;

    // QPalette::Mid = background color for the help and log dock bars as well
    // as inactive tabs.
    // QPalette::Dark = background color around the "Auto Scroll" button.
    // We use the same color for Dark and Mid currently.
    QColor mid;

    // QPalette::Highlight = highlight background color, used on menus.
    QColor highlight;

    // QPalette::Disabled / QPalette::Text = the foreground color of disabled
    // text and the selection background color.
    QColor disabled_text;

    int value_difference = window.value() - window_text.value();
    if (std::abs(value_difference) < 32) {
        // If we are on the darker end of the spectrum we lighten the background.
        if (window.value() < 127) {
            window = QColor::fromHsv(window.hue(),
                                      window.saturation(),
                                      window.value() + 32 - value_difference);
        } else {
            // Otherwise we can darken the foreground color.
            window_text = QColor::fromHsv(window_text.hue(),
                                      window_text.saturation(),
                                      window_text.value() - (32 - value_difference));
        }
    }

    // If we are using a dark text on light background we use the light background
    // color for active tabs, and darken it somewhat for inactive tabs.
    if (window_text.value() < window.value()) {
        button = window;
        mid = window.darker(125);
        highlight = window.darker(150);
        // Disabled text is rendered twice, once in disabled text foreground
        // color and once with a "shadow" color. We base the disabled text
        // colors here off the background color, to make them pop less than
        // regular text.
        disabled_text = window.darker(200);
        disabled_shadow = window.darker(250);
    } else {
        // When using light text on a dark background the active tab pops more
        // as a lighter version of the background color.
        mid = window;

        // QtColor::lighter() multiplies the value of the color by the provided
        // percentage factor, so if the value is zero it has no effect. In that
        // case we darken the foreground color, since hue information is lost
        // in maximum black.
        if (window.value() > 0) {
            button = window.lighter(150);
            highlight = window.lighter(175);
            disabled_shadow = mid.lighter(200);
            disabled_text = mid.lighter(225);
        } else {
            button = window_text.darker(300);
            highlight = window_text.darker(350);
            disabled_text = window_text.darker(400);
            disabled_shadow = window_text.darker(450);
        }
    }

    QPalette palette = QPalette(
        QBrush(window_text),      // windowText
        QBrush(button),           // button
        QBrush(disabled_shadow),  // light
        QBrush(mid),              // dark
        QBrush(mid),              // mid
        QBrush(window_text),      // text
        QBrush(window_text),      // bright_text
        QBrush(window),           // base
        QBrush(window)            // window
    );

    // Set a few other colors, namely the foreground color of disabled text
    // and the selection background color.
    palette.setBrush(QPalette::Disabled, QPalette::Text, QBrush(disabled_text));
    palette.setBrush(QPalette::Normal, QPalette::Highlight, QBrush(highlight));
    palette.setBrush(QPalette::Normal, QPalette::HighlightedText, QBrush(window_text));

    qApp->setPalette(palette);
}
