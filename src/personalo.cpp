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

// Diese Datei definiert die Klasse PersonalO

#include "personalo.hpp"
#include "chat.hpp"
#include <QPushButton>

/**
 * @param benutzername Chat::nutzername
 * @param nutzer Chat::nutzer_h
 * @param name_arg Voreingestellter Benutzername
 * @param parent Parent
 */
PersonalO::PersonalO( std::string const& benutzername, Hineinschreiben const& nutzer, std::string const& name_arg, Chat* parent ) :
    QDialog( parent ),
    chat_par( parent )
{
    ui.setupUi( this );
    ui.buttonBox -> button( QDialogButtonBox::Cancel ) -> setText("Abbrechen");

    this -> setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

    ui.comboBox -> insertItem( 0, "" );

    for ( std::string const& currnutzer : nutzer.namen_meinchat() )
        if ( currnutzer == name_arg )
            ui.comboBox -> setItemText( 0, QString::fromStdString( name_arg ) );
        else if ( currnutzer != benutzername )
            ui.comboBox -> addItem( QString::fromStdString( currnutzer ) );

    ui.comboBox -> setCurrentIndex( 0 );
    ui.comboBox -> model() -> sort( 0 );

    connect( this, &PersonalO::accepted, [this] () { start(); } );
}

///\cond
void PersonalO::start() { // Neuen Chat starten
    QString const partner = ui.comboBox -> currentText(); // der ausgewählte Benutzername

    partner == "" ? qWarning("Keinen Namen ausgewählt!")
                  : chat_par -> make_chat( partner.toStdString() );
}
///\endcond