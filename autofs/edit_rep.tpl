<style>
{literal}
h2, p, .autofs_main{
color:#00378A;
}
.round{
    -moz-border-radius:4px;
    border-radius:4px;
}
.drop-shadow {
    -webkit-box-shadow:0 1px 4px rgba(0, 0, 0, 0.3), 0 0 60px rgba(0, 0, 0, 0.1) inset;
    -moz-box-shadow:0 1px 4px rgba(0, 0, 0, 0.3), 0 0 40px rgba(0, 0, 0, 0.1) inset;
    box-shadow:0 1px 4px rgba(0, 0, 0, 0.3), 0 0 40px rgba(0, 0, 0, 0.1) inset;

}
.autofs_main img{
    margin-right:5px;
    display:block;
}
fieldset{
    border : none;
    padding:15px 30px 15px 15px;
    background-color:#e5Ebf0;
    margin:5px;
}
{/literal}
</style>
<div class='autofs_main'>
<h2>Edit a remote nfs directory</h2>
<table><tr><td style='vertical-align:top'>
<img src='images/folder.png' />
</td><td style='text-align:right;vertical-align:top'>
<input type='hidden' name='autofs_mapname' value='{$mapname}'/>
<input type='hidden' name='autofs_orig_rep_path' value='{$rep->id}'/>

<fieldset class='round'>
<legend>
<b>Local path</b>
</legend>
<label for="autofs_rep_path">Local Path</label>
<input type='text' name='autofs_rep_path' value='{$rep->path}'/>
</fieldset>

<fieldset class='round'>
<legend>
<b>Remote filesystem</b>
</legend>
<label for="autofs_rep_remote_path">Directory</label>
<input type='text' name='autofs_rep_remote_path' value='{$rep->remote_path}'/>
<br />
<label for="autofs_rep_server">NFS Server Adress</label>
<input type='text' name='autofs_rep_server' value='{$rep->server}'/>
</fieldset>

<fieldset class='round'>
<legend>
<b>Options</b>
</legend>
<label for="autofs_rep_options">Mount options</label>
<input type='text' name='autofs_rep_options' value='{$rep->options}' />
</fieldset>
</td></tr><tr><td></td><td style='text-align:right'>
<input type='submit' name='_autofs_edit_form_rep_posted' value='Edit' />
<input type='submit' name='_autofs_cancel' value='Cancel' />
</td></tr></table>
</div>
<div>
{foreach from=$message item=mess}
{$mess}
{/foreach}
</div>
