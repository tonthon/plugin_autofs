<h2>Remove MountPoint</h2>
Are you sure you want to remove the following mountPoint : <br />
{$mpointname} : {$mpoint}<br/>
You will also remove the whole list of repertories associated to this mount point.
<input type='hidden' value='{$mpointname}' name='autofs_mpointname' />
<input type='hidden' value='{$mpoint}' name='autofs_mpoint' />
<input type='submit' name='_autofs_del_mpoint_posted' value='Remove' />
<input type='submit' name='autofs_cancel' value='Cancel' />
