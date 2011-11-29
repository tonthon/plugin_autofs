<?php
/*
 * Autofs plugin's main class
 * - provides some configuration tools to add automount entries
 */

class BaseMount {
  var $config;
  public function __construct( $config ) {
    $this->config = $config;
  }
  public function _add( $ldapentry, $ldapentry_dn ) {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($config->current['BASE']);
    $ldap->create_missing_trees("", preg_replace( "/^[^,]+,/", "", $ldapentry_dn )); 
    $ldap->cd($ldapentry_dn);
    $ldap->add($ldapentry);
  }
  public function _deleteBranch( $ldapentry_dn ) {
    $ldap = $this->config->get_ldap_link( );
    $ldap->rmdir_recursive( $ldapentry_dn );
  }
  public function _deleteEntry( $ldapentry_dn ) {
    $ldap = $this->config->get_ldap_link( );
    $ldap->rmdir( $ldapentry_dn );
  }
  public function _update( $attrs, $ldapentry_dn ) {
    $ldap = $this->config->get_ldap_link( );
    $ldap->rename( $ldapentry_dn );
  }
}

class mountPoint extends BaseMount{
  /*
   * mountPoint object:
   * @point : the point to mount on
   * @name : name of the associated mountbbranch
   *
   * @mreps : the reps to mount on this mountpoint
   */
  var $point;
  var $name;
  var $mreps = array();
  public function __construct( $path, $nisMapEntry, $config, 
    $branch=null ) {
      
    parent::__construct($config);
    $this->point = $path;

    // retrieve the branch in which the reps are stored
    $this->nisMapEntry = $nisMapEntry;
    $this->name = substr($this->nisMapEntry, 5);
    $this->getMappedReps( );

    // The branch the mountpoints are configured on 
    // typcally : ou=autofs,dc=...,dc=...
    $this->branch = $branch;
  }
  public function getMappedReps( ) {
    /*
     * Get the mapped reps associated to the mount point 
     */    
    $ldap = $this->config->get_ldap_link();
    $ldap->search("(&(objectClass=nisObject)(nisMapName=$this->nisMapEntry))");
    while ( $ldapentry = $ldap->fetch() ){
      $path = $ldapentry['cn'][0];
      $nfsCommand = $ldapentry['nisMapEntry'][0];
      list($server, $remote_path, $options) = $this->_parseNfsCommand($nfsCommand);
      $this->mreps[] = new mRep($path, $server, $remote_path, $options, $config);
    }
  }
  static function _parseNfsCommand( $nfsCommand ){
    list($options, $rest) = explode(' ', $nfsCommand, 2);
    list($serv, $rep) = explode(':', $rest, 2);
    return array($serv, $rep, $options);
  }
  public function insert( ) {
    /*
     * Insert the current MPoint into LDAP
     */
    $this->_insertIntoMasterBranch();
    $this->_addMBranch();
  }
  public function delete( ){
    /*
     * Remove Current MPoint from LDAP
     */
    $ldapBranchDn = "nisMapEntry=" . $this->nisMapEntry . "," . $this->branch;
    $masterBranchEntryDn = $this->_getMasterBranchDn( $this->point );
    $this->_deleteBranch($ldapBranchDn);
    $this->_deleteEntry($masterBranchEntryDn);
  }
  private function _getMasterBranchDn( $cn ){
    /*
     * return the dn for an object in the master branch 
     */
    return "cn=" . $cn . ",nisMapEntry=auto.master," . $this->branch;
  }
  private function _getLdapEntry( ) {
    /*
     * Returns the associated ldapentry
     */
    $ldapentry = array();
    $ldapentry['cn'] = $this->point;
    $ldapentry['nisMapEntry'] = $this->nisMapEntry;
    $ldapentry['objectClass'] = array('nisObject');
    $ldapentry['nisMapName'] = 'auto.master';
    return $ldapentry;
  }
  private function _insertIntoMasterBranch( ) {
    /*
     * Insert the mountpoint into the master branch
     * (/dummymount)
     *
     */
    $ldapentry = $this->_getLdapEntry( );
    $ldapentry_dn = "cn=" . $ldapentry['cn'] . ',nisMapName=auto.master,' . $this->branch;
    $this->_add($ldapentry, $ldapentry_dn);
  }
  private function _addMBranch( ) {
    /*
     * Add the Branch associated with the mount point (auto.dummybranch)
     */
    $ldapentry = array();
    $ldapentry['nisMapName'] = $this->nisMapEntry;
    $ldapentry['objectClass'] = array('top', 'nisMap');
    $ldapentry_dn = 'nisMapName=' . $this->nisMapEntry . "," . $this->branch;
    $this->_add($ldapentry, $ldapentry_dn);
  }
}

class mRep {
  /*
   * This datas are stored under a auto.... specific branch 
   * MountRep object (under the auto.mountpointname branch):
   * @path: the path to mount on
   * 
   * @server: the nfs' server address
   * @remote_path: the path to mount
   * @options: mount options
   */
  var $path;
  var $server;
  var $remote_path;
  var $options;

  public function __construct( $path, $server, $remote_path, $options, $ldap ) {
    $this->path = $path;
    $this->server = $server;
    $this->remote_path = $remote_path;
    $this->options = $options;
    $this->ldap = $ldap;
  }
  public function insert(){
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
    $this->retrieveMountPoints();
  }
  private function retrieveMountPoints( ){
    /*
     * Retrieves all configured mountpoints 
     */
    $this->mountPoints = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd( $this->_getOu() );
    $ldap->search("(&(objectClass=nisObject)(nisMapName=auto.master))");
    while ($ldapentry = $ldap->fetch()){
      $mPoint = new mountPoint( $ldapentry['cn'][0],
                                $ldapentry['nisMapEntry'][0], 
                                $this->config);       
      $this->mountPoints[] = $mPoint;
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
    $toEdit = $this->isEditFormMPoint( );
    $toRemove = $this->isRemoveFormMPoint( );
    if ( isset($_POST['autofs_add_form_mpoint'] ) ){
      return $this->displayAddMPointForm();
    }elseif ( $toEdit != null ) {
      return $this->displayEditMPointForm( $toEdit );
    } elseif ($toRemove != null ){
      return $this->displayRemoveMPointForm( $toRemove );
    } else {
      return $this->displayList();
    }
  }
  public function isEditFormMPoint( ) {
    foreach ($_POST as $key=>$value){
      if (substr($key, 0, 24) == 'autofs_edit_form_mpoint_'){
        return substr($key, 24);
      } 
    }
    return null;
  }
  public function isRemoveFormMPoint( ) {
    foreach ($_POST as $key=>$value){
      if (substr($key, 0, 18) == 'autofs_del_mpoint_') {
        return substr($key, 18);
      }
    }
    return null;
  }

  public function displayAddMPointForm( ) {
    $smarty= get_smarty();
    $smarty->caching = 0;
    return ($smarty->fetch (get_template_path('add_mpoint.tpl', TRUE)));
  }
  public function displayEditMPointForm( $toEdit ) {
    $smarty= get_smarty();
    $smarty->caching = 0;
    $name = 'auto.' . $toEdit;
    $point = $this->_getPointFromName($name);
    $smarty->assign('mpoint', $point);
    $smarty->assign('mpointname', $name);
    return ($smarty->fetch (get_template_path('add_mpoint.tpl', TRUE) ) );
  }
  public function displayRemoveMPointForm( $toRemove ){
    $smarty= get_smarty();
    $smarty->caching = 0;
    $name = "auto." . $toRemove;
    $point = $this->_getPointFromName($name);
    $smarty->assign('mpoint', $point);
    $smarty->assign('mpointname', $toRemove);
    return ($smarty->fetch (get_template_path('remove_mpoint.tpl', TRUE) ) );
  }
  public function displayList(){
    $smarty= get_smarty();
    $smarty->caching = 0;
    $smarty->assign('mountPoints', $this->mountPoints);
    $smarty->assign('message', $this->message);
    return ($smarty->fetch (get_template_path('autofs.tpl', TRUE)));
  }

  static function getDefaultMountOpts () {
    /*
     * Returns the default options used while configuring a new mount point
     */ 
    return "-fstype=nfs,rw,soft,async ";
  }
  public function save_object( ){
    if ( isset( $_POST['_autofs_add_form_mpoint_posted'] ) ){
      $this->message = "Autofs received <br />";
      if ( isset( $_POST['autofs_mountpoint'] ) && 
        isset ( $_POST['autofs_mountname'] ) ){
          $name = get_post('autofs_mountname');
          $point = get_post('autofs_mountpoint');
          $nisMapEntry = 'auto.' . $name;
          $ldap = $this->config->get_ldap_link();
          $this->newMountPoint = new mountPoint($point,
                                                $nisMapEntry,
                                                $this->config, 
                                                $this->_getOu()
                                              );
        }      
    }elseif (isset($_POST['_autofs_del_mpoint_posted'] ) ){
      $name = "auto." . get_post('autofs_mpointname');
      $point = $this->_getPointFromName($name);
      $this->rmMountPoint = new mountPoint($point, 
                                           $name,
                                           $this->config, 
                                           $this->_getOu());
    }
    if (isset($this->newMountPoint)){
      $this->newMountPoint->insert();
      $this->retrieveMountPoints();
    }
    if (isset($this->rmMountPoint)){
      $this->message = "Deleting";
      $this->rmMountPoint->delete();
    }
    if ( isset( $_POST['autofs_remove'] ) ){
    }
  }
  private function _getPointFromName( $name ){
    $ldap = $this->config->get_ldap_link();
    $ldap->cd( $this->_getOu() );
    $ldap->search("(&(objectClass=nisObject)"
      . "(nisMapName=auto.master)"
      . "(nisMapEntry=$name))");
    while ($ldapentry = $ldap->fetch()){
      $point = $ldapentry['cn'][0];
    }
    return $point;
  }
  public function check( ) {
#    $message = plugin::check( );
#    $message[] = $this->newMountPoint->valid()
#    $message[] = $this->validMountPoint( );
#    return $message;
  }
  public function save ( ) {
  }
#  public function validMountPoint( ){
#    if ( isset($this->newMountPoint) ){
#      if ( !$this->validMountPointName($this->newMountPoint['name']) ){
#        $message = msgPool::invalid(sprintf(
#                                           _("Invalid mount point name %s"), 
#                                           ($this->newMountPoint['name'])
#                                           ), 
#                                   "", 
#                                   "", 
#                                   "Only A-Z and a-z characters are allowed" );
#      } elseif ( !$this->validPath($this->newMountPoint['point'])){
#        $message = msgPool::invalid(sprintf(
#                                            _("Invalid mount path %s"),
#                                            ($this->newMountPoint['point'])
#                                          ),
#                                   "",    
#                                   "",    
#                                   "/mnt/dummypath");
#      }
#    } 
#    return $message;
#  }
#  static function validMountPointName( $mpname ){
#    if ( ereg("[a-zA-Z]+", $mpname ) ){
#      return true;
#    else {
#      return false;
#    }
#    }
#  }
#  static function validPath( $path ){
#    return true;
#  }
}
?>
