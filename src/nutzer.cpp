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

// Diese Datei definiert die Klasse Nutzer

#include "nutzer.hpp"
#include "hineinschreiben.hpp"

/**
 * @param nutzer Chat::nutzer_h
 * @param admins Chat::admins_h
 * @param parent Parent
 */
Nutzer::Nutzer( Hineinschreiben const& nutzer, Hineinschreiben const& admins, QWidget* parent ) :
    QDialog( parent )
{
    ui.setupUi( this );

    this -> setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

    for ( QString const& currnutzer : nutzer )
        ui.listWidget -> addItem( admins.hineingeschrieben( currnutzer ) ? currnutzer + " (Admin)" : currnutzer ); // Falls currnutzer ein Admin ist " (Admin)" anhängen

    ui.listWidget -> sortItems();
}
