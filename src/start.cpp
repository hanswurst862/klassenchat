/* Copyright (C) 2015,2016 Lukas Bartl
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

// Diese Datei steuert den Start und das Beenden des Chats

#include "chat.hpp"
#include "lockfile.hpp"
#include "simpledialog.hpp"
#include "filesystem.hpp"
#include "global.hpp"
#include "klog.hpp"

using namespace static_paths;
using std::make_unique;

inline bool lockfile_exist( Datei const& lockfile ) {
    if ( lockfile.exist() )
        return true;
    else {
        this_thread::sleep_for( 200ms ); // Es könnte sein, dass währenddessen jemand das Lockfile erstellt
        return lockfile.exist();
    }
}

namespace {
    std::string nutzername; // mein Benutzername am Anfang
}

/// Starten des Chats. Aufgerufen nach dem Anmelden.
/**
 * 1. Überprüfen, ob mein Benutzername verboten ist, wenn ja, dann nochmal einen Namen eingeben.
 * 2. Prüft ob mein Benutzername in #std_admins vorhanden ist, und setzt danach #std_admin.
 * 3. Überprüfen, ob jemand im %Chat ist, wenn nicht lösche ich den Chatverlauf (#std_admin) oder verlasse den %Chat (Lockfile).
 * 4. Setzen von #checkfile, #terminatefile und #infofile.
 * 5. Überprüfen, ob mein Benutzername vergeben ist, wenn ja, dann nochmal einen Namen eingeben.
 * 6. Wenn ich ein #std_admin bin, wird das Passwort-Feld geöffnet, wenn nicht, wird start2() aufgerufen.
 */
void Chat::start() {
    nutzername = ui.NutzernameA -> text().toStdString(); // mein Benutzername

    if ( nutzername == "" ) {
        qWarning("Keinen Namen eingegeben!");
        ui.NutzernameA->setFocus();
        return;
    }

    if ( verboten( nutzername ) ) { // Prüfen ob Benutzername verboten ist
        klog("Nutername verboten!");
        createDialog( "Fehler", "Ein Admin hat diesen Benutzernamen verboten!\nBitte einen anderen Benutzernamen wählen!", this, true );
        ui.NutzernameA->setFocus();
        return;
    }

    checkfile = makeToNutzerDatei( checkdir, x_plum_anfang, nutzername );
    terminatefile = makeToNutzerDatei( terminatedir, x_plum_anfang, nutzername );
    infofile = makeToNutzerDatei( infodir, x_plum_anfang, nutzername );

    bool lock = lockfile_exist( *lockfile );
    flags[std_admin] = enthaelt( Chat::std_admins, nutzername ); // Standard-Admin

    if ( ! lock ) // Niemand ist im Chat
        if ( flags[std_admin] || x_plum_anfang ) {
            resetcv();

            if ( ! lockfile_exist( x_plum_anfang ? lockfile_norm : lockfile_plum ) ) { // Lockfile des anderen Chats
                nutzer_verwaltung.reset();

                senddir.removeInhalt();
                infodir.removeInhalt();
                terminatedir.removeInhalt();
                checkdir.removeInhalt();
            }
        } else {
            klog("Kein Admin im Chat!");
            Lockfile( this ).exec();
            return;
        }
    else if ( nutzer_verwaltung.getNutzer( x_plum_anfang, nutzername ) ) { // Prüfen ob Benutzername vergeben ist
        checkfile.touch();
        this_thread::sleep_for( 500ms ); // Warten bis die Datei von dem Nutzer gelöscht wird

        if ( checkfile.exist() ) { // Bei diesem Nutzer ist der Chat abgestürzt
            checkfile.remove();
            lockfile_mtx.lock();
                lockfile->remove();
            lockfile_mtx.unlock();
        } else {
            klog("Nutzername vergeben!");
            createDialog( "Fehler", "Es ist bereits jemand mit diesem Benutzernamen angemeldet!\nBitte einen anderen Benutzernamen wählen!", this, true );
            ui.NutzernameA->setFocus();
            return;
        }
    }

    if ( flags[std_admin] ) {
        klog("Admin, öffne Passwortfeld...");

        ui.Label2 -> setText( QString::fromStdString( "Passwort für " + nutzername + ": " ) );
        ui.LineStackedWidget -> setCurrentIndex( 1 ); // Passwort
    } else
        start2();
}

/// Überprüft das %Passwort. Aufgerufen nach Eingabe des Passworts.
/**
 * Wenn das eingegebene %Passwort richtig war, wird start2() aufgerufen, wenn nicht, wird der %Chat beendet.
 */
void Chat::passwort() {
    if ( passwords.getpass( nutzername ) == ui.PasswortA -> text().toStdString() ) { // richtiges Passwort
        ui.PasswortA -> setText(""); // für einen Neustart
        ui.LineStackedWidget -> setCurrentIndex( 0 ); // für einen Neustart
        start2();
    } else { // Falsches Passwort, beenden
        qCritical("Falsches Admin-Passwort!");
        qApp -> exit( EXIT_FAILURE );
    }
}

/// Starten des Chats. Aufgerufen durch start() oder passwort().
/**
 * 1. Ruft NutzerVerwaltung::makeNutzerIch() auf.
 * 2. Setzt #oberadmin und ruft klassenchat() und start_threads() auf.
 * 3. Öffnet das Chatfenster.
 * 4. Schreibt in #chatfile_all, dass ich den Chat betreten habe.
 * 5. Ruft main_thread() auf.
 */
void Chat::start2() { // Nachdem Admin Passwort eingegeben hat
    nutzer_verwaltung.makeNutzerIch( x_plum_anfang, std::move( nutzername ) );

    flags[x_oberadmin] = ! nutzer_ich.x_plum && nutzer_ich.nutzername == oberadmin; // Oberadmin

    terminatefile.remove();
    infofile.remove();

    klassenchat();
    start_threads();

    this -> setWindowTitle( QString::fromStdString( nutzer_ich.nutzername + " - Chat" ) );

    ui.action_berall_den_Chat_beenden -> setEnabled( nutzer_ich.admin ); // Explizit abschalten, da Tastenkombination
    ui.actionNeues_Admin_Passwort -> setEnabled( nutzer_ich.admin );
    ui.actionWarnung_senden -> setEnabled( ! nutzer_ich.x_plum );

    ui.menuBar -> setVisible( true ); // Menüleiste einblenden
    ui.NachrichtB -> setFocus(); // Fokus auf Input
    ui.MainStackedWidget -> setCurrentIndex( 1 ); // Chatfenster

    chatfile_all_mtx.lock();
        chatfile_all->ostream( true ) << nutzer_ich.nutzername << " hat den Chat betreten\n";
    chatfile_all_mtx.unlock();

    flags.set( x_main ); // Info an closeEvent, dass sich das Fenster nicht mehr einfach schließen lässt, sondern x_close gesetzt werden soll (chat.cpp)
    main_thread(); // aktualisieren.cpp
}

/// %Chat beenden oder neustarten.
/**
 * Beendet den %Chat oder startet ihn neu, je nachdem ob #x_restart gesetzt wurde.
 * Wird von verlauf_up() aufgerufen, wenn pruefen_main() true zurückgibt.
 *
 * Schließt zuerst das Fenster und ruft dann stop_threads() auf.
 * Bei einem Neustart wird anschließend alles auf den Zustand nach dem Konstruktor-Aufruf gesetzt und dann das Fenster wieder angezeigt.
 */
void Chat::stop() {
    flags.reset( x_main );
    this->close(); // Fenster schließen

    stop_threads();

    if ( flags[x_restart] ) { // Chat neustarten
        ui.BrowseA -> setText("");
        ui.MainStackedWidget -> setCurrentIndex( 0 );
        ui.NutzernameA -> selectAll();
        ui.menuBar -> setVisible( false );

        flags.reset();
        chats_ac.clear();
        inhalt.clear();

        chatfile_all = x_plum_anfang ? &chatfile_plum : &chatfile_norm;
        lockfile = x_plum_anfang ? &lockfile_plum : &lockfile_norm;

        this -> setWindowTitle("Handout Kräuterhexe");
        this -> show();
    } else
        qApp -> quit(); // Chat beenden
}
