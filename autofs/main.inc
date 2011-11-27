<?php
/*
 * Autofs plugin
 */
/* Remove locks created by this plugin.
 *
 */
if ( $remove_lock ){
  if ( session::is_set('autofs') ){
    // If necessary  : unlock
  }
}
if ( $cleanup ) {
  session::un_set('autofs');
} else {
  if ( !session::is_set('autofs') ) {
    session::set('autofs', new autofs($config, $dn));
  }
  $autofs = session::get('autofs');
  /*
   * Usefull if the plugin provides an input field
   */
  //  $display= $autofs->save_object();

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
