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
{if isset($mpointname)}
<h2>Edit a mountpoint</h2>
{else}
<h2>Add a new mountpoint</h2>
{/if}
<table><tr><td>
<img src='images/drive.png' alt='drive'/>
</td><td style='text-align:right'>
<fieldset class='round'>
<label for='autofs_mountname'>Nom du point de montage</label>
{if isset(  $mpointname)}
<input type='text' name='disablefield' value='{$mpointname}' disabled='true' />
<input type='hidden' name='autofs_mountname' value='{$mpointname}' />
{else}
<input type='text' name='autofs_mountname'>
{/if}
<br />
<br />
<label for='autofs_mountpoint'>Chemin de destination</label>
<input type='text' name='autofs_mountpoint' {if isset($mpoint)}value='{$mpoint}'{/if} />
</fieldset>
</td></tr><tr><td></td><td style='text-align:right'>
{if isset($mpointname)}
<input type='submit' name='_autofs_edit_form_mpoint_posted' value='Edit' />
{else}
<input type='submit' name='_autofs_add_form_mpoint_posted' value='Create' />
{/if}
<input type='submit' name='autofs_cancel' value='Cancel' />
</td></tr></table>
</div>
