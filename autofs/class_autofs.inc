<?php
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
      $this->action->setAction( 'add_mpoint' );
      $name = get_post('autofs_mountname');
      if ( $this->_getPointFromName( $name ) == null){
        $point = get_post('autofs_mountpoint');
        $nisMapEntry = 'auto.' . $name;

        $this->newMountPoint = new mountPoint($point,
          $nisMapEntry,
          $this->config, 
          $this->_getOu()
        );
      }
    } elseif (isset($_POST['_autofs_del_mpoint_posted'] ) ){
      $this->action->setAction( 'del_mpoint' );
      $name = "auto." . get_post('autofs_mountname');
      $point = $this->_getPointFromName($name);
      if ( $point != null ) {
      $this->rmMountPoint = new mountPoint($point, 
                                           $name,
                                           $this->config, 
                                           $this->_getOu()
                                         );
      }
    } elseif ( isset( $_POST['_autofs_edit_form_mpoint_posted'] ) ){
      $this->action->setAction( 'edit_mpoint' );
      $name = "auto." . get_post( 'autofs_mountname' );
      $point = $this->_getPointFromName( $name );
      $newPoint = get_post( 'autofs_mountpoint' );
      if ( ( $point != $newPoint )&&( $newPoint != '' )){
        $this->newPoint = $newPoint;
        $this->editMountPoint = new mountPoint($point, 
          $name,
          $this->config, 
          $this->_getOu());
      }
    } elseif (  isset(  $_POST['_autofs_add_form_rep_posted'])){
      $this->action->setAction( 'add_rep' );
      $mapName = "auto." . get_post( 'autofs_mapname' );
      $path = get_post(  'autofs_rep_path' );
      $server = get_post( 'autofs_rep_server' );
      $remote_path = get_post( 'autofs_rep_remote_path' );
      $options = get_post( 'autofs_rep_options' );
      $this->action->message( "Map name : $mapName" );
      foreach ( $_POST as $key=>$item ){
        $this->action->message(  $key."->".$item);
      }
      $this->newRep = new mRep( $path,
                               $server,
                               $remote_path,
                               $options,
                               $this->config,
                               $this->_getOu( ),
                               $mapName
                              );
    }elseif ( isset( $_POST['_autofs_del_form_rep_posted'])){
      $this->action->setAction( 'del_rep' );
      $mapName = "auto." . get_post( 'autofs_mapname' );
      $path = get_post( 'autofs_rep_path' );
      $server = "";
      $remote_path = "";
      $options = "";
      foreach (  $_POST as $key=>$item ){
        $this->action->message(   $key."->".$item);
      }
      $this->delRep = new mRep( $path,
                               $server,
                               $remote_path,
                               $options,
                               $this->config,
                               $this->_getOu( ),
                               $mapName
                              );
    }elseif (  isset(  $_POST['_autofs_edit_form_rep_posted'] ) ){
      $this->action->setAction(  'edit_rep' );
      $mapName = get_post(  'autofs_mapname' );
      $origPath = get_post(  'autofs_orig_rep_path' );
      $path = get_post(  'autofs_rep_path' );
      $server = get_post( 'autofs_rep_server');
      $remote_path = get_post( 'autofs_rep_remote_path');
      $options = get_post(  'autofs_rep_options');

      $this->editRep = new mRep( $origPath, 
                          "", 
                          "", 
                          "", 
                          $this->config, 
                          $this->_getOu(  ), 
                          $mapName);
      $this->newRepInfos = array( $path, $server, $remote_path, $options );
      $this->action->message( $origPath . "=>" . $path);
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
      $mPoint = new mountPoint( $ldapentry['cn'][0],
        $ldapentry['nisMapEntry'][0], 
        $this->config);       
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
        $rep = new mRep( $path, '', '', '', 
                        $this->config, $this->_getOu( ), $mapName);
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
