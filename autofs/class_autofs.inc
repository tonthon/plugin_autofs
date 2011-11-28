<?php
/*
 * Autofs plugin's main class
 * - provides some configuration tools to add automount entries
 */

class mPoint extends plugin {
  /*
   * Mountpoint object:
   * @point: the mountpoint (under the auto.master branch)
   * 
   * This datas are stored under a auto.... specific branch 
   * and are parsed from the nisMapEntry associated with them
   * @server: the nfs' server address
   * @rep: the rep to mount
   * @options: mount options
   */
  var $server;
  var $rep;
  var $point;
  var $options;

  public function __construct( $ldapentry, $ldap ) {
    $this->point = $ldapentry['cn'][0];
    $nisMapEntry  = $ldapentry['nisMapEntry'][0];

    $this->point = $point;
    $this->getMappedRep( $nisMapEntry, $ldap );
  }

  public function getMappedRep( $nisMapEntry, $ldap ) {
    /*
     * Get the mapped rep associated to the mount point 
     */    
    $ldap->search("(&(objectClass=nisObject)(nisMapName=$nisMapEntry))");
    while ( $ldapentry = $ldap->fetch() ){
      $nfsCommand = $ldapentry['nisMapEntry'][0];
    }
    $this->_parseNfsCommand($nfsCommand);
  }
  private function _parseNfsCommand( $nfsCommand ){
    list($options, $rest) = explode(' ', $nfsCommand, 2);
    list($serv, $rep) = explode(':', $rest, 2);
    $this->_setServer($serv);
    $this->_setRep($rep);
    $this->_setOptions($options);
  }
  private function _setServer($server){
    $this->server = $server;
  }
  private function _setRep($rep){
    $this->rep = $rep;
  }
  private function _setOptions($options){
    $this->options = $options;
  }
}

class autofs extends plugin{
  var $attributes = array('nisMapName', 'nisMapEntry', 'cn');
  var $objectclasses = array('nisObject', 'top', 'nisMap');
  var $plHeadline = "Autofs plugin";
  var $description = "This plugin provides a ldap map for nfs-autofs
    integration";

  public function __construct ( &$config, $dn = null ) {
    /*
     * Constructor
     * @config : the global config object
     *          (containing more than the configuration)
     * @dn : dn passed to our plugin
     */
    $this->config= $config;


    $this->mountPoints = array();

    $this->ldap= $this->config->get_ldap_link();
    $this->ldap->cd( $this->_getOu() );
  }
  private function retrieveMountPoints( ){
    /*
     * Retrieves all configured mountpoints 
     */
    $ldap->search("(&(objectClass=nisObject)(nisMapName=auto.master))");
    while ($ldapentry = $ldap->fetch()){
      $mountPoint = new mPoint( $ldapentry, $this->ldap);       
      $this->mountPoints[] = $mountPoint;
    }
  }
  private function _getOu( ) {
    /*
     * Returns the Ou for the autofs specific branch 
     */
    return('ou=autofs,' . $this->config->current['BASE']);
  }
  public function execute () {
    /*
     * Returns the content to be displayed
     */
    // Smarty is the templating engine
    $smarty= get_smarty();
    $smarty->caching = 0;
    $smarty->assign('mountPoints', $this->mountPoints);
    return ($smarty->fetch (get_template_path('autofs.tpl', TRUE)));
  }

  public function getDefaultMountOpts () {
    /*
     * Returns the default options used while configuring a new mount point
     */ 
    return "-fstype=nfs,rw,soft,async ". $this->ipNfsServer . ":";
  }
  public function save ( ) {

  }
}
?>
