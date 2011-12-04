<?php
/*
 * This file is part of the autofs plugin for fusiondirectory 
 * Copyright (C) 2011 Gaston TJEBBES - Majerti <g.t at majerti.fr>
 *  
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Autofs plugin
 * main file
 */

// Remove locks created by this plugin.
if ( $remove_lock ){
  if ( session::is_set('autofs') ){
    // If necessary  : unlock
  }
}
if ( $cleanup ) {
  session::un_set('autofs');
} else {
  if (isset($_POST['autofs_cancel'])){
    session::un_set('autofs');
  }
  if ( !session::is_set('autofs') ) {
    session::set('autofs', new autofs($config, $dn));
  }
  $autofs = session::get('autofs');
  /*
   * Get all datas from post and get vars
   * Usefull if the plugin provides an input field
   */
  $display= $autofs->save_object();

  /*
   * Rendered datas are put in the display var
   */
  $display= $autofs->execute ();

  /*
   * Page header generation
   */
  //  $display= print_header(get_template_path('plugins/log/images/plugin.png'), _("System log view")).$display;

  /*
   * We set our plugin back in the session
   */
  session::set('autofs', $autofs);
}
