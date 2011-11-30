{if isset($mpointname)}
<h2>Edit a mountpoint</h2>
{else}
<h2>Add a new mountpoint</h2>
{/if}
<label for='autofs_mountname'>Nom du point de montage</label>
{if isset(  $mpointname)}
<input type='text' name='disablefield' value='{$mpointname}' disabled='true' />
<input type='hidden' name='autofs_mountname' value='{$mpointname}' />
{else}
<input type='text' name='autofs_mountname'>
{/if}
<label for='autofs_mountpoint'>Chemin de destination</label>
<input type='text' name='autofs_mountpoint' {if isset($mpoint)}value='{$mpoint}'{/if} />
{if isset($mpointname)}
<input type='submit' name='_autofs_edit_form_mpoint_posted' value='Edit' />
{else}
<input type='submit' name='_autofs_add_form_mpoint_posted' value='Create' />
{/if}
<input type='submit' name='autofs_cancel' value='Cancel' />
