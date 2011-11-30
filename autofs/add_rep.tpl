<h2>Add a remote NFS repository to mount under : {$mapname}</h2>
<input type='hidden' name='autofs_mapname' value='{$mapname}'/>
<fieldset>
<legend>
<b>Local path</b>
</legend>
<label for="autofs_rep_path">Local Path</label>
<input type='text' name='autofs_rep_path' />
</fieldset>

<fieldset>
<legend>
<b>Remote filesystem</b>
</legend>
<label for="autofs_rep_remote_path">Directory</label>
<input type='text' name='autofs_rep_remote_path' />
<br />
<label for="autofs_rep_server">NFS Server Adress</label>
<input type='text' name='autofs_rep_server' />
</fieldset>

<fieldset>
<legend>
<b>Options</b>
</legend>
<label for="autofs_rep_options">Mount options</label>
<input type='text' name='autofs_rep_options' value='{$mountoptions}' />
</fieldset>
<input type='submit' name='_autofs_add_form_rep_posted' value='Add' />
<input type='submit' name='_autofs_cancel' value='Cancel' />
