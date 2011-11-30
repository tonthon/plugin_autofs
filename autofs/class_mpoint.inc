<?php
class BaseLdap {
  /*
   * Base class to manage autofs ldap relation
   */
  protected $config;
  protected $branch;
  public function __construct( $config, $branch ) {
    $this->config = $config;
    $this->branch = $branch;
  }
  protected function _add( $ldapentry, $ldapentry_dn ) {
    $ldap = $this->config->get_ldap_link();
    $ldap->cd($this->config->current['BASE']);
    $ldap->create_missing_trees("", preg_replace( "/^[^,]+,/", "", $ldapentry_dn )); 
    $ldap->cd($ldapentry_dn);
    $ldap->add($ldapentry);
  }
  protected function _deleteBranch( $ldapentry_dn ) {
    $ldap = $this->config->get_ldap_link( );
    $ldap->cd($this->config->current['BASE']);
    $ldap->rmdir_recursive( $ldapentry_dn );
  }
  protected function _deleteEntry( $ldapentry_dn ) {
    $ldap = $this->config->get_ldap_link( );
    $ldap->cd($this->config->current['BASE']);
    $ldap->rmdir( $ldapentry_dn );
  }
  protected function _update( $attrs, $ldapentry_dn ) {
    $ldap = $this->config->get_ldap_link( );
    $ldap->rename( $attrs, $ldapentry_dn );
  }
  protected function _rename_branch( $ldapentry_dn, $new_dn){
    $ldap = $this->config->get_ldap_link( );
    $ldap->rename_dn( $ldapentry_dn, $new_dn);
  }
  protected function _getDn( $cn, $autobranch='auto.master' ){
    /*
     * return the dn for an object in the master branch 
     */
    return "cn=" . $cn . ",nisMapName=" . $autobranch . "," . $this->branch;
  }
  protected static function _parseNfsCommand( $nfsCommand ){
    list($options, $rest) = explode(' ', $nfsCommand, 2);
    list($serv, $rep) = explode(':', $rest, 2);
    return array($serv, $rep, $options);
  }
}

class mountPoint extends BaseLdap{
  /*
   * mountPoint object:
   * @point : the base directory to mount on
   * @name : name of the associated mountbbranch
   *
   * @mreps : the reps to mount on this mountpoint
   * 
   * point is the local directory we will mount or stuff on
   * mreps are all the reps mounted in the local directory
   * mreps are stored in the specific branch auto.@name
   */
  var $point;
  var $name;
  var $mreps = array();
  public function __construct( $path, $nisMapEntry, $config, 
    $branch=null ) {

      parent::__construct($config, $branch);
      $this->point = $path;

      // the branch in which the reps are stored
      $this->nisMapEntry = $nisMapEntry;
      $this->name = substr($this->nisMapEntry, 5);
      $this->_getMappedReps( );

      // The branch the mountpoints are configured on 
      // typically : ou=autofs,dc=...,dc=...
      $this->branch = $branch;
  }
  public function insert( ) {
    /*
     * Insert the current MPoint into LDAP
     */
    $this->_insertIntoMasterBranch();
    $this->_addMBranch();
  }
  public function delete( ) {
    /*
     * Remove Current MPoint from LDAP
     */
    $ldapBranchDn = "nisMapName=" . $this->nisMapEntry . "," . $this->branch;
    $masterBranchEntryDn = $this->_getDn( $this->point );
    $this->_deleteBranch($ldapBranchDn);
    $this->_deleteEntry($masterBranchEntryDn);
  }
  public function update ( $newCn ) {
    /*
     * Update the current mountpoint with the new given cn
     */
    $old_dn = $this->_getDn(  $this->point );
    $attrs = array(  );
    $attrs[ 'cn' ] = $newCn;
    $new_dn = $this->_getDn( $newCn );
    $this->_update( $attrs, $old_dn );
    $this->_rename_branch( $old_dn, $new_dn );
    $this->point = $newCn;
  }
  protected function _getMappedReps( ) {
    /*
     * Get the mapped reps associated to the mount point 
     */    
    $ldap = $this->config->get_ldap_link();
    $ldap->search("(&(objectClass=nisObject)(nisMapName=$this->nisMapEntry))");
    while ( $ldapentry = $ldap->fetch() ){
      $path = $ldapentry['cn'][0];
      $nfsCommand = $ldapentry['nisMapEntry'][0];
      list($server, $remote_path, $options) = $this->_parseNfsCommand($nfsCommand);
      $this->mreps[] = new mRep($path, $server, $remote_path, $options, 
                                    $this->config, $this->branch, 
                                    $this->nisMapEntry);
    }
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
     * (e.g. : cn=/dummymount)
     */
    $ldapentry = $this->_getLdapEntry( );
    $ldapentry_dn = "cn=" . $ldapentry['cn'] . ',nisMapName=auto.master,' . $this->branch;
    $this->_add($ldapentry, $ldapentry_dn);
  }
  private function _addMBranch( ) {
    /*
     * Add the Branch associated with the mount point
     * ( e.g. : auto.dummybranch)
     */
    $ldapentry = array();
    $ldapentry['nisMapName'] = $this->nisMapEntry;
    $ldapentry['objectClass'] = array('top', 'nisMap');
    $ldapentry_dn = 'nisMapName=' . $this->nisMapEntry . "," . $this->branch;
    $this->_add($ldapentry, $ldapentry_dn);
  }
}

class mRep extends BaseLdap {
  /*
   * This datas are stored under a auto.... specific branch 
   * MountRep object (under the auto.mountpointname branch):
   * @path: the path to mount on
   * 
   * @server: the nfs' server address
   * @remote_path: the path to mount
   * @options: mount options
   */
  var $id;
  var $path;
  var $server;
  var $remote_path;
  var $options;
  var $mapname;
  var $replacement_char = '---';
  public function __construct( $path, $server, $remote_path, $options, 
      $config, $branch, $mapname ) {
    parent::__construct( $config, $branch );
    if ( substr_count( $path, $this->replacement_char) > 0){
      $path = $this->_getPathFromId( $path);
    }
    $this->path = $path;
    $this->server = $server;
    $this->remote_path = $remote_path;
    $this->options = $options;
    $this->mapname = $mapname;
    $this->id = $this->_getIdFromPath( $this->path);
  }
  private function _getNisMapEntry( ){
    $tpl = "%s %s:%s";
    return sprintf($tpl, $this->options, $this->server, $this->remote_path);
  }
  private function _getLdapEntry( ){
    $ldapentry = array( );
    $ldapentry['cn'] = $this->path;
    $ldapentry['nisMapName'] = $this->mapname;
    $ldapentry['objectClass'] = 'nisObject';
    $ldapentry['nisMapEntry'] = $this->_getNisMapEntry( );
    return $ldapentry;
  }
  public function insert(){
    $ldapentry = $this->_getLdapEntry( );
    $ldapentry_dn = $this->_getDn( $ldapentry['cn'], $this->mapname );
    $this->_add( $ldapentry, $ldapentry_dn );
  }
  public function delete( ){
    $ldapentry_dn = $this->_getDn( $this->path, $this->mapname );
    $this->_deleteEntry( $ldapentry_dn);
  }
  public function update( $newDatas ){
    list( $path, $this->server, $this->remote_path, $this->options ) = $newDatas;
    $old_dn = $this->_getDn(  $this->path, $this->mapname );
    $new_dn = $this->_getDn( $path, $this->mapname );
    $attrs = array( );
    $attrs['cn'] = $path;
    $attrs['nisMapEntry'] = $this->_getNisMapEntry( );
    $this->_update( $attrs, $old_dn);
    if ( $path != $this->path ){
      $this->_rename_branch( $old_dn, $new_dn );
    }
  }

  public function fetch( ){
    /* 
     * Retrieve the informations associated to this rep
     * when initialized with void values
     */
    $dn = $this->_getDn(  $this->path, $this->mapname );
    $ldap = $this->config->get_ldap_link( );
    $ldap->cat( $dn );
    $attrs = $ldap->fetch( );
    $nisMapEntry = $attrs['nisMapEntry'][0];
    list($this->server, 
          $this->remote_path, 
          $this->options) = $this->_parseNfsCommand($nisMapEntry);
  }

  protected function _getIdFromPath( $path ){
    /*
     * Set a unique id for the current object
     */
    // Silly isn't it ?
    return str_replace( ' ', $this->replacement_char, $path);
  }

  protected function _getPathFromId( $id ){
    return str_replace( $this->replacement_char, ' ', $id);
  }
  
}
