<h2>Remove Directory</h2>
Are you sure you want to remove the following Directory : <br />
{$path}<br/>
From the following map : <br />
auto.{$mapname}
<br />
<input type='hidden' value='{$path}' name='autofs_rep_path' />
<input type='hidden' value='{$mapname}' name='autofs_mapname' />
<input type='submit' name='_autofs_del_form_rep_posted' value='Remove' />
<input type='submit' name='autofs_cancel' value='Cancel' />

<div>
{foreach from=$message item=mess}
{$mess}
{/foreach}
</div>
