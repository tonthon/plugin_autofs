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
 * Autofs plugin's main class
 * - provides some configuration tools to add automount entries
 */


class StateMachine {
  var $action = 'list';
  var $messages = array( );
  var $storage;

  public function setAction( $action ){
    $this->action = $action;
    $this->messages = array(  );
  }

  public function store( $datas ){
    $this->storage = $datas;
  }

  public function message( $message ){
    $this->messages[] = $message;
    $this->messages[] = '<br />';
  }

}

class autofs extends plugin{
  var $attributes = array('nisMapName', 'nisMapEntry', 'cn');
  var $objectclasses = array('nisObject', 'top', 'nisMap');
  var $plHeadline = "Autofs plugin";
  var $description = "This plugin provides a ldap map for nfs-autofs
    integration";

  /* Return plugin informations for acl handling  */
  static function plInfo()
  {
	  return array(
		  "plShortName"   => _("Autofs"),
		  "plDescription" => _("Autofs manager"),
		  "plSection"     => "addons",
		  "plPriority"    => 5,
		  "plProvidedAcls"  => array()
		  );
  }
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

  public function save_object( ){
    /*
     * Handle POST AND GET methods
     */
    $this->action = new StateMachine( );
    if ( isset( $_POST['_autofs_add_form_mpoint_posted'] ) ){
      $this->_add_mpoint();
    } elseif (isset($_POST['_autofs_del_mpoint_posted'] ) ){
      $this->_del_mpoint();
    } elseif ( isset( $_POST['_autofs_edit_form_mpoint_posted'] ) ){
      $this->_edit_mpoint();
    } elseif (  isset(  $_POST['_autofs_add_form_rep_posted'])){
      $this->_add_rep();
    }elseif ( isset( $_POST['_autofs_del_form_rep_posted'])){
      $this->_del_rep();
    }elseif (  isset(  $_POST['_autofs_edit_form_rep_posted'] ) ){
      $this->_edit_rep();
    }

    if (isset($this->newMountPoint)){
      $this->newMountPoint->insert();
      $this->newMountPoint = null;
      $this->retrieveMountPoints();
    }
    if (isset($this->rmMountPoint)){
      $this->rmMountPoint->delete();
      $this->rmMountPoint = null;
      $this->retrieveMountPoints();
    }
    if ( isset( $this->newPoint ) ){
      $this->editMountPoint->update( $this->newPoint );
      $this->newPoint = null;
      $this->editMountPoint = null;
      $this->retrieveMountPoints();
    }
    if ( isset( $this->newRep ) ){
      $this->newRep->insert( );
      $this->newRep = null;
      $this->retrieveMountPoints( );
    }
    if (  isset(  $this->delRep )){
      $this->delRep->delete( );
      $this->delRep = null;
      $this->retrieveMountPoints(  );
    }
    if ( isset( $this->editRep)){
      $this->editRep->update( $this->newRepInfos );
      $this->editRep = null;
      $this->newRepInfos = null;
      $this->retrieveMountPoints(  );
    }
  }

  protected function _get_mpoint($point, $name){
     return new mountPoint($point,
			  $name,
			  $this->config,
			  $this->_getOu()
			  );

  }

  protected function _get_mrep($path, $server, $remote_path, $options, $mapName){
     return new mRep( $path,
                               $server,
                               $remote_path,
                               $options,
                               $this->config,
                               $this->_getOu( ),
                               $mapName
                              );
  }

  protected function _add_mpoint(){
      $this->action->setAction( 'add_mpoint' );
      $name = trim(get_post('autofs_mountname'));
      if ( $this->_getPointFromName( $name ) == null){
        $point = trim(get_post('autofs_mountpoint'));
        $nisMapEntry = 'auto.' . $name;
        $this->newMountPoint = $this->_get_mpoint($point, $nisMapEntry);
      }
  }

  protected function _del_mpoint(){
      $this->action->setAction( 'del_mpoint' );
      $name = "auto." . trim(get_post('autofs_mountname'));
      $point = $this->_getPointFromName($name);
      if ( $point != null ) {
      $this->rmMountPoint = $this->_get_mpoint($point, $name);
      }
  }

  protected function _edit_mpoint(){
      $this->action->setAction( 'edit_mpoint' );
      $name = "auto." . trim(get_post( 'autofs_mountname' ));
      $point = $this->_getPointFromName( $name );
      $newPoint = trim(get_post( 'autofs_mountpoint' ));
      if ( ( $point != $newPoint )&&( $newPoint != '' )){
        $this->newPoint = $newPoint;
        $this->editMountPoint = $this->_get_mpoint($point, $name);
      }
  }

  protected function _add_rep(){
      $this->action->setAction( 'add_rep' );
      $mapName = "auto." . trim(get_post( 'autofs_mapname' ));
      $path = trim(get_post( 'autofs_rep_path' ));
      $server = trim(get_post( 'autofs_rep_server' ));
      $remote_path = trim(get_post( 'autofs_rep_remote_path' ));
      $options = trim(get_post( 'autofs_rep_options' ));
      $this->action->message( "Map name : $mapName" );
      foreach ( $_POST as $key=>$item ){
        $this->action->message(  $key."->".$item);
      }
      $this->newRep = $this->_get_mrep($path,
                               $server,
                               $remote_path,
                               $options,
                               $mapName
                              );
  }

  protected function _del_rep(){
      $this->action->setAction( 'del_rep' );
      $mapName = "auto." . trim(get_post( 'autofs_mapname' ));
      $path = trim(get_post( 'autofs_rep_path' ));
      $server = "";
      $remote_path = "";
      $options = "";
      foreach (  $_POST as $key=>$item ){
        $this->action->message(   $key."->".$item);
      }
      $this->delRep = $this->_get_mrep( $path,
                               $server,
                               $remote_path,
                               $options,
                               $mapName
                              );
  }

  protected function _edit_rep(){
      $this->action->setAction(  'edit_rep' );
      $mapName = trim(get_post(  'autofs_mapname' ));
      $origPath = trim(get_post(  'autofs_orig_rep_path' ));
      $path = trim(get_post(  'autofs_rep_path' ));
      $server = trim(get_post( 'autofs_rep_server'));
      $remote_path = trim(get_post( 'autofs_rep_remote_path'));
      $options = trim(get_post(  'autofs_rep_options'));

      $this->editRep = $this->_get_mrep( $origPath, "", "", "", $mapName);
      $this->newRepInfos = array( $path, $server, $remote_path, $options );
      $this->action->message( $origPath . "=>" . $path);
  }

  public function execute () {
    /*
     * Returns the content to be displayed
     */
    // Smarty is the templating engine
    $toEdit = $this->isEditFormMPoint( );
    $toDel = $this->isDelFormMPoint( );
    $addRep = $this->isAddFormRep( );
    list( $delMapName, $delRep ) = $this->isDelFormRep( );
    list( $editMapName, $editRep ) = $this->isEditFormRep( );
    if ( isset($_POST['autofs_add_form_mpoint'] ) ){
      return $this->displayAddMPointForm();
    } elseif ( $toEdit != null ) {
      return $this->displayEditMPointForm( $toEdit );
    } elseif ($toDel != null ){
      return $this->displayDelMPointForm( $toDel );
    } elseif ( $addRep != null ) {
      return $this->displayAddRepForm( $addRep );
    } elseif (  $delRep != null ){
      $this->action->message( $delMapName . " => " . $delRep);
      return $this->displayDelRepForm( $delMapName, $delRep );
    } elseif ( $editRep != null){
      $this->action->message( $editMapName . " => " . $editRep->path);
      return $this->displayEditRepForm( $editMapName, $editRep );
    }else {
      return $this->displayList();
    }
  }

  protected function retrieveMountPoints( ){
    /*
     * Retrieves all configured mountpoints
     */
    $this->mountPoints = array();
    $ldap = $this->config->get_ldap_link();
    $ldap->cd( $this->_getOu() );
    $ldap->search("(&(objectClass=nisObject)(nisMapName=auto.master))");
    while ($ldapentry = $ldap->fetch()){
      $mPoint = $this->_get_mpoint( $ldapentry['cn'][0],
                                    $ldapentry['nisMapEntry'][0] );
      $this->mountPoints[] = $mPoint;
    }
  }

  protected function _getOu( ) {
    /*
     * Returns the Ou for the autofs specific branch
     */
    return('ou=autofs,' . $this->config->current['BASE']);
  }

  static function _getDefaultMountOpts () {
    /*
     * Returns the default options used while configuring a new mount point
     */
    return "-fstype=nfs,rw,soft,async ";
  }

  private function _getPointFromName( $name ){
    /*
     * Retrieves a mountpoint from its mountname
     */
    $point = null;
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

  protected static function isEditFormMPoint( ) {
    /*
      Returns the name of the mpoint if we want to edit it
     */
    foreach ($_POST as $key=>$value){
      if (substr($key, 0, 24) == 'autofs_edit_form_mpoint_'){
        return substr($key, 24);
      }
    }
    return null;
  }

  protected static function isDelFormMPoint( ) {
    /*
      Returns the name of the mpoint if we want to remove it
     */
    foreach ($_POST as $key=>$value){
      if (substr($key, 0, 18) == 'autofs_del_mpoint_') {
        return substr($key, 18);
      }
    }
    return null;
  }

  protected static function isAddFormRep( ){
    /*
     * Return the name of the mountpoint
     * if we want to add a rep to it
     */
    foreach ( $_POST as $key=>$value){
      if ( substr( $key, 0, 16) == 'autofs_add_mrep_') {
        return substr( $key, 16 );
      }
    }
    return null;
  }

  protected function isDelFormRep( ){
    /*
     * Return the name of the directory to delete
     * and the mapname it belongs to
     */
    foreach (  $_POST as $key=>$value){
      if (  substr( $key, 0, 16 ) == 'autofs_del_mrep_') {
        // The key appears as follow : autofs_del_mrep_<mapname>___<dirname>
        $datas = substr( $key, 16 );
        list( $mapName, $path ) = explode( '___', $datas, 2 );
        return array( $mapName, $path );
      }
    }
    return array(null, null);
  }

  protected function isEditFormRep( ){
    /*
     * Return the rep object to edit
     * and the mapname it belongs to
     */
    foreach ( $_POST as $key=>$value ){
      if ( substr( $key, 0, 17 ) == 'autofs_edit_mrep_'){
        $datas = substr( $key, 17);
        list( $mapName, $path) = explode( '___', $datas, 2 );
        $mapName = 'auto.' . $mapName;
        $rep = $this->_get_mrep( $path, '', '', '', $mapName);
        $rep->fetch( );
        $this->action->message( "Retrieving datas");
        $this->action->message( "rep : $rep->server");
        return array( $mapName, $rep );
      }
    }
    return array( null, null);
  }

  protected function displayList(){
    $smarty= get_smarty();
    $smarty->caching = 0;
    $rep = $this->mountPoints[0]->mreps[0];
    $this->msg(  $rep->path);
    $this->msg( $rep->id);
    $smarty->assign('mountPoints', $this->mountPoints);
//    $smarty->assign('message', $this->action->messages);
    return ($smarty->fetch (get_template_path('autofs.tpl', TRUE)));
  }

  protected function displayAddMPointForm( ) {
    $smarty= get_smarty();
    $smarty->caching = 0;
    return ($smarty->fetch (get_template_path('add_mpoint.tpl', TRUE)));
  }

  protected function displayDelMPointForm( $toDel ){
    $smarty= get_smarty();
    $smarty->caching = 0;
    $name = "auto." . $toDel;
    $point = $this->_getPointFromName($name);
    $smarty->assign('mpoint', $point);
    $smarty->assign('mpointname', $toDel);
    return ($smarty->fetch (get_template_path('del_mpoint.tpl', TRUE) ) );
  }

  protected function displayEditMPointForm( $toEdit ) {
    $smarty= get_smarty();
    $smarty->caching = 0;
    $name = 'auto.' . $toEdit;
    $point = $this->_getPointFromName($name);
    $smarty->assign('mpoint', $point);
    $smarty->assign('mpointname', $toEdit);
    return ($smarty->fetch (get_template_path('add_mpoint.tpl', TRUE)));
  }

  protected function displayAddRepForm( $mapName ){
    $smarty= get_smarty( );
    $smarty->caching = 0;
    $smarty->assign( 'mapname', $mapName );
    $smarty->assign( 'mountoptions', $this->_getDefaultMountOpts( ));
    return ( $smarty->fetch ( get_template_path( 'add_rep.tpl', TRUE )));
  }

  protected function displayDelRepForm( $mapName, $path ){
    $smarty= get_smarty(  );
    $smarty->caching = 0;
    $smarty->assign(  'mapname', $mapName );
    $smarty->assign(  'path', $path);
//    $smarty->assign(   'message', $this->action->messages);
    return ( $smarty->fetch ( get_template_path( 'del_rep.tpl', TRUE )));
  }

  protected function displayEditRepForm( $mapName, $rep ){
    $smarty = get_smarty( );
    $smarty->caching = 0;
    $smarty->assign( 'mapname', $mapName);
    $smarty->assign( 'rep', $rep );
//    $smarty->assign( 'message', $this->action->messages);
    return ( $smarty->fetch( get_template_path( 'edit_rep.tpl', TRUE )));
  }
  protected function msg( $msg){
    $this->action->message( $msg);
  }

  public function check( ) {
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
