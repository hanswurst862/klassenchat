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

// Dieser Header deklariert die Klasse Lockfile

#ifndef LOCKFILE_HPP
#define LOCKFILE_HPP

#include "ui_lockfile.h"

/// Dieser Dialog zeigt eine Meldung an, dass kein %Admin im %Chat ist, und bietet einen Button zum Beenden des Chats.
class Lockfile final : public QDialog
{
    Q_OBJECT

public:
    explicit Lockfile( QWidget* parent = nullptr ); ///< Konstruktor

private:
    Ui::Lockfile ui {}; ///< UI des Dialogs
    void closeEvent( QCloseEvent* event ) override final; ///< Schließt den ganzen %Chat, statt nur den Dialog
};

#endif // LOCKFILE_HPP
