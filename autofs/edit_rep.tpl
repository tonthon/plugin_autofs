<h2>Edit a remote nfs directory</h2>
<input type='hidden' name='autofs_mapname' value='{$mapname}'/>
<input type='hidden' name='autofs_orig_rep_path' value='{$rep->id}'/>
<fieldset>
<legend>
<b>Local path</b>
</legend>
<label for="autofs_rep_path">Local Path</label>
<input type='text' name='autofs_rep_path' value='{$rep->path}'/>
</fieldset>

<fieldset>
<legend>
<b>Remote filesystem</b>
</legend>
<label for="autofs_rep_remote_path">Directory</label>
<input type='text' name='autofs_rep_remote_path' value='{$rep->remote_path}'/>
<br />
<label for="autofs_rep_server">NFS Server Adress</label>
<input type='text' name='autofs_rep_server' value='{$rep->server}'/>
</fieldset>

<fieldset>
<legend>
<b>Options</b>
</legend>
<label for="autofs_rep_options">Mount options</label>
<input type='text' name='autofs_rep_options' value='{$rep->options}' />
</fieldset>
<input type='submit' name='_autofs_edit_form_rep_posted' value='Edit' />
<input type='submit' name='_autofs_cancel' value='Cancel' />
<div>
{foreach from=$message item=mess}
{$mess}
{/foreach}
</div>
