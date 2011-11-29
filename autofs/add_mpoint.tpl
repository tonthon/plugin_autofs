<h2>Add a new mountpoint</h2>
<label for='autofs_mountname'>Nom du point de montage</label>
<input type='text' name='autofs_mountname' {if isset($mpointname)}value='{$mpointname}' disabled='true'{/if} />
<label for='autofs_mountpoint'>Chemin de destination</label>
<input type='text' name='autofs_mountpoint' {if isset($mpoint)}value='{$mpoint}'{/if} />
<input type='submit' name='_autofs_add_form_mpoint_posted' value='Create' />
<input type='submit' name='autofs_cancel' value='Cancel' />
