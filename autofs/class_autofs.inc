<?php
/*
 * Autofs plugin's main class
 * - provides the ldap scheme
 * - provides some configuration tools
 */

class autofs extends plugin{
  var $attributes = array();
  var $objectclasses = array();
  var $plHeadline = "Autofs plugin";
  var $description = "This plugin provides a ldap map for nfs-autofs 
    integration";

  public function __construct ( $config, $dn=null ) {
    /*
     * Constructor
     * @config : 
     * @dn : 
     */
    $this->config= $config;
  }
  public function execute () {
    /*
     * Returns the content to be displayed
     */
    // Smarty is the templating engine
    $smarty= get_smarty();
    return ($smarty->fetch (get_template_path('autofs.tpl', TRUE)));
  }

}
?>
