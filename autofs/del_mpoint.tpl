<h2>Remove Local Mount Point</h2>
Are you sure you want to remove the following mount point : <br />
{$mpointname} : {$mpoint}<br/>
<b>You will also remove its associated map and the whole list of directories associated to it.</b>
<input type='hidden' value='{$mpointname}' name='autofs_mountname' />
<input type='hidden' value='{$mpoint}' name='autofs_mountpoint' />
<input type='submit' name='_autofs_del_mpoint_posted' value='Remove' />
<input type='submit' name='autofs_cancel' value='Cancel' />
