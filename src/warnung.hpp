/* Copyright (C) 2015 Lukas Bartl
 * Diese Datei ist Teil des Klassenchats.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Dieser Header deklariert die Klasse Warnung

#ifndef WARNUNG_HPP
#define WARNUNG_HPP

#include "ui_warnung.h"

class Chat;

///\cond
class Warnung : public QDialog
{
    Q_OBJECT

public:
    explicit Warnung( Chat* parent );

private:
    Ui::Warnung ui {};
};
///\endcond

#endif // WARNUNG_HPP
